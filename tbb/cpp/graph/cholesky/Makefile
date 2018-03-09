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
PROG=cholesky
ARGS=4 2

# The C++ compiler
ifneq (,$(shell which icpc 2>/dev/null))
CXX=icpc
endif # which icpc

ifeq ($(shell uname), Darwin)
override CXXFLAGS += -Wl,-rpath,$(TBBROOT)/lib  -Wl,-rpath,$(MKLROOT)/lib
# MKL support
override MKL_LIBS += -lmkl_core -lmkl_sequential -lmkl_intel
else
ifeq ($(shell uname), Linux)
LIBS+= -lrt 
endif
# MKL support
override MKL_LIBS += -lmkl_rt
endif

# C++0x support
override CXXFLAGS += -std=c++0x

all:	release test

release: *.cpp
	$(CXX) -O2  -DNDEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb $(MKL_LIBS) $(LIBS)

debug: *.cpp
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb_debug $(MKL_LIBS) $(LIBS)

clean:
	$(RM) $(PROG) *.o *.d

test:
	$(run_cmd) ./$(PROG) $(ARGS)
