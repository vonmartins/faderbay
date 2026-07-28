"""Carcasa Faderbay — estructura base de las dos piezas (base + tapa).

Iteración de cascarón: solo el volumen exterior, el vaciado interior, los postes
de apoyo de la PCB (sin taladrar) y el encaje entre piezas (step joint / shiplap).
SIN recortes de conectores (faders, display, botones, encoder, jacks, switch,
USB-C) ni taladros. Objetivo: validar proporciones, plano de partición y encaje.

Sistema de coordenadas: el mismo que el STEP de la PCB (X centrado en 0, cara
inferior de la placa en Z=0, cara superior en Z=PCB_THICKNESS). Así los recortes
de la próxima iteración usarán directamente las coordenadas de inspect_pcb.py.

El USB-C no está en este diseño: su recorte se cotará a mano más adelante.
"""

import math
import os

from build123d import (
    Axis,
    Box,
    Compound,
    Cylinder,
    Keep,
    Plane,
    Pos,
    Rot,
    SlotOverall,
    export_step,
    extrude,
    fillet,
    import_step,
    split,
)

# ============================================================
#   PARÁMETROS
# ============================================================

# --- Cotas medidas con inspect_pcb.py (frame del STEP, en mm) ---
PCB_THICKNESS = 1.51          # grosor de la placa
ENV_X_MIN, ENV_X_MAX = -180.0, 180.0   # envolvente de componentes en X
ENV_Y_MIN, ENV_Y_MAX = -65.0, 80.91    # en Y (el rebose por +Y es el interruptor)
ENV_Z_MAX = 26.81             # componente más alto (encoder), Z absoluto
PIN_Z_MIN = -3.01             # pin más profundo por debajo de la placa
BOARD_Y_MAX = 75.0            # borde real de la placa en +Y (para apoyar postes)

# --- Cotas medidas con el pie de rey sobre la faderbay real (mm) ---
DISPLAY_TOP_OVER_BOARD = 14.0   # de la PCB a lo más alto del display (el más alto que va cerrado)
DISPLAY_Y = 50.7                # posición Y (centro) del display
JACK_TIP_PROTRUSION = 1.6       # saliente de la punta de plástico del minijack hacia afuera
USBC_TIP_PROTRUSION = 1.6       # saliente de la punta del USB-C hacia afuera
SWITCH_TIP_PROTRUSION = 1.9     # saliente del interruptor on/off hacia afuera

# Ruta del STEP y mapeo modelo 3D -> componente (para derivar posiciones reales).
STEP_PATH = "Hardware/Kicad/rev0.2/faderbay.step"
MODEL_TO_COMPONENT = {
    "pta6043-2x10cixxxx": "Fader",
    "EC11E15204A3": "Encoder",
    "NHD-14432WG-BTFH-V_T": "Display",
    "PJ-320B": "Jack",
    "430182070816": "Boton",
    "RA1H1C112R": "Switch",
}

# --- Espesores de material ---
wall_thickness = 3.0          # pared lateral
floor_thickness = 3.0         # suelo de la base
top_thickness = 3.0           # techo de la tapa

# --- Holguras interiores ---
interior_clearance_xy = 1.0   # holgura por lado entre envolvente y pared interior
under_board_clearance = 4.0   # hueco bajo la placa (debe superar |PIN_Z_MIN|)
clearance_over_board = 9.0    # altura libre delante sobre la placa (faders asoman;
                              # encoder y faders atraviesan el techo, no la marcan)

# --- Plano de partición (horizontal), Z absoluto ---
partition_z = 7.5             # altura del corte base/tapa (≈ board_top + 6)

# --- Encaje entre piezas (step joint / shiplap) ---
lip_height = 5.0              # altura del labio que sube desde la partición
lip_inset = wall_thickness / 2.0   # espesor del labio = mitad interior de la pared
fit_tolerance = 0.15         # holgura de deslizamiento labio/rebaje (press-fit MJF)

# --- Fillets (radios) ---
fillet_vertical = 6.0        # aristas verticales exteriores
fillet_top = 3.0             # canto superior exterior
fillet_bottom = 1.0          # canto inferior exterior
interior_fillet = 3.0        # esquinas verticales de la cavidad

# --- Inclinación del plano superior (cuña ergonómica) ---
top_slope_angle = 5.0        # grados; el plano sube hacia +Y (parte trasera)

