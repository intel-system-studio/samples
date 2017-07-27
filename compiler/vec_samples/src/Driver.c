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

#define ROW 101
#define COL 101

/*
 * Using COLBUF=1 makes each row of 'a' have the same alignment (e.g. a[][102] rather
 * than a[][101] so that the vectorizer can use aligned load instructions
 * note that 16 byte alignment of floats instead of doubles would require COLBUF=3.
 */
#ifdef ALIGNED
#define COLBUF 1
#else
#define COLBUF 0
#endif
#define COLWIDTH COL+COLBUF
#define REPEATNTIMES 1000000

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif /* _WIN32 */

#include <stdio.h>
#include <math.h>
#include "Multiply.h"

/* routine to initialize an array with data */

void init_matrix(int row, int col, FTYPE off, FTYPE  a[][COLWIDTH])
{
    int i,j;

    for (i=0; i< row;i++) {
        for (j=0; j< col;j++) {
            a[i][j] = fmod(i*j+off,10.0);
        }
    }
    if (COLBUF>0)
        for  (i=0;i<row;i++)
            for (j=col;j<COLWIDTH;j++)
                a[i][j]=0.0;
}

void init_array(int length, FTYPE off, FTYPE a[])
{
    int i;

    for (i=0; i< length;i++)
        a[i] = fmod(i+off,10.0);
    if (COLBUF>0)
        for  (i=length;i<COLWIDTH;i++)
                a[i]=0.0;
}

void printsum(int length, FTYPE ans[]) {
    /* Doesn't print the whole matrix - Just a very simple Checksum */
    int i;
    double sum=0.0;

    for (i=0;i<length;i++) sum+=ans[i];

    printf("Sum of result = %f\n", sum);
}



double clock_it(void)
{
#ifdef _WIN32
   clock_t start;
   double  duration;

   start = clock();
   duration = (double)(start) / CLOCKS_PER_SEC;
   return duration;
#else
    double duration = 0.0;
    struct timeval start;

    gettimeofday(&start, NULL);
    duration = (double)(start.tv_sec + start.tv_usec/1000000.0);
    return duration;
#endif /* _WIN32 */
}



int main()
{
    double execTime = 0.0;
    double startTime, endTime;

    int k, size1, size2;

/*
 * Tell the compiler to align the a, b, and x arrays on 16-byte
 * boundaries.  This allows the vectorizer to use aligned instructions
 * and produce faster code.
 */
#ifdef ALIGNED
#ifdef _WIN32
    _declspec(align(16)) FTYPE a[ROW][COLWIDTH];
    _declspec(align(16)) FTYPE b[ROW];
    _declspec(align(16)) FTYPE x[COLWIDTH];
#else
    FTYPE a[ROW][COLWIDTH]  __attribute__((aligned(16)));
    FTYPE b[ROW]            __attribute__((aligned(16)));
    FTYPE x[COLWIDTH]       __attribute__((aligned(16)));
#endif /* _WIN32 */
#else
    FTYPE a[ROW][COLWIDTH];
    FTYPE b[ROW];
    FTYPE x[COLWIDTH];
#endif
    size1 = ROW;
    size2 = COLWIDTH;

    printf("\nROW:%d COL: %d\n",ROW,COLWIDTH);

    /* initialize the arrays with data */
    init_matrix(ROW,COL,1,a);
    init_array(COL,3,x);

    /* start timing the matrix multiply code */
    startTime = clock_it();
    for (k = 0;k < REPEATNTIMES;k++) {
#ifdef NOFUNCCALL
        int i, j;
        for (i = 0; i < size1; i++) {
            b[i] = 0;
            for (j = 0;j < size2; j++) {
                b[i] += a[i][j] * x[j];
            }
        }
#else
        matvec(size1,size2,a,b,x);
#endif
        x[0] = x[0] + 0.000001;
    }
    endTime = clock_it();
    execTime = endTime - startTime;

    printf("Execution time is %2.3f seconds\n", execTime);
    printf("GigaFlops = %f\n", (((double)REPEATNTIMES * (double)COL * (double)ROW * 2.0) / (double)(execTime))/1000000000.0);
    printsum(COL,b);

    return 0;
}
