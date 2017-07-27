/* ==============================================================
* 
* SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
* http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
* 
* Copyright 2005-2016 Intel Corporation
* 
* THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
* NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
* 
* =============================================================
*/
#include <string.h>
#include <jni.h>
extern "C" {
    JNIEXPORT jstring JNICALL
	Java_com_intel_tbb_example_tachyon_tachyonView_getAbi(JNIEnv *env,
                                                     jobject thiz)
    {
		#if defined(__arm__)
		  #if defined(__ARM_ARCH_7A__)
			#if defined(__ARM_NEON__)
			  #if defined(__ARM_PCS_VFP)
				#define ABI "armeabi-v7a/NEON (hard-float)"
			  #else
				#define ABI "armeabi-v7a/NEON"
			  #endif
			#else
			  #if defined(__ARM_PCS_VFP)
				#define ABI "armeabi-v7a (hard-float)"
			  #else
				#define ABI "armeabi-v7a"
			  #endif
			#endif
		  #else
		   #define ABI "armeabi"
		  #endif
		#elif defined(__i386__)
		   #define ABI "x86"
		#elif defined(__x86_64__)
		   #define ABI "x86_64"
		#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
		   #define ABI "mips64"
		#elif defined(__mips__)
		   #define ABI "mips"
		#elif defined(__aarch64__)
		   #define ABI "arm64-v8a"
		#else
		   #define ABI "unknown"
		#endif

    	char message[16];
    	strcpy(message, "ABI: ");
    	strcat(message, ABI);
    	return env->NewStringUTF(message);
    }
}
