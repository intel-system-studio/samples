#=======================================================================
#
# /* Copyright (C) 2010-2017 Intel Corporation. All Rights Reserved.
 #
 # The source code, information and material ("Material") 
 # contained herein is owned by Intel Corporation or its 
 # suppliers or licensors, and title to such Material remains 
 # with Intel Corporation or its suppliers or licensors.
 # The Material contains proprietary information of Intel or 
 # its suppliers and licensors. The Material is protected by 
 # worldwide copyright laws and treaty provisions.
 # No part of the Material may be used, copied, reproduced, 
 # modified, published, uploaded, posted, transmitted, distributed 
 # or disclosed in any way without Intel's prior express written 
 # permission. No license under any patent, copyright or other
 # intellectual property rights in the Material is granted to or 
 # conferred upon you, either expressly, by implication, inducement, 
 # estoppel or otherwise. Any license under such intellectual 
 # property rights must be express and approved by Intel in writing.
 # Third Party trademarks are the property of their respective owners.
 # Unless otherwise agreed by Intel in writing, you may not remove 
 # or alter this notice or any other notice embedded in Materials 
 # by Intel or Intel's suppliers or licensors in any way.
#
# ========================================================================


NAME:=nqueens_advisor

ifeq ($(shell uname -m),x86_64)
Arch=intel64
CXXFLAGS+= -m64
else
Arch=ia32
CXXFLAGS+= -m32
endif #

CXX = g++
#CXX = icpc
TBB_DIR = $(TBBROOT)
ADV_DIR = $(ADVISOR_2019_DIR)

all: build
release: build
debug: build_debug

build: 1_nqueens_serial 3_nqueens_tbb 4_nqueens_cilk 5_nqueens_omp
build_debug: 1_nqueens_serial_debug 3_nqueens_tbb_debug 4_nqueens_cilk_debug 5_nqueens_omp_debug

1_nqueens_serial: nqueens_serial.cpp
	$(CXX) $(CXXFLAGS) nqueens_serial.cpp -o 1_nqueens_serial -O2 -g -I $(ADV_DIR)/include/ -ldl
1_nqueens_serial_debug: nqueens_serial.cpp
	$(CXX) $(CXXFLAGS) nqueens_serial.cpp -o 1_nqueens_serial_debug -O0 -g -D_DEBUG -I $(ADV_DIR)/include/ -ldl


3_nqueens_tbb: nqueens_tbb.cpp
ifeq ($(TBB_DIR),)
	@echo TBBROOT environment variable not defined. Required to build TBB sample.
else
	$(CXX) $(CXXFLAGS) nqueens_tbb.cpp -I $(TBB_DIR)/include/ -o 3_nqueens_tbb -O2 -g -ltbb -L $(LD_LIBRARY_PATH)
endif
3_nqueens_tbb_debug: nqueens_tbb.cpp
ifeq ($(TBB_DIR),)
	@echo TBBROOT environment variable not defined. Required to build TBB sample.
else
	$(CXX) $(CXXFLAGS) nqueens_tbb.cpp -I $(TBB_DIR)/include/ -o 3_nqueens_tbb_debug -O0 -g -D_DEBUG -ltbb_debug -L $(LD_LIBRARY_PATH)
endif

4_nqueens_cilk: nqueens_cilk.cpp
	icpc $(CXXFLAGS) nqueens_cilk.cpp -o 4_nqueens_cilk -O2 -g -lcilkrts
4_nqueens_cilk_debug: nqueens_cilk.cpp
	icpc $(CXXFLAGS) nqueens_cilk.cpp -o 4_nqueens_cilk_debug -O0 -g -D_DEBUG -lcilkrts

5_nqueens_omp: nqueens_omp.cpp
	icpc -qopenmp $(CXXFLAGS) nqueens_omp.cpp -c -g
	icpc nqueens_omp.o -o 5_nqueens_omp -liomp5 -lpthread -L $(LD_LIBRARY_PATH) 
	rm -f nqueens_omp.o
5_nqueens_omp_debug: nqueens_omp.cpp
	icpc -qopenmp $(CXXFLAGS) nqueens_omp.cpp -c -O0 -g -D_DEBUG 
	icpc nqueens_omp.o -o 5_nqueens_omp_debug -liomp5 -lpthread -L $(LD_LIBRARY_PATH) 
	rm -f nqueens_omp.o

EXECUTABLES =  1_nqueens_serial 3_nqueens_tbb 4_nqueens_cilk 5_nqueens_omp 1_nqueens_serial_debug 3_nqueens_tbb_debug 4_nqueens_cilk_debug 5_nqueens_omp_debug

clean::
	rm -f $(EXECUTABLES) *.o
