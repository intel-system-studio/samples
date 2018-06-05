#======================================================================= 
 # Copyright (C) 2010-2018 Intel Corporation. All Rights Reserved.
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


NAME:=mmult_advisor

ifeq ($(shell uname -m),x86_64)
Arch=intel64
CXXFLAGS+= -m64
else
Arch=ia32
CXXFLAGS+= -m32
endif #

CXX = g++
#CXX = icpc
ADV_DIR = $(ADVISOR_2019_DIR)

all: build
release: build
debug: build_debug

build: 1_mmult_serial 2_mmult_annotated 3_mmult_omp
build_debug: 1_mmult_serial_debug 2_mmult_annotated_debug 3_mmult_omp_debug

#ADVISOR COMMENT: If you add annotations to the serial version, you should uncomment the -I $(ADV_DIR)/include/ addition
# at the end of the 2 build lines below. Also, ensure the ADVISOR_2019_DIR environment variable is defined.
1_mmult_serial: mmult_serial.cpp
	$(CXX) $(CXXFLAGS) mmult_serial.cpp -o 1_mmult_serial -O2 -g #-I $(ADV_DIR)/include/
1_mmult_serial_debug: mmult_serial.cpp
	$(CXX) $(CXXFLAGS) mmult_serial.cpp -o 1_mmult_serial_debug -O0 -g -D_DEBUG #-I $(ADV_DIR)/include/ 
2_mmult_annotated: mmult_annotated.cpp
ifeq ($(ADV_DIR),)
	@echo ADVISOR_2019_DIR environment variable not defined. Required to build annotated sample.
else
	$(CXX) $(CXXFLAGS) mmult_annotated.cpp -I $(ADV_DIR)/include/ -o 2_mmult_annotated -O2 -g -ldl
endif
2_mmult_annotated_debug: mmult_annotated.cpp
ifeq ($(ADV_DIR),)
	@echo ADVISOR_2019_DIR environment variable not defined. Required to build annotated sample.
else
	$(CXX) $(CXXFLAGS) mmult_annotated.cpp -I $(ADV_DIR)/include/ -o 2_mmult_annotated_debug -O0 -g -D_DEBUG -ldl
endif
3_mmult_omp: mmult_omp.cpp
	icpc -qopenmp $(CXXFLAGS) mmult_omp.cpp -c -g
	icpc mmult_omp.o -o 3_mmult_omp -liomp5 -lpthread -L $(LD_LIBRARY_PATH) 
	rm -f mmult_omp.o
3_mmult_omp_debug: mmult_omp.cpp
	icpc -qopenmp $(CXXFLAGS) mmult_omp.cpp -c -O0 -g -D_DEBUG 
	icpc mmult_omp.o -o 3_mmult_omp_debug -liomp5 -lpthread -L $(LD_LIBRARY_PATH) 
	rm -f mmult_omp.o

EXECUTABLES =  1_mmult_serial 2_mmult_annotated 3_mmult_omp 1_mmult_serial_debug 2_mmult_annotated_debug 3_mmult_omp_debug

clean::
	rm -f $(EXECUTABLES) *.o
	
