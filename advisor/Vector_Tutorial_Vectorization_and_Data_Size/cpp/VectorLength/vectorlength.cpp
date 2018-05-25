#include <random>			// See Legal Information at the bottom of the file.
#include <iostream>
#include <time.h>
using namespace std;

void setup();

const int NUM_ELEMENTS = 50000000;

float floatsA[NUM_ELEMENTS];
float floatsB[NUM_ELEMENTS];
float floatsC[NUM_ELEMENTS];
double doublesA[NUM_ELEMENTS];
double doublesB[NUM_ELEMENTS];
double doublesC[NUM_ELEMENTS];

time_t clockStart;
time_t clockEnd;

int main ()
{
	setup();

	time(&clockStart);	/*********************** Scalar Float ***********************/
	#pragma novector
	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		floatsC[i] = (floatsA[i] / floatsB[i]) + (sin(floatsA[i]) * floatsB[i]) - (sqrt(floatsB[i]) / (floatsB[i] + cos(floatsA[i])));
		floatsC[i] += (floatsA[i] * cos(floatsB[i])) / (sqrt(floatsB[i]));
	}
	time(&clockEnd);
	cout << "Unvectorized float loop:  " << clockEnd - clockStart << " seconds.\n";

	time(&clockStart);	/********************** Scalar Double ***********************/
	#pragma novector
	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		doublesC[i] = (doublesA[i] / doublesB[i]) + (sin(doublesA[i]) * doublesB[i]) - (sqrt(doublesB[i]) / (doublesB[i] + cos(doublesA[i])));
		doublesC[i] += (doublesA[i] * cos(doublesB[i])) / (sqrt(doublesB[i]));
	}
	time(&clockEnd);
	cout << "Unvectorized double loop: " << clockEnd - clockStart << " seconds.\n";

	time(&clockStart);	/*********************** Vector Float ***********************/
	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		floatsC[i] = (floatsA[i] / floatsB[i]) + (sin(floatsA[i]) * floatsB[i]) - (sqrt(floatsB[i]) / (floatsB[i] + cos(floatsA[i])));
		floatsC[i] += (floatsA[i] * cos(floatsB[i])) / (sqrt(floatsB[i]));
	}
	time(&clockEnd);
	cout << "Vectorized float loop:    " << clockEnd - clockStart << " seconds.\n";

	time(&clockStart);	/********************** Vector Double ***********************/
	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		doublesC[i] = (doublesA[i] / doublesB[i]) + (sin(doublesA[i]) * doublesB[i]) - (sqrt(doublesB[i]) / (doublesB[i] + cos(doublesA[i])));
		doublesC[i] += (doublesA[i] * cos(doublesB[i])) / (sqrt(doublesB[i]));
	}
	time(&clockEnd);
	cout << "Vectorized double loop:    " << clockEnd - clockStart << " seconds.\n";

	return EXIT_SUCCESS;
}

void setup()
{
	for (int i = 0; i < NUM_ELEMENTS; i++)
	{
		floatsA[i] = rand() % 10000;
		floatsB[i] = (rand() % 9999) + 1;
		doublesA[i] = rand() % 10000;
		doublesB[i] = (rand() % 9999) + 1;
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
 * This file is intended for use with the "Advantages of Vectorization
 * & the Effects of Data Size" tutorial.
 */