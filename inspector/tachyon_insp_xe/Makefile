# Copyright 2005-2009 Intel Corporation.  All Rights Reserved.
#
# The source code contained or described herein and all documents related
# to the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material is protected
# by worldwide copyright laws and treaty provisions.  No part of the
# Material may be used, copied, reproduced, modified, published, uploaded,
# posted, transmitted, distributed, or disclosed in any way without
# Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other
# intellectual property right is granted to or conferred upon you by
# disclosure or delivery of the Materials, either expressly, by
# implication, inducement, estoppel or otherwise.  Any license under such
# intellectual property rights must be express and approved by Intel in
# writing.

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

TBB_FOR_EXAMPLES_INSTALL_DIR=tbbforexamples

ifeq ($(shell uname -m),x86_64)
Arch=intel64
CXXFLAGS+= -m64
else
Arch=ia32
CXXFLAGS+= -m32
endif # 

# The C++ compiler
CXX = g++
CXXFLAGS += -Isrc

# The default dataset
export DATASET = simpleballs
# The default runtime arguments
export ARGS =

CXXFLAGS += -DDEFAULT_MODELFILE=dat/simpleballs.dat

# define name suffix
SUFFIX = .$(VERSION)

LIBS ?= -lm

include common/gui/Makefile.gmake

ifeq ($(UI),x)
CXXFLAGS += -DX_FULLSYNC 
ifneq (,$(findstring -lXext,$(LIBS)))
CXXFLAGS += -DX_NOSHMPIX
endif
endif # X

MYCXXFLAGS = $(CXXFLAGS)

all: build_debug
release: build
debug: build_debug

build: build_find_and_fix_memory_errors build_find_and_fix_threading_errors build_create_and_use_suppressions
build_debug: build_find_and_fix_memory_errors_debug build_find_and_fix_threading_errors_debug build_create_and_use_suppressions_debug


find_and_fix_memory_errors: build_find_and_fix_memory_errors
find_and_fix_memory_errors_debug: build_find_and_fix_memory_errors_debug run_find_and_fix_memory_errors_debug
find_and_fix_threading_errors: build_find_and_fix_threading_errors
find_and_fix_threading_errors_debug: build_find_and_fix_threading_errors_debug run_find_and_fix_threading_errors_debug
create_and_use_suppressions: build_create_and_use_suppressions
create_and_use_suppressions_debug: build_create_and_use_suppressions_debug run_create_and_use_suppressions_debug

build_find_and_fix_memory_errors: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=find_and_fix_memory_errors ADD_TBB=1 build_one
build_find_and_fix_memory_errors_debug: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=find_and_fix_memory_errors ADD_TBB=1 ADD_DEBUG=1 build_one
run_find_and_fix_memory_errors_debug: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=find_and_fix_memory_errors run_one

build_find_and_fix_threading_errors: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=find_and_fix_threading_errors ADD_TBB=1 build_one
build_find_and_fix_threading_errors_debug: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=find_and_fix_threading_errors ADD_TBB=1 ADD_DEBUG=1 build_one
run_find_and_fix_threading_errors_debug:
	$(MAKE) VERSION=find_and_fix_threading_errors run_one

build_create_and_use_suppressions: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=create_and_use_suppressions ADD_TBB=1 build_one
build_create_and_use_suppressions_debug: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=create_and_use_suppressions ADD_TBB=1 ADD_DEBUG=1 build_one
run_create_and_use_suppressions_debug:
	$(MAKE) VERSION=create_and_use_suppressions run_one

#
# Per-build Makefile rules (for recursive $(MAKE) calls from above)
#

SVERSION = $(VERSION)

ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -O0 -g -D_DEBUG 
else
MYCXXFLAGS += -O2
endif


ifeq ($(ADD_TBB),1)
MYCXXFLAGS += -I $(TBB_FOR_EXAMPLES_INSTALL_DIR)/include
ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -DTBB_USE_DEBUG 
LIBS += -ltbb_debug 
else
LIBS += -ltbb 
endif
LIBS += -L$(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch)  -Wl,-rpath $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch)
endif

SOURCE = common/gui/$(UI)video.cpp src/$(SVERSION)/$(SVERSION).cpp src/video.cpp src/api.cpp src/apigeom.cpp src/apitrigeom.cpp src/bndbox.cpp src/box.cpp src/camera.cpp src/coordsys.cpp src/cylinder.cpp src/extvol.cpp src/getargs.cpp src/global.cpp src/grid.cpp src/imageio.cpp src/imap.cpp src/intersect.cpp src/jpeg.cpp src/light.cpp src/objbound.cpp src/parse.cpp src/plane.cpp src/ppm.cpp src/quadric.cpp src/render.cpp src/ring.cpp src/shade.cpp src/sphere.cpp src/texture.cpp src/tgafile.cpp src/trace_rest.cpp src/triangle.cpp src/ui.cpp src/util.cpp src/vector.cpp src/vol.cpp

build_one:	$(EXE)

run_one:
	./$(EXE) $(ARGS) dat/$(DATASET).dat

$(EXE): $(SOURCE)
	$(CXX) $(MYCXXFLAGS) -o $@ $(SOURCE) $(LIBS)
	$(RM) *.o

clean: VERSION = *
clean:
	$(RM) $(EXE) *.o *.d
