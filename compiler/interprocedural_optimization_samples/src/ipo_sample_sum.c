//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2016 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
//=============================================================
//
// DESCRIPTION]
// One of three source files, and one library file, used
// to demonstrate how to use Interprocedural Optimization (IPO)
// during compilation and linking.
//
// The files needed to compile are:
//  ipo_sample_defs.h
//  ipo_sample_init.c
//  ipo_sample_main.c
//  ipo_sample_sum.c
// 
// [COMPILE] 
// Use the following compiler options to compile: 
// 
// Windows*: /Qipo
// 
// Linux* and OS X*: -ipo
//

#include "ipo_sample_defs.h"

// This function will be inlined and deleted by intraprocedural
// optimization (IP).
static float add3(float x)
{
  return x + 3;
}

// This function will be inlined into external callers by
// interprocedural optimization (IPO).
float sum(float a[], int n)
{
  float result; 
  int i;
  result = 0;
  for (i = 0; i < n; i++) 
    result += a[i] + add3(a[i]);
  return result; 
}
