@echo off

set SRCDIR=src\
set DESTDIR_DEBUG=debug\
set DESTDIR_RELEASE=release\
set SRCFILES=%SRCDIR%*.c
set TARGET=int_sin.exe

if "%1"=="release" (set FLAG_REL=/O2
  echo release building at %FLAG_REL% ...
  mkdir %DESTDIR_RELEASE% 2>nul
  icl /nologo %FLAG_REL% %SRCFILES% /Fo%DESTDIR_RELEASE% /Fe%TARGET%
  goto eof
  )
if "%1"=="debug" (set FLAG_DBG=/Zi /Od
  echo debug building at %FLAG_DBG% ...
  mkdir %DESTDIR_DEBUG% 2>nul
  icl /nologo %FLAG_DBG% %SRCFILES% /Fo%DESTDIR_DEBUG% /Fe%TARGET%
  goto eof
  )
if "%1"=="run" goto run
if "%1"=="clean" goto clean

:error
echo invalid entry
echo Syntax:
echo      build release- Edit batch file to set required opt level /O1 /O2 /O3 to FLAG_REL, by default /O2
echo      build debug- compile at /Zi /Od
echo	  build run - To run the application
echo      build clean - clean build directory
goto eof

:run
int_sin.exe
goto eof
:clean
echo removing files...
if exist "*.exe" del *.exe
if exist "*.pdb" del *.pdb
if exist "*.ilk" del *.ilk
rmdir /Q /S %DESTDIR_DEBUG% %DESTDIR_RELEASE% 2>nul

:eof

