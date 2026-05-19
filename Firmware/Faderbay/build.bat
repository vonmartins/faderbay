@echo off
setlocal

set CLT=C:\Development\ST\STM32CubeCLT_1.18.0
set PATH=%CLT%\GNU-tools-for-STM32\bin;%CLT%\Ninja\bin;%PATH%
set PROJECT=%~dp0
set PROJECT=%PROJECT:~0,-1%

cmake.exe -S "%PROJECT%" -B "%PROJECT%\build\Debug" -G Ninja ^
    -DCMAKE_TOOLCHAIN_FILE="%PROJECT%\cmake\gcc-arm-none-eabi.cmake" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_MAKE_PROGRAM="%CLT%\Ninja\bin\ninja.exe" ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

cmake.exe --build "%PROJECT%\build\Debug"
