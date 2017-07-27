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
// =============================================================


#include "Util.hpp"
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <iomanip>

using namespace std;

bool errorf (float a, float b, int scale)
{
    const float maxAbsoluteError = FLT_MIN ;        //1.175494351e-38
    const float maxRelativeError = FLT_EPSILON ;    //1.192092896e-07

    float fab = fabs(a - b);
    if (fab <= maxAbsoluteError) {
        return false;
    }

    float fa  = fabs(a),
          fb  = fabs(b);
    if (fab / ((fa > fb) ? fa : fb) <= scale*maxRelativeError) {
        return false;
    }

    return true;
}

FloatError::FloatError (int scale)
: maxAbsoluteError(FLT_MIN),        //1.175494351e-38
  maxRelativeError(FLT_EPSILON),    //1.192092896e-07
  relativeLimit(scale*maxRelativeError)
{
    max_error = 0;
}

FloatError::FloatError (float err)
: maxAbsoluteError(FLT_MIN),        //1.175494351e-38
  maxRelativeError(FLT_EPSILON),    //1.192092896e-07
  relativeLimit(err)
{
    max_error = 0;
}

bool FloatError::errorf(float a, float b)
{
    float fab = fabs(a - b);
    if (fab <= maxAbsoluteError) {
        return false;
    }

    float fa = fabs(a),
          fb = fabs(b),
          fx = fab / ((fa > fb) ? fa : fb);

    if (fx <= relativeLimit) {
        return false;
    }

    if (max_error < fx) {
        max_error = fx;
    }

    return true;
}

float randf (float fmin, float fmax)
{
    return fmin + (fmax - fmin)*(float)rand()/RAND_MAX;
}


int ilog2 (int x)
{
    int ilog = 0;

    for (; x > 1; x >>= 1) {
        ++ilog;
    }

    return ilog;
}
