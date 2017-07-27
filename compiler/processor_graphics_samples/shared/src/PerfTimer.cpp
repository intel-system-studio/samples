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


#include <PerfTimer.h>

const int GNSEC = 1000000000;

PerfTimer::PerfTimer()
{
#ifndef __linux__
    QueryPerformanceFrequency(&freq);
#endif
}

void PerfTimer::start()
{
#ifdef __linux__
    clock_gettime(CLOCK_REALTIME, &stamp1);
#else
    QueryPerformanceCounter(&stamp1);
#endif
}

void PerfTimer::stop()
{
#ifdef __linux__
    clock_gettime(CLOCK_REALTIME, &stamp2);
#else
    QueryPerformanceCounter(&stamp2);
#endif
}

#ifdef __linux__
int PerfTimer::subtract(timespec &result, const timespec &x, const timespec &y)
{
    if (y.tv_nsec < x.tv_nsec) {
        result.tv_nsec = GNSEC + y.tv_nsec - x.tv_nsec;
        result.tv_sec = y.tv_sec - 1 - x.tv_sec;
    }
    else {
        result.tv_nsec = y.tv_nsec - x.tv_nsec;
        result.tv_sec = y.tv_sec - x.tv_sec;
    }
}
#endif

size_t PerfTimer::nanosec() const
{
#ifdef __linux__
    timespec sub;
    subtract(sub, stamp1, stamp2);

    return sub.tv_sec * GNSEC + sub.tv_nsec;
#else
    return (stamp2.QuadPart - stamp1.QuadPart)*1000000000 / freq.QuadPart;
#endif
}

double PerfTimer::sec() const
{
#ifdef __linux__
    timespec sub;
    subtract(sub, stamp1, stamp2);

    return (double)sub.tv_sec + (double)sub.tv_nsec/GNSEC;
#else
    return double(stamp2.QuadPart - stamp1.QuadPart) / double(freq.QuadPart);
#endif
}

simple_timer::simple_timer() :sampling_count(0), time(0)
{
}

void simple_timer::clear()
{
    time = 0;
    sampling_count = 0;
}

void simple_timer::add(const PerfTimer& pc)
{
    time += pc.nanosec();
    sampling_count++;
}

void simple_timer::add(const size_t& ns)
{
    time += ns;
    sampling_count++;
}

size_t simple_timer::nano_sec () const
{
    return time;
}

int simple_timer::samples () const
{
    return sampling_count;
}
