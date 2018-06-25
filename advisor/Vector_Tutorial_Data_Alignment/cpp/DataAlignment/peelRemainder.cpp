
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
using namespace std;

/******* Controls *******/
//#define ALIGNED_START
//#define PADDED
/************************/

#define TYPE float
const int CALL_COUNT = 1000000;
const int WIDTH = 35;

#ifdef PADDED
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
TYPE _declspec(align(32,OFFSET)) table[PADDED_WIDTH]; 		// The comma will likely be underlined in red; don't worry about it.
#else
TYPE table[PADDED_WIDTH];__attribute__((align(32,OFFSET)))
#endif

void setup (TYPE table[PADDED_WIDTH]);

#define ELEMENTS_PER_32_BYTES (32 / sizeof(TYPE))
#define PEELED (((32 - OFFSET) % 32) / sizeof(TYPE))
#define REMAINDER ((PADDED_WIDTH - PEELED) % ELEMENTS_PER_32_BYTES)
#define BODY (PADDED_WIDTH - REMAINDER - PEELED)

int main ()
{
	setup(table);
	/**** Program output; not important to demonstration ****/
	cout << "Table is at address " << &(table) << ". This is offset " << (uintptr_t) &(table) % 32 << " from a 32 byte boundary.\n";
	cout << "We have " << WIDTH << " elements of " << sizeof(TYPE) << " bytes each, plus " << PADDED_WIDTH - WIDTH 
			 << " padding elements. \nThis means we will have:\n  ";
	cout << PEELED << " Peeled Elements\n  " << BODY << " Vector Body Elements (" << BODY / ELEMENTS_PER_32_BYTES
			 << " Vector Iterations)\n  " << REMAINDER << " Remainder Elements\n";
	/********************************************************/

	#pragma novector
	for (int x = 0; x < CALL_COUNT; x++)
	{
		#pragma nounroll
		for (int j = 0; j < PADDED_WIDTH; j++)
		{
			table[j] = (table[j] / 5.3f) * (table[j]  * table[j] + table[j]) - (12.5f / (table[j] + 0.3f))
								 + (table[j] / (14.3f / (table[j] + 1.4f))) - (table[j] / 23.0f) + (14.8f / (2.4f + table[j]));
		}
	}

	return EXIT_SUCCESS;
}

void setup (TYPE table[PADDED_WIDTH])
{
	for (int w = 0; w < WIDTH; w++)
	{
		table[w] = rand() % 10000 / 3;
	}

	#ifdef PADDED
	for (int w = WIDTH; w < PADDED_WIDTH; w++)
	{
		table[w] = 0;
	}
	#endif
}

