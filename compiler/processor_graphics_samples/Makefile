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

SUBDIRS := convolution_sample crossfade_sample histogramlocals_sample matrix_multiply_sample nbody_sample test_gfx_vector_sample shared_local_memory_sample
OPTION := 

ifdef debug
  OPTION := debug
endif

ifdef option
  RUNTIME_FLAGS := option="$(option)"
endif

all:
	@for dir in $(SUBDIRS); \
	do $(MAKE) -C $$dir $(OPTION) || exit 1; \
	done

run: all
	@for dir in $(SUBDIRS); \
	do $(MAKE) -C $$dir run $(RUNTIME_FLAGS) || exit 1;\
	done

clean:
	@echo " Cleaning..."
	@for dir in $(SUBDIRS); \
	do $(MAKE) -C $$dir clean $(RUNTIME_FLAGS) || exit 1;\
	done

.PHONY: clean
