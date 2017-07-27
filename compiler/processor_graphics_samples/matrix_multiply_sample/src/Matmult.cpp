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
 * Matmult.cpp
 * This file contains 3 offload implementations of matrix multipliction
 * 1) Trivial implementation, with outer loop vectorization with pragma simd
 * 2) Much faster tiled approach, where matrix tiles are cached in local arrays
 *
 *    Note that performance of this version relies on full unrolling of several
 *    loops which substantially increases the compile time. So, for
 *    convenience, there are COMPILE_*_VERSION macros enabling or disabling
 *    compilation of each of these versions.
 *
 */


#include "Workload.hpp"
#include "Util.hpp"
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

template <class FTYPE>
class Matmult : public Workload
{
protected:

    static int id;

    static const char* name ();
    static Workload* create () {
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
        return new Matmult();
    }

public:

    virtual bool open ();
    virtual bool execute_offload (int do_offload);
    virtual bool execute_cpu ();
    virtual bool validate ();
    virtual bool close ();

protected:

    int m_width, m_height, m_common;
    int m_matAsize,
        m_matBsize,
        m_matCsize;
    FTYPE * m_matA,
          * m_matB,
          * m_matC,
          * m_matGold;

    void execute_validate (FTYPE *);

    virtual void execute_part (int do_offload, int off, int n);
};

template <class FTYPE>
class MatmultLocalsAN : public Matmult<FTYPE>
{
protected:

    static int id;

    static const char* name ();
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
        return new MatmultLocalsAN();
    }

    virtual void execute_part (int do_offload, int off, int n);
    virtual bool open ();
    virtual bool execute_cpu();
};

const char* Matmult<float>::name ()          {return "matmult";}
int Matmult<float>::id = Workload::registry(Matmult<float>::name,
    Matmult<float>::create);

const char* MatmultLocalsAN<float>::name ()  {return "matmultlocalsan";}
int MatmultLocalsAN<float>::id = Workload::registry(
    MatmultLocalsAN<float>::name, MatmultLocalsAN<float>::create);

const char* Matmult<double>::name ()         {return "matmult_d";}
int Matmult<double>::id = Workload::registry(Matmult<double>::name,
    Matmult<double>::create);

const char* MatmultLocalsAN<double>::name () {return "matmultLocalsan_d";}
int MatmultLocalsAN<double>::id = Workload::registry(
    MatmultLocalsAN<double>::name, MatmultLocalsAN<double>::create);



#define TILE_m 16
#define TILE_k (64 / static_cast<int>(sizeof(FTYPE)))
#define TILE_n (128 / static_cast<int>(sizeof(FTYPE)))


static float randf ()
{
    const float scale = 10.0f/RAND_MAX;
    return scale*rand();
}

template <class FTYPE>
bool Matmult<FTYPE>::open ()
{
    prop_get("common", m_common = 1 << 6);
    prop_get("width",  m_width  = m_common);
    prop_get("height", m_height = m_common);

    m_matAsize = m_height * m_common,
    m_matBsize = m_common * m_width,
    m_matCsize = m_height * m_width;

    cout << endl
          << "common = " << m_common << endl
          << "width  = " << m_width  << endl
          << "height = " << m_height << endl;

    m_matA = (FTYPE*)w_malloc_check(m_matAsize*sizeof(FTYPE)),
    m_matB = (FTYPE*)w_malloc_check(m_matBsize*sizeof(FTYPE)),
    m_matC = (FTYPE*)w_malloc_check(m_matCsize*sizeof(FTYPE)); memset(m_matC,
        0, m_matCsize*sizeof(FTYPE));
    m_matGold = 0;

    srand(12345);
    for (int i = 0; i < m_matAsize; i++) {
        m_matA[i] = (FTYPE)randf();
    }
    for (int i = 0; i < m_matBsize; i++) {
        m_matB[i] = (FTYPE)randf();
    }

    return true;
}

