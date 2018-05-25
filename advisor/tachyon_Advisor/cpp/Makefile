#=======================================================================
#
# /* Copyright (C) 2010-2017 Intel Corporation. All Rights Reserved.
 #
 # The source code, information and material ("Material") 
 # contained herein is owned by Intel Corporation or its 
 # suppliers or licensors, and title to such Material remains 
 # with Intel Corporation or its suppliers or licensors.
 # The Material contains proprietary information of Intel or 
 # its suppliers and licensors. The Material is protected by 
 # worldwide copyright laws and treaty provisions.
 # No part of the Material may be used, copied, reproduced, 
 # modified, published, uploaded, posted, transmitted, distributed 
 # or disclosed in any way without Intel's prior express written 
 # permission. No license under any patent, copyright or other
 # intellectual property rights in the Material is granted to or 
 # conferred upon you, either expressly, by implication, inducement, 
 # estoppel or otherwise. Any license under such intellectual 
 # property rights must be express and approved by Intel in writing.
 # Third Party trademarks are the property of their respective owners.
 # Unless otherwise agreed by Intel in writing, you may not remove 
 # or alter this notice or any other notice embedded in Materials 
 # by Intel or Intel's suppliers or licensors in any way.
 
# ========================================================================

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
#NAME=tachyon
NAME = $(VERSION)
TBB_FOR_EXAMPLES_INSTALL_DIR=tbbforexamples
ADV_DIR = $(ADVISOR_2018_DIR)

ifeq ($(shell uname -m),x86_64)
Arch=intel64
CXXFLAGS+= -m64
else
Arch=ia32
CXXFLAGS+= -m32
endif # 

# The C++ compiler
CXX = g++
#CXX = icpc
CXXFLAGS += -Isrc

# The default dataset
#export DATASET = simpleballs
# The default runtime arguments
#export ARGS =


# define name suffix

ifeq ($(ADD_DEBUG),1)
SUFFIX = _debug
CXXFLAGS += -DDEFAULT_MODELFILE=dat/simpleballs.dat
#SUFFIX = .$(VERSION)_debug.out
#SUFFIX = $(VERSION)_debug
#NAME = $(VERSION)_debug
else
SUFFIX =
CXXFLAGS += -DDEFAULT_MODELFILE=dat/balls.dat
#SUFFIX = $(VERSION).out
#SUFFIX = .$(VERSION).out
#NAME = $(VERSION)
endif
#SUFFIX = .out

LIBS ?= -lm -ldl

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

build: build_1_tachyon_serial build_2_tachyon_annotated build_3_tachyon_tbb build_4_tachyon_cilk build_5_tachyon_omp
build_debug: build_1_tachyon_serial_debug build_2_tachyon_annotated_debug build_3_tachyon_tbb_debug build_4_tachyon_cilk_debug build_5_tachyon_omp_debug

1_tachyon_serial: build_1_tachyon_serial
1_tachyon_serial_debug: build_1_tachyon_serial_debug
2_tachyon_annotated: build_2_tachyon_annotated
2_tachyon_annotated_debug: build_2_tachyon_annotated_debug
3_tachyon_tbb: build_3_tachyon_tbb
3_tachyon_tbb_debug: build_3_tachyon_tbb_debug
4_tachyon_cilk: build_4_tachyon_cilk
4_tachyon_cilk_debug: build_4_tachyon_cilk_debug
5_tachyon_omp: build_5_tachyon_omp
5_tachyon_omp_debug: build_5_tachyon_omp_debug

build_1_tachyon_serial:
	$(MAKE) VERSION=1_tachyon_serial build_one
build_1_tachyon_serial_debug:
	$(MAKE) VERSION=1_tachyon_serial ADD_DEBUG=1 build_one
build_2_tachyon_annotated:
ifeq ($(ADV_DIR),)
	@echo ADVISOR_2018_DIR environment variable not defined. Required to build annotated sample.
else	
	$(MAKE) VERSION=2_tachyon_annotated ANNOTATE=1 build_one
endif
build_2_tachyon_annotated_debug:
ifeq ($(ADV_DIR),)
	@echo ADVISOR_2018_DIR environment variable not defined. Required to build annotated sample.
else	
	$(MAKE) VERSION=2_tachyon_annotated ANNOTATE=1 ADD_DEBUG=1 build_one
