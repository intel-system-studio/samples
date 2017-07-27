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

#define N 1000

#include <stdio.h>
#include <math.h>
#include "ipo_sample_defs.h"

int main(void)
{
  float sumres;
  double powres;
  float a[N];
  // Call a function from another file to be inlined.
  // Not inlined due to compiler heuristics.
  init(a, N);
  // Call a function from another file to be inlined.
  // This should be inlined.
  sumres = sum(a, N);
  // This won't be inlined unless IPO is active and the math
  // library is also compiled with IPO.
  powres = pow(2.0, 4.0);

  fprintf(stdout, "%g %g\n", sumres, powres);
  return 0;
}
