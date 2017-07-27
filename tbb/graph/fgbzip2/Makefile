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
run_cmd=
PROG=fgbzip2
ARGS=-b=9 -a=async_node $(PROG).exe
PERF_RUN_ARGS=-b=9 -a=async_node $(PROG).exe

# Trying to find if icl.exe is set
CXX1 = $(TBB_CXX)-
CXX2 = $(CXX1:icl.exe-=icl.exe)
CXX  = $(CXX2:-=cl.exe)

# The C++ compiler options
MYCXXFLAGS = /EHsc /W3 /nologo /D _CONSOLE /D _MBCS /D WIN32 /D _CRT_SECURE_NO_WARNINGS /D _CRT_NONSTDC_NO_DEPRECATE /wd4267 /wd4244
MYLDFLAGS =/INCREMENTAL:NO /NOLOGO /DEBUG /FIXED:NO $(LDFLAGS)

all: release test

release:
	$(CXX) *.cpp /MD /O2 /Zi /D NDEBUG $(MYCXXFLAGS) /link tbb.lib $(LIBS) $(MYLDFLAGS) /OUT:$(PROG).exe

debug:
	$(CXX) *.cpp /MDd /Od /Zi /D TBB_USE_DEBUG /D _DEBUG $(MYCXXFLAGS) /link tbb_debug.lib $(LIBS) $(MYLDFLAGS) /OUT:$(PROG).exe

clean:
	@cmd.exe /C del $(PROG).exe *.obj *.?db *.manifest *.bz2

test:
	$(PROG) $(ARGS)

perf_build: release

perf_run:
	$(PROG) $(PERF_RUN_ARGS)

