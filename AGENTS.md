# Faderbay — Agent / Context Guide

> Single source of truth for AI assistants (Claude Code, OpenCode, Warp).
> `CLAUDE.md` and `WARP.md` are symlinks to this file — edit only `AGENTS.md`.

## What this is

Faderbay is a **MIDI/CV fader-bay controller** (standard potentiometer faders,
not motorized). This repo holds the
firmware plus hardware (KiCad), CAD, and documentation. The firmware targets an
**STM32F405 (Cortex-M4F)** and is the only part that builds.

Status: firmware **in progress / not finished** (see `README.md`).

## Repository layout

```
Firmware/Faderbay/     ← the firmware (all build/dev work happens here)
Hardware/Kicad/         ← PCB, schematics (rev0.1)
CAD/                    ← mechanical / enclosure (untracked WIP)
Documentation/          ← datasheets, pinout planning, spec sheets
Images/                 ← concept + render images
Legacy/                 ← old material, don't build on it
Tools/                  ← misc helper files (e.g. clickup-ids.md)
```

## Firmware architecture (`Firmware/Faderbay/`)

Layered, **cooperative scheduler** — no RTOS. `main.c` runs CubeMX peripheral
init, then `Device_Init()`, then a bare `while(1) { Scheduler_Run(); }`.
Each module exposes an `*_Init()` and a periodic `*_Process()` registered with
`Scheduler_RegisterTask(name, fn, period_ms)`.

Layers (top → bottom):

