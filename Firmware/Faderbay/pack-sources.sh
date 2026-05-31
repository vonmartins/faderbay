#!/usr/bin/env bash
# Zips all user-owned .c sources listed in CMakeLists.txt and their headers.
# Run from anywhere — script resolves its own location.

ROOT="$(cd "$(dirname "$0")" && pwd)"
OUT_FILE="$(dirname "$(dirname "$ROOT")")/faderbay_sources.zip"

SOURCES=(
    App/app_state.c
    App/fader_control.c
    App/midi_control.c
    App/ui_control.c
    Drivers/fader_driver.c
    Drivers/mux_driver.c
    Drivers/midi_driver.c
    Drivers/display_driver.c
    Drivers/encoder_driver.c
    Drivers/button_driver.c
    ResourceMgr/scheduler.c
    ResourceMgr/adc.c
    ResourceMgr/spi.c
    ResourceMgr/uart.c
    ResourceMgr/gpio.c
    ResourceMgr/timer.c
    Config/config.c
    Lib/NanoLog/nlog.c
)

HEADER_DIRS=(
    App
    Drivers
    ResourceMgr
    Config
    Lib/NanoLog
)

FILES=(CMakeLists.txt)

for s in "${SOURCES[@]}"; do
    FILES+=("$s")
done

for dir in "${HEADER_DIRS[@]}"; do
    abs="$ROOT/$dir"
    if [[ -d "$abs" ]]; then
        while IFS= read -r -d '' h; do
            FILES+=("${h#"$ROOT/"}")
        done < <(find "$abs" -maxdepth 1 -name "*.h" -print0)
    fi
done

missing=0
present=()
for f in "${FILES[@]}"; do
    if [[ -f "$ROOT/$f" ]]; then
        present+=("$f")
    else
        echo "Warning: Missing: $f" >&2
        missing=1
    fi
done

[[ -f "$OUT_FILE" ]] && rm "$OUT_FILE"

cd "$ROOT"
zip -9 "$OUT_FILE" "${present[@]}"

echo "Created: $OUT_FILE"
echo "Files  : ${#present[@]}"
