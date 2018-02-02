@echo off

set EXEC=FluidAnimate.exe
set EXEC_PGO=FluidAnimatePGO.exe
set SRCDIR=src\
set DESTDIR=release\
set EXTRA_INCLUDE=
set EXTRA_LIB=

rem PRODUCT_NAME is only defined in Intel environment
rem if in Visual Studio environment
if /i "%PRODUCT_NAME%"=="" (
	set CC=cl
	set CC_FLAGS=/Ox /GL /fp:fast /EHsc /MD
	set SRCFILES=%SRCDIR%cell_pool.cpp %SRCDIR%fluid_animate.cpp %SRCDIR%IO.cpp %SRCDIR%main.cpp %SRCDIR%timer.cpp
)

rem else if in Intel environment
if /i NOT "%PRODUCT_NAME%"=="" (
	set CC=icl
	set CC_FLAGS=/O2 /Oi /fp:fast /Qipo /MD /EHsc 
	set SRCFILES=%SRCDIR%cell_pool.cpp %SRCDIR%fluid_animate.cpp %SRCDIR%IO.cpp %SRCDIR%main.cpp %SRCDIR%timer.cpp %SRCDIR%fluid_animateAN.cpp %SRCDIR%cell_poolAN.cpp
)

set LINK_FLAGS=/INCREMENTAL:NO /SUBSYSTEM:CONSOLE /MANIFEST:NO

if /i "%1"=="clean" goto clean
if /i "%1"=="run" goto run
if /i "%1"=="help" goto helpmsg

:options
if "%1"=="" goto compile
if /i "%1"=="pgo" goto compilepgo
if /i "%1"=="perf_num" set CC_FLAGS=%CC_FLAGS% -D PERF_NUM
shift
goto options

:compilepgo
set CC_FLAGS=/O2 /Oi /fp:fast /Qipo /MD /EHsc /Qprof-dir "release" /Qprof-gen
mkdir %DESTDIR% 2>nul
echo %CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCFILES% /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC%
%CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCFILES% /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC%

:runpgo
cd %DESTDIR%
%EXEC% -O 0
cd ..\

:compilepgo_use
set CC_FLAGS=/O2 /Oi /fp:fast /Qipo /MD /EHsc /Qprof-dir "release" /Qprof-use
mkdir %DESTDIR% 2>nul
echo %CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCFILES% /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC_PGO%
%CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCFILES% /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC_PGO%
echo PGO optimized application: "%DESTDIR%%EXEC_PGO%"
goto eof

:compile
mkdir %DESTDIR% 2>nul
echo %CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCFILES% /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC%
%CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCFILES% /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC%
goto eof

:run
shift
set args=
:collectargs
if /i NOT "%1"=="" (
	set args=%args% %1
	shift
	goto collectargs
)
cd %DESTDIR%
%EXEC% %args%
goto eof

:helpmsg
echo "Syntax: build [|pgo|run|clean]"
echo "     build pgo - Building FluidAnimate with PGO"
echo "     build - Building FluidAnimate with normally"
echo "     build run [-o {0|1|2}] - run FluidAnimate"
echo "     build clean - clean build directory"
goto eof

:clean
echo removing files...
rmdir /Q /S %DESTDIR% 2>nul

:eof