# --- Recortes de conectores (dimensiones datasheet + holgura) ---
fader_slot_width = 6.0       # ancho de la ranura del fader (eje X)
fader_slot_length = 65.0     # largo de la ranura del fader (eje Y): recorrido + mando
encoder_hole_dia = 8.0       # agujero del eje del encoder (techo)
button_hole_dia = 6.0        # agujero del pulsador (techo)
display_window_w = 62.0      # ventana visible del display (X)  [confirmar datasheet]
display_window_h = 20.0      # ventana visible del display (Y)  [confirmar datasheet]
jack_hole_dia = 6.5          # agujero del jack 3.5mm (pared trasera)
switch_cutout_w = 19.0       # recorte del interruptor (X)      [confirmar datasheet]
switch_cutout_h = 13.0       # recorte del interruptor (Z)      [confirmar datasheet]

# --- Postes de apoyo de la PCB (sin taladrar todavía) ---
post_size = 8.0              # lado del poste cuadrado
post_inset = 6.0             # separación de las esquinas de la placa
# Posiciones (X, Y) de los 4 postes. Provisionales: en la iteración de taladros
# se alinearán con los agujeros de montaje reales detectados por inspect_pcb.py.
POST_POSITIONS = [
    (ENV_X_MIN + post_inset, ENV_Y_MIN + post_inset),
    (ENV_X_MAX - post_inset, ENV_Y_MIN + post_inset),
    (ENV_X_MIN + post_inset, BOARD_Y_MAX - post_inset),
    (ENV_X_MAX - post_inset, BOARD_Y_MAX - post_inset),
]

# --- Visor ---
explode_gap = 60.0           # separación en Z de la tapa en vista abierta (solo visor)
exploded_view = False        # True: tapa separada; False: caja cerrada (montada)
show_pcb = True              # incluir la PCB importada dentro de la caja

# --- Salida ---
OUTPUT_DIR = "CAD/output"

# ============================================================
#   GEOMETRÍA DERIVADA
# ============================================================

assert under_board_clearance > abs(PIN_Z_MIN), (
    "under_board_clearance no deja pasar los pines de la placa"
)

# Límites de la cavidad interior.
interior_x_min = ENV_X_MIN - interior_clearance_xy
interior_x_max = ENV_X_MAX + interior_clearance_xy
interior_y_min = ENV_Y_MIN - interior_clearance_xy
interior_y_max = ENV_Y_MAX + interior_clearance_xy
interior_z_min = -under_board_clearance                 # suelo interior
interior_z_max = PCB_THICKNESS + clearance_over_board   # techo interior

# Límites del volumen exterior.
outer_x_min = interior_x_min - wall_thickness
outer_x_max = interior_x_max + wall_thickness
outer_y_min = interior_y_min - wall_thickness
outer_y_max = interior_y_max + wall_thickness
outer_z_min = interior_z_min - floor_thickness
outer_z_max = interior_z_max + top_thickness

# Inclinación del techo: subida del borde trasero (+Y) respecto al delantero.
slope = math.tan(math.radians(top_slope_angle))
slope_rise = slope * (outer_y_max - outer_y_min)
back_top_z = outer_z_max + slope_rise          # cota exterior superior atrás
back_ceiling_z = interior_z_max + slope_rise   # techo interior atrás

# Comprobación de altura: el display (lo más alto que va cerrado) debe caber bajo
# el techo inclinado en su posición Y (la cuña sube atrás, que es donde está).
panel_over_board_at_display = clearance_over_board + slope * (DISPLAY_Y - outer_y_min)
assert panel_over_board_at_display >= DISPLAY_TOP_OVER_BOARD + 1.0, (
    "el techo no deja hueco para el display en su posición"
)


