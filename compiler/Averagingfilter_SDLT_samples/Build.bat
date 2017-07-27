@echo off

set EXEC=AveragingFilter.exe
set SRCDIR=src\
set DESTDIR=release\
set EXTRA_INCLUDE=
set EXTRA_LIB=
set BMP=res\
rem PRODUCT_NAME is only defined in Intel environment
rem if in Visual Studio environment
if /i "%PRODUCT_NAME%"=="" (
	echo "please use Intel(R) C++ Compiler 16.0 in Intel(R) Parallel Studio XE 2016 update 1 or newer"
	goto eof 
)
rem else if in Intel environment
if /i NOT "%PRODUCT_NAME%"=="" (
	set CC=icl
	set CC_FLAGS=/GS /Qrestrict /W3 /QxAVX /Gy /Zi /O2 /Qstd=c++11 /Oi /MD /D_CRT_SECURE_NO_WARNINGS
)

set LINK_FLAGS=/INCREMENTAL:NO /SUBSYSTEM:CONSOLE /MANIFEST:NO

if /i "%1"=="clean" goto clean
if /i "%1"=="run" goto run

:options
if "%1"=="" goto compile

if /i "%1"=="vec-report" set CC_FLAGS=%CC_FLAGS% /Qvec-report1
if /i "%1"=="perf_num" set CC_FLAGS=%CC_FLAGS% -D PERF_NUM
shift
goto options

:compile
mkdir %DESTDIR% 2>nul
echo on
%CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% %SRCDIR%*.cpp /link %LINK_FLAGS% %EXTRA_LIB% /out:%DESTDIR%%EXEC%
@echo off
goto eof

:run
%DESTDIR%%EXEC% %BMP%nahelam.bmp %BMP%nahelam_out.bmp
goto eof

:clean
echo removing files...
rmdir /Q /S %DESTDIR% 2>nul
:eof
