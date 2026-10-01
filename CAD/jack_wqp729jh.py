"""Modelo 3D simple del jack WQP729JH (3.5mm right-angle con tuerca).

Representativo. Mismo sistema de coordenadas que la huella (origen = origen del
footprint; eje del jack a Z=5.5 sobre la PCB; cuerpo en +X, tuerca hacia -X).

Montaje a lo largo del eje X (de panel hacia atrás):
  tuerca (Ø6, L=4.3) -> cuello (1) -> cuerpo (15)

Uso:
  python -m ocp_vscode              # arrancar visor
  .venv/bin/python CAD/jack_wqp729jh.py            # ver stereo en el visor
  .venv/bin/python CAD/jack_wqp729jh.py --mono     # ver mono en el visor
  .venv/bin/python CAD/jack_wqp729jh.py --export   # exportar STEP (mono + stereo)
"""

import os
import sys

from build123d import Box, Cylinder, Pos, Rot, Color, Compound, Plane, export_step, mirror

AXIS_Z = 5.5    # altura del eje del jack sobre la PCB

COLOR_RED = Color(0.502, 0.047, 0.047)   # #800c0c (mono -R)
COLOR_BLUE = Color(0.294, 0.455, 0.659)  # #4b74a8 (stereo -B)

OUT_DIR = os.path.normpath(
    os.path.join(os.path.dirname(__file__), "..", "Hardware", "Kicad", "rev0.2", "3dmodels")
)

# posiciones de pin (provisionales, de la huella): x, y, (ancho_x, ancho_y en planta)
PINS = {
    "1": (0.85, 0.0, (0.6, 1.8)),   # Sleeve
    "2": (13.1, -5.1, (1.8, 0.6)),  # Ring  (solo stereo)
    "3": (15.0, 0.0, (0.6, 1.8)),   # Tip
    "4": (13.1, 5.1, (1.8, 0.6)),   # Shunt
}


def build_jack(stereo=True):
    body_color = COLOR_BLUE if stereo else COLOR_RED
    body = Pos(7.5, 0.0, AXIS_Z) * Box(15.0, 10.0, 10.5)
    body.color = body_color; body.label = "body"
    neck = Pos(-0.5, 0.0, AXIS_Z) * Box(1.0, 8.1, 7.8)
    neck.color = body_color; neck.label = "neck"
    nut = Pos(-3.15, 0.0, AXIS_Z) * Rot(0, 90, 0) * Cylinder(radius=3.0, height=4.3)
    nut.color = Color(0.78, 0.78, 0.82); nut.label = "nut"
    parts = [body, neck, nut]
    for name, (x, y, (wx, wy)) in PINS.items():
        if name == "2" and not stereo:
            continue
        pin = Pos(x, y, -0.5) * Box(wx, wy, 5.0)
        pin.color = Color(0.85, 0.78, 0.45); pin.label = f"pin{name}"
        parts.append(pin)
    return parts


def export_all():
    os.makedirs(OUT_DIR, exist_ok=True)
    for stereo, name in [(True, "WQP729JH-B"), (False, "WQP729JH-R")]:
        # KiCad coloca los modelos 3D con el eje Y invertido respecto a la huella;
        # espejamos en Y (plano XZ) pieza a pieza para que las patillas caigan
        # sobre sus pads sin perder el color de cada parte.
        mirrored = []
        for p in build_jack(stereo):
            m = mirror(p, about=Plane.XZ)
            m.color = p.color
            m.label = p.label
            mirrored.append(m)
        asm = Compound(label=name, children=mirrored)
        path = os.path.join(OUT_DIR, name + ".step")
        export_step(asm, path)
        print("Exportado:", path)


if __name__ == "__main__":
    if "--export" in sys.argv:
        export_all()
    else:
        parts = build_jack(stereo="--mono" not in sys.argv)
        try:
            from ocp_vscode import show
            show(*parts, names=[p.label for p in parts])
            print(f"Enviado al visor. {'MONO' if '--mono' in sys.argv else 'STEREO'}")
        except Exception as exc:  # noqa: BLE001
            print(f"No se pudo enviar al visor ({exc}). ¿Está 'python -m ocp_vscode' activo?")
