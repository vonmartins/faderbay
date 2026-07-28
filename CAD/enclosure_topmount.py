"""Carcasa Faderbay — propuesta B: PCB colgada del top (top-mount).

La PCB se atornilla a torretas que cuelgan del panel superior, paralela a él, así
los componentes registran contra el panel (distancia constante): display a ras,
faders/encoder asoman consistentes.

Forma exterior: fondo plano sobre la mesa, **paredes verticales 90°** y **panel
superior inclinado** `tilt_angle` grados (consola). La PCB va inclinada el mismo
ángulo, colgada del panel. Como los conectores salen en el plano de la PCB (por
tanto inclinados), la pared trasera lleva una **pequeña faceta angulada** solo en
la franja de los conectores, para que salgan rectos; el resto de la trasera y la
frontal quedan verticales.

Reparto en dos piezas:
  - CUBIERTA: panel + paredes + faceta trasera con TODOS los conectores (enteros)
    + torretas de la PCB.
  - PLACA DE FONDO: plancha inferior que cierra por debajo.
La costura es un perímetro bajo, por debajo de la PCB y de los conectores.

Frame: el del STEP (X centrado, cara inferior de la PCB en Z=0). La PCB se lleva a
su posición inclinada con la transformación L. USB-C no modelado (cota a mano).

Estado: esqueleto (encaje cubierta/placa a tope; falta el labio).
"""

import json
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

# --- Cotas medidas (frame del STEP, en mm) ---
PCB_THICKNESS = 1.51
ENV_X_MIN, ENV_X_MAX = -180.0, 180.0
ENV_Y_MIN, ENV_Y_MAX = -65.0, 80.91
BOARD_Y_MAX = 75.0
DISPLAY_TOP_OVER_BOARD = 14.0   # medido con pie de rey (cara superior PCB -> top display)

# Salientes medidos hacia afuera (pie de rey)
JACK_TIP_PROTRUSION = 1.6
USBC_TIP_PROTRUSION = 1.6
SWITCH_TIP_PROTRUSION = 1.9

STEP_PATH = "Hardware/Kicad/rev0.2/faderbay.step"
MODEL_TO_COMPONENT = {
    "pta6043-2x10cixxxx": "Fader",
    "EC11E15204A3": "Encoder",
    "NHD-14432WG-BTFH-V_T": "Display",
    "PJ-320B": "Jack",
    "430182070816": "Boton",
    "RA1H1C112R": "Switch",
    "USB_C_Receptacle_HRO_TYPE-C-31-M-12": "USB",
    "934EW-1": "LED"
}

# --- Espesores ---
wall_thickness = 3.0
floor_thickness = 3.0        # plancha de fondo
top_thickness = 3.0          # panel superior

# --- Montaje top-mount ---
standoff_height = 14.0       # cara superior PCB -> cara interior panel (display a ras)
tilt_angle = 5.0            # inclinación de consola (panel y PCB)
pin_clearance = 1.0          # holgura de los pines de la PCB sobre el suelo interior

# --- Holguras ---
interior_clearance_xy = 1.0
back_clearance = 0          # hueco entre el conector más profundo (switch) y la pared trasera

# --- Reparto cubierta / placa ---
split_z = 6.0                # costura horizontal (baja, por debajo de conectores)

# --- Fillets ---
fillet_vertical = 5.0
fillet_top = 3.0
fillet_bottom = 1.0
interior_fillet = 3.0

# --- Torretas de la PCB (taladro guía para tornillo autorroscante) ---
boss_dia = 6.0               # diámetro de la torreta cilíndrica (pequeña por SMD alrededor)
boss_hole_dia = 2.5          # taladro guía (M3 autorroscante)
# Taladros de montaje M3 (Ø3.2) de la PCB, medidos en el STEP. Distintos de los 4
# del display (Ø2.8). Añade aquí los que pongas en el futuro (p. ej. centro).
BOSS_POSITIONS = [
    (-174.1, -58.5),
    (174.0, -58.5),
    (67.5, 37.5),
    (154.0, 47.0),
]

