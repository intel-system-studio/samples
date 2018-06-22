
/* Copyright (C) 2010-2017 Intel Corporation. All Rights Reserved.
 *
 * The source code, information and material ("Material") 
 * contained herein is owned by Intel Corporation or its 
 * suppliers or licensors, and title to such Material remains 
 * with Intel Corporation or its suppliers or licensors.
 * The Material contains proprietary information of Intel or 
 * its suppliers and licensors. The Material is protected by 
 * worldwide copyright laws and treaty provisions.
 * No part of the Material may be used, copied, reproduced, 
 * modified, published, uploaded, posted, transmitted, distributed 
 * or disclosed in any way without Intel's prior express written 
 * permission. No license under any patent, copyright or other
 * intellectual property rights in the Material is granted to or 
 * conferred upon you, either expressly, by implication, inducement, 
 * estoppel or otherwise. Any license under such intellectual 
 * property rights must be express and approved by Intel in writing.
 * Third Party trademarks are the property of their respective owners.
 * Unless otherwise agreed by Intel in writing, you may not remove 
 * or alter this notice or any other notice embedded in Materials 
 * by Intel or Intel's suppliers or licensors in any way.
 *
 * This file is intended for use with the "Data Alignment, Padding,
 * & Peel/Remainder Loops" tutorial.
 */
 

#include <iostream>			
#include <random>
#include <cstdlib>
#include <stdint.h>
using namespace std;

/******* Controls *******/
//#define ALIGNED_START
//#define ALIGNED_ROWS
/************************/
#define TYPE float
#define ELEMENTS_PER_32_BYTES (32 / sizeof(TYPE))
const int REPEAT = 4000;
const int HEIGHT = /*100000; //*/ 100;
const int WIDTH = /*35; //*/ 35001;

#ifdef ALIGNED_ROWS
const int PADDED_WIDTH = WIDTH + ((32 - ((WIDTH * sizeof(TYPE)) % 32)) / sizeof(TYPE));
#else 
const int PADDED_WIDTH = WIDTH;
#endif

#ifdef ALIGNED_START
#define OFFSET 0
#else
#define OFFSET 16
#endif

#ifdef _WIN32
TYPE _declspec(align(32,OFFSET)) table[HEIGHT][PADDED_WIDTH];		// The comma will likely be underlined in red; don't worry about it.
#else
TYPE table[HEIGHT][PADDED_WIDTH];__attribute__((align(32,OFFSET)))
#endif
void setup (TYPE table[HEIGHT][PADDED_WIDTH]);

int main ()
{
	setup(table);
	/**** Program output; not important to demonstration ****/
	cout << "Table is at address " << &(table) << ". That's offset " << (uintptr_t) &(table) % 32 << " from a 32 byte boundary.\n";
	cout << "Width is " << WIDTH << ". This is " << WIDTH * sizeof(TYPE) << " bytes, since each entry is " << sizeof(TYPE) << " bytes long.\n";
	cout << "Since " << WIDTH * sizeof(TYPE) << " % 32 is " << (WIDTH * sizeof(TYPE)) % 32 << ", we have that many extra bytes per row.\n" ;
	cout << "Therefore we need " << 32 - ((WIDTH * sizeof(TYPE)) % 32) << " more bytes (" << (32 - ((WIDTH * sizeof(TYPE)) % 32)) / sizeof(TYPE) 
			 << " elements) to align rows.\n" << "The padded width is " << PADDED_WIDTH << " (" << PADDED_WIDTH - WIDTH << " more), which is " 
			 << PADDED_WIDTH * sizeof(TYPE) << " bytes long.\n";
	/********************************************************/
	#ifdef ALIGNED_ROWS
	__assume(WIDTH%ELEMENTS_PER_32_BYTES==0);
	#endif
	for (int x = 0; x < REPEAT; x++)	// Extend runtime by repeating entire array.
	{
		#pragma omp for collapse(1)			// Prevent loop collapsing
		#pragma novector
		for (int i = 0; i < HEIGHT; i++)
		{
			#pragma nounroll
			for (int j = 0; j < PADDED_WIDTH; j++)
			{
				table[i][j] = (table[i][j] / 5.3f) * (table[i][j]  * table[i][j] + table[i][j]) - (12.5f / (table[i][j] + 0.3f))
											+ (table[i][j] / (14.3f / (table[i][j] + 1.4f))) - (table[i][j] / 23.0f) + (14.8f / (2.4f + table[i][j]))
											- ((table[i][j] * (3.4f / (1.2f + table[i][j]))) / (4.5f + table[i][j] - (3.4f / (table[i][j] + 1.2f))));
			}
		}
	}

	return EXIT_SUCCESS;
}

void setup (TYPE table[HEIGHT][PADDED_WIDTH])
{
	for (int h = 0; h < HEIGHT; h++)
	{
		for (int w = 0; w < WIDTH; w++)
		{
			table[h][w] = rand() % 10000 / 3;
		}
		for (int w = WIDTH; w < PADDED_WIDTH; w++)
		{
			table[h][w] = 0;
		}
	}
}

