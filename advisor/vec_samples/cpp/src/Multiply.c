/*
 * Copyright (C) 2010-2016 Intel Corporation. All Rights Reserved.
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

#include <math.h>
#include "Multiply.h"

FTYPE t, sumx;

// The "restrict" qualifier tells the compiler that pointer b does
// not alias with pointers a and x meaning that pointer b does not point
// to the same memory location as a or x and there is no overlap between
// the corresponding arrays.
// Without the "restrict" qualifer the compiler may try to generate
// additional runtime checks to determine whether or not b and a or x are
// aliased and multiple versions of the inner loop. Avoiding the runtime
// checks will increase the performance of the generated code.
// When using the "restrict" qualifier, you'll need to compile with
// the "-restrict" compiler option


#ifdef NOALIAS
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[restrict], FTYPE x[], FTYPE wr[])
#else
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[], FTYPE x[], FTYPE wr[])
#endif
{
	int i, j, k, l;

	for (i = 0; i < size1; i++) {
		b[i] = 0;

#ifdef ALIGNED
#pragma vector aligned
#endif

		/* The following inner loop does vectorize but it generates a remainder loop because the trip count is not divisible  */
		/* by the vector length. By running a survey and trip count analysis you can see the current vector efficiency as well*/
		/* as the recommendations that Vectorization Advisor has for increasing the efficiency.                               */
#pragma nounroll
		for (j = 0;j < size2; j++) {
			b[i] += a[i][j] * x[j] * wr[i];
		}
		/* In the following loop we are indexing the row of "a" by "l", this causes a constant stride instead of a unit stride*/
#ifdef ALIGNED
#pragma vector aligned
#endif
#pragma nounroll
#pragma vector always
		for (l = 0;l < size2; l++) { 
			b[i] += a[l][i] * x[l];
			
		}
		/* The following loop will not vectorize because of a reduction dependency caused by the addition in sumx.            */
	/* By running survey you can see the "Assumed dependency", select this loop in the survey and then run a dependency  */
	/* analysis to verify that the dependency is real. Then you can apply the reduction clause by setting the REDUCTION */
	/* define. The compiler will then generate correctly vectorized code. Note: if you just specify a simd clause without*/
	/* specifying the reduction then the code generated will not be correct.                                             */
#pragma nounroll
#ifdef REDUCTION
#pragma omp simd reduction(+:sumx)
#endif
	for (k = 0;k < size2; k++) {
		sumx += x[k]*b[k];
	}
	}
	
}
