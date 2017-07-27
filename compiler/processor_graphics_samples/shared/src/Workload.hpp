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
 * Workload.hpp - the base class for any sample workload
 */

#pragma once
#include "Properties.hpp"
#include <string>

class Workload
{
protected:
public:

    enum ExecMode
    {
        OFFLOADoff,
        OFFLOADon,
        CPU
    };

    Properties* props;
    ExecMode exmode;

    double cpu_share;

    static int registry (const char* (*name)(), Workload* (*create)());

    bool prop_get(const char* key, char*& value)
    {
        return props->get(key, value);
    };
    bool prop_get (const char* key, std::string& value)
    {
        return props->get(key, value);
    };
    bool prop_get (const char* key, int& value)
    {
        return props->get(key, value);
    };
    bool prop_get (const char* key, unsigned int& value)
    {
        return props->get(key, value);
    };
    bool prop_get (const char* key, float&  value)
    {
        return props->get(key, value);
    };
    bool prop_get (const char* key, double& value)
    {
        return props->get(key, value);
    }
    bool prop_get (const char* key, bool& value) {
        return props->get(key, value);
    }

    unsigned get_cpu_share(unsigned n, unsigned alignment = 64)
    {
        if (exmode != OFFLOADon)
            return n;
        unsigned rn = align((unsigned)((double)n * cpu_share), alignment);
        return rn > n ? n : rn;
    }

    unsigned get_gpu_share(unsigned n, unsigned alignment = 64)
    {
        return n - get_cpu_share(n, alignment);
    }

public:

    bool verify;

    virtual ~Workload ()        {};

    bool setup (Properties*);

    virtual bool open ()                          = 0;
    virtual bool execute_offload (int do_offload) = 0;
    virtual bool execute_cpu ()                   = 0;
    virtual bool validate ()                      = 0;
    virtual bool close ()                         = 0;

    bool execute_offload ()
    {
        return execute_offload(exmode == OFFLOADon);
    }

    inline static unsigned align(unsigned v, unsigned alignment)
    {
        return (v + alignment - 1) & ~(alignment - 1);
    }
};

inline void* w_malloc(size_t bytes)
{
    return _mm_malloc(Workload::align(bytes, 4096), 4096);
}

inline void  w_free(void* memory)   {_mm_free(memory);}

inline void* w_malloc_check (size_t bytes)
{
    void* ptr = w_malloc(bytes);
    if (ptr == 0)
        throw "Memory allocation failure";
    return ptr;
}
