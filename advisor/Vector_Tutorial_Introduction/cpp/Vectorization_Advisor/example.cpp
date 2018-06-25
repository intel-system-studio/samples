#include <iostream>			// See Legal Information at the bottom of the file.
#include <random>
#include <cstdlib>

using namespace std;

long function (int slowDown[], long i);

const long SIZE = 75000;
const long SLOWDOWNSIZE = 256;

int slowdown[SIZE][SLOWDOWNSIZE];

long arrayA[SIZE];
long arrayB[SIZE];

int main ()
{
	// Setup
	for (long i = 0; i < SIZE; i++)
	{
		arrayA[i] = rand() % 5000;
		arrayB[i] = rand() % 5000;
	}

	/****************** Normal, unit stride loop ******************/
	cout << "Normal Unit Stride Loop: \n";
	//#pragma ivdep
	for (long i = 0; i < SIZE; i++)
	{
		arrayA[i] = i + 15;
		function(slowdown[i], i);
	}
	cout << " --- DONE --- \n\n";

	/************* Not vectorizable due to dependency *************/
	cout << "Dependency loop: \n";
	for (long i = 1; i < SIZE - 1; i++)
	{
		arrayB[i] = 5 + arrayB[i - 1];
		function(slowdown[i], i);
	}
	cout << " --- DONE --- \n\n";

	/********************** Constant stride ***********************/
	cout << "Constant Stride Loop: \n";
	//#pragma ivdep
	for (long i = 0; i < SIZE; i += 2)
	{
		arrayB[i] = 2 + i;
		function(slowdown[i], i);
	}
	cout << " --- DONE --- \n\n";

	/********************** Non-unit stride ***********************/
	cout << "Non-unit Stride Loop: \n";
	for (long i = 0; i < SIZE; i += (i > 10000)? 1 : 50)
	{
		arrayA[i] = i;
		function(slowdown[i], i);
	}
	cout << " --- DONE --- \n\n";

	return EXIT_SUCCESS;
}

