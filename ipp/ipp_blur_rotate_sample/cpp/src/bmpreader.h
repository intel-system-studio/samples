/*
//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2005-2018 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================
*/

#ifndef BMPREADER_H_
#define BMPREADER_H_

#include "ippi.h"

enum ReturnStatus
{
    STS_ERR_FAILED                = -999,
    STS_ERR_NOT_INITIALIZED       = -998,
    STS_ERR_NOT_ENOUGH_DATA       = -996,
    STS_ERR_NULL_PTR              = -995,
    STS_ERR_INIT                  = -899,
    STS_ERR_END_OF_STREAM         = -895,
    STS_ERR_ALLOC                 = -883,
    STS_ERR_UNSUPPORTED           = -879,
    STS_ERR_INVALID_PARAMS        = -876,
    STS_ERR_FILE_OPEN             = -875,
    STS_ERR_FORMAT                = -874,
    STS_OK                        =  0,
};

typedef int Status;

Status ReadFile(const char *pFileName, Ipp8u **pImg, IppiSize &imgSize, int &imgStep, int &numChannels);

#define MAX_WIDTH  4096
#define MAX_HEIGHT 2160

#endif /* BMPREADER_H_ */
