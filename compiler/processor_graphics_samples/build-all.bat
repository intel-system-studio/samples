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
@echo off

set SAMPLES=convolution_sample crossfade_sample histogramlocals_sample matrix_multiply_sample nbody_sample test_gfx_vector_sample shared_local_memory_sample

rem This script needs ICL with graphics offload environment i.e. %PATH%, %LIB%,
rem %INCLUDE%. Please setup environment using iclvars.bat script from icl
rem installation, it is accessible from start menu

rem PRODUCT_NAME is only defined in Intel environment
rem if in Visual Studio environment
if /i "%PRODUCT_NAME%"=="" (
        echo Need Intel C++ Compiler with graphics offload environment to build the sample.
        goto eof
)

rem else if in Intel environment
if /i NOT "%PRODUCT_NAME%"=="" (
	set CC=icl
        where ld >nul 2>&1 || echo ld not found on the %%PATH%%, please install ^
binutils and add them to the path. See Release Notes for instructions. ^
E.g. set "PATH=C:\Program Files (x86)\Binutils for MinGW (64 bit)\bin;%%PATH%%" && exit /b 3
)

:TMPFILE
set TMP_FILE="%TMP%\%RANDOM%.tmpicl"
if exist "%TMP_FILE%" goto :TMPFILE

icl > %TMP_FILE% 2>&1
for /F "tokens=1 delims=:" %%G IN ('type %TMP_FILE% ^| findstr "\/help"') do set ICL_CMPLR=%%G
del %TMP_FILE%
if  "%ICL_CMPLR%" NEQ "icl" (
  echo Error: icl not found on the %%PATH%%, please setup environment using iclvars.bat from icl installation
  exit /b 1
)

gfx_sys_check > %TMP_FILE% 2>&1

for /F "tokens=1 delims= " %%G IN ('type %TMP_FILE% ^| findstr "GPU"') do (
  if "%%G" EQU "GPU" goto found_gfx_sys_check
)
del %TMP_FILE%
echo Error: gfx_sys_check not found on the %%PATH%%, please setup environment using iclvars.bat from icl installation 
exit /b 2

:found_gfx_sys_check

:options
if /i "%1"=="debug" goto debug
if /i "%1"=="release" goto release
if /i "%1"=="clean" goto clean
if /i "%1"=="run" goto run
if /i "%1"=="help" goto helpmsg

rem Default build release
:release
set OPTION=
set DESTDIR=release
goto build

:debug
set OPTION=debug
set DESTDIR=debug
goto build

:run
shift
set args=
:collectargs
if /i NOT "%~1"=="" (
	set args=%args% %1
	shift
	goto collectargs
)
set OPTION=run %args%

:build
for %%S in (%SAMPLES%) do (
  echo Entering %%S
  cd %%S
  echo call build.bat %OPTION%
  call build.bat %OPTION%
  cd ..\
  echo.
)
goto eof

:helpmsg
echo "Syntax: build-all [debug|release|run|clean]"
echo "     build-all debug - Building all samples in Debug configuration"
echo "     build-all release - Building all samples in Release configuration"
echo "     build-all run [release|debug] - Run all samples"
echo "     build-all clean - Clean build directory"
echo "     build-all - Default building all samples in Release configuration"
goto eof

:clean
echo removing files...
if "%DESTDIR%"=="" set DESTDIR=Release
rmdir /Q /S convolution_sample\%DESTDIR% 2>nul
rmdir /Q /S crossfade_sample\%DESTDIR% 2>nul
rmdir /Q /S histogramlocals_sample\%DESTDIR% 2>nul
rmdir /Q /S matrix_multiply_sample\%DESTDIR% 2>nul
rmdir /Q /S nbody_sample\%DESTDIR% 2>nul
rmdir /Q /S test_gfx_vector_sample\%DESTDIR% 2>nul
rmdir /Q /S shared_local_memory_sample\%DESTDIR% 2>nul

:eof
