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
 Contains several kernels illustrating use of GFXVector4<T> and GFXVector3<T>
 utility class enabling operating on 3 or 4 component structures in a compact
 way using overloaded operators (similarly to OpenCL float4 type).

 Also illustrates a simple technique to share a task between CPU and GPU.
 */

#include <mathimf.h>
#include "Workload.hpp"
#include "GFXVector.hpp"
#include "Util.hpp"
#include <iostream>

using namespace std;

template <class T>
class TestGFXVector : public Workload
{
protected:

    static int id;

    static const char* name ();
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
        return new TestGFXVector();
    }

    /*virtual*/ bool open ();
    /*virtual*/ bool execute_offload (int do_offload);
    /*virtual*/ bool execute_cpu () { return false; }
    /*virtual*/ bool validate ();
    /*virtual*/ bool close ();

    virtual void execute_part(int do_offload, int offset, int size);

    virtual void execute_serial (GFXVector4<T>* out);

    int m_size, m_tmpSize;
    GFXVector4<T> * m_inputArray1, * m_inputArray2, * m_tmpArray,
        * m_outputArray;

    bool error(T diff)const { return diff != T(0); }
    bool error(float diff) { return !(diff < 0.01f); }
};


const char* TestGFXVector<float>::name ()        {return "testgfxvector_f";}
int TestGFXVector<float>::id = Workload::registry(TestGFXVector<float>::name,
    TestGFXVector<float>::create);


template <class T>
bool TestGFXVector<T>::open ()
{
    prop_get("size", m_size = 1024*1024*8);

    int padding = 0;


    m_inputArray1 = (GFXVector4<T>*)w_malloc_check(m_size *
        sizeof(GFXVector4<T>));
    m_inputArray2 = (GFXVector4<T>*)w_malloc_check(m_size *
        sizeof(GFXVector4<T>));
    m_tmpArray    = (GFXVector4<T>*)w_malloc_check(m_size *
        sizeof(GFXVector4<T>));
    m_outputArray = (GFXVector4<T>*)w_malloc_check(m_size *
        sizeof(GFXVector4<T>));

    for (int i = 0; i < m_size; i++) {

        GFXVector4<T>(T(i * 37 * 41 % 512));
        m_inputArray1[i] = GFXVector4<T>(T((i * 37 * 41) % 512)) +
            GFXVector4<T>(T(1), T(2), T(3), T(4));
        m_inputArray2[i] = GFXVector4<T>(T((i * 37 * 41 * 11) % 512)) +
            GFXVector4<T>(T(7), T(11), T(13), T(17));
    }

    m_outputArray[0:m_size] = 0;
    m_tmpArray[0:m_size] = 0;
    return true;
}


template <class T>
bool TestGFXVector<T>::close ()
{
    w_free(m_inputArray1);
    w_free(m_inputArray2);
    w_free(m_tmpArray);
    w_free(m_outputArray);
    return true;
}

template <class T>
bool TestGFXVector<T>::validate ()
{
    GFXVector4<T> * referenceArray = (GFXVector4<T>*)w_malloc_check(m_size *
        sizeof(GFXVector4<T>));

    execute_serial(referenceArray);

    int errorCount = 0, maxErrorCount = 30;
    for (int i = 0; i < m_size; i++) {

        if (error(abs(referenceArray[i] - m_outputArray[i]).sum())) {
            errorCount++;
            if (errorCount <= maxErrorCount) {
                cout.precision(10);
                cout << "ERROR: i = " << i
                     << ", Result: ("
                     << m_outputArray[i].x << ", "
                     << m_outputArray[i].y << ", "
                     << m_outputArray[i].z << ", "
                     << m_outputArray[i].w << ") "
                     << ", Reference: ("
                     << referenceArray[i].x << ", "
                     << referenceArray[i].y << ", "
                     << referenceArray[i].z << ", "
                     << referenceArray[i].w << ") "
                     << endl;
            }
        }
    }

    cout << "Total errors: " << errorCount << endl;

    w_free(referenceArray);

    return errorCount == 0;
}



template <class T>
bool TestGFXVector<T>::execute_offload(int do_offload)
{
    int cpuShare = get_cpu_share(m_size);
    if (cpuShare > 0 && cpuShare < m_size) {
        _Cilk_spawn execute_part(0, m_size - cpuShare, cpuShare);
        execute_part(1, 0, m_size - cpuShare);
    }
    else {
        execute_part(do_offload, 0, m_size);
    }
    return true;
}

template <class T>
void TestGFXVector<T>::execute_serial (GFXVector4<T>* out)
{
    int size = m_size;
    GFXVector4<T> * in1 = m_inputArray1, * in2 = m_inputArray2, * tmp =
        m_tmpArray;

    for (int i = 0; i < m_size; i++) {
        tmp[i] = T(2) * in1[i] + T(3) * in2[i] / max(in2[i] - in1[i], T(1));
    }

    for (int i = 0; i < m_size; i++) {
        GFXVector4<T> sum = 0;
        for (int j = -5; j < 5; j++) {
            if (i + j >= 0 && i + j < size) {
                sum += tmp[i + j];
            }
        }
        *(GFXVector3<T>*)&sum /= max(GFXVector3<T>(sum.a),
            GFXVector3<T>((T)1));
        out[i] = sum;
    }
}


template <class T>
void TestGFXVector<T>::execute_part(int do_offload, int off, int n)
{
    GFXVector4<T> * in1 = m_inputArray1, * in2 = m_inputArray2,
        * tmp = m_tmpArray, * out = m_outputArray;
    int size = m_size;

    #pragma offload target(gfx) if (do_offload)\
        pin(in1, in2, tmp: length(size))
    #pragma simd
    _Cilk_for (int i = off; i < off + n; i++) {
        GFXVector4<T> t1 = in1[i], t2 = in2[i];
        tmp[i] = 2 * t1 + 3 * t2 / max(t2 - t1, (T)1);
    }

    #pragma offload target(gfx) if (do_offload) pin(tmp, out: length(size))
    #pragma simd
    _Cilk_for (int i = off; i < off + n; i++) {
        GFXVector4<T> sum = 0;
        for (int j = -5; j < 5; j++) {
            if (i + j >= 0 && i + j < size) {
                sum += tmp[i + j];
            }
        }
        *(GFXVector3<T>*)&sum /= max(GFXVector3<T>(sum.a),
            GFXVector3<T>((T)1));
        out[i] = sum;
    }
}
