#include <iostream>		// See Legal Information at the bottom of the file.
#include <random>
#include <time.h>
#include <math.h>
using namespace std;

void setup();

const int SIZE = 9000;
double tableA[SIZE][SIZE];
double tableB[SIZE][SIZE];
double results[SIZE][SIZE];

time_t start = 0;
time_t ending = 0;

int main ()
{
	setup();

	time(&start);		/******************* Vectorized Row Traversing Loop ******************/
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			results[x][y] = (tableA[x][y] / tableB[x][y]) - ((double) 500.0 * atan(tableB[x][y]));
		}
	}
	time(&ending);
	cout << "(vectorized) Across rows:                  " <<  ending - start << " seconds\n";

	time(&start);		/**************** Interchanged Column Traversing Loop ****************/
	for (int x = 0; x < SIZE; x++)
	{
		for (int y = 0; y < SIZE; y++)
		{
			results[y][x] = (tableA[y][x] / tableB[y][x]) - ((double) 500.0 * atan(tableB[y][x]));
		}
	}
	time(&ending);
	cout << "(vectorized) Down Columns:                 " <<  ending - start << " seconds\n";

	time(&start);		/***************** Vectorized Column Traversing Loop *****************/
	for (int x = 0; x < SIZE; x++)
	{
		cout << "";	// Prevents loop interchange
		for (int y = 0; y < SIZE; y++)
		{
			results[y][x] = (tableA[y][x] / tableB[y][x]) - ((double) 500.0 * atan(tableB[y][x]));
		}
	}
	time(&ending);
	cout << "(vectorized) Down Columns, no interchange: " <<  ending - start << " seconds\n";

	time(&start);		/********************* Scalar Row Traversing Loop ********************/
	#pragma novector
	for (int x = 0; x < SIZE; x++)
	{
		#pragma novector
		for (int y = 0; y < SIZE; y++)
		{
			results[x][y] = (tableA[x][y] / tableB[x][y]) - ((double) 500.0 * atan(tableB[x][y]));
		}
	}
	time(&ending);
	cout << "(unvectorized) Across Rows:                " <<  ending - start << " seconds\n";

	time(&start);		/******************* Scalar Column Traversing Loop *******************/
	#pragma novector
	for (int x = 0; x < SIZE; x++)
	{
		#pragma novector
		for (int y = 0; y < SIZE; y++)
		{
			results[y][x] = (tableA[y][x] / tableB[y][x]) - ((double) 500.0 * atan(tableB[y][x]));
		}
	}
	time(&ending);
	cout << "(unvectorized) Down Columns:               " <<  ending - start << " seconds\n";
}

void setup()
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			tableA[i][j] = (double) (rand() % 5000);
			tableB[i][j] = (double) ((rand() % 4900) + 1);
		}
	}
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
 * This file is intended for use with the "Stride and Memory Access
 * Patterns" tutorial.
 */