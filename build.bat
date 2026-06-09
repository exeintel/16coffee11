@echo off
title Building 16coffee11 v0.1
echo ============================================
echo  Building 16coffee11 v0.1
echo ============================================
echo.
echo [1/2] Compiling 16coffee11.dll ...
gcc -shared -o 16coffee11.dll sourse\lib.c -luser32 -lshell32 -Wl,--out-implib,lib16coffee11.a
if %errorlevel% neq 0 (
    echo FAILED: DLL compilation failed.
    pause
    exit /b 1
)
echo OK: 16coffee11.dll created.
echo.
echo [2/2] Compiling 16coffee11.exe ...
gcc -o 16coffee11.exe sourse\main.c -L. -l16coffee11
if %errorlevel% neq 0 (
    echo FAILED: EXE compilation failed.
    pause
    exit /b 1
)
echo OK: 16coffee11.exe created.
echo.
echo ============================================
echo  Build complete!
echo    16coffee11.dll
echo    16coffee11.exe
echo ============================================
pause
