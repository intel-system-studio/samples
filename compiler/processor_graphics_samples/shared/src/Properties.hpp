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


#pragma once
#include <sstream>
#include <string>
#include <vector>


class Properties : public std::vector<std::pair<std::string, std::string> >
{
    typedef std::pair<std::string, std::string> elem;

    std::istringstream is;
    std::ostringstream os;

    bool look (const char* key, std::string*& value);

public:

    bool write (const char* fname) const;
    bool read  (const char* fname);
    bool readln (char* line);

    void put (const char* key, const char* value);
    bool get (const char* key, char*&      value);

    template <typename T>
    void put (const char* key, T  value);

    template <typename T>
    bool get (const char* key, T& value);

    template <typename T>
    bool get (const char* key, T* ptr, int count);
};
