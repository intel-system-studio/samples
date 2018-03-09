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
PROG=convex_hull_bench
ARGS=
PERF_RUN_ARGS = silent auto 40000000
LIGHT_ARGS = 4 400

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

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
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o convex_hull_sample convex_hull_sample.cpp -ltbb $(LIBS)
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o convex_hull_bench convex_hull_bench.cpp -ltbb $(LIBS)

debug: *.cpp
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o convex_hull_sample convex_hull_sample.cpp -ltbb_debug $(LIBS)
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o convex_hull_bench convex_hull_bench.cpp -ltbb_debug $(LIBS)

clean:
	$(RM) convex_hull_bench convex_hull_sample *.o *.d

perf_build: release

perf_run:
	$(run_cmd) ./convex_hull_sample $(PERF_RUN_ARGS)

test:
	$(run_cmd) ./$(PROG) $(ARGS)

light_test:
	$(run_cmd) ./$(PROG) $(LIGHT_ARGS)
