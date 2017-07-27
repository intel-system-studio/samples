@echo off
SETLOCAL

rem CommandPromptType is set in both VS and Intel, but not in normal DOS environment
if /i "%VCINSTALLDIR%"=="" (
	echo "environment not set up (needs Intel environment)"
	goto eof
)

set EXEC=matrix.exe
set SAMPLENAME=Matrix
set SRCDIR=src\
rem default is release build
set DESTDIR=release\

rem PRODUCT_NAME is only defined in Intel environment
rem if in Visual Studio environment
if /i "%PRODUCT_NAME%"=="" (
	echo "environment not set up (needs Intel environment)"
	goto eof
)

rem else if in Intel environment
if /i NOT "%PRODUCT_NAME%"=="" (
	set CC=icl
	set LINKER=xilink
)

set LINK_FLAGS=/INCREMENTAL:NO /SUBSYSTEM:CONSOLE /MANIFEST:NO


if /i "%1"=="clean" goto clean
if /i "%1"=="run" goto run
if /i "%1"=="help" goto help
if /i "%1"=="debug" goto debug
if /i "%1"=="release" goto release

rem default is release build
goto release

:debug
set DESTDIR=debug\
set CC_FLAGS=/c /ZI /nologo /W1 /Od /Qftz- /D WIN32 /D _DEBUG /D _CONSOLE /D _UNICODE /D UNICODE /EHsc /MDd /GS /Zc:wchar_t /Zc:forScope /TP
set EXTRA_INCLUDE=
set EXTRA_LIB=
goto compile_and_link

:release
set DESTDIR=release\
set CC_FLAGS=/c /Zi /nologo /W1 /O2 /Qipo /Qftz /D WIN32 /D NDEBUG /D _CONSOLE /D _UNICODE /D UNICODE /EHsc /MD /GS /Zc:wchar_t /Zc:forScope /TP 
set EXTRA_INCLUDE=
set EXTRA_LIB=

:compile_and_link
mkdir %DESTDIR% 2>nul
echo on
%CC% %CC_FLAGS% %EXTRA_INCLUDE% /Fo%DESTDIR% /Fd%DESTDIR% %SRCDIR%*.cpp 
%LINKER% %LINK_FLAGS% %EXTRA_LIB% %DESTDIR%*.obj /out:%DESTDIR%%EXEC%
@echo off
goto eof

:run
%DESTDIR%%EXEC% %2
goto eof

:help
echo "Syntax: build [debug|release|run|clean]"
echo "     build debug - Build %SAMPLENAME% without optimization"
echo "     build release - Build %SAMPLENAME% with optimization"
echo "     build run - Run %SAMPLENAME%"
echo "     build clean - Clean build directory"
goto eof

:clean
echo removing files...
rmdir /Q /S %DESTDIR% 2>nul

:eof
ENDLOCAL
