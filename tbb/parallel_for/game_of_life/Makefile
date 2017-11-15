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

# GNU Makefile that builds and runs example.
run_cmd=
PROG=game_of_life
ARGS=2:4 -t 5
LIGHT_ARGS=1:2 -t 5
ifneq (,$(shell which icc 2>/dev/null))
CXX=icc
endif # icc

ifeq ($(shell uname), Linux)
ifeq ($(target), android)
LIBS+= --sysroot=$(SYSROOT)
run_cmd=../../common/android.linux.launcher.sh
else
LIBS+= -lrt 
endif
else ifeq ($(shell uname), Darwin)
override CXXFLAGS += -Wl,-rpath,$(TBBROOT)/lib
endif

all:	release test

release: src/Evolution.cpp src/Update_state.cpp src/Game_of_life.cpp
	$(CXX) -O2 -DNDEBUG -D_CONSOLE $(CXXFLAGS) -o $(PROG) $^ -ltbb $(LIBS)

debug: src/Evolution.cpp src/Update_state.cpp src/Game_of_life.cpp
	$(CXX) -O0 -D_CONSOLE -g -DTBB_USE_DEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb_debug $(LIBS)

clean:
	rm -f $(PROG) *.o *.d

test:
	$(run_cmd) ./$(PROG) $(ARGS)
light_test:
	$(run_cmd) ./$(PROG) $(LIGHT_ARGS)
