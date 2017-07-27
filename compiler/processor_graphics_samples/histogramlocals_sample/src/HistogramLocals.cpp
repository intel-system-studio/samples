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


/*
 * HistogramLocals.cpp
 * Contains 2 offload implementation of byte histogram:
 *  - For a regular byte array (1 byte per pixel)
 *  - For array or 4-component pixels (4 bytes per pixel), separate for each
 * components
 *
 *  Illustrates calculation of the histogram of a byte array using local
 *  arrays in register to cache data blocks in temporary histograms.
 */


#include "Workload.hpp"
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <string.h>

using namespace std;

class HistogramLocals : public Workload
{
protected:

    static int id;

    static const char* name ()              {return "histogramlocals";}
    static Workload* create ()              {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "nso it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new HistogramLocals();
    }

    HistogramLocals(int szHistogram = 256)
        :m_szHistogram(szHistogram) {}

    virtual bool open ();
    virtual bool execute_offload (int do_offload);
    virtual bool execute_cpu ()                {return false;}
    virtual bool validate ();
    virtual bool close ();

    virtual void execute_validate(unsigned int * referenceArray);

    int m_szMatrix;
    int m_szGlobalWorkStep;
    int m_szHistogram;

    unsigned char * m_inputArray;
    unsigned int* m_tmpArray;
    unsigned int* m_outputArray;

};

class HistogramLocals4 : public HistogramLocals
{
protected:

    static int id;

    HistogramLocals4()
        :HistogramLocals(1024) {}

    static const char* name ()              {return "histogramlocals4";}
    static Workload* create ()              {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD, " <<
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new HistogramLocals4();
    }

    virtual bool execute_cpu();
    virtual bool execute_offload (int do_offload);

    virtual void execute_validate(unsigned int * referenceArray);
};


int HistogramLocals::id = Workload::registry(name, create);
int HistogramLocals4::id = Workload::registry(name, create);

bool HistogramLocals::open ()
{
    prop_get("szMatrix", m_szMatrix = 1024 * 2048);
    prop_get("szGlobalWorkStep", m_szGlobalWorkStep = 256);

    if ( (m_szMatrix % m_szGlobalWorkStep) != 0) {
        cout << "szMatrix must be divisible by szGlobalWorkStep in this "
             << "simple example implementation" << endl;
        return false;
    }
    m_szMatrix *= 4; // input value in 4-byte elements, convert to byte count

    m_inputArray = (unsigned char*)w_malloc_check(m_szMatrix);
    m_tmpArray = (unsigned int *)w_malloc_check(m_szHistogram *
        m_szGlobalWorkStep * sizeof(unsigned int));
    m_outputArray = (unsigned int *)w_malloc_check(m_szHistogram *
        sizeof(unsigned int));

    srand(12345);
    for (int i = 0; i < m_szMatrix; i++) {
        m_inputArray[i] = (unsigned int)rand();
    }
    m_outputArray[0:m_szHistogram] = 0;
    m_tmpArray[0:m_szHistogram * m_szGlobalWorkStep] = 0;

    return true;
}


bool HistogramLocals::validate ()
{
    unsigned int * referenceArray =
        (unsigned int *)w_malloc_check(m_szHistogram * sizeof(unsigned int));
    memset(referenceArray, 0, sizeof(unsigned int) * m_szHistogram);

    execute_validate(referenceArray);

    int errorCount=0, maxErrorCount=10;
    for (int i = 0; i < m_szHistogram; ++i) {
        if (m_outputArray[i] != referenceArray[i]) {
            if (errorCount < maxErrorCount) {
                cout << "INCORRECT result at index " << i << ", Result="
                     << m_outputArray[i] << ", Reference result="
                     << referenceArray[i] << endl;
            }
            errorCount++;
        }
    }
    if (errorCount > 0) {
        cout << "Total errors: " << errorCount <<endl;
    }

    w_free(referenceArray);

    return errorCount == 0;
}

bool HistogramLocals::close ()
{
    w_free(m_inputArray);
    w_free(m_tmpArray);
    w_free(m_outputArray);
    return true;
}

void HistogramLocals::execute_validate(unsigned int * outputArray)
{
    for (int i = 0; i < m_szMatrix; ++i) {
        ++outputArray[m_inputArray[i]];
    }
}

