 # =============================================================
 #Copyright (C) 2010-2017 Intel Corporation. All Rights Reserved.
 
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
 #==============================================================

CXX=icc
INCLUDES=-I src
CXXFLAGS=-c -g -O2 -std=c99 $(INCLUDES) $(BITS)

PROGS=MatVector

.PHONY: default
default: Driver.o Multiply.o
	$(CXX) Driver.o Multiply.o -o vec_samples $(LIBS)

.PHONY: baseline
baseline: default

baseline: CXXFLAGS = -c -g -O2 -std=c99 $(INCLUDES) $(BITS)
baseline: Driver.o Multiply.o
	$(CXX) Driver.o Multiply.o -o vec_samples $(LIBS)

xhost: CXXFLAGS = -c -g -xHost -std=c99 -DNOALIAS $(INCLUDES) $(BITS)
xhost: Driver.o Multiply.o
	$(CXX) Driver.o Multiply.o -o vec_samples $(LIBS)

noalias: CXXFLAGS = -c -g -O2 -std=c99 -DNOALIAS $(INCLUDES) $(BITS)
noalias: Driver.o Multiply.o
	$(CXX) Driver.o Multiply.o -o vec_samples $(LIBS)

reduction: CXXFLAGS = -c -g -xHost -std=c99 -qopenmp -DNOALIAS -DREDUCTION $(INCLUDES) $(BITS)
reduction: Driver.o Multiply.o
	$(CXX) -qopenmp Driver.o Multiply.o -o vec_samples $(LIBS)

align: CXXFLAGS = -c -g -xHost -std=c99 -qopenmp -DNOALIAS -D ALIGNED -DREDUCTION $(INCLUDES) $(BITS)
align: Driver.o Multiply.o
	$(CXX) -qopenmp Driver.o Multiply.o -o vec_samples $(LIBS)

nofunc: CXXFLAGS = -c -g -xHost -std=c99 -qopenmp -D NOFUNCCALL -DNOALIAS -DREDUCTION $(INCLUDES) $(BITS)
nofunc: Driver.o Multiply.o
	$(CXX) -qopenmp Driver.o Multiply.o -o vec_samples $(LIBS)

Driver.o: src/Driver.c
	$(CXX) $(BITS) $(CXXFLAGS) -o $@ src/Driver.c

Multiply.o: src/Multiply.c
	$(CXX) $(BITS) $(CXXFLAGS) -o $@ src/Multiply.c

.PHONY: clean
clean::
	-rm -f $(PROGS) *.o *.out *.optrpt
