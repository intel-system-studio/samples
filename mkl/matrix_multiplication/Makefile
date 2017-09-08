#==============================================================
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

CC := icc
SRCDIR := src
BUILDDIR := release
LIBFLAGS := -mkl -static-intel

all: $(BUILDDIR)/dgemm_example $(BUILDDIR)/dgemm_with_timing $(BUILDDIR)/matrix_multiplication $(BUILDDIR)/dgemm_threading_effect_example

$(BUILDDIR)/%: $(BUILDDIR)/%.o
	$(CC) $< $(LIBFLAGS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) -c $< -o $@

run_dgemm_example: $(BUILDDIR)/dgemm_example
	./$(BUILDDIR)/dgemm_example

run_dgemm_with_timing: $(BUILDDIR)/dgemm_with_timing
	./$(BUILDDIR)/dgemm_with_timing

run_matrix_multiplication: $(BUILDDIR)/matrix_multiplication
	./$(BUILDDIR)/matrix_multiplication

run_dgemm_threading_effect_example: $(BUILDDIR)/dgemm_threading_effect_example
	./$(BUILDDIR)/dgemm_threading_effect_example

clean:
	@echo " Cleaning..."
	@rm -fr $(BUILDDIR) 2>/dev/null || true

.PHONY: clean
.PRECIOUS: $(BUILDDIR)/%.o
