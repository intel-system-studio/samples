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
PROG=square
ARGS=0 input.txt output.txt
PERF_RUN_ARGS=auto input.txt output.txt silent

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

SOURCES = square.cpp gen_input.cpp

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

release: $(SOURCES)
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb $(LIBS)

debug: $(SOURCES)
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb_debug $(LIBS)

clean:
	$(RM) $(PROG) *.o *.d input.txt output.txt

test:
	$(run_cmd) ./$(PROG) $(ARGS)

perf_build: $(SOURCES)
	$(CXX) -O2 -msse2 -DNDEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb $(LIBS)

perf_run:
	$(run_cmd) ./$(PROG) $(PERF_RUN_ARGS)
