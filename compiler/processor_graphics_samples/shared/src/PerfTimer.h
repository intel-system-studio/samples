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


#ifndef __PERFTIMER_H__
#define __PERFTIMER_H__

#ifdef __linux__
#include <time.h>
#else
#include <windows.h>
#endif

class PerfTimer
{
private:
#ifdef __linux__
    timespec stamp1, stamp2;
#else
    LARGE_INTEGER freq, stamp1, stamp2;
#endif

public:
    PerfTimer();

    void start();
    void stop();

    size_t nanosec() const;
    double sec() const;

private:
#ifdef __linux__
    static int subtract(timespec &result,
                        const timespec &x,
                        const timespec &y);
#endif
};

class simple_timer
{
private:
    int sampling_count;
    size_t time;

public:
    simple_timer();

    void clear();

    void add(const PerfTimer& pc);
    void add(const size_t& ns);

    size_t nano_sec() const;
    int samples() const;
};

#endif //__PERFTIMER_H__
