# ClickUp ID Map — Faderbay Workspace

Quick reference for Claude Code sessions. Use `clickup-cli` (alias `clkup`) for all operations.
See skill at `~/.claude/skills/clickup/SKILL.md` for command reference.

---

## Workspace

| Name | ID |
|---|---|
| Workspace | `90121856073` |

---

## Spaces

| Space | ID |
|---|---|
| Faderbay | `90128114539` |
| Negocio | `90128114541` |

---

## Space: Faderbay — Folders & Lists

| Folder | Folder ID | List | List ID |
|---|---|---|---|
| Hardware | `901212010246` | Rev 0.1 (bring-up) | `901219126473` |
| Hardware | `901212010246` | Rev 0.2 | `901219126475` |
| Firmware | `901212010247` | FW Hardening | `901219126476` |
| Producto y lanzamiento | `901212010248` | Carcasa y mecánica | `901219126479` |
| Producto y lanzamiento | `901212010248` | Go-to-market | `901219126480` |

---

## Space: Negocio — Lists (sin folder)

| List | List ID |
|---|---|
| Finanzas | `901219126481` |
| Aprendizaje | `901219126482` |
| Ideas / futuros productos | `901219126483` |

---

## Custom Fields (shared across all lists)

### Área — `050cab3d-c5ac-43f6-b230-c53440b4fcbf`

| Opción | UUID |
|---|---|
| HW | `5f13ce38-3582-4375-bc12-a8cdd3e65917` |
| FW | `8a5d304b-90e0-49a0-a17a-8a10fdd94f8e` |
| CAD | `e34456f8-b684-492f-acaf-8e805d7c5e5a` |
| Negocio | `9bcd8271-a629-4798-bad7-4b3515180621` |
| Admin | `303ced95-41bf-47cf-baa5-e3eafa7545a3` |

### Esfuerzo — `b18735ce-507a-4609-888e-1f3cb67e5b5b`

| Opción | UUID |
|---|---|
| Un rato | `ea1d3bac-c1e9-4738-8fbf-fb75c78986bb` |
| Una tarde | `123b6103-5fd0-4f44-8935-bfabe1f7aec8` |
| Varias sesiones | `a8ca6dfa-b152-4943-8997-71f72063202e` |

> Para dropdowns, `clickup-cli field set` necesita el UUID de la opción, no el nombre.
> Ejemplo: `clickup-cli field set 050cab3d-c5ac-43f6-b230-c53440b4fcbf --value 8a5d304b-90e0-49a0-a17a-8a10fdd94f8e TASK_ID`

---

## Statuses

`Someday` · `Next` · `In Progress` · `Blocked` · `Done`

## Prioridades

`1`=Urgent · `2`=High · `3`=Normal · `4`=Low
