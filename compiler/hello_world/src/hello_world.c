/* 
 * ============================================================== 
 * 
 * SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT, 
 * http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/ 
 * 
 * Copyright 2016 Intel Corporation 
 * 
 * THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS. 
 * 
 * ============================================================= 
 */

#include <stdlib.h>
#include <stdio.h>


int main(void) {
	printf("Hello, World!\n");
	#ifdef __INTEL_COMPILER
		printf("Intel(R) C++ Compiler was used for compiling this sample\n");
	#else
		printf("Intel(R) C++ Compiler was not used for compiling this sample\n");
	#endif
	return 0;
}
