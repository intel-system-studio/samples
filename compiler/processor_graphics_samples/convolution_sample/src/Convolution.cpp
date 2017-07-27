//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2016 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================


/*
 * Convolution.cpp
 * This file contains 2 offload implementations of 2-dimensional
 * convolution with dynamically sized filter and relying
 * on image 0 padding to handle out-of-image pixel in the filter window:
 *
 * 1) Convolution:
 *    Uses a vector function defined to calcualate the destination pixel.
 *    Simple implementation with reasonable performance
 *
 * 2) ConvolutionLocalsAN:
 *    Explicit caching in local tiles and use of Array Notations for data
 *    parallel operations.
 *    More complicated implementation with better performance
 *
 * Advice for implementing convolution on non-padded images, with
 * filter window resize on to protect from out-of-image pixels:
 * Implement 2 functions:
 * 1) With explicit control of out-of-image pixels to handle boundary areas
 *    This one does not need to be optimized, may be even scalar
 * 2) Optimized, without control of out-of-image pixels - for innter area
 *
 */


#include <mathimf.h>
#include "Workload.hpp"
#include "Util.hpp"
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;


class Convolution : public Workload
{
protected:

    static int id;

    static const char* name ()          {return "convolution";}
    static Workload* create ()          {

#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif

        return new Convolution();
    }

public:

    virtual bool open ();
    virtual bool validate ();
    virtual bool close ();

    bool execute_offload (int do_offload)
    {
        return execute_offload(image_dst, do_offload);
    }
    bool execute_cpu () {return execute_parallel(image_dst);}

protected:

    int filter_width, filter_height;
    float * filter;

    int image_width, image_height;
    int padding_vert, padding_horz;
    int image_src_row_size, image_src_total_size, image_src_offset;
    int *   image_src;
    float * image_dst, * image_ref;

    virtual bool execute_offload  (float*, int offload);
    bool execute_parallel (float*);
    void execute_serial   (float*);
};

int Convolution::id = Workload::registry(name, create);

class ConvolutionLocalsAN : public Convolution
{
protected:

    static int id;

    static const char* name ()        {return "ConvolutionLocalsAN";}
    static Workload* create ()        {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new ConvolutionLocalsAN();
    }

    bool open ();
    bool execute_offload (float*, int offload);
    bool execute_cpu ();
};

int ConvolutionLocalsAN::id = Workload::registry(name, create);

#define tile_width  32
#define tile_height 8
#define max_filter_size 10

#define tile_borders (max_filter_size ^ 1)


bool Convolution::open ()
{

    // setup filter
    props->get("filter_width",  filter_width  = 3);
    props->get("filter_height", filter_height = filter_width);
    filter_width  |= 1;
    filter_height |= 1;

    filter = (float*)w_malloc_check((filter_width * filter_height) *
        sizeof(float));

    filter[0 : filter_width * filter_height] = 0;
    const float val = 1.0f / (filter_width + filter_height - 1.0f);
    filter[filter_height / 2 * filter_width : filter_width] = val;
    filter[filter_width / 2 : filter_height : filter_width] = val;

    // setup source image
    props->get("image_width",  image_width  = 1024);
    props->get("image_height", image_height = image_width);

    cout    << endl
            << "filter_width  = " << filter_width  << endl
            << "filter_height = " << filter_height << endl
            << "image_width  = "  << image_width   << endl
            << "image_height = "  << image_height  << endl
            << endl;

    padding_horz = align(filter_width / 2, 8);
    padding_vert = filter_height / 2;
    image_src_row_size = padding_horz * 2 + image_width;
    image_src_total_size = image_src_row_size * (image_height +
        padding_vert * 2);
    image_src_offset = image_src_row_size * padding_vert + padding_horz;

    image_src = (int*)w_malloc_check(image_src_total_size * sizeof(int));
    image_src[0 : image_src_total_size] = 0;

    srand(12345);
    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            image_src[image_src_offset + y * image_src_row_size + x] =
                rand() % 256;
        }
    }
    image_ref = 0;
    int sz;
    image_dst = (float*)w_malloc_check(sz = image_width * image_height *
        sizeof(int));
    memset(image_dst, 0, sz);
    return true;
}

bool ConvolutionLocalsAN::open ()
{
    if (!Convolution::open()) {
        return false;
    }

    if (filter_width > max_filter_size || filter_height > max_filter_size) {
        cout << "filter size too big (max = " << max_filter_size << ")"
             << endl;
        return false;
    }

    if ((image_width % tile_width) != 0 || (image_height % tile_height) != 0) {
        cout << "image tiling is incorrect" << endl;
        return false;
    }
    return true;
}

