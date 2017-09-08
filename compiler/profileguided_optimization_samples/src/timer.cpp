//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright (C) Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================

#include "timer.h"
#ifdef _WIN32
#include <Windows.h>
#include <intrin.h>
#else
#include <sys/time.h>
#endif

// Description:
// Registers the current clock tick value in m_start_clock_tick, current time value in m_start_time
// Microsoft Windows* uses __rdtsc for clock ticks and QueryPerformanceFrequency/QueryPerformanceCounter for time
// Linux* uses the rdtsc instruction for clock ticks and get_timeofday for time
void CUtilTimer::start() {
#ifdef _WIN32
	
	// Clock ticks
	//__rdtsc() is an intrinsic provided by Microsoft Visual Studio* in intrin.h header file
	m_start_clock_tick = __rdtsc();

	// Time
	// QueryPerformanceFrequency works with QueryPerformanceCounter to return a human-readable time, provided in Windows.h
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_frequency);
	unsigned __int64 now;
	QueryPerformanceCounter((LARGE_INTEGER *)&now);
	// Divide the raw counter by m_frequency for time in seconds
	m_start_time = static_cast<double>(now) / m_frequency;
#else

	// Clock ticks
	// On Linux, rdtsc instruction is used since we don't have intrinsic equivalent of __rdtsc()
	unsigned lower, higher;
	// rdtsc instruction returns a 64 bit clock tick 
	// whose lower 32 bits is stored in EAX and higher 32 bits are stored in EDX register
	__asm__ __volatile__("rdtsc":"=a"(lower), "=d"(higher));
	// Constructing the 64 bit value from EAX and EDX
	m_start_clock_tick = ((unsigned long long)lower)|(((unsigned long long)higher)<<32);

	// Time
	struct timeval start;
	gettimeofday(&start, 0); //Returns the time of the day
	//tv_sec records time in seconds and tv_usec records time in micro seconds
	m_start_time = ((double) start.tv_sec + (double) start.tv_usec/1000000.0); 
#endif
}

// Description:
// Registers the current clock tick value in m_end_clock_tick, current time value in m_end_time
// Windows uses __rdtsc for clock ticks and QueryPerformanceFrequency/QueryPerformanceCounter for time
// Linux* uses the rdtsc instruction for clock ticks and get_timeofday for time
void CUtilTimer::stop() {
#ifdef _WIN32

	// Clock ticks
	m_end_clock_tick = __rdtsc();

	// Time
	unsigned __int64 now;
	QueryPerformanceCounter((LARGE_INTEGER *)&now);
	m_end_time = static_cast<double>(now) / m_frequency;
#else

	// Clock ticks
	unsigned lower, higher;
	__asm__ __volatile__("rdtsc":"=a"(lower), "=d"(higher));
	m_end_clock_tick = ((unsigned long long)lower)|(((unsigned long long)higher)<<32);

	// Time
	struct timeval start;
	gettimeofday(&start, 0);
	m_end_time = ((double) start.tv_sec + (double) start.tv_usec/1000000.0);
#endif
}

// Description:
// Returns the number of clock ticks taken between start and stop
long long CUtilTimer::get_ticks() {
	return (m_end_clock_tick - m_start_clock_tick);
}

// Description:
// Returns the number of seconds taken between start and stop
double CUtilTimer::get_time() {
	return (m_end_time - m_start_time);
}

