# Obsidian Vault — Faderbay Knowledge Base

Quick reference for AI sessions. The project's design knowledge (architecture,
decisions, hardware, BOM, per-module designs) lives in an **Obsidian vault**,
not in the repo. Read the relevant note **before** non-trivial work; update it
**after** significant decisions or architectural changes.

- **Vault name:** `Mnemosyne`
- **Project folder:** `/home/vonmartins/Mnemosyne/Projects/Faderbay/`

> ⚠️ **Code is the source of truth.** Some notes (especially `SD/`) went stale
> after the Fase 1/2 refactors. When a note and the code disagree, trust the
> code — and fix the note.

---

## How to access it (differs per tool)

| Tool | Access method |
|---|---|
| **Claude Code** | Direct filesystem — Read/Edit/Write on the paths above. (No Obsidian MCP configured here.) |
| **Warp / OpenCode** | Obsidian MCP (Local REST API plugin) — host `127.0.0.1`, port `27124`. |

Both point at the same files, so edits from either are consistent.

---

## Vault map (`Projects/Faderbay/`)

### Index
- `README.md` — project overview, specs table, and links to everything below.

### Hardware
- `Hardware.md` — schematic, components, circuitry
- `Power Supply.md` — power design
- `BOM.md` — bill of materials (LCSC-verified)
- `Bugs Rev 0.1.md` — HW bugs found during rev 0.1 bring-up

### Firmware
- `Firmware.md` — stack, peripherals, main modules
- `Firmware Architecture.md` — layered architecture (App / Drivers / ResourceMgr)
- `Code Review FW.md` — full review findings + Fase 0–2 log
- `Roadmap FW Hardening.md` — firmware hardening / professionalization plan
- `Prompts Claude Code.md` — reusable prompts

### Project
- `Backlog.md` — pending tasks and improvements
- `Decisiones de Diseño.md` — **design decision log** (rationale for choices made)
- `Desarrollo de Producto.md` — product phases (concept → production)
- `Design.md` — industrial / mechanical / UX design

### Per-module software designs — `SD/`
One note per firmware module, following `SD/_Template_Module.md`
(sections: Overview, Contexto, API Pública, Funciones Privadas, Variables
Internas, Notas de Implementación). Modules: `Scheduler`, `ADC`, `SPI`, `UART`,
`GPIO`, `Timer`, `FaderDriver`, `MuxDriver`, `MidiDriver`, `DisplayDriver`,
`EncoderDriver`, `ButtonDriver`, `FaderControl`, `MidiControl`, `UIControl`,
`AppState`, `Main`, `FlashStorage`.

### Incidencias — `Issues/`
One note per project incidence (bug, hardware defect, investigation…), following
the template below. **Project-wide**, not FW-only: the area (`HW/FW/CAD/Negocio/
Admin`) is declared in the frontmatter. Chronological, append-only log. See the
full protocol (ClickUp + Obsidian + Git) in `AGENTS.md` → "Gestión de incidencias".

---

## Plantilla de incidencia (`Issues/<nombre-corto>.md`)

Copia esta plantilla al abrir una incidencia nueva. Es un **log cronológico
acumulativo**: se añaden entradas con timestamp, nunca se sobrescriben.

```markdown
---
type: issue
area: <HW | FW | CAD | Negocio | Admin>
status: abierta          # abierta | resuelta
clickup: <enlace a la tarea de ClickUp>
branch: fix/<nombre-corto>   # si aplica trabajo en el repo; si no, dejar vacío
created: <YYYY-MM-DD>
updated: <YYYY-MM-DD>
---

# <título de la incidencia>

## Contexto
[Descripción inicial: síntoma y cómo se reproduce o en qué consiste]

## Evolución
### <YYYY-MM-DD HH:MM>
[qué se probó/verificó, resultado (confirma o descarta hipótesis), próximo paso]

## Resolución
[causa raíz — se rellena al cerrar, con el timestamp de cierre]
```

Reglas:
- **Una entrada por sesión/avance**, con timestamp `YYYY-MM-DD HH:MM`. No borres
  ni reescribas entradas anteriores.
- Al cerrar: rellena **Resolución**, pon `status: resuelta`, sube `updated`,
  cierra la tarea de ClickUp y deja la rama lista para merge (no mergees solo).
- Sustituye el histórico de bugs de hardware del vault (`Bugs Rev 0.1.md`) de
  aquí en adelante; ese queda como registro previo.

---

## When to read

Before you decide or design — not for every edit. Read the relevant note when:
- Making an **architectural / design decision** → check `Decisiones de Diseño`,
  `Firmware Architecture`, and the module's `SD/<Module>` note.
- Touching a specific module → skim `SD/<Module>` for intent and constraints.
- Hardware/power/BOM questions → the Hardware notes above.

## When to update

Not everything gets documented — **only decisions, architecture, and rationale.**
After finishing a change, update the vault when you:
- Made a **design decision** worth remembering → append to `Decisiones de Diseño`.
- Changed **architecture or a module's public API/behavior** → update
  `Firmware Architecture` and/or the relevant `SD/<Module>` note.
- Opened/worked/closed an incidence (any area) → its `Issues/<nombre-corto>` note
  (see the template above and the protocol in `AGENTS.md`).

Keep implementation minutiae in code + comments; keep the *why* in the vault.

### Note conventions
- YAML frontmatter: `type`, `status`, `created`, `updated` (ISO date).
  Bump `updated` when you edit.
- Link between notes with wiki-links: `[[Note Name]]`.
- Match the existing note's language (mostly Spanish) and structure.
- New module design → copy `SD/_Template_Module.md`.
