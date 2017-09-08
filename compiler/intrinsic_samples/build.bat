@ECHO OFF
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

if "%1" == "clean"    goto clean
if "%1" == "release"  goto release
if "%1" == "debug"    goto debugdir
if "%1" == "run"      goto run

echo No action taken...
echo syntax: %~nx0 [clean^|debug^|release^|run]
goto done

:clean
if exist "debugdir" del /F /Q debugdir\*.*
if exist "debugdir" rmdir debugdir
if exist "release" del /F /Q release\*.*
if exist "release" rmdir release
goto done

:release
if exist "release" goto nextRelease
mkdir release
:nextRelease
set BinOut=.\release

@ECHO ON
icl /c /O2 /Fo:%BinOut%\intrin_dot_sample.obj .\src\intrin_dot_sample.c
icl %BinOut%\intrin_dot_sample.obj /O2 /link /out:%BinOut%\intrin_dot_sample.exe

icl /c /O2 /Fo:%BinOut%\intrin_double_sample.obj .\src\intrin_double_sample.c 
icl %BinOut%\intrin_double_sample.obj /O2 /link /out:%BinOut%\intrin_double_sample.exe

icl /c /O2 /Fo:%BinOut%\intrin_ftz_sample.obj .\src\intrin_ftz_sample.c
icl %BinOut%\intrin_ftz_sample.obj /O2 /link /out:%BinOut%\intrin_ftz_sample.exe
@ECHO OFF
goto done

:debugdir
if exist "debugdir" goto nextdebugdir
mkdir debugdir
:nextdebugdir
set BinOut=.\debugdir

@ECHO ON
icl /c /Od /Fo:%BinOut%\intrin_dot_sample.obj .\src\intrin_dot_sample.c
icl %BinOut%\intrin_dot_sample.obj /Od /link /out:%BinOut%\intrin_dot_sample.exe

icl /c /Od /Fo:%BinOut%\intrin_double_sample.obj .\src\intrin_double_sample.c 
icl %BinOut%\intrin_double_sample.obj /Od /link /out:%BinOut%\intrin_double_sample.exe

icl /c /Od /Fo:%BinOut%\intrin_ftz_sample.obj .\src\intrin_ftz_sample.c
icl %BinOut%\intrin_ftz_sample.obj /Od /link /out:%BinOut%\intrin_ftz_sample.exe
@ECHO OFF
goto done

:run
@ECHO OFF
if exist "debugdir" (set BinOut=.\debugdir) else goto checkRelease
@ECHO ON
rem Running debug executables...
%BinOut%\intrin_dot_sample.exe
%BinOut%\intrin_double_sample.exe
%BinOut%\intrin_ftz_sample.exe

:checkRelease
@ECHO OFF
if exist "release" (set BinOut=.\release) else goto done
@ECHO ON
rem Running release executables...
%BinOut%\intrin_dot_sample.exe
%BinOut%\intrin_double_sample.exe
%BinOut%\intrin_ftz_sample.exe

:done