endif
build_3_tachyon_tbb: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=3_tachyon_tbb ADD_TBB=1 build_one
build_3_tachyon_tbb_debug: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=3_tachyon_tbb ADD_TBB=1 ADD_DEBUG=1 build_one
build_4_tachyon_cilk: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=4_tachyon_cilk ADD_CILK=1 ADD_TBB=1 CXX=icpc build_one
build_4_tachyon_cilk_debug: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=4_tachyon_cilk ADD_CILK=1 ADD_DEBUG=1 ADD_TBB=1 CXX=icpc build_one
build_5_tachyon_omp:
	$(MAKE) VERSION=5_tachyon_omp ADD_OMP=1 CXX=icpc build_one
build_5_tachyon_omp_debug:
	$(MAKE) VERSION=5_tachyon_omp ADD_DEBUG=1 ADD_OMP=1 CXX=icpc build_one


$(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib:
	$(TBB_FOR_EXAMPLES_INSTALL_DIR)/set_tbb_symlinks.sh

#
# Per-build Makefile rules (for recursive $(MAKE) calls from above)
#

SVERSION = $(VERSION)

ifeq ($(ANNOTATE),1)
MYCXXFLAGS += -I $(ADV_DIR)/include/
endif

ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -O0 -g -D_DEBUG 
else
MYCXXFLAGS += -O2 -g -fno-inline
endif



ifeq ($(ADD_TBB),1)
MYCXXFLAGS += -I $(TBB_FOR_EXAMPLES_INSTALL_DIR)/include
ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -DTBB_USE_DEBUG -I $(TBB_FOR_EXAMPLES_INSTALL_DIR)/include
LIBS += -ltbb_debug -L$(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch) -Wl,-rpath $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch)
else
LIBS += -ltbb -L$(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch)  -Wl,-rpath $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch)
endif
endif

ifeq ($(ADD_CILK),1)
LIBS += -lcilkrts
endif

ifeq ($(ADD_OMP),1)
LIBS += -liomp5 -L $(LIBRARY_PATH)
endif

SOURCE = common/gui/$(UI)video.cpp src/$(SVERSION)/$(SVERSION).cpp src/video.cpp src/api.cpp src/apigeom.cpp src/apitrigeom.cpp src/bndbox.cpp src/box.cpp src/camera.cpp src/coordsys.cpp src/cylinder.cpp src/extvol.cpp src/getargs.cpp src/global.cpp src/grid.cpp src/imageio.cpp src/imap.cpp src/intersect.cpp src/jpeg.cpp src/light.cpp src/objbound.cpp src/parse.cpp src/plane.cpp src/ppm.cpp src/quadric.cpp src/render.cpp src/ring.cpp src/shade.cpp src/sphere.cpp src/texture.cpp src/tgafile.cpp src/trace_rest.cpp src/triangle.cpp src/ui.cpp src/util.cpp src/vector.cpp src/vol.cpp

#TEMPo_FILES = $(SOURCE:.cpp=.o)

build_one: $(EXE).pseudo

run_one:
ifeq ($(UI),mac)
	export DYLD_LIBRARY_PATH="$(DYLD_LIBRARY_PATH):$(TBBLIBSPATH)"; ./$(EXE) $(ARGS) dat/$(DATASET).dat
else
	./$(EXE) $(ARGS) dat/$(DATASET).dat
endif

$(EXE).pseudo: $(SOURCE)
ifeq ($(UI),mac)
	mkdir -p $(APPRES)/English.lproj $(NAME).$(VERSION).app/Contents/MacOS
	$(PBXCP) xcode/English.lproj/main.nib $(APPRES)/English.lproj
	$(PBXCP) xcode/Info.plist $(APPRES)
endif
ifeq ($(ADD_OMP),1)
	$(CXX) -qopenmp $(MYCXXFLAGS) $(SOURCE) -c
	$(CXX) *.o -o $(EXE) $(LIBS)
	$(RM) *.o

else
	$(CXX) $(MYCXXFLAGS) -o $(EXE) $(SOURCE) $(LIBS)
	$(RM) *.o
endif

#clean: VERSION = *

EXECUTABLES =  1_tachyon_serial 2_tachyon_annotated 3_tachyon_tbb 4_tachyon_cilk 5_tachyon_omp 1_tachyon_serial_debug 2_tachyon_annotated_debug 3_tachyon_tbb_debug 4_tachyon_cilk_debug 5_tachyon_omp_debug
clean:
	$(RM) $(EXECUTABLES) *.o *.d
ifeq ($(UI),mac)
	rm -rf $(NAME).*
endif
