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

# GNU Makefile that builds and runs example.
run_cmd=
PROG=dot_product.exe
ARGS=

CXXFLAGS += -D__PSTL_USE_TBB -std=c++11
# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc 

ifneq (, $(filter $(target), mic))
CXXFLAGS += -mmic
else 
CXXFLAGS += -xHOST
endif
CXXFLAGS += -qopenmp-simd
endif # which icc

ifeq ($(shell uname), Linux)
LIBS+= -lrt
else ifeq ($(shell uname), Darwin)
override CXXFLAGS += -Wl,-rpath,$(TBBROOT)/lib
endif

all:    release test

release: *.cpp
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb $(LIBS)

debug: *.cpp
	$(CXX) -O0 -g -DTBB_USE_DEBUG=1 $(CXXFLAGS) -o $(PROG) $^ -ltbb_debug $(LIBS)

clean:
	$(RM) $(PROG) *.o *.d

test:
	$(run_cmd) ./$(PROG) $(ARGS)

perf_build: release

perf_run: test
