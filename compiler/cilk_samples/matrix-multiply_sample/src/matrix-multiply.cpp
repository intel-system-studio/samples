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
 
#include <cstdlib>
#include <cilk/cilk.h>
#include <iostream>
#include "cilktime.h"

#define DEFAULT_MATRIX_SIZE 4

// Multiply double precsion square n x n matrices. A = B * C
// Matrices are stored in row major order.
// A is assumed to be initialized.
void matrix_multiply(double* A, double* B, double* C, unsigned int n)
{
    if (n < 1) {
        return;
    }

    cilk_for(unsigned int i = 0; i < n; ++i) {
        // This is the only Intel(R) Cilk(TM) Plus keyword used in this program
		// Note the order of the loops and the code motion of the i * n and k * n
		// computation. This gives a 5-10 performance improvment over exchanging
		// the j and k loops.
		int itn = i * n;
        for (unsigned int k = 0; k < n; ++k) {
            for (unsigned int j = 0; j < n; ++j) {
    	        int ktn = k * n;
                // Compute A[i,j] in the innner loop.
                A[itn + j] += B[itn + k] * C[ktn + j];
            }
        }
    }
    return;
}

void print_matrix(double* M, int nn)
{
    for (int i = 0; i < nn; ++i) {
        for (int j = 0; j < nn; ++j) {
            std::cout << M[i * nn + j] << ",  ";
        }
        std::cout << std::endl;
    }
    return;
}

int main(int argc, char** argv) {
    // Create random input matrices. Override the default size with argv[1]
    // Warning: Matrix indexing is 0 based.
    int nn = DEFAULT_MATRIX_SIZE;
    if (argc > 1) {
        nn = std::atoi(argv[1]);
    }

    std::cout << "Simple algorithm: Multiply two " << nn << " by " << nn
        << " matrices, computing A = B*C" << std::endl;

    double* A = (double*) calloc(nn* nn, sizeof(double));
    double* B = (double*) calloc(nn* nn, sizeof(double));
    double* C = (double*) calloc(nn* nn, sizeof(double));
    if (NULL == A || NULL == B || NULL == C) {
        std::cout << "Fatal Error. Cannot allocate matrices A, B, and C."
            << std::endl;
        return 1;
    }

    // Populate B and C pseudo-randomly - 
    // The matrices are populated with random numbers in the range (-1.0, +1.0)
    cilk_for(int i = 0; i < nn * nn; ++i) {
        B[i] = (float) ((i * i) % 1024 - 512) / 512;
    }
    cilk_for(int i = 0; i < nn * nn; ++i) {
        C[i] = (float) (((i + 1) * i) % 1024 - 512) / 512;
    }

    // Multiply to get A = B*C 
    unsigned long long start_tick, end_tick;
    long elapsed_milliseconds;

    start_tick = cilk_getticks();
    matrix_multiply(A, B, C, (unsigned int)nn);
    end_tick = cilk_getticks();

    float par_time = (end_tick - start_tick) / 1000.f;
    std::cout << " Matrix Multiply took " << par_time << " seconds."
        << std::endl;

    // If n is small, print the results
    if (nn <= 8) {
        std::cout << "Matrix A:" << std::endl;
        print_matrix(B, nn);
        std::cout << std::endl << "Matrix B:" << std::endl;
        print_matrix(C, nn);
        std::cout << std::endl << "Matrix C = A * B:" << std::endl;
        print_matrix(A, nn);
    }

    free(A);
    free(B);
    free(C);
    return 0;
}