# --- Encaje cubierta↔placa: tornillos desde el plate que roscan en torretas del
# cover. Reutiliza las 4 torretas de la PCB + 2 torretas nuevas en las esquinas
# traseras. El plate lleva agujeros de paso. ---
joint_clear_dia = 4.5        # Ø de paso del tornillo en la placa (holgura para la inclinación)
# Esquinas traseras (world XY). AJUSTAR a donde haya hueco de SMD (zona trasera muy poblada).
BACK_BOSS_XY = [
    (-170.0, 55.0),
    (170.0, 55.0),
]

# --- Faceta trasera de conectores (un rectángulo simple que cubre todo) ---
facet_margin_side = 3.0      # margen en X a cada lado (del minijack extremo / switch)
facet_margin_z = 4.0         # margen en Z por arriba y por abajo
facet_wall_thickness = 1.5   # grosor de pared SOLO en la franja de conectores (resto: wall_thickness)

# --- Recortes ---
fader_slot_width = 3.0
fader_slot_length = 65.0
encoder_hole_dia = 8.0
button_hole_dia = 6.0
display_window_w = 62.0
display_window_h = 20.0
jack_hole_dia = 5.2
switch_cutout_w = 15.0       # marco/actuador del switch (actuador medido ~14 + holgura)
switch_cutout_h = 10.5       # (actuador medido ~9.5 + holgura)
usb_cutout_w = 9           # boca USB-C (X)  [confirmar datasheet]
usb_cutout_h = 3           # boca USB-C (Z)  [confirmar datasheet]
led_hole_dia = 3.0           # agujero del LED testigo (indicador, pequeño)

# --- Offset del centro de cada agujero respecto al centro del componente ---
# Frame STEP: +X = a lo largo de la fila de conectores; +Z = hacia arriba en la faceta.
# Ajústalos viendo el render hasta que el agujero case con la abertura real.
jack_offset_x, jack_offset_z = 0.0, 0.2
usb_offset_x, usb_offset_z = 0.0, 0.2
led_offset_x, led_offset_z = 0.0, 3.2      # el lente del LED va más arriba que el centro del cuerpo
switch_offset_x, switch_offset_z = 0.3, 1.8  # centrar en el actuador, no en el bbox del switch

# --- Visor / salida ---
show_pcb = True
OUTPUT_DIR = "CAD/output"

# ============================================================
#   HELPERS
# ============================================================


