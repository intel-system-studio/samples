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


// Simple header file for abstracting machine-dependent timers
//
// This file declares the following functions:
//
// cilk_getticks() - Returns a timer value with a resolution of
// milliseconds
//
// cilk_ticks_to_seconds - Converts the timer value to seconds.


#ifdef _WIN32
#include <Windows.h>

static inline unsigned long long cilk_getticks()
{
    // Fetch number of milliseconds that have elapsed sin the system started
     return GetTickCount();
}

static inline double cilk_ticks_to_seconds(unsigned long long ticks)
{
     return ticks * 1.0e-3;
}
#endif

#if defined __unix__ || defined __APPLE__
#include <time.h>
#include <sys/time.h>

static inline unsigned long long cilk_getticks()
{
     struct timeval t;
     gettimeofday(&t, 0);
     return t.tv_sec * 1000000ULL + t.tv_usec;
}

static inline double cilk_ticks_to_seconds(unsigned long long ticks)
{
     return ticks * 1.0e-6;
}
#endif
