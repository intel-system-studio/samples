# ==============================================================
# 
# SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
# http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
# 
# Copyright 2005-2016 Intel Corporation
# 
# THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
# 
# =============================================================
#
# To compile with the GNU* C/C++ compiler, creating an execution file with the
# extension ".gcc" for binary instrumentation, issue:
#
#   > make
#
# To compile with the Intel(R) C++ Compiler for Linux*, creating an execution
# file with the extension ".icc":
#
#   Source <path_to_compiler_bin>/compilervars.sh or iccvars.csh;
#
#   > make icc
#
# To compile with the Intel(R) C++ Compiler for Linux* with Intel(R) MKL library
# creating an executionfile with the extension ".mkl":
#
#   Source <path_to_compiler_bin>/compilervars.sh or iccvars.csh;
#
#   > make mkl
#
# To compile with the Intel(R) C++ Compiler for Linux to cross compile for the
# Intel(R) Xeon Phi(TM) coprocessor, creating an execution file with the
# extension ".mic":
#
#   Source <path_to_compiler_bin>/compilervars.sh intel64
#
#   > make mic
#
# To compile them all, use the source line from the Intel MIC architecture
# option above, then type:
#
#   > make all

SHELL = /bin/sh

PARAMODEL = -DUSE_THR	# Default parallelism using pthreads/Win threads
#PARAMODEL = -DUSE_OMP -fopenmp	# Use OpenMP for multithreading

GCC     = gcc
ICC     = icc
CFLAGS  = -g -O3
OPTFLAGS = -xSSE3 
# OPTFLAGS = -xHost -fno-alias
# add -DALIGNED to the multiply.c and matrix.c
LDFLAGS = -lpthread -lm

GCFLAGS = $(CFLAGS) $(PARAMODEL)
ICFLAGS = $(CFLAGS) $(PARAMODEL)-DICC -debug inline-debug-info #-vec-report3 -qopt-report -qopt-report-phase=vec
MCFLAGS = $(CFLAGS) $(PARAMODEL)-DICC -debug inline-debug-info -vec-report3 -qopt-report=3 -qopt-report-phase=vec
MKFLAGS = $(CFLAGS) -DUSE_MKL   -DICC -mkl -debug inline-debug-info


GCC_EXE = matrix.gcc
ICC_EXE = matrix.icc
MIC_EXE = matrix.mic
MKL_EXE = matrix.mkl


srcdir = src

gcc: $(GCC_EXE)
icc: $(ICC_EXE)
mic: $(MIC_EXE) mic-pushed
mkl: $(MKL_EXE)
all: $(GCC_EXE) $(ICC_EXE) $(MIC_EXE) $(MKL_EXE)


OBJS = util.o thrmodel.o multiply.o matrix.o 


matrix.gcc: $(srcdir)/matrix.c $(srcdir)/multiply.c $(srcdir)/multiply.h $(srcdir)/util.c $(srcdir)/thrmodel.c
	$(GCC) $(GCFLAGS) -c $(srcdir)/util.c -D_LINUX
	$(GCC) $(GCFLAGS) -c $(srcdir)/thrmodel.c -D_LINUX
	$(GCC) $(GCFLAGS) -c $(srcdir)/multiply.c -D_LINUX
	$(GCC) $(GCFLAGS) -c $(srcdir)/matrix.c -D_LINUX
	$(GCC) $(GCFLAGS) -g $(OBJS) -o $@ $(LDFLAGS)


matrix.icc: $(srcdir)/matrix.c $(srcdir)/multiply.c $(srcdir)/multiply.h $(srcdir)/util.c $(srcdir)/thrmodel.c
	$(ICC) $(ICFLAGS) -c $(srcdir)/util.c -D_LINUX
	$(ICC) $(ICFLAGS) -c $(srcdir)/thrmodel.c -D_LINUX
	$(ICC) $(ICFLAGS) $(OPTFLAGS) -c $(srcdir)/multiply.c -D_LINUX
	$(ICC) $(ICFLAGS) $(OPTFLAGS) -c $(srcdir)/matrix.c -D_LINUX
	$(ICC) $(ICFLAGS) $(OBJS) -o $@ $(LDFLAGS)


matrix.mic: $(srcdir)/matrix.c $(srcdir)/multiply.c $(srcdir)/multiply.h $(srcdir)/util.c $(srcdir)/thrmodel.c
	$(ICC) $(MCFLAGS) -mmic -c $(srcdir)/util.c -D_LINUX
	$(ICC) $(MCFLAGS) -mmic -c $(srcdir)/thrmodel.c -D_LINUX
	$(ICC) $(MCFLAGS) -mmic -c $(srcdir)/multiply.c -D_LINUX
	$(ICC) $(MCFLAGS) -mmic -c $(srcdir)/matrix.c -D_LINUX
	$(ICC) $(MCFLAGS) -mmic $(OBJS) -o $@ $(LDFLAGS)

mic-pushed:	matrix.mic
	scp matrix.mic mic0:/tmp
	touch $@

matrix.mkl:     $(srcdir)/matrix.c $(srcdir)/multiply.c $(srcdir)/multiply.h $(srcdir)/util.c $(srcdir)/thrmodel.c
	$(ICC) $(MKFLAGS) -c $(srcdir)/util.c -D_LINUX
	$(ICC) $(MKFLAGS) -c $(srcdir)/thrmodel.c -D_LINUX
	$(ICC) $(MKFLAGS) $(OPTFLAGS) -c $(srcdir)/multiply.c -D_LINUX
	$(ICC) $(MKFLAGS) $(OPTFLAGS) -c $(srcdir)/matrix.c -D_LINUX
	$(ICC) $(MKFLAGS) $(OBJS) -o $@ $(LDFLAGS)

clean:
	@rm -rf $(OBJS) $(GCC_EXE) $(ICC_EXE) $(MKL_EXE) $(MIC_EXE) *.o mic-pushed

# * Other names and brands may be claimed as the property of others.
