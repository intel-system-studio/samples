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

#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>

#if _WIN32
#include <io.h>
#ifndef F_OK
#define F_OK 0
#endif
#define access _access
#else
#include <unistd.h>
#endif

const long INPUT_SIZE = 1000000;

//! Generates sample input for square.cpp
void gen_input( const char *fname ) {
    long num = INPUT_SIZE;
    FILE *fptr = fopen(fname, "w");
    if(!fptr) {
        throw std::runtime_error("Could not open file for generating input");
    }

    int a=0;
    int b=1;
    for( long j=0; j<num; ++j ) {
        fprintf(fptr, "%u\n",a);
        b+=a;
        a=(b-a)%10000;
        if (a<0) a=-a;
    }

    if(fptr) {
        fclose(fptr);
    }
}

void generate_if_needed( const char *fname ) {
    if ( access(fname, F_OK) != 0 )
        gen_input(fname);
}
