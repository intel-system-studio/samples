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
 
# ========================================================================


NAME:=mpi_sample

ifeq ($(shell uname -m),x86_64)
Arch=intel64
CXXFLAGS+= -m64
else
Arch=ia32
CXXFLAGS+= -m32
endif #

#CXX = g++
CXX = mpiicpc
ADV_DIR = $(ADVISOR_2018_DIR)

all: build
release: build
debug: build_debug

build: 1_mpi_sample_serial 2_mpi_sample_annotated 3_mpi_sample_omp
build_debug: 1_mpi_sample_serial_debug 2_mpi_sample_annotated_debug 3_mpi_sample_omp_debug

#ADVISOR COMMENT: If you add annotations to the serial version, you should uncomment the -I $(ADV_DIR)/include/ addition
# at the end of the 2 build lines below. Also, ensure the ADVISOR_2018_DIR environment variable is defined.
1_mpi_sample_serial: mpi_sample_serial.cpp
	$(CXX) $(CXXFLAGS) mpi_sample_serial.cpp -o 1_mpi_sample_serial -O2 -g #-I $(ADV_DIR)/include/
1_mpi_sample_serial_debug: mpi_sample_serial.cpp
	$(CXX) $(CXXFLAGS) mpi_sample_serial.cpp -o 1_mpi_sample_serial_debug -O0 -g -D_DEBUG #-I $(ADV_DIR)/include/ 
2_mpi_sample_annotated: mpi_sample_annotated.cpp
ifeq ($(ADV_DIR),)
	@echo ADVISOR_2018_DIR environment variable not defined. Required to build annotated sample.
else
	$(CXX) $(CXXFLAGS) mpi_sample_annotated.cpp -I $(ADV_DIR)/include/ -o 2_mpi_sample_annotated -O2 -g -ldl
endif
2_mpi_sample_annotated_debug: mpi_sample_annotated.cpp
ifeq ($(ADV_DIR),)
	@echo ADVISOR_2018_DIR environment variable not defined. Required to build annotated sample.
else
	$(CXX) $(CXXFLAGS) mpi_sample_annotated.cpp -I $(ADV_DIR)/include/ -o 2_mpi_sample_annotated_debug -O0 -g -D_DEBUG -ldl
endif
3_mpi_sample_omp: mpi_sample_omp.cpp
	$(CXX) -qopenmp $(CXXFLAGS) mpi_sample_omp.cpp -c -g
	$(CXX) mpi_sample_omp.o -o 3_mpi_sample_omp -liomp5 -lpthread -L $(LD_LIBRARY_PATH) 
	rm -f mpi_sample_omp.o
3_mpi_sample_omp_debug: mpi_sample_omp.cpp
	$(CXX) -qopenmp $(CXXFLAGS) mpi_sample_omp.cpp -c -O0 -g -D_DEBUG 
	$(CXX) mpi_sample_omp.o -o 3_mpi_sample_omp_debug -liomp5 -lpthread -L $(LD_LIBRARY_PATH) 
	rm -f mpi_sample_omp.o

EXECUTABLES =  1_mpi_sample_serial 2_mpi_sample_annotated 3_mpi_sample_omp 1_mpi_sample_serial_debug 2_mpi_sample_annotated_debug 3_mpi_sample_omp_debug

clean::
	rm -f $(EXECUTABLES) *.o
	

