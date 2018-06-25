#include <iostream>
#include <cstdlib>

using namespace std;

const long int SIZE = 3500000;

typedef struct tricky
{
	int member1;
	float member2;
} tricky;

tricky structArray[SIZE];

int main()
{
	cout << "Starting.\n";
	for (long int i = 0; i < SIZE; i++)
	{
		structArray[i].member1 = (i / 25) + i - 78;
	}
	cout << "Done.\n";
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