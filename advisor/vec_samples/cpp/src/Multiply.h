/*
 * Copyright (C) 2010-2014 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all
 * documents related to the source code ("Material") are owned by 
 * Intel Corporation or its suppliers or licensors. Title to the
 * Material remains with Intel Corporation or its suppliers and
 * licensors. The Material is protected by worldwide copyright
 * laws and treaty provisions.  No part of the Material may be
 * used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed,  or disclosed in any way
 * except as expressly provided in the license provided with the
 * Materials.  No license under any patent, copyright, trade
 * secret or other intellectual property right is granted to or
 * conferred upon you by disclosure or delivery of the Materials,
 * either expressly, by implication, inducement, estoppel or
 * otherwise, except as expressly provided in the license
 * provided with the Materials. 
 *
 * Header file for vec_samples tutorial. See "Tutorial: Auto-vectorization"
 * in the Intel(R) C++ Compiler Tutorials document
 */
#include <stdio.h> 

#ifndef FTYPE
#define FTYPE float
#endif

#ifdef NOALIAS
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[restrict], FTYPE x[], FTYPE wr[]);
#else
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[], FTYPE x[], FTYPE wr[]);
#endif
