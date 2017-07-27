# ==============================================================
#
# SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
# http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
#
# Copyright 2016 Intel Corporation
#
# THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
#
# =============================================================

# Common Makefile that builds and runs example.

# Just specify your program basename
PROG=stereo
ARGS=
PERF_RUN_ARGS=

# Trying to find if icl.exe is set
CXX1 = $(TBB_CXX)-
CXX2 = $(CXX1:icl.exe-=icl.exe)
CXX  = $(CXX2:-=cl.exe)

# TBB libs
TBBLIB=tbb.lib
TBBLIB_DEBUG=tbb_debug.lib

# OpenCL lib
OPENCL_LIB=OpenCL.lib

# The C++ compiler options
MYCXXFLAGS=/TP /EHsc /W3 /nologo /D _CONSOLE /D _MBCS /D WIN32 /D _CRT_SECURE_NO_DEPRECATE /D _CRT_SECURE_NO_WARNINGS /D _SCL_SECURE_NO_WARNINGS $(CXXFLAGS)
MYLDFLAGS=/INCREMENTAL:NO /NOLOGO /DEBUG $(LDFLAGS)

all: release test
release:
	$(CXX) *.cpp /MD /O2 /D NDEBUG $(MYCXXFLAGS) /link $(TBBLIB) $(OPENCL_LIB) $(MYLDFLAGS) /OUT:$(PROG).exe
debug:
	$(CXX) *.cpp /MDd /Od /Zi /D TBB_USE_DEBUG /D _DEBUG $(MYCXXFLAGS) /link $(TBBLIB_DEBUG) $(OPENCL_LIB) $(MYLDFLAGS) /OUT:$(PROG).exe
clean:
	@cmd.exe /C del $(PROG).exe *.obj *.?db *.manifest
test:
	$(PROG) $(ARGS)
compiler_check:
	@$(CXX) >nul 2>&1 || echo "$(CXX) command not found. Check if CXX=$(CXX) is set properly"

perf_build: release

perf_run:
	$(PROG) $(PERF_RUN_ARGS)
