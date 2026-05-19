#!/usr/bin/env bash
set -e

PROJECT="$(cd "$(dirname "$0")" && pwd)"

cmake -S "$PROJECT" -B "$PROJECT/build/Debug" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$PROJECT/cmake/gcc-arm-none-eabi.cmake" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build "$PROJECT/build/Debug"
