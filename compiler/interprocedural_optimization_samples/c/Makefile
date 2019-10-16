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

CC := icc
SRCDIR := src
BUILDDIR := out
CFLAGS := -O2 -std=c99 -ipo
LIBFLAGS := -ipo -qopt-report:3 -qopt-report-file:$(BUILDDIR)/ipo_sample.optrpt
TARGET := $(BUILDDIR)/ipo_samples.exe

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))

$(TARGET): $(OBJECTS)
	$(CC) $^ $(LIBFLAGS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) -c $(CFLAGS) -o $@ $<

run:
	./$(TARGET)

clean:
	@echo "Cleaning..."
	@rm -fr $(BUILDDIR) $(TARGET) 2>/dev/null || true

.PHONY: clean

