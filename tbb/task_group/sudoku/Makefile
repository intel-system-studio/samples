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
PROG=sudoku
ARGS=4 input1 verbose
PERF_RUN_ARGS=auto input1 silent

# icpc by default
ifneq (,$(shell which icc 2>/dev/null))
    CXX=icpc
endif

# autodetect C++11 support
include ../../common/examples-common.inc

TBBLIB = -ltbb
TBBLIB_DEBUG = -ltbb_debug

ifneq (,$(filter icc icpc,$(CXX)))
ifeq ($(offload), mic)
override CXXFLAGS += -D__TBB_MIC_OFFLOAD=1
# Replace -ltbb with -tbb in the offload mode
TBBLIB = -tbb
TBBLIB_DEBUG += -offload-option,mic,ld,"-ltbb_debug"
endif
endif

ifeq ($(shell uname), Linux)
ifeq ($(target), android)
LIBS+= --sysroot=$(SYSROOT)
run_cmd=../../common/android.linux.launcher.sh
else
LIBS+= -lrt 
endif
else ifeq ($(shell uname), Darwin)
override CXXFLAGS += -Wl,-rpath,$(TBBROOT)/lib
endif

all:	release test

release: *.cpp
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o $(PROG) $^ $(TBBLIB) $(LIBS) $(CXX0XFLAGS)

debug: *.cpp
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o $(PROG) $^ $(TBBLIB_DEBUG) $(LIBS) $(CXX0XFLAGS)

clean:
	$(RM) $(PROG) *.o *.d

test:
	$(run_cmd) ./$(PROG) $(ARGS)

perf_build: release

perf_run:
	$(run_cmd) ./$(PROG) $(PERF_RUN_ARGS)

