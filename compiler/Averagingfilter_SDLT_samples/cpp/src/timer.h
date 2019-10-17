//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// ===============================================================

#ifndef TIMER_H
#define TIMER_H

class CUtilTimer {
public:
	CUtilTimer():
		m_start_time(0.0),
		m_end_time(0.0),
		m_start_clock_tick(0),
		m_end_clock_tick(0)
	{};
	// Registers the current clock tick and time value in m_start_clock_tick and m_start_time
	void start();
	// Registers the current clock tick and time value in m_end_clock_tick and m_end_time
	void stop();
	// Returns the number of seconds taken between start and stop
	double get_time();
	// Returns the number of clock ticks taken between start and stop
	long long get_ticks();
private:
	// the start time and end time in seconds
	double m_start_time, m_end_time; 
	// the start clock tick and end clock tick
	unsigned long long m_start_clock_tick, m_end_clock_tick;
	// the frequency for QueryPerformance
#ifdef _WIN32
	unsigned __int64 m_frequency;
#endif
};

#endif // TIMER_H
