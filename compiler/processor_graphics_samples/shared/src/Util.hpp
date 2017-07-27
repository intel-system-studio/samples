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


#pragma once


bool        errorf (float    a,    float b, int scale = 1);
inline bool errorf (unsigned a, unsigned b, int scale = 1)  {return a != b;}


struct FloatError
{
    const float maxAbsoluteError,    // 1.175494351e-38
                maxRelativeError,    // 1.192092896e-07
                relativeLimit;       // scale*maxRelativeError

    float max_error;

    FloatError (int scale = 1);
    FloatError (float err /*= 1.0e-3f*/);

    bool errorf (float a, float b);
};

float randf (float fmin, float fmax);
int   ilog2 (int x);
