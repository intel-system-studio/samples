@echo off

if /i "%1"=="clean" goto clean
if /i "%1"=="run" goto run
if /i "%1"=="help" goto help
if /i "%1"=="usage" goto help

rem PRODUCT_NAME is only defined in Intel environment
if /i "%PRODUCT_NAME%"=="" (
	echo   This program is intended for use with the Intel C++ Compiler. 
	echo   Please run the setup file at YOUR_INTEL_COMPILER_INSTALL_DIRECTORY\windows\bin\compilervars.bat. 
	echo   By default, this is "C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\bin\compilervars.bat"
	goto eof
)

set EXEC=roofline_demo.exe
set SRCDIR=src\
set DESTDIR=release\
set EXTRA_INCLUDE=
set EXTRA_LIB=

set LINK_FLAGS=/INCREMENTAL:NO /SUBSYSTEM:CONSOLE /MANIFEST:NO

set CC=icl
set CC_FLAGS=/Zi /O2 /arch:CORE-AVX2 /Qopenmp
set USE_INTEL=1

:compile
mkdir %DESTDIR% 2>nul
echo on
%CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCDIR%*.cpp /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC%
@echo off
goto eof

:run
%DESTDIR%%EXEC% %2
goto eof

:help
echo USAGE:
echo to compile:   build
echo to clean:     build clean
echo to run:       build run
echo.
goto eof

:clean
echo removing files...
rmdir /Q /S %DESTDIR% 2>nul

:eof

