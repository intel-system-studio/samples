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
EXTRA_CFLAGS :=
LIBFLAGS :=

EXENAME=matrix-multiply
RELEASE_BUILDDIR := release
RELEASE_TARGET := $(RELEASE_BUILDDIR)/$(EXENAME)
RCFLAGS := -O2 -ipo

release: $(RELEASE_TARGET)

DEBUG_BUILDDIR := debug
DEBUG_TARGET := $(DEBUG_BUILDDIR)/$(EXENAME)
DCFLAGS := -O0 

debug: $(DEBUG_TARGET)

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
DEBUG_OBJECTS := $(patsubst $(SRCDIR)/%,$(DEBUG_BUILDDIR)/%,$(SOURCES:.cpp=.o))
RELEASE_OBJECTS := $(patsubst $(SRCDIR)/%,$(RELEASE_BUILDDIR)/%,$(SOURCES:.cpp=.o))

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	@echo " Linking..."
	$(CXX) $^ $(LIBFLAGS) -o $(DEBUG_TARGET)

$(DEBUG_BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(DEBUG_BUILDDIR)
	$(CXX) -c $(DCFLAGS) $(EXTRA_CFLAGS) -o $@ $< 

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	@echo " Linking..."
	$(CXX) $^ $(LIBFLAGS) -o $(RELEASE_TARGET)

$(RELEASE_BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(RELEASE_BUILDDIR)
	$(CXX) -c $(RCFLAGS) $(EXTRA_CFLAGS) -o $@ $< 

ifdef debug
RUN_TARGET := $(DEBUG_TARGET)
else
RUN_TARGET := $(RELEASE_TARGET)
endif

run: $(RUN_TARGET)
ifeq ($(shell uname -s),Darwin)
	export  DYLD_LIBRARY_PATH="$(LIBRARY_PATH)";  ./$(RUN_TARGET) $(option)
else
	./$(RUN_TARGET) $(option)
endif

clean:
	@echo " Cleaning..."
	@rm -fr debug release 2>/dev/null || true

.PHONY: clean