template <class FTYPE>
bool MatmultLocalsAN<FTYPE>::open ()
{
    if (!Matmult<FTYPE>::open()) {
        return false;
    }
    if (this->m_height % TILE_m != 0) {
        cout << "For tiled implementation height must be divisible by "
             << TILE_m <<endl;
        return false;
    }
    if (this->m_width % TILE_n != 0) {
        cout << "For tiled implementation width must be divisible by "
             << TILE_n <<endl;
        return false;
    }
    if (this->m_common % TILE_k != 0) {
        cout << "For tiled implementation width must be divisible by "
             << TILE_k <<endl;
        return false;
    }

    return true;
}

template <class FTYPE>
bool Matmult<FTYPE>::validate ()
{
    if (m_matGold == 0) {
        m_matGold = (FTYPE*)w_malloc_check(m_matCsize*sizeof(FTYPE));
        execute_validate(m_matGold);
    }

    int errs = 0;
    const int fscale = m_width + m_height;

    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            int k = i * m_width + j;
            if (errorf((float)m_matC[k], (float)m_matGold[k], fscale)) {
                if (errs < 10) {
                    cout << "error in [" << i <<"," << j << "] "
                          << m_matC[k] << " != " << m_matGold[k]
                          << endl;
                }
                errs++;
            }
        }
    }

    if (errs != 0) {
        cout << "total errors: " << errs << endl;
    }

    return errs == 0;
}

template <class FTYPE>
bool Matmult<FTYPE>::close ()
{
    w_free(m_matA);
    w_free(m_matB);
    w_free(m_matC);
    w_free(m_matGold);
    return true;
}

#ifdef MKL
//To compile:
//  (1) Run setup script
//      <compiler installation dir>\mkl\bin\ia32\mklvars_ia32.bat
//  (2) Use compiler options /DMKL /Qmkl

#include <mkl.h>
#include <mkl_cblas.h>

template <class FTYPE>
bool Matmult<FTYPE>::execute_cpu ()
{
    //cout << "MKL" << endl;
    cblas_sgemm (
            CblasRowMajor,
            CblasNoTrans, CblasNoTrans,
            m_height, m_width, m_common,
            1,
            m_matA, m_height,
            m_matB, m_common,
            0,
            m_matC,  m_width
    );

    return true;
}

#else

template <class FTYPE>
bool Matmult<FTYPE>::execute_cpu ()
{
    _Cilk_for (int i = 0; i < m_height; ++i) {
        #pragma simd
        for (int j = 0; j < m_width; ++j) {
            FTYPE sum = 0.0;
            for (int k = 0; k < m_common; ++k) {
                sum += m_matA[i * m_common + k] * m_matB[k * m_width + j];
            }
            m_matC[i * m_width + j] = sum;
        }
    }
    return true;
}

//template <class FTYPE>
//bool Matmult<FTYPE>::execute_cpu ()
//{
//    int width = m_width, common = m_common, height = m_height;
//    FTYPE * __restrict A = (FTYPE *)m_matA;
//    FTYPE * __restrict B = (FTYPE *)m_matB;
//    FTYPE * __restrict C = (FTYPE *)m_matC;
//
//    for (int i = 0; i < height; ++i) {
//        for (int j = 0; j < width; ++j) {
//            FTYPE sum = 0.0;
//            for (int k = 0; k < common; ++k)
////                sum += A[i * common + k] * B[k * width + j];
////            C[i * width + j] = sum;
//                C[i * width + j] += A[i * common + k] * B[k * width + j];
//        }
//    }
//    return true;
//}

#endif

template <class FTYPE>
void Matmult<FTYPE>::execute_validate (FTYPE * matAB)
{
    _Cilk_for (int i = 0; i < m_height; ++i) {
        #pragma simd
        for (int j = 0; j < m_width; ++j) {
            FTYPE sum = 0.0;
            for (int k = 0; k < m_common; ++k) {
                sum += m_matA[i * m_common + k] * m_matB[k * m_width + j];
            }
            matAB[i * m_width + j] = sum;
        }
    }
}

