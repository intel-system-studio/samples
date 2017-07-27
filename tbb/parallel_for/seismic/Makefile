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
PROG=Seismic
ARGS=auto 0
PERF_RUN_ARGS=auto 10000 silent
LIGHT_ARGS=1:2 100

# Trying to find if icl.exe is set
CXX1 = $(TBB_CXX)-
CXX2 = $(CXX1:icl.exe-=icl.exe)
CXX  = $(CXX2:-=cl.exe)

# Uncomment one of next lines to choose user interface type (console, gdiplus, direct draw)
#UI = con
UI = gdi
#UI = dd

# Machine architecture, auto-detected from TBB_TARGET_ARCH by default
# Use XARCH variable to change it. See index.html for more information
ARCH0 = $(TBB_TARGET_ARCH)-
ARCH1 = $(ARCH0:ia32-=x86)
ARCH2 = $(ARCH1:intel64-=AMD64)
XARCH = $(ARCH2:-=x86)

MAKEINC = ../../common/gui/Makefile.win
SOURCES = seismic_video.cpp universe.cpp main.cpp 
 
all: release test
release: compiler_check
	@$(MAKE) -f $(MAKEINC) UI=$(UI) CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" LFLAGS="$(LDFLAGS) tbb.lib $(LIBS)" XARCH=$(XARCH) RCNAME=SeismicSimulation SOURCE="$(SOURCES)" EXE=$(PROG).exe build_one
debug: compiler_check
	@$(MAKE) -f $(MAKEINC) UI=$(UI) DEBUG=_debug CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS) /D TBB_USE_DEBUG" LFLAGS="$(LDFLAGS) tbb_debug.lib $(LIBS)" XARCH=$(XARCH) RCNAME=SeismicSimulation SOURCE="$(SOURCES)" EXE=$(PROG).exe build_one
clean:
	@cmd.exe /C del $(PROG).exe *.obj *.?db *.manifest msvs\SeismicSimulation.res >nul 2>&1
test:
	$(PROG) $(ARGS) 

perf_build: compiler_check
	@$(MAKE) -f $(MAKEINC) UI=con CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" LFLAGS="$(LDFLAGS) tbb.lib $(LIBS)" XARCH=$(XARCH) RCNAME=SeismicSimulation SOURCE="$(SOURCES) " EXE=$(PROG).exe build_one
perf_run:
	$(PROG) $(PERF_RUN_ARGS)

light_test:
	$(PROG) $(LIGHT_ARGS)

compiler_check:
	@echo compiler_test>compiler_test && @$(CXX) /E compiler_test >nul 2>&1  || echo "$(CXX) command not found. Check if CXX=$(CXX) is set properly"
	@cmd.exe /C del compiler_test
