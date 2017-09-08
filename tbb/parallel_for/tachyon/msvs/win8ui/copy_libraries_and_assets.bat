@echo on
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
:: Architecture
if ("%1") == ("") goto error0
:: Release/Debug
if ("%2") == ("") goto error0
:: Output directory
if (%3) == ("") goto error0
set arch=%1
if ("%2") == ("debug") set postfix=_debug
set output_dir="%3"
if ("%4") == ("") set dat_file="%output_dir%\..\..\dat\balls.dat"

:: Actually we can set install root by ourselves
if ("%TBBROOT%") == ("") set TBBROOT=%~dp0..\..\..\..\..\

:: ordered from oldest to newest, so we end with newest available version
if ("%VS110COMNTOOLS%") NEQ ("") set vc_dir=vc11_ui
if ("%VS120COMNTOOLS%") NEQ ("") set vc_dir=vc12_ui
echo Using %vc_dir% libraries

if exist "%TBBROOT%\bin\%arch%\%vc_dir%\tbb%postfix%.dll" set interim_path=bin\%arch%
if exist "%TBBROOT%..\redist\%arch%\tbb\%vc_dir%\tbb%postfix%.dll" set interim_path=..\redist\%arch%\tbb
if exist "%TBBROOT%\lib\%arch%\%vc_dir%\tbb%postfix%.lib" set interim_lib_path=lib\%arch%
if ("%interim_path%") == ("") goto error1
if ("%interim_lib_path%") == ("") goto error1

:: We know everything we wanted and there are no errors
:: Copying binaries

copy "%TBBROOT%\%interim_path%\%vc_dir%\tbb%postfix%.dll" "%output_dir%"
copy "%TBBROOT%\%interim_path%\%vc_dir%\tbb%postfix%.pdb" "%output_dir%"
copy "%TBBROOT%\%interim_path%\%vc_dir%\tbbmalloc%postfix%.dll" "%output_dir%"
copy "%TBBROOT%\%interim_path%\%vc_dir%\tbbmalloc%postfix%.pdb" "%output_dir%"
copy "%TBBROOT%\%interim_lib_path%\%vc_dir%\tbb%postfix%.lib" "%output_dir%"

:: Copying DAT-file
echo Using DAT-file %dat_file% 
if exist %dat_file% copy %dat_file% "%output_dir%\Assets\balls.dat"

goto end
:error0
echo Custom build script usage: %0 [ia32 or intel64] [release or debug] [output dir] [dat-file]
exit /B 1
:error1
echo Could not determine path to Intel TBB libraries
exit /B 1

:end
exit /B 0
