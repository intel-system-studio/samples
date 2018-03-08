@echo off
REM ==============================================================
REM 
REM SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
REM http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
REM 
REM Copyright (C) Intel Corporation
REM 
REM THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
REM NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
REM PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
REM 
REM =============================================================

if "%DXSDK_DIR%"=="" goto error_no_DXSDK
goto end

:error_no_DXSDK
echo DirectX SDK Check : error : This example requires the DirectX SDK.  Either (re)-install the DirectX SDK, or set the DXSDK_DIR environment variable to indicate where it is installed.

:end

