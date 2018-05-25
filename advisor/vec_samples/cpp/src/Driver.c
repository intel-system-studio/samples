/*
 * Copyright (C) 2010-2014 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all
 * documents related to the source code ("Material") are owned by 
 * Intel Corporation or its suppliers or licensors. Title to the
 * Material remains with Intel Corporation or its suppliers and
 * licensors. The Material is protected by worldwide copyright
 * laws and treaty provisions.  No part of the Material may be
 * used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed,  or disclosed in any way
 * except as expressly provided in the license provided with the
 * Materials.  No license under any patent, copyright, trade
 * secret or other intellectual property right is granted to or
 * conferred upon you by disclosure or delivery of the Materials,
 * either expressly, by implication, inducement, estoppel or
 * otherwise, except as expressly provided in the license
 * provided with the Materials. 
 *
 * Part of the vec_samples tutorial. See "Tutorial: Auto-vectorization"
 * in the Intel(R) C++ Compiler Tutorials document
 */

#ifdef __AVX__
#define ALIGN_BOUNDARY 32 
#else
#define ALIGN_BOUNDARY 16
#endif
#define COL 47
#define ROW 47


// Using COLBUF=1 makes each row of 'a' have the same alignment (e.g. a[][102] rather
// than a[][101] so that the vectorizer can use aligned load instructions
// note that 16 byte alignment of floats instead of doubles would require COLBUF=3.
#ifdef ALIGNED
#define COLBUF 1
#define OFFSET 0
#else
#define COLBUF 0
#define OFFSET 8
#endif
#define COLWIDTH COL+COLBUF
#define REPEATNTIMES 1000000

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif // _WIN32

#include <stdio.h>
#include <math.h>
#include "Multiply.h"

//routine to initialize an array with data

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
	// Doesn't print the whole matrix - Just a very simple Checksum
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
#endif // _WIN32
}



// Tell the compiler to align the a, b, and x arrays 
// boundaries.  This allows the vectorizer to use aligned instructions
// and produce faster code.
#ifdef _WIN32
_declspec(align(ALIGN_BOUNDARY, OFFSET)) FTYPE a[ROW][COLWIDTH];
_declspec(align(ALIGN_BOUNDARY, OFFSET)) FTYPE b[ROW];
_declspec(align(ALIGN_BOUNDARY, OFFSET)) FTYPE x[COLWIDTH];
_declspec(align(ALIGN_BOUNDARY, OFFSET)) FTYPE wr[COLWIDTH];
#else
FTYPE a[ROW][COLWIDTH]	__attribute__((align(ALIGN_BOUNDARY, OFFSET)));
FTYPE b[ROW]			__attribute__((align(ALIGN_BOUNDARY, OFFSET)));
FTYPE x[COLWIDTH]		__attribute__((align(ALIGN_BOUNDARY, OFFSET)));
FTYPE wr[COLWIDTH]		__attribute__((align(ALIGN_BOUNDARY, OFFSET)));
#endif // _WIN32

int main()
{
	double execTime = 0.0;
	double startTime, endTime;

    int k, size1, size2;

	size1 = ROW;
#ifdef ALIGNED
	size2 = COLWIDTH;
#else
	size2 = COL;
#endif

    printf("\nROW:%d COL: %d\n",ROW,COLWIDTH);

    // initialize the arrays with data
	init_matrix(ROW,COL,1,a);
	init_array(COL,3,x);
	init_array(COL, 3, wr);

	//start timing the matrix multiply code
	startTime = clock_it();
	for (k = 0;k < REPEATNTIMES;k++) {
#ifdef NOFUNCCALL
        	int i, j, l, m, sumx;
		for (i = 0; i < size1; i++) {
			b[i] = 0;
#ifdef ALIGNED
#pragma vector aligned
#endif
			#pragma nounroll
			for (j = 0;j < size2; j++) {
				b[i] += a[i][j] * x[j]*wr[i];
			}
#ifdef ALIGNED
#pragma vector aligned
#endif
			#pragma nounroll
			for (l = 0;l < size2; l++) { 
				b[i] += a[l][i] * x[l];
			}
		}
		#pragma nounroll
#ifdef REDUCTION
		#pragma omp simd reduction(+:sumx)
#endif
		for (m = 0;m < size2; m++) {
			sumx += x[m]*b[m];
		}
#else
		matvec(size1,size2,a,b,x, wr); 
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

