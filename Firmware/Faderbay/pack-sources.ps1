# pack-sources.ps1
# Zips all user-owned .c sources listed in CMakeLists.txt and their headers.
# Run from anywhere — script resolves its own location.

$Root    = $PSScriptRoot
$OutFile = Join-Path (Split-Path (Split-Path $Root)) "faderbay_sources.zip"

# ── Sources (mirrors target_sources in CMakeLists.txt, user layers only) ──────
$Sources = @(
    # App layer
    "App/app_state.c"
    "App/fader_control.c"
    "App/midi_control.c"
    "App/ui_control.c"
    # Driver layer
    "Drivers/fader_driver.c"
    "Drivers/mux_driver.c"
    "Drivers/midi_driver.c"
    "Drivers/display_driver.c"
    "Drivers/encoder_driver.c"
    "Drivers/button_driver.c"
    # Resource Manager layer
    "ResourceMgr/scheduler.c"
    "ResourceMgr/adc.c"
    "ResourceMgr/spi.c"
    "ResourceMgr/uart.c"
    "ResourceMgr/gpio.c"
    "ResourceMgr/timer.c"
    # Config
    "Config/config.c"
    # NanoLog
    "Lib/NanoLog/nlog.c"
)

# ── Header directories (mirrors target_include_directories, user dirs only) ───
$HeaderDirs = @(
    "App"
    "Drivers"
    "ResourceMgr"
    "Config"
    "Lib/NanoLog"
)

# ── Collect all relative paths ─────────────────────────────────────────────────
$Files = [System.Collections.Generic.List[string]]::new()
$Files.Add("CMakeLists.txt")

foreach ($s in $Sources) {
    $Files.Add($s)
}

foreach ($dir in $HeaderDirs) {
    $abs = Join-Path $Root $dir
    if (Test-Path $abs) {
        Get-ChildItem -Path $abs -Filter "*.h" | ForEach-Object {
            $rel = $_.FullName.Substring($Root.Length).TrimStart('\', '/').Replace('\', '/')
            $Files.Add($rel)
        }
    }
}

# ── Validate ───────────────────────────────────────────────────────────────────
$missing = $Files | Where-Object { -not (Test-Path (Join-Path $Root $_)) }
if ($missing) {
    $missing | ForEach-Object { Write-Warning "Missing: $_" }
}

$present = @($Files | Where-Object { Test-Path (Join-Path $Root $_) })

# ── Zip ────────────────────────────────────────────────────────────────────────
if (Test-Path $OutFile) { Remove-Item $OutFile }

Push-Location $Root
try {
    Compress-Archive -Path $present -DestinationPath $OutFile -CompressionLevel Optimal
} finally {
    Pop-Location
}

Write-Host "Created: $OutFile"
Write-Host "Files  : $($present.Count)"
