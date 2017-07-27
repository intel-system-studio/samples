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
 * CrossFade.cpp
 * This file contains 2 offload implementations of a very simple
 * cross-fade filter
 * 1) operating on byte arrays
 * 2) Operating on int array and extracting bytes using shifts and masking
 * (1) is faster
 */

#include "Workload.hpp"
#include "Util.hpp"
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

class CrossFade : public Workload
{
protected:

    static int id;

    static const char* name ()        {return "crossfade";}
    static Workload* create ()        {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new CrossFade();
    }

public:

    bool open ();
    bool validate ();
    bool close ();

protected:

    int             m_arrayWidth;
    int             m_arrayHeight;
    int             m_arraySize;

    unsigned char * m_inputArray1;
    unsigned char * m_inputArray2;
    unsigned char * m_outputArray;

    unsigned        m_blendFactor;

    bool execute_offload (int offload);
    bool execute_cpu ();
};

int CrossFade::id = Workload::registry(name, create);

class CrossFade_ints : public CrossFade
{
protected:
    static int id;

    static const char* name ()        {return "crossfade_ints";}
    static Workload* create ()        {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new CrossFade_ints();
    }

    bool execute_offload (int offload);
    bool execute_cpu ();
};

int CrossFade_ints::id = Workload::registry(CrossFade_ints::name,
    CrossFade_ints::create);



bool CrossFade::open ()
{
    prop_get("width",  m_arrayWidth  = 4096);
    prop_get("height", m_arrayHeight = m_arrayWidth);
    prop_get("blendFactor", m_blendFactor = 128);

    m_arraySize = m_arrayWidth * m_arrayHeight * 4;

    m_inputArray1 = (unsigned char*)w_malloc_check(m_arraySize);
    m_inputArray2 = (unsigned char*)w_malloc_check(m_arraySize);
    m_outputArray = (unsigned char*)w_malloc_check(m_arraySize);

    srand(12345);
    for (int i = 0; i< m_arraySize; i++) {
        m_inputArray1[i] = rand();
        m_inputArray2[i] = rand();
    }
    m_outputArray[0:m_arraySize] = 0;

    return true;
}

bool CrossFade::close()
{
    w_free(m_inputArray1);
    w_free(m_inputArray2);
    w_free(m_outputArray);
    return true;
}


bool CrossFade::validate()
{
    unsigned char* validationOutputArray =
        (unsigned char*)w_malloc_check(m_arraySize);
    memset(validationOutputArray, 0, m_arraySize);

    unsigned a1 = 256 - m_blendFactor;
    unsigned a2 = m_blendFactor;

    for (int i=0; i<m_arrayHeight; i++) {
        for (int j=0; j<m_arrayWidth*4; j++) {
            int idx = i*m_arrayWidth*4 + j;
            validationOutputArray[idx] = (m_inputArray1[idx] * a1 +
                m_inputArray2[idx] * a2) >> 8;
        }
    }

    int errorCount = 0, maxErrorCount = 50;
    for (int z = 0; z < m_arraySize; z++) {
        unsigned char REF = validationOutputArray[z];
        unsigned char RES = m_outputArray[z];
        if (REF != RES) {
            if (errorCount < maxErrorCount) {
                cout << "z = " << z <<", REF = " << (unsigned)REF
                     << ", RES = " << (unsigned)RES
                     << ", input1 = " << (unsigned)m_inputArray1[z]
                     << ", input2 = " << (unsigned)m_inputArray2[z]
                     <<endl;
            }
            errorCount++;
        }
    }
    if (errorCount > 0) {
        cout << "total errors: " << errorCount <<endl;
    }

    w_free(validationOutputArray);

    return errorCount == 0;
}

/********************************************************************
 * Code for offload (all versions)
 */