void HistogramLocals4::execute_validate(unsigned int * outputArray)
{
    for (int i = 0; i < m_szMatrix; i+=4) {
        ++outputArray[0   + m_inputArray[i]];
        ++outputArray[256 + m_inputArray[i + 1]];
        ++outputArray[512 + m_inputArray[i + 2]];
        ++outputArray[768 + m_inputArray[i + 3]];
    }
}

bool HistogramLocals4::execute_cpu()
{
    unsigned char * inputArray = (unsigned char *)m_inputArray;
    unsigned int* tmpArray = m_tmpArray;
    unsigned int* outputArray = m_outputArray;
    int szMatrix = m_szMatrix;
    int szGlobalWorkStep = m_szGlobalWorkStep;

    int szPerThread = szMatrix/szGlobalWorkStep;

    m_tmpArray[0:m_szGlobalWorkStep * 1024] = 0;

    _Cilk_for(int i = 0; i < szGlobalWorkStep; i++) {
        int out[256];
        #define BS 256
        for (int m = 0; m < szPerThread; m+=1024) {
            unsigned char ip[BS*4];
            short ipc[BS];
            ip[:] = inputArray[i * szPerThread + m:BS*4];
            for (int c = 0; c < 4; c++) {
                ipc[:] = ip[c:BS:4];
                out[:] = tmpArray[i * 1024 + c * 256:256];
                #pragma unroll
                for (unsigned int j = 0; j < 256; j++) {
                    ++out[ipc[j]];
                }
                tmpArray[i * 1024 + c * 256:256] = out[:];
            }
        }
    }


    m_outputArray[0:1024] = 0;
    for (int i = 0; i < szGlobalWorkStep; i++) {
        m_outputArray[0:1024] += tmpArray[i * 1024:1024];
    }
    return true;
}


/********************************************************************
 * Code for offload
 */

bool HistogramLocals::execute_offload(int do_offload)
{
    unsigned char* inputArray = m_inputArray;
    unsigned int* tmpArray = m_tmpArray;
    unsigned int* outputArray = m_outputArray;
    int szMatrix = m_szMatrix;
    int szGlobalWorkStep = m_szGlobalWorkStep;

    int szPerThread = szMatrix/szGlobalWorkStep;


    #pragma offload target(gfx) if (do_offload > 0) \
        pin(inputArray: length(szMatrix)) \
        pin(tmpArray: length(256*szGlobalWorkStep))
    _Cilk_for (int i = 0; i < szGlobalWorkStep; i++) {
        int out[256];
        out[:] = 0;
        for (int m = 0; m < szPerThread; m+=256) {
            int ip[256];
            ip[:] = inputArray[i * szPerThread + m:256];
            #pragma unroll(32)
            for (int j = 0; j < 256; j++) {
                ++out[ip[j]];
            }
        }
        tmpArray[i*256:256] = out[:];
    }

    m_outputArray[0:256] = 0;
    for (int i = 0; i < szGlobalWorkStep; i++) {
        m_outputArray[0:256] += tmpArray[i*256:256];
    }
    return true;
}

bool HistogramLocals4::execute_offload(int do_offload)
{
    unsigned char * inputArray = (unsigned char *)m_inputArray;
    unsigned int* tmpArray = m_tmpArray;
    unsigned int* outputArray = m_outputArray;
    int szMatrix = m_szMatrix;
    int szGlobalWorkStep = m_szGlobalWorkStep;

    int szPerThread = szMatrix/szGlobalWorkStep;

    m_tmpArray[0:m_szGlobalWorkStep * 1024] = 0;

    #pragma offload target(gfx) if (do_offload > 0) \
        pin(inputArray: length(szMatrix)) \
        pin(tmpArray: length(1024*szGlobalWorkStep))
    _Cilk_for (int i = 0; i < szGlobalWorkStep; i++) {
        int out[256];
        #define BS 256
        for (int m = 0; m < szPerThread; m+=1024) {
            unsigned char ip[BS*4];
            short ipc[BS];
            ip[:] = inputArray[i * szPerThread + m:BS*4];
            for (int c = 0; c < 4; c++) {
                ipc[:] = ip[c:BS:4];
                out[:] = tmpArray[i * 1024 + c * 256:256];
                #pragma unroll
                for (unsigned int j = 0; j < 256; j++) {
                    ++out[ipc[j]];
                }
                tmpArray[i * 1024 + c * 256:256] = out[:];
            }
        }
    }

    m_outputArray[0:1024] = 0;
    for (int i = 0; i < szGlobalWorkStep; i++) {
        m_outputArray[0:1024] += tmpArray[i * 1024:1024];
    }
    return true;
}
