@ECHO OFF
setlocal
Rem ****************************************************************************
Rem Content:
Rem      Windows Build Script for Sample vec_samples
Rem
Rem Part of the vec_samples tutorial. See "Tutorial: Auto-vectorization"
Rem in the Intel(R) C++ Compiler Tutorials document
Rem ****************************************************************************

del *.o *.obj *.exe *.pdb > nul 2>&1
if ""%1"" == ""clean"" exit /b

set CFLAGS=/Qstd=c99 /fp:fast /Isrc
set NAME=vec_samples.exe

if ""%1"" == ""vec_report1"" (
    set CFLAGS=%CFLAGS% /O2 /Qopt-report:1 /Qopt-report-phase:vec /D NOFUNCCALL
    set NAME=vec_samples_report1.exe
) else if ""%1"" == ""vec_report2"" (
    set CFLAGS=%CFLAGS% /O2 /Qopt-report:2 /Qopt-report-phase:vec /D NOFUNCCALL
    set NAME=vec_samples_report2.exe
) else if ""%1"" == ""pointer_disam"" (
    set CFLAGS=%CFLAGS% /O2 /Qopt-report:2 /Qopt-report-phase:vec /D NOALIAS
    set NAME=vec_samples_pointer_disam.exe
) else if ""%1"" == ""align"" (
    set CFLAGS=%CFLAGS% /O2 /Qopt-report:4 /Qopt-report-phase:vec /D NOALIAS /D ALIGNED
    set NAME=vec_samples_align.exe
) else if ""%1"" == ""ipo"" (
    set CFLAGS=%CFLAGS% /O2 /Qipo /Qopt-report:2 /Qopt-report-phase:vec /D NOALIAS /D ALIGNED
    set NAME=vec_samples_ipo.exe
) else (
    set CFLAGS=%CFLAGS% /O1
    set NAME=%NAME%
)

@ECHO ON

icl /nologo %CFLAGS% /c src\*.c
icl %CFLAGS% /Fe:%NAME% *.obj

@ECHO OFF
endlocal