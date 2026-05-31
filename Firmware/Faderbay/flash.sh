#!/usr/bin/env bash
set -e

PROJECT="$(cd "$(dirname "$0")" && pwd)"

"$PROJECT/build.sh"

openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
    -c "program $PROJECT/build/Debug/Faderbay.elf verify reset exit"
