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
NAME=Fractal
ARGS=auto
PERF_RUN_ARGS=auto 1 1000000 silent
LIGHT_ARGS=auto 1 1000

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

ifeq ($(shell uname), Linux)
LIBS+= -lrt 
endif

include ../../common/gui/Makefile.gmake

SOURCES= ../../common/gui/$(UI)video.cpp fractal.cpp main.cpp

override CXXFLAGS += $(UI_CXXFLAGS)

all:	release test

release: $(SOURCES)
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o $(EXE) $(SOURCES) $(MACUIOBJS) -ltbb $(LIBS)

debug:
	$(CXX) -g -O0 -DTBB_USE_DEBUG $(CXXFLAGS) -o $(EXE) $(SOURCES) $(MACUIOBJS) -ltbb_debug $(LIBS)

clean:
	$(RM) $(EXE) *.o *.d

test:
	$(run_cmd) ./$(EXE) $(ARGS)

light_test:
	$(run_cmd) ./$(EXE) $(LIGHT_ARGS)

perf_build: release

perf_run:
	$(run_cmd) ./$(EXE) $(PERF_RUN_ARGS)
