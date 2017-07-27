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
	printf("*******************************\n");
	#ifdef __INTEL_COMPILER
		printf("  Intel(R) Compiler was used!\n");
	#else
		printf("Intel(R) Compiler was NOT used!\n");
	#endif
	printf("*******************************\n");
	return 0;
}
