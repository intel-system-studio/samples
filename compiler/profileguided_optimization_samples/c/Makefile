#==============================================================
#
# SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
# http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
#
# Copyright Intel Corporation
#
# THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
#
# =============================================================

CXX := icpc
SRCDIR := src
BUILDDIR := release
CFLAGS := -std=c++11
PGOFLAGS := 
EXTRA_CFLAGS := 
LIBFLAGS := 
RUNTIME_FLAGS := 
EXECUTABLE := FluidAnimate
TARGET := $(BUILDDIR)/$(EXECUTABLE)  

ifdef pgo
  PGOFLAGS := -prof-dir ./intel-release -prof-gen -O3 -no-prec-div -static -fp-model fast=2 -xHost
endif

ifdef pgouse
  PGOFLAGS := -prof-dir ./intel-release -prof-use -fast
  TARGET := $(BUILDDIR)/FluidAnimatePGO 
endif

ifdef perf_num
  EXTRA_CFLAGS += -D PERF_NUM 
endif

ifdef option
  RUNTIME_FLAGS += -o $(option) 
endif

ifdef outfile
  RUNTIME_FLAGS += -f $(outfile)
endif

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
SOURCESGCC := $(SRCDIR)/cell_pool.cpp $(SRCDIR)/fluid_animate.cpp $(SRCDIR)/IO.cpp $(SRCDIR)/main.cpp $(SRCDIR)/timer.cpp
OBJECTSGCC := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCESGCC:.cpp=.o))

icpc: $(TARGET)

ifneq ($(shell uname -s),Darwin)
gcc: CXX := g++
else
gcc: CXX := clang++
inde: CXX := icl
endif
gcc: CFLAGS:=-O2 -flto -std=c++0x -Wno-invalid-offsetof -march=native
gcc: TARGETGCC

$(TARGET): $(OBJECTS)
	$(CXX) $^ $(LIBFLAGS) -o $(TARGET)

TARGETGCC: $(OBJECTSGCC)
	$(CXX) $^ $(LIBFLAGSGCC) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@mkdir -p intel-release 
	$(CXX) -c $(CFLAGS) $(PGOFLAGS) $(EXTRA_CFLAGS) -o $@ $<

run: 
	cd $(BUILDDIR); ./$(EXECUTABLE) $(RUNTIME_FLAGS)

clean:
	@echo " Cleaning..."
	@rm -fr $(BUILDDIR) $(TARGET) 2>/dev/null || true
	@rm -r intel-release 2>/dev/null || true
	@rm -f *.bmp *.valsig

.PHONY: clean
