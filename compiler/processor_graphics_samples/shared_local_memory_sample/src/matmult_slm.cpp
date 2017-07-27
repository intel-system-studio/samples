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


// This is an example of a Gen kernel implementing matrix multiplication
// and utilizing Shared Local Memory feature via the new programming model
// extensions.
//

#include <iostream>
#include <memory.h>
#include <stdio.h>

#include "gfx/gfx_rt.h" // prototypes for _GFX* functions
#include "cilk/cilk.h"

#define _aligned_malloc _mm_malloc
#define _aligned_free _mm_free

// ----------------------------------------------------------------------------

#define SLM_TILE_X (4*8)
#define SLM_TILE_Y (4*8)
#define SLM_TILE_K (4*8)

#define SIZE_X (2*SLM_TILE_X)
#define SIZE_Y (2*SLM_TILE_Y)
#define SIZE_K (2*SLM_TILE_X)

#ifdef __GFX__
#define BARRIER _gfx_gpgpu_thread_barrier()
#else
#define BARRIER
#endif

#define TILE_Y 8
#define TILE_K 8
#define TILE_X 8

// The naive straightforward algorithm used to obtain reference results on CPU
static void matmult_naive(float A[][SIZE_K],
                          float B[][SIZE_X],
                          float C[][SIZE_X])
{
    for (int y = 0; y < SIZE_Y; y++) {
        for (int x = 0; x < SIZE_X; x++) {
            C[y][x] = (float)0;

            for (int k = 0; k < SIZE_K; k++) {
                C[y][x] += A[y][k] * B[k][x];
            }
        }
    }
}

// SLM-based algorithm
__declspec(target(gfx_kernel))
void matmult_tiled_slm(
    float A[][SIZE_K], float B[][SIZE_X], float C[][SIZE_X])
{
    // The parallel loop nest below iterates over "supertiles" in the resulting
    // matrix C and it is parallelized across thread groups, 1 iteration per
    // group, which effectively means that the loop nest is peeled off.
    // This kernel is programmed so that each thread group calculates one
    // resulting supertile in matrix C.
    //
    _Cilk_for _Thread_group (int tg_y = 0; tg_y < SIZE_Y; tg_y += SLM_TILE_Y) {
        _Cilk_for _Thread_group (int tg_x = 0; tg_x < SIZE_X;
                                 tg_x += SLM_TILE_X) {
            // declare "supertiles" of each matrix to be allocated in SLM
            __thread_group_local float slm_atile[SLM_TILE_Y][SLM_TILE_K];
            __thread_group_local float slm_btile[SLM_TILE_K][SLM_TILE_X];
            __thread_group_local float slm_ctile[SLM_TILE_Y][SLM_TILE_X];

            // initialize the result supertile (in parallel)
            //slm_ctile[:][:] = (float)0;
            _Cilk_for (int i0 = 0; i0 < SLM_TILE_Y; i0++)
            _Cilk_for (int i1 = 0; i1 < SLM_TILE_X; i1++)
                slm_ctile[i0][i1] = (float)0;

            // calculate the dot product of supertiles:
            for (int super_k = 0; super_k < SIZE_K; super_k += SLM_TILE_K) {
                // cache A's and B's "supertiles" in SLM (in parallel)
                //slm_atile[:][:] = A[tg_y:SLM_TILE_Y][super_k:SLM_TILE_K];
                _Cilk_for (int i0 = 0; i0 < SLM_TILE_Y; i0++)
                _Cilk_for (int i1 = 0; i1 < SLM_TILE_K; i1++)
                    slm_atile[i0][i1] = A[tg_y+i0][super_k+i1];

                //slm_btile[:][:] = B[super_k:SLM_TILE_K][tg_x:SLM_TILE_X];
                _Cilk_for (int i0 = 0; i0 < SLM_TILE_K; i0++)
                _Cilk_for (int i1 = 0; i1 < SLM_TILE_X; i1++)
                    slm_btile[i0][i1] = B[super_k+i0][tg_x+i1];

                // need a barrier, since every tile in tiles are used by
                // multiple threads in the group
                BARRIER;

                // now multiply the supertiles as usual matrices (in parallel)
                // ...
                // ... using the most effective tiled algorithm:
                _Cilk_for (int t_y = 0; t_y < SLM_TILE_Y; t_y += TILE_Y) {
                    _Cilk_for (int t_x = 0; t_x < SLM_TILE_X; t_x += TILE_X) {
                        // allocate tiles in registers
                        float atile[TILE_Y][TILE_K], btile[TILE_X];
                        float ctile[TILE_Y][TILE_X];

                        // ... and initialize ctile to zero
                        ctile[:][:] = (float)0;

                        // calculate the dot product of the tiles
                        for (int k = 0; k < SLM_TILE_K; k += TILE_K) {
                            atile[:][:] = slm_atile[t_y:TILE_Y][k:TILE_K];

                            for (int k_ind = 0; k_ind < TILE_K; k_ind++) {
                                btile[:] = slm_btile[k+k_ind][t_x:TILE_X];

                                // multiply current btile row by atile's
                                // current element and add up to corresponding
                                // ctile row
                                for (int y_ind = 0; y_ind < TILE_Y; y_ind++) {
                                    ctile[y_ind][:] += atile[y_ind][k_ind] *
                                        btile[:];
                                }
                            }
                        }
                        // flush the thread-local ctile (registers) into the
                        // thread group-local supertile (SLM) adding up
                        // elements
                        slm_ctile[t_y:TILE_Y][t_x:TILE_X] += ctile[:][:];
                    }
                }

                // barrier to make sure
                // (1) next iteration of the loop does not overwrite a and b
                //   SLM tiles used in the above calculation of slm_ctile
                // (2) on the last iteration of the loop, all threads wait
                //   for the SLM ctile calculation to be completed before
                //   writing it back to memory below this loop
                BARRIER;
            }

            // write (in parallel) the result supertile back to memory:
            //C[tg_y:SLM_TILE_Y][tg_x:SLM_TILE_X] = slm_ctile[:][:];
            _Cilk_for (int i0 = 0; i0 < SLM_TILE_Y; i0++)
            _Cilk_for (int i1 = 0; i1 < SLM_TILE_X; i1++)
                C[tg_y+i0][tg_x+i1] = slm_ctile[i0][i1];

            // next iteration of the loop zeroes out slm_ctile - make sure this
            // always happens after slm_ctile has been dumped to memory (above
            // loop nest completes):
            BARRIER;
        }
    }
}

