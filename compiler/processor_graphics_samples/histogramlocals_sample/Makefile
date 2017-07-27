#==============================================================
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

CXX := icpc
SRCDIR := src
SRCDIR_SHARED=../shared/src
BUILDDIR := release
EXTRA_INCLUDE := -I $(SRCDIR_SHARED)
CFLAGS := -O2 -ipo
EXTRA_CFLAGS := 
LIBFLAGS := 
LDFLAGS :=
RUNTIME_FLAGS := 

ifdef debug
  BUILDDIR := debug
  CFLAGS := -O0 -g
endif

TARGET := $(BUILDDIR)/histogramlocals 

GEN_ARCH =$(shell gfx_sys_check 2> /dev/null | grep -m1 'GPU architecture:.*'  | sed ' s/^.*://' | tr -d ' ')
GEN_RT   =$(shell gfx_sys_check 2> /dev/null | grep -m1 'vISA support:.*' | sed ' s/^.*://' | tr -d ' ')
GEN_ARCH := $(addprefix $(GEN_ARCH),:$(GEN_RT))

EXTRA_CFLAGS := -MMD -qoffload_arch=$(GEN_ARCH)
LDFLAGS = -lrt $(CFLAGS) -qoffload_arch=$(GEN_ARCH)

ifdef option
  RUNTIME_FLAGS += $(option) 
endif

SOURCES_SHARED := $(wildcard $(SRCDIR_SHARED)/*.cpp)
OBJECTS_SHARED := $(patsubst $(SRCDIR_SHARED)/%,$(BUILDDIR)/%,$(SOURCES_SHARED:.cpp=.o))
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))

icpc: $(TARGET)

$(TARGET): $(OBJECTS_SHARED) $(OBJECTS)
	$(CXX) $^ $(LIBFLAGS) $(LDFLAGS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR_SHARED)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) -c $(CFLAGS) $(EXTRA_INCLUDE) $(EXTRA_CFLAGS) -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) -c $(CFLAGS) $(EXTRA_INCLUDE) $(EXTRA_CFLAGS) -o $@ $<

run: $(TARGET)
	cp $(SRCDIR)/*.prop $(BUILDDIR)
	./$(TARGET) $(RUNTIME_FLAGS)

clean:
	@echo " Cleaning..."
	@rm -fr $(BUILDDIR) $(TARGET) 2>/dev/null || true
	@rm -f *.bmp *.valsig

.PHONY: clean
