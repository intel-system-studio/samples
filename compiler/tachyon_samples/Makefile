#//==============================================================
#//
#// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
#// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
#//
#// Copyright (C) Intel Corporation
#//
#// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
#// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
#//
#// =============================================================

# The original source for this example is
# Copyright (c) 1994-2008 John E. Stone
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

# GNU Makefile that builds and runs example.
NAME:=tachyon

# The C++ compiler
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

# The default runtime arguments
export ARGS = dat/balls.dat
export PERF_RUN_ARGS = silent dat/balls3.dat
export LIGHT_ARGS= dat/model2.dat

# define name suffix
SUFFIX = .$(VERSION)

LIBS ?= -lm

include scripts/Makefile.gmake

ifeq ($(UI),x)
UI_CXXFLAGS += -DX_FULLSYNC 
ifneq (,$(findstring -lXext,$(LIBS)))
UI_CXXFLAGS += -DX_NOSHMPIX
endif
endif # X

MYCXXFLAGS = -I src/ $(CXXFLAGS) $(UI_CXXFLAGS) 

all: build run
release: build
debug: build_debug
solution: build_solution run_solution
test: run

build: build_serial build_openmp build_tbb
build_debug: build_serial_debug build_openmp_debug build_tbb_debug
build_solution: build_serial build_openmp_solution build_tbb_solution
run: run_serial run_openmp run_tbb
run_solution: run_serial run_openmp_solution run_tbb_solution

serial: build_serial run_serial
serial_debug: build_serial_debug run_serial
openmp: build_openmp run_openmp
openmp_debug: build_openmp_debug run_openmp
openmp_solution: build_openmp_solution run_openmp_solution
tbb: build_tbb run_tbb
tbb_debug: build_tbb_debug run_tbb
tbb_solution: build_tbb_solution run_tbb_solution

light_test:  build_tbb run_tbb
light_test_one: build_tbb run_tbb


build_serial:
	$(MAKE) VERSION=serial build_one
build_serial_debug:
	$(MAKE) VERSION=serial ADD_DEBUG=1 build_one
run_serial:
	$(MAKE) VERSION=serial run_one

build_tbb:
	$(MAKE) VERSION=tbb ADD_TBB=1 build_one
build_tbb_debug:
	$(MAKE) VERSION=tbb ADD_TBB=1 ADD_DEBUG=1 build_one
build_tbb_solution:
	$(MAKE) VERSION=tbb_solution SVERSION=tbb ADD_TBB=1 build_one
run_tbb:
	$(MAKE) VERSION=tbb run_one
run_tbb_solution:
	$(MAKE) VERSION=tbb_solution run_one

build_openmp:
	$(MAKE) VERSION=openmp ADD_OMP=1 build_one
build_openmp_debug:
	$(MAKE) VERSION=openmp ADD_OMP=1 ADD_DEBUG=1 build_one
build_openmp_solution:
	$(MAKE) VERSION=openmp_solution SVERSION=openmp ADD_OMP=1 build_one
run_openmp:
	$(MAKE) VERSION=openmp run_one
run_openmp_solution:
	$(MAKE) VERSION=openmp_solution run_one

perf_build:
	$(MAKE) UI=con VERSION=tbb ADD_TBB=1 build_one
perf_run:
	$(MAKE) UI=con VERSION=tbb run_perf_one



#
# Per-build Makefile rules (for recursive $(MAKE) calls from above)
#

SVERSION = $(VERSION)

ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -O0 -g -D_DEBUG
else
MYCXXFLAGS += -O2 -DNDEBUG
endif

ifeq ($(ADD_OMP),1)
MYCXXFLAGS += -qopenmp
LIBS += -qopenmp
endif

ifeq ($(ADD_TBB),1)
MYCXXFLAGS += -tbb -std=c++11
ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -DTBB_USE_DEBUG -tbb
LIBS += -ltbb_debug
else
LIBS += -ltbb
endif
endif

#ifeq

ifeq ($(shell uname), Linux)
LIBS += -lrt 
endif

SOURCE = src/common/gui/$(UI)video.cpp src/build_with_$(SVERSION)/build_with_$(VERSION).cpp src/main.cpp src/pthread.cpp src/tachyon_video.cpp src/api.cpp src/apigeom.cpp src/apitrigeom.cpp src/bndbox.cpp src/box.cpp src/camera.cpp src/coordsys.cpp src/cylinder.cpp src/extvol.cpp src/global.cpp src/grid.cpp src/imageio.cpp src/imap.cpp src/intersect.cpp src/jpeg.cpp src/light.cpp src/objbound.cpp src/parse.cpp src/plane.cpp src/ppm.cpp src/quadric.cpp src/render.cpp src/ring.cpp src/shade.cpp src/sphere.cpp src/texture.cpp src/tgafile.cpp src/trace_rest.cpp src/triangle.cpp src/ui.cpp src/util.cpp src/vector.cpp src/vol.cpp

build_one:	$(EXE)

run_one:
	$(run_cmd) ./$(EXE) $(ARGS)

run_perf_one:
	$(run_cmd) ./$(EXE) $(PERF_RUN_ARGS)


$(EXE): $(SOURCE)
	$(CXX) $(MYCXXFLAGS) -o $@ $(SOURCE) $(MACUIOBJS) $(LIBS)
	$(RM) *.o

clean: VERSION = *
clean:
	$(RM) $(EXE) *.o *.d errorfile.txt
