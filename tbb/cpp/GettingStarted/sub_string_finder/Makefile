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
PROG=sub_string_finder_extended
ARGS=
LIGHT_PROG=sub_string_finder

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

TBBLIB = -ltbb
TBBLIB_DEBUG = -ltbb_debug

ifneq (,$(findstring mic, $(offload)))
override CXXFLAGS += -qoffload-arch=$(offload) -qopt-report-phase:offload -D__TBB_MIC_OFFLOAD=1
# Replace -ltbb with -tbb in the offload mode
TBBLIB = -tbb
TBBLIB_DEBUG += -qoffload-option,mic,ld,"-ltbb_debug"
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
ifeq ($(offload),)
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o sub_string_finder sub_string_finder.cpp $(TBBLIB) $(LIBS)
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o sub_string_finder_pretty sub_string_finder_pretty.cpp $(TBBLIB) $(LIBS)
endif
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o sub_string_finder_extended sub_string_finder_extended.cpp $(TBBLIB) $(LIBS)

debug: *.cpp
ifeq ($(offload),)
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o sub_string_finder sub_string_finder.cpp $(TBBLIB_DEBUG) $(LIBS)
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o sub_string_finder_pretty sub_string_finder_pretty.cpp  $(TBBLIB_DEBUG) $(LIBS)
endif
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o sub_string_finder_extended sub_string_finder_extended.cpp $(TBBLIB_DEBUG) $(LIBS)

clean:
	$(RM) sub_string_finder sub_string_finder_extended sub_string_finder_pretty *.o *.d

test:
	$(run_cmd) ./$(PROG) $(ARGS)

light_test:
	$(run_cmd) ./$(LIGHT_PROG) $(ARGS)
