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
BUILDDIR := release
CFLAGS := -O2 -ipo 
EXTRA_CFLAGS := 
LIBFLAGS := 
LDFLAGS :=
RUNTIME_FLAGS := 

ifdef debug
  BUILDDIR := debug
  CFLAGS := -O0 -g
endif

TARGET := $(BUILDDIR)/matmult_slm 

GEN_ARCH =$(shell gfx_sys_check 2> /dev/null | grep -m1 'GPU architecture:.*'  | sed ' s/^.*://' | tr -d ' ')
GEN_RT   =$(shell gfx_sys_check 2> /dev/null | grep -m1 'vISA support:.*' | sed ' s/^.*://' | tr -d ' ')
GEN_ARCH := $(addprefix $(GEN_ARCH),:$(GEN_RT))

EXTRA_CFLAGS := -MMD -qoffload_arch=$(GEN_ARCH)
LDFLAGS = -lrt $(CFLAGS) -qoffload_arch=$(GEN_ARCH)

ifdef option
  RUNTIME_FLAGS += $(option) 
endif

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))

icpc: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ $(LIBFLAGS) $(LDFLAGS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

ifneq (,$(GEN_RT))
  ifneq (,$(findstring 3.0,$(GEN_RT)))
    run_slm = echo Warning: SLM sample cannot run on current or older version of graphics drivers...
  else
    run_slm = echo "Running $(TARGET) ..."; ./$(TARGET) $(RUNTIME_FLAGS)
  endif
else
  run_slm = echo Processor graphics driver is not installed. Cannot run SLM sample.
endif

run: $(TARGET)
	@$(call run_slm) || true

clean:
	@echo " Cleaning..."
	@rm -fr $(BUILDDIR) $(TARGET) 2>/dev/null || true
	@rm -f *.bmp *.valsig

.PHONY: clean