// Bogs the loops down
#pragma omp declare simd
long function (int slowDown[], long i)
{
	slowDown[0] = i / 100;
	slowDown[1] = i / 100;
	slowDown[2] = i / 100;
	slowDown[3] = i / 100;
	slowDown[4] = i / 100;
	slowDown[5] = i / 100;
	slowDown[6] = i / 100;
	slowDown[7] = i / 100;
	slowDown[8] = i / 100;
	slowDown[9] = i / 100;
	slowDown[10] = i / 100;
	slowDown[11] = i / 100;
	slowDown[12] = i / 100;
	slowDown[13] = i / 100;
	slowDown[14] = i / 100;
	slowDown[15] = i / 100;
	slowDown[16] = i / 100;
	slowDown[17] = i / 100;
	slowDown[18] = i / 100;
	slowDown[19] = i / 100;
	slowDown[20] = i / 100;
	slowDown[21] = i / 100;
	slowDown[22] = i / 100;
	slowDown[23] = i / 100;
	slowDown[24] = i / 100;
	slowDown[25] = i / 100;
	slowDown[26] = i / 100;
	slowDown[27] = i / 100;
	slowDown[28] = i / 100;
	slowDown[29] = i / 100;
	slowDown[30] = i / 100;
	slowDown[31] = i / 100;
	slowDown[32] = i / 100;
	slowDown[33] = i / 100;
	slowDown[34] = i / 100;
	slowDown[35] = i / 100;
	slowDown[36] = i / 100;
	slowDown[37] = i / 100;
	slowDown[38] = i / 100;
	slowDown[39] = i / 100;
	slowDown[40] = i / 100;
	slowDown[41] = i / 100;
	slowDown[42] = i / 100;
	slowDown[43] = i / 100;
	slowDown[44] = i / 100;
	slowDown[45] = i / 100;
	slowDown[46] = i / 100;
	slowDown[47] = i / 100;
	slowDown[48] = i / 100;
	slowDown[49] = i / 100;
	slowDown[50] = i / 100;
	slowDown[51] = i / 100;
	slowDown[52] = i / 100;
	slowDown[53] = i / 100;
	slowDown[54] = i / 100;
	slowDown[55] = i / 100;
	slowDown[56] = i / 100;
	slowDown[57] = i / 100;
	slowDown[58] = i / 100;
	slowDown[59] = i / 100;
	slowDown[60] = i / 100;
	slowDown[61] = i / 100;
	slowDown[62] = i / 100;
	slowDown[63] = i / 100;
	slowDown[64] = i / 100;
	slowDown[65] = i / 100;
	slowDown[66] = i / 100;
	slowDown[67] = i / 100;
	slowDown[68] = i / 100;
	slowDown[69] = i / 100;
	slowDown[70] = i / 100;
	slowDown[71] = i / 100;
	slowDown[72] = i / 100;
	slowDown[73] = i / 100;
	slowDown[74] = i / 100;
	slowDown[75] = i / 100;
	slowDown[76] = i / 100;
	slowDown[77] = i / 100;
	slowDown[78] = i / 100;
	slowDown[79] = i / 100;
	slowDown[80] = i / 100;
	slowDown[81] = i / 100;
	slowDown[82] = i / 100;
	slowDown[83] = i / 100;
	slowDown[84] = i / 100;
	slowDown[85] = i / 100;
	slowDown[86] = i / 100;
	slowDown[87] = i / 100;
	slowDown[88] = i / 100;
	slowDown[89] = i / 100;
	slowDown[90] = i / 100;
	slowDown[91] = i / 100;
	slowDown[92] = i / 100;
	slowDown[93] = i / 100;
	slowDown[94] = i / 100;
	slowDown[95] = i / 100;
	slowDown[96] = i / 100;
	slowDown[97] = i / 100;
	slowDown[98] = i / 100;
	slowDown[99] = i / 100;
	slowDown[100] = i / 100;
	slowDown[101] = i / 100;
	slowDown[102] = i / 100;
	slowDown[103] = i / 100;
	slowDown[104] = i / 100;
	slowDown[105] = i / 100;
	slowDown[106] = i / 100;
	slowDown[107] = i / 100;
	slowDown[108] = i / 100;
	slowDown[109] = i / 100;
	slowDown[110] = i / 100;
	slowDown[111] = i / 100;
	slowDown[112] = i / 100;
	slowDown[113] = i / 100;
	slowDown[114] = i / 100;
	slowDown[115] = i / 100;
	slowDown[116] = i / 100;
	slowDown[117] = i / 100;
	slowDown[118] = i / 100;
	slowDown[119] = i / 100;
	slowDown[120] = i / 100;
	slowDown[121] = i / 100;
	slowDown[122] = i / 100;
	slowDown[123] = i / 100;
	slowDown[124] = i / 100;
	slowDown[125] = i / 100;
	slowDown[126] = i / 100;
	slowDown[127] = i / 100;
	slowDown[128] = i / 100;
	slowDown[129] = i / 100;
	slowDown[130] = i / 100;
	slowDown[131] = i / 100;
	slowDown[132] = i / 100;
	slowDown[133] = i / 100;
	slowDown[134] = i / 100;
	slowDown[135] = i / 100;
	slowDown[136] = i / 100;
	slowDown[137] = i / 100;
	slowDown[138] = i / 100;
	slowDown[139] = i / 100;
	slowDown[140] = i / 100;
	slowDown[141] = i / 100;
	slowDown[142] = i / 100;
	slowDown[143] = i / 100;
	slowDown[144] = i / 100;
	slowDown[145] = i / 100;
	slowDown[146] = i / 100;
	slowDown[147] = i / 100;
	slowDown[148] = i / 100;
	slowDown[149] = i / 100;
	slowDown[150] = i / 100;
	slowDown[151] = i / 100;
	slowDown[152] = i / 100;
	slowDown[153] = i / 100;
	slowDown[154] = i / 100;
	slowDown[155] = i / 100;
	slowDown[156] = i / 100;
	slowDown[157] = i / 100;
	slowDown[158] = i / 100;
	slowDown[159] = i / 100;
	slowDown[160] = i / 100;
	slowDown[161] = i / 100;
	slowDown[162] = i / 100;
	slowDown[163] = i / 100;
	slowDown[164] = i / 100;
	slowDown[165] = i / 100;
	slowDown[166] = i / 100;
	slowDown[167] = i / 100;
	slowDown[168] = i / 100;
	slowDown[169] = i / 100;
	slowDown[170] = i / 100;
	slowDown[171] = i / 100;
	slowDown[172] = i / 100;
	slowDown[173] = i / 100;
	slowDown[174] = i / 100;
	slowDown[175] = i / 100;
	slowDown[176] = i / 100;
	slowDown[177] = i / 100;
	slowDown[178] = i / 100;
	slowDown[179] = i / 100;
	slowDown[180] = i / 100;
	slowDown[181] = i / 100;
	slowDown[182] = i / 100;
	slowDown[183] = i / 100;
	slowDown[184] = i / 100;
	slowDown[185] = i / 100;
	slowDown[186] = i / 100;
	slowDown[187] = i / 100;
	slowDown[188] = i / 100;
	slowDown[189] = i / 100;
	slowDown[190] = i / 100;
	slowDown[191] = i / 100;
	slowDown[192] = i / 100;
	slowDown[193] = i / 100;
	slowDown[194] = i / 100;
	slowDown[195] = i / 100;
	slowDown[196] = i / 100;
	slowDown[197] = i / 100;
	slowDown[198] = i / 100;
	slowDown[199] = i / 100;
	slowDown[200] = i / 100;
	slowDown[201] = i / 100;
	slowDown[202] = i / 100;
	slowDown[203] = i / 100;
	slowDown[204] = i / 100;
	slowDown[205] = i / 100;
	slowDown[206] = i / 100;
	slowDown[207] = i / 100;
	slowDown[208] = i / 100;
	slowDown[209] = i / 100;
	slowDown[210] = i / 100;
	slowDown[211] = i / 100;
	slowDown[212] = i / 100;
	slowDown[213] = i / 100;
	slowDown[214] = i / 100;
	slowDown[215] = i / 100;
	slowDown[216] = i / 100;
	slowDown[217] = i / 100;
	slowDown[218] = i / 100;
	slowDown[219] = i / 100;
	slowDown[220] = i / 100;
	slowDown[221] = i / 100;
	slowDown[222] = i / 100;
	slowDown[223] = i / 100;
	slowDown[224] = i / 100;
	slowDown[225] = i / 100;
	slowDown[226] = i / 100;
	slowDown[227] = i / 100;
	slowDown[228] = i / 100;
	slowDown[229] = i / 100;
	slowDown[230] = i / 100;
	slowDown[231] = i / 100;
	slowDown[232] = i / 100;
	slowDown[233] = i / 100;
	slowDown[234] = i / 100;
	slowDown[235] = i / 100;
	slowDown[236] = i / 100;
	slowDown[237] = i / 100;
	slowDown[238] = i / 100;
	slowDown[239] = i / 100;
	slowDown[240] = i / 100;
	slowDown[241] = i / 100;
	slowDown[242] = i / 100;
	slowDown[243] = i / 100;
	slowDown[244] = i / 100;
	slowDown[245] = i / 100;
	slowDown[246] = i / 100;
	slowDown[247] = i / 100;
	slowDown[248] = i / 100;
	slowDown[249] = i / 100;
	slowDown[250] = i / 100;
	slowDown[251] = i / 100;
	slowDown[252] = i / 100;
	slowDown[253] = i / 100;
	slowDown[254] = i / 100;
	slowDown[255] = i / 100;
	
	return i + 1;
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
 * This file is intended for use with the "Introduction to Advisor XE 
 * 2016" and "Intel Advisor XE 2016 on the Command Line + Automation" 
 * tutorials.
 */