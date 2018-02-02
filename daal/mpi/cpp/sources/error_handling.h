/* file: error_handling.h */
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

/*
!  Content:
!    Auxiliary error-handling functions used in C++ samples
!******************************************************************************/


#ifndef _ERROR_HANDLING_H
#define _ERROR_HANDLING_H

const int fileError = -1001;

void checkAllocation(void *ptr)
{
    if (!ptr)
    {
        std::cout << "Error: Memory allocation failed" << std::endl;
        exit(-1);
    }
}

void checkPtr(void *ptr)
{
    if (!ptr)
    {
        std::cout << "Error: NULL pointer" << std::endl;
        exit(-2);
    }
}

void fileOpenError(const char *filename)
{
    std::cout << "Unable to open file '" << filename << "'" << std::endl;
    exit(fileError);
}

void fileReadError()
{
    std::cout << "Unable to read next line" << std::endl;
    exit(fileError);
}

void sparceFileReadError()
{
    std::cout << "Incorrect format of file" << std::endl;
    exit(fileError);
}

#endif
