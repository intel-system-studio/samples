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
NAME=pover
ARGS=
LIGHT_ARGS= --polys 10 --size 5x5

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

include ../../common/gui/Makefile.gmake

ifeq ($(UI),x)
UI_CXXFLAGS += -DX_FULLSYNC
endif # X

override CXXFLAGS += $(UI_CXXFLAGS)

SRCFILES = ../../common/gui/$(UI)video.cpp pover_video.cpp polymain.cpp polyover.cpp

ifeq ($(shell uname), Linux)
LIBS+= -lrt 
endif

all:	release test

release: $(SRCFILES)
ifeq ($(compiler),xl)
	# Avoiding "1586-346 (U) An error occurred during code generation.  The code generation return code was 40." with -O3.
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o $(EXE) $(SRCFILES) -ltbb -ltbbmalloc $(LIBS)
else
	$(CXX) -O3 -DNDEBUG $(CXXFLAGS) -o $(EXE) $(SRCFILES) $(MACUIOBJS) -ltbb -ltbbmalloc $(LIBS)
endif

debug:  $(SRCFILES)
	$(CXX) -g -O0 -DTBB_USE_DEBUG -D_DEBUG $(CXXFLAGS) -o $(EXE) $(SRCFILES) $(MACUIOBJS) -ltbb_debug -ltbbmalloc_debug $(LIBS)

clean:
	$(RM) $(EXE) *.o *.d

test:
	$(run_cmd) ./$(EXE) $(ARGS)

light_test:
	$(run_cmd) ./$(EXE) $(LIGHT_ARGS)