bool Convolution::validate ()
{
    if (image_ref == 0) {
        image_ref = (float*)w_malloc_check(image_width * image_height *
            sizeof(int));
        execute_serial(image_ref);
    }

    int errs = 0;
    float max_err = 0.0f;
    float threshold = 0.0001;

    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            float a = image_dst[image_width * y + x],
                  b = image_ref[image_width * y + x];
            if (fabs(a - b) > threshold) {
                if (fabs(a - b) > max_err)
                    max_err = fabs(a - b);
                if (++errs < 10) {
                    cout  << "error in [" << x <<"," << y << "] "
                          << a << " != " << b
                          << endl;
                }
            }
        }
    }

    if (errs != 0) {
        cout << "total errors: " << errs << endl;
        cout << "max error:" << max_err << endl;
    }

    return errs == 0;
}

bool Convolution::close()
{
    w_free(image_ref);
    w_free(image_dst);
    w_free(image_src);
    w_free(filter);
    return true;
}

void Convolution::execute_serial (float * image)
{
    const int w2 = filter_width/2, h2 = filter_height/2;

    for (int iy = 0; iy < image_height; ++iy) {
        for (int ix = 0; ix < image_width; ++ix) {
            float sum = 0.5f;
            for (int fy = 0; fy < filter_height; ++fy) {
                for (int fx = 0; fx < filter_width; ++fx) {
                    int ifx = ix - w2 + fx, ify = iy - h2 + fy;
                    sum += filter[fy * filter_width + fx] *
                        (float)image_src[image_src_offset + ify *
                        image_src_row_size + ifx];
                }
            }
            image[iy * image_width + ix] = sum;
        }
    }
}

/********************************************************************
 * Code for offload (all versions)
 */

#define ptr(x, y, data, W)           ((data) + ((W)*(y) + (x)))

__declspec(target(gfx))
__declspec(vector( linear(ix),  \
    uniform(iy, filter_width, filter_height, filter_addr, src_rowsz,  \
        src_addr, image_width, dst_addr) \
    ))
void filter_pixel(int ix, int iy,
    int filter_width, int filter_height, float* filter_addr,
    int src_rowsz, int* src_addr,
    int image_width, float* dst_addr)
{
    const int w2 = filter_width/2, h2 = filter_height/2;

    float sum = 0.5f;

    float * filter_row = filter_addr;
    int*   src_row    = ptr(ix - w2, iy - h2, src_addr, src_rowsz);

    for (int fy = 0; fy < filter_height; ++fy) {
        float * filter_ptr = filter_row;
        int * src_ptr = src_row;
        for (int fx = 0; fx < filter_width; ++fx) {
             sum += (*filter_ptr++) * float(*src_ptr++);
        }
        filter_row += filter_width;
        src_row    += src_rowsz;
    }

    *ptr(ix, iy, dst_addr, image_width) = sum;
}

bool Convolution::execute_offload (float* image, int do_offload)
{
    int image_width         = this->image_width,
    filter_width            = this->filter_width,
    filter_height           = this->filter_height,
    image_src_row_size      = this->image_src_row_size,
    image_src_total_size    = this->image_src_total_size,
    image_src_offset        = this->image_src_offset;

    int image_size = image_height * image_width;

    float* filter = this->filter;
    int*   image_src = this->image_src;

    #pragma offload target(gfx) if (do_offload > 0) \
        pin (filter:    length(filter_width * filter_height)) \
        pin (image_src: length(image_src_total_size)) \
        pin (image:     length(image_size))
    _Cilk_for (int iy = 0;  iy < image_height; ++iy) {
        #pragma simd
        for (int ix = 0; ix < image_width; ++ix) {
            filter_pixel(ix, iy,
                filter_width, filter_height, filter,
                image_src_row_size, image_src + image_src_offset,
                image_width, image);
        }
    }
    return true;
}

bool Convolution::execute_parallel (float * image)
{
    const int image_width   = this->image_width,
    filter_width            = this->filter_width,
    filter_height           = this->filter_height,
    image_src_row_size      = this->image_src_row_size,
    image_src_total_size    = this->image_src_total_size;
    image_src_offset        = this->image_src_offset;

    const int image_size = image_height * image_width;

    float* filter = this->filter;
    int*   image_src = this->image_src;

    _Cilk_for (int iy = 0;  iy < image_height; ++iy) {
        #pragma simd
        for (int ix = 0; ix < image_width; ++ix) {
            filter_pixel(ix, iy,
            filter_width, filter_height, filter,
            image_src_row_size, image_src + image_src_offset,
            image_width, image);
        }
    }

    return true;
}