def box_from_bounds(x0, x1, y0, y1, z0, z1):
    cx, cy, cz = (x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2
    return Pos(cx, cy, cz) * Box(x1 - x0, y1 - y0, z1 - z0)


def load_placements(children):
    raw = {}
    for child in children:
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


def tf_point(x, y, z, s, c, dz):
    """Aplica L (rotación tilt sobre X + traslación dz en Z) a un punto."""
    return (x, y * c - z * s, y * s + z * c + dz)


# ============================================================
#   PCB: datos (con caché en disco) + geometría (solo si se va a mostrar)
# ============================================================
# Importar el STEP y sacar bounding boxes es lo caro (~30 s). Como la PCB no
# cambia entre iteraciones de la carcasa, cacheamos en disco las posiciones y
# cotas que necesitamos. Si el STEP no cambió y no hay que mostrar la PCB, ni se
# importa: la construcción de la carcasa baja a unos segundos.
CACHE_PATH = os.path.join(OUTPUT_DIR, "pcb_cache.json")


CACHE_VERSION = 2   # súbelo si cambia la estructura de los datos cacheados


def compute_pcb_data(pcb):
    """Posiciones de componentes + cotas de la PCB necesarias para la carcasa."""
    children = list(pcb.children)
    board_ymax = board_zmin = None
    for ch in children:
        if ch.label == "Faderbay_PCB":            # el outline de la placa
            bbb = ch.bounding_box()
            board_ymax, board_zmin = bbb.max.Y, bbb.min.Z
    bb = pcb.bounding_box()
    return {
        "placements": load_placements(children),
        "board_ymax": board_ymax,
        "board_zmin": board_zmin,
        "pcb_min_y": bb.min.Y,
        "pcb_min_z": bb.min.Z,
    }


step_key = [CACHE_VERSION, os.path.getmtime(STEP_PATH),
            sorted([k, v] for k, v in MODEL_TO_COMPONENT.items())]
pcb_data = None
if os.path.exists(CACHE_PATH):
    with open(CACHE_PATH) as f:
        _cached = json.load(f)
    if _cached.get("key") == step_key:
        pcb_data = _cached["data"]

pcb_clean = None
if pcb_data is None or show_pcb:
    # Importar: caché inválida (hay que recalcular) o el visor necesita el sólido.
    pcb_raw = import_step(STEP_PATH)
    # Filtra el display duplicado del STEP (dos instancias en la misma XY, una
    # desplazada 7 mm en Z). Nos quedamos con la de mayor Z; el resto tal cual.
    display_children = [c for c in pcb_raw.children if "NHD" in c.label]
    drop = (min(display_children, key=lambda c: c.bounding_box().max.Z)
            if len(display_children) > 1 else None)
    pcb_children = [c for c in pcb_raw.children if c is not drop]
    pcb_clean = Compound(children=pcb_children)
    if pcb_data is None:
        pcb_data = compute_pcb_data(pcb_clean)
        os.makedirs(OUTPUT_DIR, exist_ok=True)
        with open(CACHE_PATH, "w") as f:
            json.dump({"key": step_key, "data": pcb_data}, f)

placements = pcb_data["placements"]

# Transformación de montaje L: rotar tilt sobre X y subir dz.
theta = math.radians(tilt_angle)
s, c = math.sin(theta), math.cos(theta)
slope = math.tan(theta)

R = Rot(tilt_angle, 0, 0)
target_min_z = floor_thickness + pin_clearance
# dz analítico desde las cotas cacheadas (z' = y*sin + z*cos, mín en ymin, zmin).
dz = target_min_z - (pcb_data["pcb_min_y"] * s + pcb_data["pcb_min_z"] * c)
L = Pos(0, 0, dz) * R
# La PCB colocada es solo para el visor: .locate() la mueve sin deepcopy.
pcb_mounted = pcb_clean.locate(L) if show_pcb else None

# ============================================================
#   GEOMETRÍA DERIVADA (frame de mesa, Z=0 en el fondo)
# ============================================================

cy0 = (ENV_Y_MIN + BOARD_Y_MAX) / 2.0            # centro Y de la PCB
_, by, bz = tf_point(0, cy0, PCB_THICKNESS, s, c, dz)  # centro cara sup. PCB tras L
# Punto del techo (panel interior): standoff perpendicular sobre la PCB.
ceil_y = by - standoff_height * s
ceil_z = bz + standoff_height * c

interior_x_min = ENV_X_MIN - interior_clearance_xy
interior_x_max = ENV_X_MAX + interior_clearance_xy
interior_y_min = ENV_Y_MIN - interior_clearance_xy
# Pared trasera pegada al OUTLINE de la placa (no a la punta de los conectores):
# así el borde de la PCB toca la pared y los conectores atraviesan sus agujeros.
# Y del borde de la placa tras L: Y' = y*cos - z*sin, máx en (board_ymax, board_zmin).
board_edge_y = pcb_data["board_ymax"] * c - pcb_data["board_zmin"] * s
board_edge_z = pcb_data["board_ymax"] * s + pcb_data["board_zmin"] * c + dz
interior_y_max = board_edge_y + back_clearance

outer_x_min = interior_x_min - wall_thickness
outer_x_max = interior_x_max + wall_thickness
outer_y_min = interior_y_min - wall_thickness
outer_y_max = interior_y_max + wall_thickness

# Techo inclinado (sube hacia +Y), pasando por el punto del panel.
interior_z_max = ceil_z + slope * (outer_y_min - ceil_y)   # techo interior delante
outer_z_min = 0.0
interior_z_min = floor_thickness
outer_z_max = interior_z_max + top_thickness
back_ceiling_z = interior_z_max + slope * (outer_y_max - outer_y_min)
back_top_z = outer_z_max + slope * (outer_y_max - outer_y_min)

top_plane = Plane(origin=(0, outer_y_min, outer_z_max), z_dir=(0, -slope, 1))
ceiling_plane = Plane(origin=(0, outer_y_min, interior_z_max), z_dir=(0, -slope, 1))

# Pared trasera INCLINADA (perpendicular a los conectores): normal (0, c, s),
# pasando por el borde de la placa (offset back_clearance a lo largo de la normal).
# El borde de la PCB queda sobre la cara interior y los conectores salen perpendic.
_bep = (0, board_edge_y + back_clearance * c, board_edge_z + back_clearance * s)
back_in_plane = Plane(origin=_bep, z_dir=(0, c, s))                 # cara interior
back_out_plane = back_in_plane.offset(wall_thickness)              # cara exterior (3 mm)
facet_out_plane = back_in_plane.offset(facet_wall_thickness)       # cara exterior faceta (1.5 mm)

# ============================================================
#   CONSTRUCCIÓN DEL CASCARÓN (fondo plano, paredes verticales, panel inclinado)
# ============================================================

# Caja grande y luego cortes inclinados: techo (top_plane) y trasera (back_out/in).
box_y_hi = board_edge_y + wall_thickness + 40.0
box_z_hi = outer_z_max + slope * (box_y_hi - outer_y_min) + 20.0

outer = box_from_bounds(
    outer_x_min, outer_x_max, outer_y_min, box_y_hi, outer_z_min, box_z_hi
)
# Fillets ANTES de los cortes (aristas limpias): las 4 verticales (frontales y
# traseras). Las traseras se cortan luego con el plano inclinado y quedan redondeadas.
outer = fillet(outer.edges().filter_by(Axis.Z), fillet_vertical)
outer = fillet(outer.edges().group_by(Axis.Z)[0], fillet_bottom)     # canto inferior
outer = split(outer, bisect_by=top_plane, keep=Keep.BOTTOM)          # techo inclinado
outer = split(outer, bisect_by=back_out_plane, keep=Keep.BOTTOM)     # trasera inclinada
# Las esquinas traseras ya no son verticales: van en la dirección (0, s, -c).
# Se redondean después del corte inclinado.
outer = fillet(outer.edges().filter_by(Axis((0, 0, 0), (0, s, -c))), fillet_vertical)

cavity = box_from_bounds(
    interior_x_min, interior_x_max, interior_y_min, box_y_hi,
    interior_z_min, box_z_hi,
)
cavity = fillet(cavity.edges().filter_by(Axis.Z), interior_fillet)
cavity = split(cavity, bisect_by=ceiling_plane, keep=Keep.BOTTOM)     # techo interior
cavity = split(cavity, bisect_by=back_in_plane, keep=Keep.BOTTOM)     # trasera interior
shell = outer - cavity

# ============================================================
#   FACETA TRASERA (solo franja de conectores)
# ============================================================
# Extensión (mundo) de cada conector tras L (contando su tamaño real). La faceta
# es un único rectángulo que cubre TODOS los conectores (incluido el switch, el
# más alto) con el mismo margen por arriba, abajo y a los lados.
def world_z(cy, cz):
    return cy * s + cz * c + dz


conn_boxes = []
for cx, cy, cz in placements.get("Jack", []):
    x, z, r = cx + jack_offset_x, cz + jack_offset_z, jack_hole_dia / 2
    conn_boxes.append((x - r, x + r, world_z(cy, z - r), world_z(cy, z + r)))
for cx, cy, cz in placements.get("Switch", []):
    x, z = cx + switch_offset_x, cz + switch_offset_z
    conn_boxes.append((
        x - switch_cutout_w / 2, x + switch_cutout_w / 2,
        world_z(cy, z - switch_cutout_h / 2), world_z(cy, z + switch_cutout_h / 2),
    ))
for cx, cy, cz in placements.get("USB", []):
    x, z = cx + usb_offset_x, cz + usb_offset_z
    conn_boxes.append((
        x - usb_cutout_w / 2, x + usb_cutout_w / 2,
        world_z(cy, z - usb_cutout_h / 2), world_z(cy, z + usb_cutout_h / 2),
    ))
for cx, cy, cz in placements.get("LED", []):
    x, z, r = cx + led_offset_x, cz + led_offset_z, led_hole_dia / 2
    conn_boxes.append((x - r, x + r, world_z(cy, z - r), world_z(cy, z + r)))

# Franja X/Z de la faceta (acotada a la zona plana de la pared, sin morder las
# esquinas redondeadas verticales).
facet_x_lo = max(min(b[0] for b in conn_boxes) - facet_margin_side, interior_x_min)
facet_x_hi = min(max(b[1] for b in conn_boxes) + facet_margin_side, interior_x_max)
facet_z_lo = min(b[2] for b in conn_boxes) - facet_margin_z
facet_z_hi = max(b[3] for b in conn_boxes) + facet_margin_z

# Faceta = pared FINA en la franja de conectores: se rebaja la cara exterior de la
# pared trasera (inclinada) hasta facet_out_plane, dejando facet_wall_thickness
# uniforme. El resto de la trasera se queda en wall_thickness.
facet_band = box_from_bounds(
    facet_x_lo, facet_x_hi,
    interior_y_max - 5.0, box_y_hi,
    facet_z_lo, facet_z_hi,
)
facet_recess = split(facet_band, bisect_by=facet_out_plane, keep=Keep.TOP)
shell = shell - facet_recess

# ============================================================
#   REPARTO: cubierta (arriba) + placa de fondo (abajo)
# ============================================================
cover = split(shell, bisect_by=Plane.XY.offset(split_z), keep=Keep.TOP)
plate = split(shell, bisect_by=Plane.XY.offset(split_z), keep=Keep.BOTTOM)

# ============================================================
#   TORRETAS (en frame STEP, luego L) — cuelgan del panel a la PCB
# ============================================================
for px, py in BOSS_POSITIONS:
    center = Pos(px, py, PCB_THICKNESS + standoff_height / 2)
    boss = center * Cylinder(boss_dia / 2, standoff_height)
    boss -= center * Cylinder(boss_hole_dia / 2, standoff_height)
    cover += L * boss

# ============================================================
#   ENCAJE CUBIERTA↔PLACA (tornillos desde la placa a torretas del cover)
# ============================================================
# 2 torretas traseras nuevas (cover): cilindros verticales de la costura hasta
# dentro del panel (así quedan unidas al cover), con guía roscante.
for bx, by in BACK_BOSS_XY:
    top = interior_z_max + slope * (by - outer_y_min) + 1.5   # hasta el panel
    h = top - split_z
    ctr = Pos(bx, by, (split_z + top) / 2)
    boss = ctr * Cylinder(boss_dia / 2, h)
    boss -= ctr * Cylinder(boss_hole_dia / 2, h)
    cover += boss

# Agujeros de paso en la placa: pie (en mundo) de las 4 torretas de PCB + 2 traseras.
# El tornillo entra desde abajo, atraviesa la placa y rosca en la torreta del cover.
plate_screw_xy = [(px, py * c - PCB_THICKNESS * s) for px, py in BOSS_POSITIONS] + list(BACK_BOSS_XY)
for sx, sy in plate_screw_xy:
    plate -= Pos(sx, sy, split_z / 2) * Cylinder(joint_clear_dia / 2, split_z + 2)

# ============================================================
#   RECORTES (en frame STEP, luego L) — perpendiculares al panel / PCB
# ============================================================
z_cut0 = PCB_THICKNESS
z_cut1 = PCB_THICKNESS + standoff_height + top_thickness + 8.0


def top_slot(cx, cy):
    section = SlotOverall(fader_slot_length, fader_slot_width, rotation=90)
    return Pos(cx, cy, z_cut0) * extrude(section, z_cut1 - z_cut0)


def top_hole(cx, cy, dia):
    return Pos(cx, cy, (z_cut0 + z_cut1) / 2) * Cylinder(dia / 2, z_cut1 - z_cut0)


def top_window(cx, cy, w, h):
    return Pos(cx, cy, (z_cut0 + z_cut1) / 2) * Box(w, h, z_cut1 - z_cut0)


y_cut0 = interior_y_max - 2.0
y_cut1 = outer_y_max + 30.0
y_cut_mid = (y_cut0 + y_cut1) / 2


def back_hole(cx, cz, dia):
    return Pos(cx, y_cut_mid, cz) * Rot(90, 0, 0) * Cylinder(dia / 2, y_cut1 - y_cut0)


def back_window(cx, cz, w, h):
    return Pos(cx, y_cut_mid, cz) * Box(w, y_cut1 - y_cut0, h)


def back_slot(cx, cz, w, h):
    """Boca tipo stadium (rectángulo con extremos semicirculares, radio = h/2):
    la forma del USB-C. Sección w(X)×h(Z) en el plano XZ, extruida a lo largo de Y."""
    section = Plane.XZ * SlotOverall(w, h)
    prism = extrude(section, (y_cut1 - y_cut0) / 2, both=True)
    return Pos(cx, y_cut_mid, cz) * prism


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
    cutters.append(back_hole(cx + jack_offset_x, cz + jack_offset_z, jack_hole_dia))
for cx, cy, cz in placements["Switch"]:
    cutters.append(back_window(cx + switch_offset_x, cz + switch_offset_z,
                               switch_cutout_w, switch_cutout_h))
for cx, cy, cz in placements.get("USB", []):
    cutters.append(back_slot(cx + usb_offset_x, cz + usb_offset_z,
                             usb_cutout_w, usb_cutout_h))
for cx, cy, cz in placements.get("LED", []):
    cutters.append(back_hole(cx + led_offset_x, cz + led_offset_z, led_hole_dia))

cutter = L * Compound(children=cutters)
cover = cover - cutter

# ============================================================
#   SALIDA
# ============================================================
os.makedirs(OUTPUT_DIR, exist_ok=True)
cover_path = os.path.join(OUTPUT_DIR, "cover.step")
plate_path = os.path.join(OUTPUT_DIR, "plate.step")
export_step(cover, cover_path)
export_step(plate, plate_path)

print("Carcasa top-mount (esqueleto, paredes verticales) generada.")
print(f"  Planta: {outer_x_max - outer_x_min:.1f} x {outer_y_max - outer_y_min:.1f} mm")
print(f"  Altura panel: delante {outer_z_max:.1f} -> detras {back_top_z:.1f} mm  "
      f"(cuña {tilt_angle:.0f}°)")
print(f"  Standoff {standoff_height:.1f} mm | costura Z={split_z:.1f} | "
      f"faceta X[{facet_x_lo:.0f},{facet_x_hi:.0f}] Z[{facet_z_lo:.1f},{facet_z_hi:.1f}]")
print(f"  Recortes: {len(placements['Fader'])} faders, {len(placements['Jack'])} jacks, "
      f"{len(placements['Boton'])} botones, {len(placements['Encoder'])} encoder, "
      f"{len(placements['Display'])} display, {len(placements['Switch'])} switch, "
      f"{len(placements.get('USB', []))} usb, {len(placements.get('LED', []))} led")
print(f"  Exportado: {cover_path}, {plate_path}")

objects = [cover, plate]
names = ["cover", "plate"]
if show_pcb:
    objects.append(pcb_mounted)
    names.append("pcb")
try:
    from ocp_vscode import show, Camera
    show(*objects, names=names, reset_camera=Camera.RESET)
    print("Enviado al visor ocp_vscode (localhost:3939).")
except Exception as exc:  # noqa: BLE001
    print(f"No se pudo enviar al visor ({exc}). ¿Está 'python -m ocp_vscode' activo?")
