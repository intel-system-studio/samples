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

LOCAL_PATH := $(realpath $(call my-dir)/..)

include $(CLEAR_VARS)

# Here we give our module name and source file(s)
LOCAL_MODULE    := abi
LOCAL_SRC_FILES := jni/abi.cpp

include $(BUILD_SHARED_LIBRARY)

ifneq (,$(wildcard Android.mk))
  $(error ndk-build should be run one level up from jni folder)
endif

#Relative paths
TBB_PATH := ../tbbforexamples/android
SRC_PATH := ../src

#Absolute paths
TBB_FULL_PATH := $(realpath $(TBB_PATH))
TBB_COMMON_FULL_PATH := $(realpath $(TBB_PATH))/common

#The path is setup for binary package
ifeq (x86,$(TARGET_ARCH_ABI))
TBB_LIBRARY_ARCH_PATH:=
else
TBB_LIBRARY_ARCH_PATH:=/$(TARGET_ARCH_ABI)
endif
#Override if needed
TBB_LIBRARY_FULL_PATH ?= $(TBB_FULL_PATH)/lib/android$(TBB_LIBRARY_ARCH_PATH)

#ifeq (,$(wildcard $(TBB_LIBRARY_FULL_PATH)/libtbb.so))
  #$(info Skipping $(TARGET_ARCH_ABI) target. $(TBB_LIBRARY_FULL_PATH)/libtbb.so library not found. Copy $(TARGET_ARCH_ABI) version of library to $(TBB_LIBRARY_FULL_PATH) folder to enable its build.)
#else

include $(CLEAR_VARS)
LOCAL_MODULE    := jni-engine
LOCAL_SRC_FILES := jni/jni-engine.cpp $(TBB_PATH)/common/gui/convideo.cpp $(SRC_PATH)/trace.tbb.cpp $(SRC_PATH)/pthread.cpp $(SRC_PATH)/tachyon_video.cpp $(SRC_PATH)/api.cpp $(SRC_PATH)/apigeom.cpp $(SRC_PATH)/apitrigeom.cpp $(SRC_PATH)/bndbox.cpp $(SRC_PATH)/box.cpp $(SRC_PATH)/camera.cpp $(SRC_PATH)/coordsys.cpp $(SRC_PATH)/cylinder.cpp $(SRC_PATH)/extvol.cpp $(SRC_PATH)/global.cpp $(SRC_PATH)/grid.cpp $(SRC_PATH)/imageio.cpp $(SRC_PATH)/imap.cpp $(SRC_PATH)/intersect.cpp $(SRC_PATH)/jpeg.cpp $(SRC_PATH)/light.cpp $(SRC_PATH)/objbound.cpp $(SRC_PATH)/parse.cpp $(SRC_PATH)/plane.cpp $(SRC_PATH)/ppm.cpp $(SRC_PATH)/quadric.cpp $(SRC_PATH)/render.cpp $(SRC_PATH)/ring.cpp $(SRC_PATH)/shade.cpp $(SRC_PATH)/sphere.cpp $(SRC_PATH)/texture.cpp $(SRC_PATH)/tgafile.cpp $(SRC_PATH)/trace_rest.cpp $(SRC_PATH)/triangle.cpp $(SRC_PATH)/ui.cpp $(SRC_PATH)/util.cpp $(SRC_PATH)/vector.cpp $(SRC_PATH)/vol.cpp
# Add -DMARK_RENDERING_AREA=1 to see graphical threads work
# Add -DTBB_USE_GCC_BUILTINS to use gcc atomics
LOCAL_CFLAGS += -std=c++11 -fexceptions -Wdeprecated-declarations  -I$(TBB_FULL_PATH)/include -I$(TBB_COMMON_FULL_PATH) -I$(realpath $(SRC_PATH)) 
LOCAL_LDLIBS := -lm -llog -ljnigraphics -L./ -L$(TBB_LIBRARY_FULL_PATH) 
LOCAL_SHARED_LIBRARIES += libtbb 
include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH := $(TBB_LIBRARY_FULL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libtbb
LOCAL_SRC_FILES := libtbb.so
include $(PREBUILT_SHARED_LIBRARY)

#endif
