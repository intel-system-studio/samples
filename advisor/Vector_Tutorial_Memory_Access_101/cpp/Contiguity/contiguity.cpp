#include <iostream>				// See Legal Information at the bottom of the file.
#include <time.h>
using namespace std;

const int LOOPS = 1500000;
const int SIZE = 14992;
const int STEPS = SIZE / 2;

float floatArray[SIZE];
double doubleArray[SIZE];

time_t start;
time_t finish;

int main()
{
	// Contiguous data access, same number of iterations as the noncontiguous.
	start = time(NULL);
	#pragma nounroll
	for (float i = 0; i < LOOPS; i++)
	{
		#pragma nounroll
		for (int j = 0; j < STEPS; j += 1)
		{
			floatArray[j] = i;
		}
	}
	finish = time(NULL);
	cout << "Contiguous Float:    " << finish - start << "\n";

	// Contiguous data access on doubles, so that it should require roughly 
	// the same number of cache line loads as the 2-stride float loop.
	start = time(NULL);
	#pragma nounroll
	for (double i = 0; i < LOOPS; i++)
	{
		#pragma nounroll
		for (int j = 0; j < STEPS; j += 1)
		{
			doubleArray[j] = i;
		}
	}
	finish = time(NULL);
	cout << "Contiguous Double:   " << finish - start << "\n";

	// Stride-2 float. Same number of iterations as the contiguous version,
	// same number of cache line loads as the double loop. Slower than both.
	start = time(NULL);
	#pragma nounroll
	for (float i = 0; i < LOOPS; i++)
	{
		#pragma nounroll
		for (int j = 0; j < STEPS * 2; j += 2)
		{
			floatArray[j] = i;
		}
	}
	finish = time(NULL);
	cout << "Noncontiguous Float: " << finish - start << "\n";

	return EXIT_SUCCESS;
}

/* Copyright (C) 2015-2016 Intel Corporation. All Rights Reserved.
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
 * This file is intended for use with the "Memory Access 101" tutorial.
 */