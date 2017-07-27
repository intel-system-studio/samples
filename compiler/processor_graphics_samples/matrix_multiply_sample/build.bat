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

set SAMPLE=matmult
set WORKLOADS="{matmult|matmultlocalsan|matmult_d|matmultlocalsan_d}"
set SRCDIR=src\
set SRCDIR_SHARED=..\shared\src\
set EXTRA_INCLUDE=/I %SRCDIR_SHARED%
set EXTRA_LIB=
set LINK_FLAGS=

rem This script needs ICL with graphics offload environment i.e. %PATH%, %LIB%,
rem %INCLUDE%. Please setup environment using iclvars.bat script from icl
rem installation, it is accessible from start menu

echo Sample: %SAMPLE%

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

gfx_sys_check > %TMP_FILE% 2>&1

for /F "tokens=1 delims= " %%G IN ('type %TMP_FILE% ^| findstr "GPU"') do (
  if "%%G" EQU "GPU" goto found_gfx_sys_check
)
del %TMP_FILE%
echo Error: gfx_sys_check not found on the %%PATH%%, please setup environment using iclvars.bat from icl installation 
exit /b 2

:found_gfx_sys_check

set ARCH=
for /F "tokens=2 delims=:" %%G IN ('gfx_sys_check -v ^| findstr "GPU architecture"') do set ARCH=%%G
if "%ARCH%"=="" (echo Error: graphics driver is not ptoperly configured && exit /b 3)
set ARCH=%ARCH: =%
for /F "tokens=2 delims=:" %%G IN ('gfx_sys_check -v ^| findstr "vISA support"') do set VISA=%%G
set VISA=%VISA: =%
set GEN_ARCH=%ARCH%:%VISA%

:options
if /i "%1"=="debug" goto debug
if /i "%1"=="release" goto release
if /i "%1"=="clean" goto clean
if /i "%1"=="run" goto run
if /i "%1"=="help" goto helpmsg

rem Default build release
:release
set DESTDIR=Release\
mkdir %DESTDIR% 2>nul
set CC_FLAGS=/O2 /Qipo /EHsc /Qoffload_arch=%GEN_ARCH% 
goto compile

:debug
set DESTDIR=Debug\
mkdir %DESTDIR% 2>nul
set CC_FLAGS=/Od /Zi /W3 /EHsc /Qoffload_arch=%GEN_ARCH%
set LINK_FLAGS=/debug

:compile
echo. >.tmp_exclude_list
echo .h >>.tmp_exclude_list
echo .hpp >>.tmp_exclude_list
echo .c >>.tmp_exclude_list
echo .cpp >>.tmp_exclude_list

rem shared
mkdir %DESTDIR%shared 2>nul
echo %CC% /c %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR%shared/ /Fd%DESTDIR%shared/ %SRCDIR_SHARED%*.cpp 
%CC% /c %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR%shared\ /Fd%DESTDIR%shared\ %SRCDIR_SHARED%*.cpp
set SHARED_BUILD_FAIL=0
if not exist %DESTDIR%shared\main.obj (set SHARED_BUILD_FAIL=1)
if not exist %DESTDIR%shared\Util.obj (set SHARED_BUILD_FAIL=1)
if not exist %DESTDIR%shared\Properties.obj (set SHARED_BUILD_FAIL=1)
if "%SHARED_BUILD_FAIL%"=="1" (echo shared build failed) else (echo shared build passed)
if exist main.obj if exist Properties.obj if exist Utial.obj (echo shared build passed) else (echo shared build failed)

echo %CC% /c %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% /Fd%DESTDIR% %SRCDIR%%SAMPLE%.cpp
%CC% /c %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% /Fd%DESTDIR% %SRCDIR%%SAMPLE%.cpp
echo %CC% %CC_FLAGS% /Fe%DESTDIR%%SAMPLE%.exe %DESTDIR%%SAMPLE%.obj %DESTDIR%shared\*.obj %LINK_FLAGS%
%CC% %CC_FLAGS% /Fe%DESTDIR%%SAMPLE%.exe %DESTDIR%%SAMPLE%.obj %DESTDIR%shared\*.obj %LINK_FLAGS%
echo xcopy %SRCDIR%* %DESTDIR% /I /Y /C /Q /R /EXCLUDE:.tmp_exclude_list
xcopy %SRCDIR%* %DESTDIR% /I /Y /C /Q /R /EXCLUDE:.tmp_exclude_list
if exist %DESTDIR%%SAMPLE%.exe (echo %SAMPLE% build passed) else (echo %SAMPLE% build failed)
goto eof

:run
shift
set args=

if /i "%1"=="debug" (
  set DESTDIR=Debug\
  shift
) else if /i "%1"=="release" (
  set DESTDIR=Release\
  shift
)


if "%DESTDIR%"=="" set DESTDIR=Release\
if not exist "%DESTDIR%" (
    echo Build Directory %DESTDIR% doesn't exist
    goto :helpmsg
)

:collectargs
if /i NOT "%~1"=="" (
	set args=%args% %1
	shift
	goto collectargs
)
echo %DESTDIR%%SAMPLE%.exe %args%
%DESTDIR%%SAMPLE%.exe %args%
goto eof

:helpmsg
echo "Syntax: build [debug|release|run|clean]"
echo "     build debug - Building %SAMPLE% sample in Debug configuration"
echo "     build release - Building %SAMPLE% sample in Release configuration"
echo "     build run [release|debug] ["workload=%WORKLOADS%"] - run %SAMPLE% sample"
echo "     build clean - clean build directory"
goto eof

:clean
echo removing files...
rmdir /Q /S %DESTDIR% 2>nul

:eof
