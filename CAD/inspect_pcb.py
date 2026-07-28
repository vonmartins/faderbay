"""Inspección del STEP de la PCB rev0.2 de Faderbay.

Extrae las cotas reales de la placa y de los componentes altos para luego
derivar de aquí (no a mano) los recortes de la carcasa. No modela nada todavía.

El export STEP de KiCad NO conserva los reference designators; conserva el
nombre del modelo 3D de fabricante como label de cada instancia del ensamblaje.
Identificamos por label (tipo de componente) + bounding box (posición y altura
de cada instancia concreta).

NOTA: el USB-C no está en este STEP (aún no hay modelo 3D). Habrá que cotarlo a
mano cuando se aborde su recorte.
"""

from build123d import import_step, GeomType

STEP_PATH = "Hardware/Kicad/rev0.2/faderbay.step"

# Label del modelo 3D en el STEP -> componente físico.
MODEL_TO_COMPONENT = {
    "pta6043-2x10cixxxx": "Fader",
    "EC11E15204A3": "Encoder",
    "NHD-14432WG-BTFH-V_T": "Display ST7920",
    "PJ-320B": "Jack TRS 3.5mm",
    "430182070816": "Boton",
    "RA1H1C112R": "Interruptor power",
    "Faderbay_PCB": "PCB",
}

# Rango de diámetro (mm) considerado taladro de montaje: descarta vías y pads.
MOUNT_HOLE_DIA_MIN = 2.4
MOUNT_HOLE_DIA_MAX = 3.8


def bbox_center_xy(bbox):
    """Centro (X, Y) de un bounding box."""
    cx = (bbox.min.X + bbox.max.X) / 2.0
    cy = (bbox.min.Y + bbox.max.Y) / 2.0
    return cx, cy


def classify(label):
    """Traduce el label del STEP a nombre de componente (o '??' si desconocido)."""
    return MODEL_TO_COMPONENT.get(label, f"?? ({label})")


def main():
    print(f"Importando {STEP_PATH} ...")
    assembly = import_step(STEP_PATH)
    children = list(assembly.children)
    print(f"Instancias en el ensamblaje: {len(children)}\n")

    # --- Bounding box de la placa desnuda y de la envolvente total ---
    pcb = next((c for c in children if c.label == "Faderbay_PCB"), None)
    if pcb is None:
        print("AVISO: no se encontró el sólido 'Faderbay_PCB'.")
        return

    pcb_bb = pcb.bounding_box()
    full_bb = assembly.bounding_box()
    board_top_z = pcb_bb.max.Z

    print("=== BOUNDING BOX PLACA (Faderbay_PCB) ===")
    print(f"  X: {pcb_bb.min.X:8.2f} .. {pcb_bb.max.X:8.2f}  ->  ancho  {pcb_bb.size.X:7.2f} mm")
    print(f"  Y: {pcb_bb.min.Y:8.2f} .. {pcb_bb.max.Y:8.2f}  ->  fondo  {pcb_bb.size.Y:7.2f} mm")
    print(f"  Z: {pcb_bb.min.Z:8.2f} .. {pcb_bb.max.Z:8.2f}  ->  grosor {pcb_bb.size.Z:7.2f} mm")
    print("\n=== BOUNDING BOX ENVOLVENTE TOTAL (placa + componentes) ===")
    print(f"  X: {full_bb.min.X:8.2f} .. {full_bb.max.X:8.2f}  ->  {full_bb.size.X:7.2f} mm")
    print(f"  Y: {full_bb.min.Y:8.2f} .. {full_bb.max.Y:8.2f}  ->  {full_bb.size.Y:7.2f} mm")
    print(f"  Z: {full_bb.min.Z:8.2f} .. {full_bb.max.Z:8.2f}  ->  alto {full_bb.size.Z:7.2f} mm")
    print(f"  (cara superior de la placa en Z = {board_top_z:.2f} mm)\n")

    # --- Componentes altos: posición (centro X/Y) y altura (Z máx) ---
    # Agrupa por tipo; excluye la propia placa.
    groups = {}
    for c in children:
        if c.label == "Faderbay_PCB":
            continue
        comp = classify(c.label)
        bb = c.bounding_box()
        cx, cy = bbox_center_xy(bb)
        z_top = bb.max.Z
        h_over_board = z_top - board_top_z
        groups.setdefault(comp, []).append((cx, cy, z_top, h_over_board))

    print("=== COMPONENTES: centro (X, Y), Z_max y altura sobre la placa ===")
    for comp in sorted(groups):
        items = sorted(groups[comp], key=lambda t: (round(t[0], 1), round(t[1], 1)))
        print(f"\n{comp}  (x{len(items)})")
        print(f"  {'#':>2}  {'X':>8}  {'Y':>8}  {'Z_max':>8}  {'h_sobre_placa':>13}")
        for i, (cx, cy, z_top, h) in enumerate(items, 1):
            print(f"  {i:>2}  {cx:8.2f}  {cy:8.2f}  {z_top:8.2f}  {h:13.2f}")

    # --- Taladros de montaje (best-effort, geométrico: verificar visualmente) ---
    # Buscamos caras cilíndricas verticales que atraviesan el grosor de la placa,
    # filtrando por diámetro típico de montaje para descartar vías/pads.
    print("\n=== TALADROS DE MONTAJE (best-effort, verificar en el visor) ===")
    thickness = pcb_bb.size.Z
    seen = {}
    for face in pcb.faces():
        if face.geom_type != GeomType.CYLINDER:
            continue
        fb = face.bounding_box()
        # Eje vertical: la cara abarca (casi) todo el grosor en Z.
        if fb.size.Z < 0.8 * thickness:
            continue
        diameter = max(fb.size.X, fb.size.Y)
        if not (MOUNT_HOLE_DIA_MIN <= diameter <= MOUNT_HOLE_DIA_MAX):
            continue
        cx, cy = bbox_center_xy(fb)
        key = (round(cx, 1), round(cy, 1))
        # Nos quedamos con el diámetro máximo visto en ese centro.
        seen[key] = max(seen.get(key, 0.0), diameter)

    if not seen:
        print("  No se detectaron taladros en el rango de diámetro esperado.")
    else:
        print(f"  {'#':>2}  {'X':>8}  {'Y':>8}  {'diam':>7}")
        for i, ((cx, cy), dia) in enumerate(sorted(seen.items()), 1):
            print(f"  {i:>2}  {cx:8.2f}  {cy:8.2f}  {dia:7.2f}")

    # --- Visualización en el visor ya levantado en localhost:3939 ---
    try:
        from ocp_vscode import show
        show(assembly, names=["faderbay_pcb"])
        print("\nEnviado al visor ocp_vscode (localhost:3939).")
    except Exception as exc:  # noqa: BLE001
        print(f"\nNo se pudo enviar al visor ({exc}). ¿Está 'python -m ocp_vscode' activo?")


if __name__ == "__main__":
    main()