def box_from_bounds(x0, x1, y0, y1, z0, z1):
    """Crea una caja a partir de sus límites min/max (posicionada, no centrada)."""
    cx, cy, cz = (x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2
    return Pos(cx, cy, cz) * Box(x1 - x0, y1 - y0, z1 - z0)


def load_placements(asm):
    """Centros (X, Y, Z) de cada componente a partir del ensamblaje del STEP.
    Deduplica instancias solapadas (p. ej. el display trae dos sólidos)."""
    raw = {}
    for child in asm.children:
        name = MODEL_TO_COMPONENT.get(child.label)
        if name is None:
            continue
        bb = child.bounding_box()
        raw.setdefault(name, []).append((
            (bb.min.X + bb.max.X) / 2,
            (bb.min.Y + bb.max.Y) / 2,
            (bb.min.Z + bb.max.Z) / 2,
        ))
    out = {}
    for name, items in raw.items():
        seen = {}
        for cx, cy, cz in items:
            key = (round(cx), round(cy))
            if key not in seen or cz > seen[key][2]:
                seen[key] = (cx, cy, cz)
        out[name] = list(seen.values())
    return out


# ============================================================
#   CONSTRUCCIÓN
# ============================================================

# 1) Sólido exterior: caja alta y plana (hasta la altura trasera). Fillets en
#    verticales y canto inferior; el superior se redondea al final, ya inclinado.
outer = box_from_bounds(
    outer_x_min, outer_x_max, outer_y_min, outer_y_max, outer_z_min, back_top_z
)
outer = fillet(outer.edges().filter_by(Axis.Z), fillet_vertical)   # verticales
outer = fillet(outer.edges().group_by(Axis.Z)[0], fillet_bottom)   # canto inferior

# 2) Plano superior inclinado (sube hacia +Y). El techo interior se corta con un
#    plano paralelo desplazado top_thickness hacia abajo -> losa de espesor
#    constante y más altura libre atrás (donde está el encoder).
top_plane = Plane(origin=(0, outer_y_min, outer_z_max), z_dir=(0, -slope, 1))
ceiling_plane = Plane(origin=(0, outer_y_min, interior_z_max), z_dir=(0, -slope, 1))
outer = split(outer, bisect_by=top_plane, keep=Keep.BOTTOM)

# 3) Cavidad interior (esquinas verticales redondeadas), techo inclinado, vaciado.
cavity = box_from_bounds(
    interior_x_min, interior_x_max,
    interior_y_min, interior_y_max,
    interior_z_min, back_ceiling_z,
)
cavity = fillet(cavity.edges().filter_by(Axis.Z), interior_fillet)
cavity = split(cavity, bisect_by=ceiling_plane, keep=Keep.BOTTOM)
shell = outer - cavity

# 4) Corte horizontal en partition_z -> base (abajo) y tapa (arriba).
base_raw = split(shell, bisect_by=Plane.XY.offset(partition_z), keep=Keep.BOTTOM)
lid_raw = split(shell, bisect_by=Plane.XY.offset(partition_z), keep=Keep.TOP)

# 5) Labio en la base: anillo de la mitad interior de la pared que sube lip_height.
lip = box_from_bounds(
    interior_x_min, interior_x_max,
    interior_y_min, interior_y_max,
    partition_z, partition_z + lip_height,
) - box_from_bounds(
    interior_x_min + lip_inset, interior_x_max - lip_inset,
    interior_y_min + lip_inset, interior_y_max - lip_inset,
    partition_z, partition_z + lip_height,
)

# 6) Postes de apoyo de la PCB: del suelo interior a la cara inferior de la placa.
posts = None
for px, py in POST_POSITIONS:
    post = box_from_bounds(
        px - post_size / 2, px + post_size / 2,
        py - post_size / 2, py + post_size / 2,
        interior_z_min, 0.0,
    )
    posts = post if posts is None else posts + post

base = base_raw + lip + posts

# 7) Rebaje en la tapa: hueco (labio + holgura) para recibir el labio de la base.
rebate = box_from_bounds(
    interior_x_min - fit_tolerance, interior_x_max + fit_tolerance,
    interior_y_min - fit_tolerance, interior_y_max + fit_tolerance,
    partition_z, partition_z + lip_height + fit_tolerance,
) - box_from_bounds(
    interior_x_min + lip_inset + fit_tolerance, interior_x_max - lip_inset - fit_tolerance,
    interior_y_min + lip_inset + fit_tolerance, interior_y_max - lip_inset - fit_tolerance,
    partition_z, partition_z + lip_height + fit_tolerance,
)
lid = lid_raw - rebate

# 8) Redondeo del canto superior sobre la cara inclinada de la tapa.
lid = fillet(lid.faces().sort_by(Axis.Z)[-1].edges(), fillet_top)

# ============================================================
#   RECORTES DE CONECTORES
# ============================================================
# Posiciones exactas del STEP; dimensiones de cada corte por parámetro.
# NO incluye USB-C (sin modelo 3D todavía; se cotará a mano más adelante).

pcb_step = import_step(STEP_PATH)
placements = load_placements(pcb_step)

# Cortes verticales que atraviesan el techo inclinado (ejes de componente = Z).
z_cut0 = partition_z
z_cut1 = back_top_z + 10.0


def top_slot(cx, cy):
    """Ranura tipo stadium a lo largo de Y, pasante por el techo."""
    section = SlotOverall(fader_slot_length, fader_slot_width, rotation=90)
    return Pos(cx, cy, z_cut0) * extrude(section, z_cut1 - z_cut0)


def top_hole(cx, cy, dia):
    return Pos(cx, cy, (z_cut0 + z_cut1) / 2) * Cylinder(dia / 2, z_cut1 - z_cut0)


def top_window(cx, cy, w, h):
    return Pos(cx, cy, (z_cut0 + z_cut1) / 2) * Box(w, h, z_cut1 - z_cut0)


# Cortes horizontales que atraviesan la pared trasera (+Y).
y_cut0 = interior_y_max - 2.0
y_cut1 = outer_y_max + 2.0
y_cut_mid = (y_cut0 + y_cut1) / 2


def back_hole(cx, cz, dia):
    return Pos(cx, y_cut_mid, cz) * Rot(90, 0, 0) * Cylinder(dia / 2, y_cut1 - y_cut0)


def back_window(cx, cz, w, h):
    return Pos(cx, y_cut_mid, cz) * Box(w, y_cut1 - y_cut0, h)


cutters = []
for cx, cy, cz in placements["Fader"]:
    cutters.append(top_slot(cx, cy))
for cx, cy, cz in placements["Encoder"]:
    cutters.append(top_hole(cx, cy, encoder_hole_dia))
for cx, cy, cz in placements["Boton"]:
    cutters.append(top_hole(cx, cy, button_hole_dia))
for cx, cy, cz in placements["Display"]:
    cutters.append(top_window(cx, cy, display_window_w, display_window_h))
for cx, cy, cz in placements["Jack"]:
    cutters.append(back_hole(cx, cz, jack_hole_dia))
for cx, cy, cz in placements["Switch"]:
    cutters.append(back_window(cx, cz, switch_cutout_w, switch_cutout_h))

cutter = Compound(children=cutters)
base = base - cutter
lid = lid - cutter

# ============================================================
#   SALIDA
# ============================================================

os.makedirs(OUTPUT_DIR, exist_ok=True)
base_path = os.path.join(OUTPUT_DIR, "base.step")
lid_path = os.path.join(OUTPUT_DIR, "lid.step")
export_step(base, base_path)
export_step(lid, lid_path)

print("Carcasa base generada.")
print(f"  Planta: {outer_x_max - outer_x_min:.2f} x {outer_y_max - outer_y_min:.2f} mm")
print(f"  Altura: {outer_z_max - outer_z_min:.2f} mm delante -> "
      f"{back_top_z - outer_z_min:.2f} mm detras  (cuña {top_slope_angle:.0f}°)")
print(f"  Partición en Z = {partition_z:.2f}  |  labio {lip_height:.1f} mm")
print(f"  Recortes: {len(placements['Fader'])} faders, {len(placements['Jack'])} jacks, "
      f"{len(placements['Boton'])} botones, {len(placements['Encoder'])} encoder, "
      f"{len(placements['Display'])} display, {len(placements['Switch'])} switch")
print(f"  Exportado: {base_path}, {lid_path}")

# Visor: caja cerrada (montada) o abierta (tapa separada en Z). La PCB se muestra
# en su sitio dentro de la caja. El desplazamiento es solo para la vista; los STEP
# exportados van siempre en posición montada.
objects = [base]
names = ["base"]
objects.append(Pos(0, 0, explode_gap) * lid if exploded_view else lid)
names.append("lid")
if show_pcb:
    objects.append(pcb_step)
    names.append("pcb")

try:
    from ocp_vscode import show
    show(*objects, names=names)
    view = "abierta" if exploded_view else "cerrada"
    print(f"Enviado al visor ocp_vscode (localhost:3939) — vista {view}"
          f"{' con PCB' if show_pcb else ''}.")
except Exception as exc:  # noqa: BLE001
    print(f"No se pudo enviar al visor ({exc}). ¿Está 'python -m ocp_vscode' activo?")