bool CrossFade::execute_offload (int do_offload)
{
    unsigned char * inputArray1 = m_inputArray1,
                  * inputArray2 = m_inputArray2,
                  * outputArray = m_outputArray;
    int arrayWidth = m_arrayWidth, arrayHeight = m_arrayHeight, arraySize =
        m_arraySize;

    unsigned a1 = 256 - m_blendFactor;
    unsigned a2 = m_blendFactor;

    #pragma offload target(gfx)if (do_offload) pin(inputArray1, inputArray2, \
        outputArray: length(arraySize))
    _Cilk_for (int i=0; i<arraySize; i++) {
        outputArray[i] = (inputArray1[i] * a1 + inputArray2[i] * a2) >> 8;
    }

    return true;
}

bool CrossFade_ints::execute_offload (int do_offload)
{
    unsigned * inputArray1 = (unsigned *)m_inputArray1,
             * inputArray2 = (unsigned *)m_inputArray2,
             * outputArray = (unsigned *)m_outputArray;
    int arrayWidth = m_arrayWidth, arrayHeight = m_arrayHeight, arraySize =
        m_arraySize/4;

    unsigned a1 = 256 - m_blendFactor;
    unsigned a2 = m_blendFactor;

    #pragma offload target(gfx)if (do_offload) pin(inputArray1, inputArray2, \
        outputArray: length(arraySize))
    _Cilk_for (int idx=0; idx < arraySize; idx++) {
        unsigned dw1 = inputArray1[idx], dw2 = inputArray2[idx];
        unsigned ch0 = ((dw1 & 0xff) * a1 + (dw2 & 0xff) * a2) >> 8;
        unsigned ch1 = (((dw1 >>  8) & 0xff) * a1 +
            ((dw2 >>  8) & 0xff) * a2) >> 8;
        unsigned ch2 = (((dw1 >> 16) & 0xff) * a1 +
            ((dw2 >> 16) & 0xff) * a2) >> 8;
        unsigned ch3 = (((dw1 >> 24)) * a1 +
            ((dw2 >> 24)) * a2) >> 8;
        outputArray[idx] = (ch0 & 0xff) | ( (ch1 & 0xff) << 8) |
            ( (ch2 & 0xff) << 16) | ( (ch3 & 0xff) << 24);
    }

    return true;
}


/********************************************************************
 * Code for cpu (all versions)
 */

bool CrossFade::execute_cpu ()
{
    unsigned char * inputArray1 = m_inputArray1,
                  * inputArray2 = m_inputArray2,
                  * outputArray = m_outputArray;
    int arrayWidth = m_arrayWidth, arrayHeight = m_arrayHeight,
        arraySize = m_arraySize;

    unsigned a1 = 256 - m_blendFactor;
    unsigned a2 = m_blendFactor;

    _Cilk_for (int i=0; i<arraySize; i++) {
        outputArray[i] = (inputArray1[i] * a1 + inputArray2[i] * a2) >> 8;
    }

    return true;
}




bool CrossFade_ints::execute_cpu ()
{
    unsigned * inputArray1 = (unsigned *)m_inputArray1,
             * inputArray2 = (unsigned *)m_inputArray2,
             * outputArray = (unsigned *)m_outputArray;
    int arrayWidth = m_arrayWidth, arrayHeight = m_arrayHeight,
        arraySize = m_arraySize/4;

    unsigned a1 = 256 - m_blendFactor;
    unsigned a2 = m_blendFactor;

    _Cilk_for (int idx=0; idx < arraySize; idx++) {
        unsigned dw1 = inputArray1[idx], dw2 = inputArray2[idx];
        unsigned ch0 = ((dw1 & 0xff) * a1 + (dw2 & 0xff) * a2) >> 8;
        unsigned ch1 = (((dw1 >>  8) & 0xff) * a1 +
            ((dw2 >>  8) & 0xff) * a2) >> 8;
        unsigned ch2 = (((dw1 >> 16) & 0xff) * a1 +
            ((dw2 >> 16) & 0xff) * a2) >> 8;
        unsigned ch3 = (((dw1 >> 24)) * a1 +
            ((dw2 >> 24)) * a2) >> 8;
        outputArray[idx] = (ch0 & 0xff) | ( (ch1 & 0xff) << 8) |
            ( (ch2 & 0xff) << 16) | ( (ch3 & 0xff) << 24);
    }

    return true;
}

