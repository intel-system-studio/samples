@echo off
::==============================================================
::
:: SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
:: http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
::
:: Copyright (C) Intel Corporation
::
:: THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
:: NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
:: PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
::
:: =============================================================
::
::
::******************************************************************************
:: Content:
::
::  Intel(R) Parallel Studio XE
::  Build for ipo_sample
::******************************************************************************

if /i "%1"=="" goto Build
if /i "%1"=="build" goto Build
if /i "%1"=="clean" goto Clean
goto Syntax

:Clean
if exist "*.obj" del *.obj
if exist "*.exe" del *.exe
if exist "*.optrpt" del *.optrpt
exit /B 0

:Build
@echo on
icl /c /nologo /Qipo src\ipo_sample_init.c
icl /c /nologo /Qipo src\ipo_sample_sum.c
icl /nologo /Qipo /Qopt-report:3 /Qopt-report-file:ipo_sample.optrpt src\ipo_sample_main.c ipo_sample_init.obj ipo_sample_sum.obj
@echo off
echo Optimization report is in ipo_sample.optrpt - open with Notepad
exit /B 0

:Syntax
echo %~nx0 [build ^| clean]
echo build is default
echo debug and run options not applicable to this sample
exit /B 1
