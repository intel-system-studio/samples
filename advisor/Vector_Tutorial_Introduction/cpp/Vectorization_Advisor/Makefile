#=======================================================================
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


CXX=icpc
INCLUDES=
CXXFLAGS=-c -g -O3 -qopenmp -std=c++0x -qopt-report5 $(INCLUDES) 

PROGS=VecTutorialIntro

.PHONY: default
default: example.o 
	$(CXX) -qopenmp example.o -o VecTutorialIntro $(LIBS)

example.o: example.cpp
	$(CXX) $(CXXFLAGS) -o $@ example.cpp

.PHONY: clean
clean::
	-rm -f $(PROGS) *.o *.out *.optrpt