// some matrix initialization function
extern void randomize_matrix(int width, int height, float m[][width]) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            m[j][i] = (float)(j*4+i);
        }
    }
}

static bool verify2D(
    char *title,
    int size_y, int size_x,
    float (*arr)[size_x], float (*ref_arr)[size_x])
{
    int err_cnt = 0;

    printf("verifying %s...", title);
    fflush(stdout);

    for (int y = 0; y < size_y; y++) {
        for (int x = 0; x < size_x; x++) {
            float val = arr[y][x];
            float gold_val = ref_arr[y][x];

            if (val != gold_val) {
                if (err_cnt == 0) {
                    printf("\n");
                }
                if (++err_cnt < 10) {
                    std::cout <<
                        "  ERROR at [" << y << "][" << x << "]: " <<
                        val << "(should be " << gold_val << ")" << std::endl;
                }
            }
        }
    }
    if (err_cnt == 0) {
        printf(" ok\n");
    }
    else {
        printf("FAILED\n");
    }
    return err_cnt == 0;
}

#define quote1(x) # x
#define quote(x) quote1(x)

#define PAGE_SIZE 4096
// the main function which simply shows how the above kernels can be called
// from the host
int main(int argc, char **argv)
{
    const int el_size = sizeof(float);
    const int a_size = SIZE_Y*SIZE_K*el_size;
    const int b_size = SIZE_K*SIZE_X*el_size;
    const int c_size = SIZE_Y*SIZE_X*el_size;
    // input1:
    float (*A)[SIZE_K]  = (float(*)[SIZE_K])_aligned_malloc(a_size, PAGE_SIZE);
    // input2:
    float (*B)[SIZE_X]  = (float(*)[SIZE_X])_aligned_malloc(b_size, PAGE_SIZE);
    // Gen result:
    float (*C1)[SIZE_X] = (float(*)[SIZE_X])_aligned_malloc(c_size, PAGE_SIZE);
    // CPU SLM-based algorithm result:
    float (*C2)[SIZE_X] = (float(*)[SIZE_X])_aligned_malloc(c_size, PAGE_SIZE);
    // CPU reference result:
    float (*C3)[SIZE_X] = (float(*)[SIZE_X])_aligned_malloc(c_size, PAGE_SIZE);

    // initialize input:
    randomize_matrix(SIZE_K, SIZE_Y, A);
    randomize_matrix(SIZE_X, SIZE_K, B);

    // make matrices accessible by Gen:
    _GFX_share(A,  a_size);
    _GFX_share(B,  b_size);
    _GFX_share(C1, c_size);

    // Enqueue task for running on Gen:
    _GFX_offload(matmult_tiled_slm, A, B, C1);

    // obtain SLM algorithm result on CPU:
    matmult_tiled_slm(A, B, C2);

    // obtain reference result on CPU:
    matmult_naive(A, B, C3);

    // wait for all offload tasks to finish:
    _GFX_wait();

    // cancel matrix sharing (remove pinning from their pages):
    _GFX_unshare(A);
    _GFX_unshare(B);
    _GFX_unshare(C1);

    // now verify results
    // check that slm algorithm produces correct results on CPU:
    bool res_cpu =
        verify2D("matrix multiply: 'SLM' CPU vs. CPU", SIZE_Y, SIZE_X, C2, C3);
    // verify Gen results:
    bool res_gen =
        verify2D("matrix multiply: SLM Gen vs. CPU", SIZE_Y, SIZE_X, C1, C3);
    // verify Gen results against SLM CPU algorithm:
    bool res_slm =
        verify2D("matrix multiply: SLM Gen vs. 'SLM' CPU", SIZE_Y, SIZE_X, C1,
                C2);

    if (!res_cpu) {
        printf("SLM algorithm failed on CPU!\n");
    }
    bool res = res_cpu && res_gen;
    printf("\n%s\n", res ? "passed" : "failed!");

    _aligned_free(A);
    _aligned_free(B);
    _aligned_free(C1);
    _aligned_free(C2);
    _aligned_free(C3);

    return res ? 0 : 1;
}
