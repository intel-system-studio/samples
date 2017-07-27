/*
 * SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
 * http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
 *
 * Copyright 2016 Intel Corporation
 *
 * THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
 *
 * Part of the vec_samples tutorial. See "Tutorial: Auto-vectorization"
 * in the Intel(R) C++ Compiler Tutorials document
 */

#include "Multiply.h"

/*
 * The "restrict" qualifier tells the compiler that pointer b does
 * not alias with pointers a and x meaning that pointer b does not point
 * to the same memory location as a or x and there is no overlap between
 * the corresponding arrays.
 * Without the "restrict" qualifer the compiler may try to generate
 * additional runtime checks to determine whether or not b and a or x are
 * aliased and multiple versions of the inner loop. Avoiding the runtime
 * checks will increase the performance of the generated code.
 * When using the "restrict" qualifier, you'll need to compile with
 * the "-restrict" compiler option
 */
#ifdef NOALIAS
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[restrict], FTYPE x[])
#else
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[], FTYPE x[])
#endif
{
    int i, j;

    for (i = 0; i < size1; i++) {
        b[i] = 0;

#ifdef ALIGNED
/*
 * The pragma vector aligned below tells the compiler to assume that the data in
 * the loop is aligned on 16-byte boundary so the vectorizer can use
 * aligned instructions to generate faster code.
 */
#pragma vector aligned
#endif

        for (j = 0;j < size2; j++) {
            b[i] += a[i][j] * x[j];
        }
    }
}
