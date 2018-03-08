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
NAME:=tachyon

TBB_FOR_EXAMPLES_INSTALL_DIR=tbbforexamples/linux/4.0.1

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
export DATASET = balls
# The default runtime arguments
export ARGS =

CXXFLAGS += -DDEFAULT_MODELFILE=dat/balls.dat

# define name suffix
SUFFIX = _$(VERSION)

LIBS ?= -lm

include common/gui/Makefile.gmake

ifeq ($(UI),x)
CXXFLAGS += -DX_FULLSYNC 
ifneq (,$(findstring -lXext,$(LIBS)))
CXXFLAGS += -DX_NOSHMPIX
endif
endif # X

MYCXXFLAGS = $(CXXFLAGS)

all: build run
release: build
# debug: build_debug

build: build_find_hotspots build_analyze_locks
run: run_find_hotspots run_analyze_locks


find_hotspots: build_find_hotspots run_find_hotspots
analyze_locks: build_analyze_locks run_analyze_locks

build_find_hotspots: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=find_hotspots ADD_TBB=1 build_one
run_find_hotspots: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=find_hotspots run_one

build_analyze_locks: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=analyze_locks ADD_TBB=1 build_one
run_analyze_locks: $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib
	$(MAKE) VERSION=analyze_locks run_one

$(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib:
	$(TBB_FOR_EXAMPLES_INSTALL_DIR)/set_tbb_symlinks.sh
miniTBBRootDir=$(TBB_FOR_EXAMPLES_INSTALL_DIR)

#
# Per-build Makefile rules (for recursive $(MAKE) calls from above)
#

SVERSION = $(VERSION)

ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -O0 -g -D_DEBUG 
else
MYCXXFLAGS += -O2 -g
endif


ifeq ($(ADD_TBB),1)
MYCXXFLAGS +=  -I $(TBB_FOR_EXAMPLES_INSTALL_DIR)/include
ifeq ($(ADD_DEBUG),1)
MYCXXFLAGS += -DTBB_USE_DEBUG -I $(TBB_FOR_EXAMPLES_INSTALL_DIR)/include
LIBS += -ltbb_debug -L$(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch) -Wl,-rpath $(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch) -lpthread
else
LIBS += -ltbb -L$(TBB_FOR_EXAMPLES_INSTALL_DIR)/lib/$(Arch)  -Wl,-rpath $(miniTBBRootDir)/lib/$(Arch) -lpthread
endif
endif

SOURCE = common/gui/$(UI)video.cpp src/linux/$(SVERSION)/$(SVERSION).cpp src/video.cpp src/api.cpp src/apigeom.cpp src/apitrigeom.cpp src/bndbox.cpp src/box.cpp src/camera.cpp src/coordsys.cpp src/cylinder.cpp src/extvol.cpp src/getargs.cpp src/global.cpp src/grid.cpp src/imageio.cpp src/imap.cpp src/intersect.cpp src/jpeg.cpp src/light.cpp src/objbound.cpp src/parse.cpp src/plane.cpp src/ppm.cpp src/quadric.cpp src/render.cpp src/ring.cpp src/shade.cpp src/sphere.cpp src/texture.cpp src/tgafile.cpp src/trace_rest.cpp src/triangle.cpp src/ui.cpp src/util.cpp src/vector.cpp src/vol.cpp

build_one:	$(EXE)

run_one:
ifeq ($(strip $(have_libX11_3)),)
ifeq ($(strip $(have_libX11_2)),)
ifeq ($(strip $(have_libX11_1)),)
	@ echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	@ echo "Warning: You  don't have library libX11.so in directories  /usr/X11R6/lib$(x64) or /usr/lib$(x64). If you want to see rendered picture:"
	@ echo "Please, install the X11 dev tools. Or if it is installed then make symlink  in /usr/X11R6/lib$(x64)/ or /usr/lib$(x64)/for libX11.so"
	@ echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	@ echo ""
endif
endif
endif
	
ifeq ($(strip $(have_libXext_3)),)
ifeq ($(strip $(have_libXext_2)),)
ifeq ($(strip $(have_libXext_1)),)
	@ echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	@ echo "Warning: You  don't have library libXext.so in directories  /usr/X11R6/lib$(x64) or /usr/lib$(x64). If you want to see rendered picture:"
	@ echo "Please, install the X11 dev tools. Or if it is installed then make symlink  in /usr/X11R6/lib$(x64)/ or /usr/lib$(x64)/for libXext.so"
	@ echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	@ echo ""
endif
endif
endif
	
	./$(EXE) $(ARGS) dat/$(DATASET).dat

$(EXE): $(SOURCE)
	$(CXX) $(MYCXXFLAGS) -o $@ $(SOURCE) $(LIBS)
	$(RM) *.o

clean: VERSION = *
clean:
	$(RM) $(EXE) *.o *.d
