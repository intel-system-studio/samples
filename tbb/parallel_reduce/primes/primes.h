/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright 2016 Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#ifndef PRIMES_H_
#define PRIMES_H_

#if __TBB_MIC_OFFLOAD
#pragma offload_attribute (push,target(mic))
#endif // __TBB_MIC_OFFLOAD

#include "tbb/task_scheduler_init.h"
#include <cstddef>
typedef std::size_t NumberType;

//! Count number of primes between 0 and n
/** This is the serial version. */
NumberType SerialCountPrimes( NumberType n);

//! Count number of primes between 0 and n
/** This is the parallel version. */
NumberType ParallelCountPrimes( NumberType n, int numberOfThreads= tbb::task_scheduler_init::automatic, NumberType grainSize = 1000);

#if __TBB_MIC_OFFLOAD
#pragma offload_attribute (pop)
#endif // __TBB_MIC_OFFLOAD

#endif /* PRIMES_H_ */