- **App/** — application logic: `app_state`, `fader_control`, `midi_control`,
  `ui_control`. This is where behavior lives.
- **Drivers/** — device drivers: `fader`, `mux`, `midi`, `display`, `encoder`,
  `button`. Hardware-facing but peripheral-agnostic where possible.
- **ResourceMgr/** — MCU peripheral wrappers: `adc`, `spi`, `uart`, `gpio`,
  `timer`, `scheduler`, `syscalls`.
- **Config/** — `config`, `faderbay_types.h` (shared enums: `fb_err_t`,
  `FaderMode_t`, `AppState_t`).
- **Core/**, **USB_DEVICE/** — STM32CubeMX-generated HAL + USB stack.
  Keep edits inside the `USER CODE BEGIN/END` guards; CubeMX regenerates the rest.
- **Lib/** — vendored git submodules:
  - `u8g2` — display graphics (ST7920 SPI, full-buffer mode)
  - `NanoLog` — lightweight logging over UART2 (`NLOG_UART_INIT`)

Key hardware bindings (from `Device_Init`): MIDI on UART5, log on USART2,
faders on ADC1 (DMA), display on SPI1, encoder on TIM3.

## Build & flash

Everything runs from `Firmware/Faderbay/`. Toolchain: **arm-none-eabi-gcc + Ninja
+ CMake**. Flashing: **OpenOCD + ST-Link**.

```bash
cd Firmware/Faderbay
./build.sh            # configure + build Debug → build/Debug/Faderbay.elf
./flash.sh            # build, then openocd program/verify/reset
```

CMake presets also exist (`Debug`, `Release`) via `CMakePresets.json`.
`compile_commands.json` is exported for **clangd** (see `.clangd`).

## Conventions & gotchas

- **New source files must be added to `CMakeLists.txt`** `target_sources()` — the
  build does not glob.
- Warnings are **strict** (`-Wall -Wextra -Wpedantic -Wshadow …`). Vendored libs
  (`u8g2`, `NanoLog`) are compiled with `-w` on purpose — don't try to fix their
  warnings. Our code should build clean.
- **`-Wstack-usage=1500`**: NanoLog's `vsnprintf` alone uses ~1.3 kB.
  **Never log from an ISR/IRQ callback** — it would push that onto the IRQ stack.
- Submodules: run `git submodule update --init --recursive` after clone.
- Header style: sections are marked with `// ===== INCLUDES / PUBLIC TYPES /
  PUBLIC FUNC. DECL. =====` banners — follow the existing pattern.
- Comments in the codebase are mixed Spanish/English; match the surrounding file.

## Commits

- **Everything git-related is in English** — commit messages, branch names, tags,
  and PR titles/descriptions. (Docs and Obsidian notes stay in Spanish.)
- **Keep commit messages short**: a single imperative subject line. Only add a
  body when strictly necessary, and keep it to a line or two.
- **No AI co-authorship** — never add `Co-Authored-By` (or any) AI trailers.

## Knowledge base (Obsidian vault "Mnemosyne")

Design knowledge — architecture, decisions, hardware, BOM, per-module designs —
lives in an **Obsidian vault**, not this repo:
`/home/vonmartins/Mnemosyne/Projects/Faderbay/`.

**Workflow:**
- **Read before deciding.** Before non-trivial design/architecture work, read the
  relevant note (`Decisiones de Diseño`, `Firmware Architecture`, `SD/<Module>`).
- **Update after building.** After a significant decision or architectural change,
  update the relevant note. Don't document everything — only decisions,
  architecture, and rationale (the *why*). Code stays the source of truth.

Full vault map, access method per tool, and conventions: **`Tools/obsidian-vault.md`**.
(Claude Code reads/edits the vault directly on disk; Warp/OpenCode use the Obsidian MCP.)

## Project management

Tasks are tracked in **Vikunja** (self-hosted, `vikunja.canllimona.cloud`), via
the `vikunja` MCP server. Project/label ID map lives in `Tools/vikunja-ids.md`.

- **Estructura plana**: **un solo proyecto** `Faderbay` (id 2) con **todas** las
  tareas dentro. No hay subproyectos; la clasificación (área, flujo, tipo) se
  lleva **solo con etiquetas**. Filtra/agrupa por etiqueta en las vistas
  (List/Table/Kanban) en vez de navegar jerarquía.
- **Estado/flujo**: `done` es un booleano de la tarea; el flujo (Next, Someday,
  In progress, Blocked…) se lleva con **buckets de la vista Kanban**, no con
  etiquetas. (Los antiguos labels `next`/`someday` se eliminaron el 2026-07-04.)
- **Área**: etiqueta `area:HW` / `area:FW` / `area:CAD` / `area:Admin`.
- **Subgrupo dentro del área**: etiqueta de "subproyecto" (`rev0.1`, `rev0.2`,
  `fw-hardening`, `carcasa`, `finanzas`, `aprendizaje`).
- **Incidencias**: prefijo `issue:<slug>` para agrupar varias cards del mismo tema
  (p. ej. `issue:midi-trs`).
- **Prioridad**: escala Vikunja 0–5 (Alta = 3).

> Migrado desde ClickUp el 2026-07-04. El workspace ClickUp queda como respaldo
> histórico; ya no se usa. `Tools/clickup-ids.md` se conserva solo como referencia.

## Gestión de incidencias (issues)

> Protocolo para cualquier agente (incluido tú en sesiones futuras). Aplica a
> **todo el proyecto**: firmware, hardware, CAD, negocio y admin. Una incidencia
> puede ser un bug, un defecto de hardware, una tarea de investigación, etc. El
> **área** se especifica dentro; el marco es el mismo para todas.

Tres piezas coordinadas:
- **Vikunja** = estado. Cada incidencia es una tarea en el proyecto `Faderbay`,
  con la etiqueta de **área** (`area:HW / area:FW / area:CAD / area:Admin`), la
  etiqueta `bug` si es un defecto, y un enlace a su nota de Obsidian en la
  descripción.
- **Obsidian** = cuaderno técnico. Cada incidencia tiene una nota en
  `Projects/Faderbay/Issues/<nombre-corto>.md`: un **log cronológico acumulativo**
  por timestamps hasta llegar a una conclusión.
- **Git** = trabajo en el repo. Cuando la incidencia implica cambios en el repo,
  se trabaja en una rama `fix/<nombre-corto>` o `issue/<nombre-corto>`. Para
  áreas sin código (p. ej. Negocio/Admin) la rama puede no aplicar.

### a) Incidencia nueva
1. Crear tarea en Vikunja (proyecto `Faderbay`) con la etiqueta `area:<X>` y, si es
   un defecto, la etiqueta `bug`. Título descriptivo; prefija `BUG:` si es un defecto.
2. Crear la nota `Issues/<nombre-corto>.md` con el frontmatter y la estructura de
   plantilla (ver `Tools/obsidian-vault.md`).
3. Enlazar la nota de Obsidian desde la descripción de la tarea de Vikunja.
4. Si aplica, crear (o proponer) la rama `fix/<nombre-corto>`.

### b) Mientras está abierta
- Cada sesión o avance relevante se añade a la nota como **entrada nueva con
  timestamp** (`YYYY-MM-DD HH:MM`). **Nunca sobrescribas** entradas anteriores:
  es un log acumulativo, no un resumen que se reescribe.
- Cada entrada documenta: qué se probó/verificó, resultado (confirma o descarta
  hipótesis) y próximo paso.
- Principio del proyecto: **una hipótesis a la vez, verificar antes de actuar.**

### c) Al listar o retomar
1. Consulta primero **Vikunja** para ver qué incidencias están abiertas.
2. Lee la **nota de Obsidian** correspondiente para recuperar todo el contexto
   **antes** de proponer ningún cambio.

### d) Al resolver
1. Añade una sección **Resolución** en la nota con la causa raíz y el timestamp
   de cierre.
2. Cambia `status: abierta` → `status: resuelta` en el frontmatter.
3. Marca como **done** la tarea correspondiente en Vikunja.
4. Indica que la rama está **lista para merge**. No hagas merge automáticamente
   salvo indicación explícita.

Plantilla exacta de la nota: **`Tools/obsidian-vault.md`**. El espíritu es el
mismo que ya se usaba para las incidencias de hardware del vault
(`Bugs Rev 0.1.md`), unificado ahora bajo `Issues/`.
