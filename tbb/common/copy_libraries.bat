@echo off
REM
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
REM

:: Getting parameters
if ("%1") == ("") goto error0
if ("%2") == ("") goto error0
if ("%3") == ("") goto error0
set arch=%1
if ("%2") == ("debug") set postfix=_debug
set output_dir=%3

:: Optional 4th parameter to set install root
if ("%4") NEQ ("") set TBBROOT=%4
:: Actually we can set install root by ourselves
if ("%TBBROOT%") == ("") set TBBROOT=%~d0%~p0..\..\

:: Getting vs folders in case vc_mt binaries are not provided
:: ordered from oldest to newest, so we end with newest available version
if ("%VS90COMNTOOLS%")  NEQ ("") set vc_dir=vc9
if ("%VS100COMNTOOLS%") NEQ ("") set vc_dir=vc10
if ("%VS110COMNTOOLS%") NEQ ("") set vc_dir=vc11
if ("%VS120COMNTOOLS%") NEQ ("") set vc_dir=vc12

:: Are we standalone/oss or inside compiler?
if exist "%TBBROOT%\bin\%arch%\%vc_dir%\tbb%postfix%.dll" set interim_path=bin\%arch%
if exist "%TBBROOT%\..\redist\%arch%\tbb\%vc_dir%\tbb%postfix%.dll" set interim_path=..\redist\%arch%\tbb
if ("%interim_path%") == ("") goto error1

:: Do we provide vc_mt binaries?
if exist "%TBBROOT%\%interim_path%\vc_mt\tbb%postfix%.dll" set vc_dir=vc_mt
if ("%vc_dir%") == ("") goto error2

:: We know everything we wanted and there are no errors
:: Copying binaries

copy "%TBBROOT%\%interim_path%\%vc_dir%\tbb%postfix%.dll" "%output_dir%"
copy "%TBBROOT%\%interim_path%\%vc_dir%\tbb%postfix%.pdb" "%output_dir%"
copy "%TBBROOT%\%interim_path%\%vc_dir%\tbbmalloc%postfix%.dll" "%output_dir%"
copy "%TBBROOT%\%interim_path%\%vc_dir%\tbbmalloc%postfix%.pdb" "%output_dir%"
if exist "%TBBROOT%\%interim_path%\%vc_dir%\tbb_preview%postfix%.dll" copy "%TBBROOT%\%interim_path%\%vc_dir%\tbb_preview%postfix%.dll" "%output_dir%"
if exist "%TBBROOT%\%interim_path%\%vc_dir%\tbb_preview%postfix%.pdb" copy "%TBBROOT%\%interim_path%\%vc_dir%\tbb_preview%postfix%.pdb" "%output_dir%"

goto end
:error0
echo number of parameters not correct
exit /B 1
:error1
echo Could not determine path to TBB libraries
exit /B 1
:error2
echo Could not determine Visual Studio version
exit /B 1

:end
exit /B 0