bool ConvolutionLocalsAN::execute_cpu ()
{
    int image_width   = this->image_width,
    filter_width            = this->filter_width,
    filter_height           = this->filter_height,
    image_src_row_size      = this->image_src_row_size,
    image_src_total_size    = this->image_src_total_size,
    image_src_offset        = this->image_src_offset;

    int image_size = image_height * image_width;
    int filter_size = filter_height * filter_width;

    float* filter = this->filter;
    int*   image_src = this->image_src;
    float * dst_addr     = (float *)this->image_dst;

    _Cilk_for (int i = 0; i < image_height; i+= tile_height) {
        float filter_tile[max_filter_size * max_filter_size];
        filter_tile[:] = filter[0:filter_size];
        for (int j = 0; j < image_width; j+= tile_width) {

            int (* src_addr)[image_src_row_size] = (int (*)[])(image_src +
                image_src_offset);
            const int src_tile_v_offset = filter_height / 2;
            const int src_tile_height = tile_height + src_tile_v_offset * 2;
            // assuming 8 is >= than max_filter_width/2
            const int src_tile_h_offset = 8;
            const int src_tile_width = tile_width + src_tile_h_offset * 2;

            float src_tile[tile_height + tile_borders][src_tile_width];
            float dst_tile[tile_height][tile_width];

            src_tile[0:src_tile_height][:] = src_addr[i -
                src_tile_v_offset:src_tile_height][j -
                src_tile_h_offset:src_tile_width];

            #pragma unroll
            dst_tile[:][:] = 0.5f;

            for (int fy = 0; fy < filter_height; fy++) {
                int src_tile_y = src_tile_v_offset - filter_height/2 + fy;
                for (int fx = 0; fx < filter_width; fx++) {
                    int src_tile_x = src_tile_h_offset - filter_width/2 + fx;
                    float fv = filter_tile[fy * filter_width + fx];
                    #pragma unroll
                    dst_tile[:][:] += fv *
                        src_tile[src_tile_y:tile_height]
                                [src_tile_x:tile_width];
                }
            }

            #pragma unroll
            for (int ii = 0; ii < tile_height; ii++) {
                dst_addr[(i + ii) * image_width + j:tile_width] =
                    dst_tile[ii][:];
            }
        }
    }

    return true;
}


bool ConvolutionLocalsAN::execute_offload (float* image, int do_offload)
{
    int image_width         = this->image_width,
    filter_width            = this->filter_width,
    filter_height           = this->filter_height,
    image_src_row_size      = this->image_src_row_size,
    image_src_total_size    = this->image_src_total_size,
    image_src_offset        = this->image_src_offset;

    int image_size = image_height * image_width;
    int filter_size = filter_height * filter_width;

    float* filter = this->filter;
    int*   image_src = this->image_src;
    float (* dst_addr)[image_width] = (float (*)[])image;

    #pragma offload target(gfx) if (do_offload > 0) \
        pin (filter:    length(filter_size)) \
        pin (image_src: length(image_src_total_size)) \
        pin (dst_addr:     length(image_size))
    _Cilk_for (int i = 0; i < image_height; i+= tile_height) {
        float filter_tile[max_filter_size * max_filter_size];
        filter_tile[:] = filter[0:filter_size];
        for (int j = 0; j < image_width; j+= tile_width) {

            int (* src_addr)[image_src_row_size] = (int (*)[])(image_src +
                image_src_offset);
            const int src_tile_v_offset = filter_height / 2;
            const int src_tile_height = tile_height + src_tile_v_offset * 2;
            // assuming 8 is >= than max_filter_width/2
            const int src_tile_h_offset = 8;
            const int src_tile_width = tile_width + src_tile_h_offset * 2;

            float src_tile[tile_height + tile_borders][src_tile_width];
            float dst_tile[tile_height][tile_width];

            src_tile[0:src_tile_height][:] = src_addr[i -
                src_tile_v_offset:src_tile_height][j -
                src_tile_h_offset:src_tile_width];

            #pragma unroll
            dst_tile[:][:] = 0.5f;

            for (int fy = 0; fy < filter_height; fy++) {
                int src_tile_y = src_tile_v_offset - filter_height/2 + fy;
                for (int fx = 0; fx < filter_width; fx++) {
                    int src_tile_x = src_tile_h_offset - filter_width/2 + fx;
                    float fv = filter_tile[fy * filter_width + fx];
                    #pragma unroll
                    dst_tile[:][:] += fv *
                        src_tile[src_tile_y:tile_height]
                                [src_tile_x:tile_width];
                }
            }

            #pragma unroll
            dst_addr[i:tile_height][j:tile_width] = dst_tile[:][:];

        }
    }

    return true;
}
