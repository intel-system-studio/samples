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
PROG=fgbzip2
ARGS=-b=9 -a=async_node $(PROG)
PERF_RUN_ARGS=-b=9 -a=async_node $(PROG)

# icpc by default
ifneq (,$(shell which icc 2>/dev/null))
    CXX=icpc
endif

# autodetect C++11 support
include ../../common/examples-common.inc

ifeq ($(shell uname), Linux)
LIBS+= -lrt -lpthread
else ifeq ($(shell uname), Darwin)
override CXXFLAGS += -Wl,-rpath,$(TBBROOT)/lib
endif

all: release test

release:
	$(CXX) -O2 -DNDEBUG *.cpp $(CXXFLAGS) -o $(PROG) -ltbb $(LIBS) $(CXX0XFLAGS)

debug:
	$(CXX) -O0 -g -DTBB_USE_DEBUG *.cpp $(CXXFLAGS) -o $(PROG) -ltbb_debug $(LIBS)  $(CXX0XFLAGS)

clean:
	$(RM) $(PROG) *.o *.d *.bz2

test:
	$(run_cmd) ./$(PROG) $(ARGS)

perf_build: release

perf_run:
	$(run_cmd) ./$(PROG) $(PERF_RUN_ARGS)
