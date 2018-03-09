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
NAME=seismic
ARGS=auto 0
PERF_RUN_ARGS=auto 10000 silent
LIGHT_ARGS=1:2 100

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

TBBLIB = -ltbb
TBBLIB_DEBUG = -ltbb_debug

ifeq ($(offload), mic)
override CXXFLAGS += -D__TBB_MIC_OFFLOAD=1 -offload-attribute-target=mic
# Replace -ltbb with -tbb in the offload mode
TBBLIB = -tbb
TBBLIB_DEBUG += -offload-option,mic,ld,"-ltbb_debug"
# Currently only console mode is supported in offload version
override UI=con
else
PERFFLAGS=-msse2
endif

SOURCES = ../../common/gui/$(UI)video.cpp universe.cpp seismic_video.cpp main.cpp

include ../../common/gui/Makefile.gmake
override CXXFLAGS += $(UI_CXXFLAGS)

ifeq ($(shell uname), Linux)
LIBS+= -lrt 
endif

all:	release test

release:
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o $(EXE) $(SOURCES) $(MACUIOBJS) $(TBBLIB) $(LIBS)

debug:
	$(CXX) -g -O0 -DTBB_USE_DEBUG $(CXXFLAGS) -o $(EXE) $(SOURCES) $(MACUIOBJS) $(TBBLIB_DEBUG) $(LIBS)

clean:
	$(RM) $(EXE) *.o *.d

test:
	$(run_cmd) ./$(EXE) $(ARGS)

perf_build: override CXXFLAGS+=$(PERFFLAGS)
perf_build: release

perf_run:
	$(run_cmd) ./$(EXE) $(PERF_RUN_ARGS)

light_test:
	$(run_cmd) ./$(EXE) $(LIGHT_ARGS)