template <class FTYPE>
bool MatmultLocalsAN<FTYPE>::execute_cpu ()
{
    int n = this->m_width, k = this->m_common, m = this->m_height;
    FTYPE * A = (FTYPE *)this->m_matA;
    FTYPE * B = (FTYPE *)this->m_matB;
    FTYPE * C = (FTYPE *)this->m_matC;

    _Cilk_for (int r = 0; r < m; r += TILE_m) {
        for (int c = 0; c < n; c += TILE_n) {
            FTYPE atile[TILE_m][TILE_k], btile[TILE_n], ctile[TILE_m][TILE_n];
            #pragma unroll
            ctile[:][:] = 0.0;
            for (int t = 0; t < k; t += TILE_k) {
                #pragma unroll
                for (int rr = 0; rr < TILE_m; rr++) {
                    atile[rr][:] = A[(r + rr) * k + t:TILE_k];
                }
                #pragma unroll
                for (int rc = 0; rc < TILE_k; rc++) {
                    btile[:] = B[ (t + rc) * n + c:TILE_n];
                    #pragma unroll
                    for (int rt = 0; rt < TILE_m; rt++) {
                        ctile[rt][:] += atile[rt][rc] * btile[:];
                    }
                }
            }
            #pragma unroll
            for (int rr = 0; rr < TILE_m; rr++) {
                C[(r + rr) * n + c:TILE_n] = ctile[rr][:];
            }
        }
    }
    return true;
}


/********************************************************************
 * Code for offload (all versions)
 */
template <class FTYPE>
bool Matmult<FTYPE>::execute_offload(int do_offload)
{
    int n = m_height;
    int cpuShare = get_cpu_share(n);
    if (cpuShare > 0 && cpuShare < n) {
        _Cilk_spawn execute_part(0, n - cpuShare, cpuShare);
        execute_part(1, 0, n - cpuShare);
    }
    else {
        execute_part(do_offload, 0, n);
    }
    return true;
}

template <class FTYPE>
void Matmult<FTYPE>::execute_part(int do_offload, int off, int m)
{
    FTYPE * matA = this->m_matA, * matB = this->m_matB, * matC = this->m_matC;
    int common = this->m_common, width  = this->m_width, height =
        this->m_height;
    #pragma offload target(gfx) if (do_offload > 0) \
        pin(matA: length(m_matAsize)) pin(matB: length(m_matBsize)) \
        pin(matC: length(m_matCsize))
    _Cilk_for (int i = off; i < off + m; ++i) {
        _Cilk_for _Simd (int j = 0; j < width; ++j) {
            FTYPE sum = 0.0;
            for (int k = 0; k < common; ++k) {
                sum += matA[i * common + k] * matB[k * width + j];
            }
            matC[i * width + j] = sum;
        }
    }
}

template <class FTYPE>
void MatmultLocalsAN<FTYPE>::execute_part(int do_offload, int off, int m)
{

    int n = this->m_width, k = this->m_common;
    FTYPE (* A)[k] = (FTYPE (*)[])this->m_matA;
    FTYPE (* B)[n] = (FTYPE (*)[])this->m_matB;
    FTYPE (* C)[n] = (FTYPE (*)[])this->m_matC;

    #pragma offload target(gfx) if (do_offload) \
        pin(A: length(this->m_matAsize)) pin(B: length(this->m_matBsize)) \
        pin(C: length(this->m_matCsize))
    _Cilk_for (int r = off; r < off + m; r += TILE_m) {
        _Cilk_for (int c = 0; c < n; c += TILE_n) {
            FTYPE atile[TILE_m][TILE_k], btile[TILE_n], ctile[TILE_m][TILE_n];
            #pragma unroll(16)
            ctile[:][:] = 0.0;
            for (int t = 0; t < k; t += TILE_k) {
                #pragma unroll(16)
                atile[:][:] = A[r:TILE_m][t:TILE_k];
                #pragma unroll(16)
                for (int rc = 0; rc < TILE_k; rc++) {
                    btile[:] = B[t+rc][c:TILE_n];
                    #pragma unroll
                    for (int rt = 0; rt < TILE_m; rt++) {
                        ctile[rt][:] += atile[rt][rc] * btile[:];
                    }
                }
            }
            #pragma unroll(16)
            C[r:TILE_m][c:TILE_n] = ctile[:][:];
        }
    }
}
