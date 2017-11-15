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
#
# To compile with the GNU* C/C++ compiler, creating executables with the
# extension ".gcc" for binary instrumentation, issue:
#
#   > make
#

SHELL = /bin/sh
srcdir = src

# Set the path where Intel(R) VTune(TM) Amplifier has been installed
VTUNE_AMPLIFIER_DIR = /opt/intel/vtune_amplifier

ifeq ($(shell uname -m),x86_64)
ARCHITECTURE=intel64
CFLAGS +=-m64 
LDFLAGS +=-L$(VTUNE_AMPLIFIER_DIR)/lib64 
else
ARCHITECTURE=x86
CFLAGS +=-m32
LDFLAGS +=-L$(VTUNE_AMPLIFIER_DIR)/lib32
endif

GCC      = gcc
CFLAGS  += -g -O3 -I$(VTUNE_AMPLIFIER_DIR)/include
LDFLAGS += -lpthread -ljitprofiling -ldl

EXE = jitprofiling

all: $(EXE)

OBJS = $(srcdir)/jitprofiling.o $(srcdir)/linux/dyncode_$(ARCHITECTURE).o

jitprofiling: $(OBJS)
	$(GCC) -g $(OBJS) -o $@  $(LDFLAGS)

clean:
	$(RM) $(OBJS)
	$(RM) $(EXE)

.PHONE: all clean

# * Other names and brands may be claimed as the property of others.
