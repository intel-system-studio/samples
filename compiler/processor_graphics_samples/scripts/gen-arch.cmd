@echo off
::==============================================================
::
:: SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
:: http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
::
:: Copyright 2016 Intel Corporation
::
:: THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
:: NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
:: PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
::
:: =============================================================

:: This script returns current GEN architecture using gfx_sys_check utility
:: gfx_sys_check provides arch name which needs to be adjusted to use with /Qoffload-arch

for /F "tokens=2 delims=:" %%G IN ('gfx_sys_check -v ^| findstr "GPU architecture"') do set ARCH=%%G
set ARCH=%ARCH: =%
for /F "tokens=2 delims=:" %%G IN ('gfx_sys_check -v ^| findstr "vISA support"') do set VISA=%%G
set VISA=%VISA: =%
set GEN_ARCH=%ARCH%:%VISA%

if "%GEN_ARCH%"=="=" (@echo Undefined) else (@echo %GEN_ARCH%)
