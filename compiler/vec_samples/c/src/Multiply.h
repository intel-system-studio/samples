/*
 * SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
 * http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
 *
 * Copyright Intel Corporation
 *
 * THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
 *
 * Header file for vec_samples tutorial. See "Tutorial: Auto-vectorization"
 * in the Intel(R) C++ Compiler Tutorials document
 */

#ifndef FTYPE
#define FTYPE double
#endif

#ifdef NOALIAS
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[restrict], FTYPE x[]);
#else
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[], FTYPE x[]);
#endif
