# ==============================================================
#
# SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
# http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
#
# Copyright (C) Intel Corporation
#
# THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
#
# =============================================================

# detect if a compiler can support C++11
# If CXX0XFLAGS already set, do not change it
ifneq (,$(findstring $(CXX), icc icpc clang++))
    # every supported icc or clang is OK
    CXX0XFLAGS ?= -std=c++0x
else
    ifneq (,$(findstring $(CXX), g++ gcc))
        ifneq (, $(strip $(shell $(CXX) -v 2>&1 | grep "clang-")))
            # This is clang actually,
            # every supported clang is OK
            CXX0XFLAGS ?= -std=c++0x
        else
            # support of lambda started GCC 4.5
            ifneq (, $(strip $(shell g++ -dumpversion | egrep "^(4\.[5-9]|[5-9])")))
               CXX0XFLAGS ?= -std=c++0x
            endif
        endif
    endif
endif

