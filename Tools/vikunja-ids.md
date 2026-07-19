# Vikunja ID Map — Faderbay

Referencia rápida para sesiones de agentes. Gestión de tareas vía el MCP
`vikunja` (instancia self-hosted en `vikunja.canllimona.cloud`).

> Migrado desde ClickUp el 2026-07-04. Sustituye a `Tools/clickup-ids.md`
> (conservado solo como respaldo histórico).

## Instancia

| Campo | Valor |
|---|---|
| URL API | `https://vikunja.canllimona.cloud/api/v1` |
| MCP | servidor `vikunja` (scope de usuario, `~/.claude.json`) |

## Proyectos

**Estructura plana**: todas las tareas viven en un único proyecto `Faderbay`.
Ya no se usan subproyectos; la clasificación se lleva **solo con etiquetas**.

| ID | Proyecto |
|---|---|
| 2 | Faderbay (todas las tareas) |
| 1 | Inbox (por defecto de Vikunja) |

> Aplanado el 2026-07-04: los antiguos subproyectos (Hardware, Firmware,
> Producto, Rev 0.1/0.2, FW Hardening, Carcasa, Go-to-market, Negocio, Finanzas,
> Aprendizaje, Ideas) se borraron tras mover sus 37 tareas a `Faderbay`. La
> distinción HW/FW/CAD/Admin y de negocio se hace con etiquetas.

## Etiquetas (globales)

| ID | Etiqueta | Uso |
|---|---|---|
| 3  | `bug` | incidencia/defecto (protocolo de issues) |
| 4  | `area:HW` | área hardware |
| 5  | `area:FW` | área firmware |
| 6  | `area:CAD` | área mecánica/carcasa |
| 7  | `area:Admin` | área negocio/administración |
| 8  | `rev0.1` | subgrupo HW: bring-up rev 0.1 |
| 9  | `rev0.2` | subgrupo HW: rev 0.2 |
| 10 | `fw-hardening` | subgrupo FW: endurecimiento firmware |
| 11 | `carcasa` | subgrupo CAD: carcasa y mecánica |
| 12 | `finanzas` | subgrupo Admin: finanzas |
| 13 | `aprendizaje` | subgrupo: temas de aprendizaje |
| 14 | `issue:midi-trs` | agrupa cards de la incidencia MIDI TRS |

> **Estado/flujo va por buckets de Kanban** (Next, Someday, In progress…), no
> por etiquetas. Los antiguos labels `next`/`someday` se borraron el 2026-07-04.
> Para agrupar varias cards de una misma incidencia, usa el prefijo `issue:<slug>`.

## Convenciones de mapeo (desde ClickUp)

- **Estado** `Done` → `task.done = true`. `Next`/`Someday`/… → etiqueta.
- **Prioridad** ClickUp → Vikunja (0–5): Urgent=4, High=3, Normal=0 (sin fijar), Low=1.
- **Campo Área** (custom field) → etiqueta `area:<X>`.
- **Incidencias/bugs** → etiqueta `bug` + enlace a la nota de Obsidian
  (`Issues/<nombre-corto>.md`) en la descripción.
