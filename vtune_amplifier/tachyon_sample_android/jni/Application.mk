# ==============================================================
# 
# SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
# http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
# 
# Copyright 2005-2016 Intel Corporation
# 
# THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
# 
# =============================================================

APP_ABI:= x86_64, x86
APP_STL:=gnustl_static
APP_GNUSTL_FORCE_CPP_FEATURES := exceptions rtti
APP_PLATFORM:=android-19
#NDK_TOOLCHAIN=x86_64-4.9
#Intel C compiler would work if compilervars.bat  are sourced. (-arch intel64 -platform android)
#NDK_TOOLCHAIN_VERSION:=4.9
NDK_DEBUG:=1
APP_OPTIM:=debug
V:=1
NDK_LOG:=1


