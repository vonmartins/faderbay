#!/usr/bin/env python3
"""
midi_analyzer.py

Analiza una captura de Waveforms (CSV: tiempo, nivel 0/1) de la salida MIDI
de un Faderbay. Hace dos cosas:

1. Decodifica la UART a 31250 baud, 8N1 (LSB first, sin paridad, 1 stop bit).
2. Decodifica MIDI parseando bytes de status / data running-status.

Uso:
    python midi_analyzer.py captura.csv
    python midi_analyzer.py captura.csv --raw-bytes
    python midi_analyzer.py captura.csv --invert      # si la señal está invertida

Asume:
    - Columna 0: tiempo en segundos.
    - Columna 1: nivel lógico (0 o 1). MIDI idle = 1 (línea alta), start bit = 0.
    - Faderbay TRS sale invertida del colector: idle = 0, mark = 5V (1).
      Detecta automáticamente; si la auto-detección falla, usar --invert.
"""

import argparse
import sys
from pathlib import Path

import numpy as np
import pandas as pd


# ---------------------------------------------------------------------------
# UART decoder
# ---------------------------------------------------------------------------

BAUD = 31250
BIT_TIME = 1.0 / BAUD  # 32 µs


def find_edges(t, level):
    """Devuelve los índices de los flancos (cambios de nivel)."""
    diff = np.diff(level.astype(np.int8))
    edges = np.where(diff != 0)[0]
    return edges


def decode_uart(t, level, invert=False, verbose=False):
    """
    Decodifica la señal UART. Devuelve lista de (timestamp, byte).

    Algoritmo: busca flanco de bajada (start bit), muestrea el centro de
    cada bit posterior (1.5 * bit_time, 2.5 * bit_time, ..., 9.5 * bit_time),
    y verifica que el bit 10 sea 1 (stop bit).
    """
    if invert:
        level = 1 - level

    # Idle level es 1 (alto). Si la mayoría de la señal está a 0,
    # probablemente está invertida y el usuario no usó --invert.
    if level.mean() < 0.5:
        print("[WARN] Señal mayoritariamente a 0. ¿Está invertida? Usa --invert.",
              file=sys.stderr)

    sample_rate = 1.0 / np.median(np.diff(t[:1000]))
    samples_per_bit = sample_rate * BIT_TIME
    if verbose:
        print(f"Sample rate detectado: {sample_rate:.0f} Hz")
        print(f"Samples por bit: {samples_per_bit:.2f}")

    bytes_out = []
    framing_errors = 0
    i = 0
    n = len(level)

    while i < n - 1:
        # Buscar flanco de bajada (start bit)
        if level[i] == 1 and level[i + 1] == 0:
            start_sample = i + 1
            # Muestrear el centro de cada bit
            byte_val = 0
            bits_ok = True
            for bit_idx in range(8):
                # Centro del bit: 1.5*bit + bit_idx*bit_time desde el flanco
                sample_offset = int((bit_idx + 1.5) * samples_per_bit)
                idx = start_sample + sample_offset
                if idx >= n:
                    bits_ok = False
                    break
                if level[idx] == 1:
                    byte_val |= (1 << bit_idx)  # LSB first

            # Verificar stop bit (debe ser 1)
            stop_idx = start_sample + int(9.5 * samples_per_bit)
            if stop_idx < n and level[stop_idx] != 1:
                framing_errors += 1
                if verbose:
                    print(f"  [framing error] t={t[start_sample]*1e3:.3f} ms, "
                          f"byte tentativo 0x{byte_val:02X}")

            if bits_ok:
                bytes_out.append((t[start_sample], byte_val))
                # Saltar al final del frame: start + 8 datos + stop = 10 bits
                i = start_sample + int(10 * samples_per_bit)
            else:
                i += 1
        else:
            i += 1

    return bytes_out, framing_errors


# ---------------------------------------------------------------------------
# MIDI decoder
# ---------------------------------------------------------------------------

STATUS_NAMES = {
    0x80: ("Note Off",       2),
    0x90: ("Note On",        2),
    0xA0: ("Aftertouch",     2),
    0xB0: ("CC",             2),
    0xC0: ("Program Change", 1),
    0xD0: ("Channel Pressure", 1),
    0xE0: ("Pitch Bend",     2),
}

REALTIME = {
    0xF8: "Clock",
    0xFA: "Start",
    0xFB: "Continue",
    0xFC: "Stop",
    0xFE: "Active Sensing",
    0xFF: "Reset",
}


def decode_midi(bytes_with_time):
    """
    Decodifica una lista de (timestamp, byte) en mensajes MIDI.
    Devuelve lista de dicts con keys: t, type, channel, data1, data2, raw.
    Maneja running status.
    """
    msgs = []
    running_status = None
    pending = []
    expected = 0

    for (t, b) in bytes_with_time:
        # Mensajes de tiempo real: pueden aparecer en cualquier momento, no
        # afectan al running status
        if b in REALTIME:
            msgs.append({
                't': t, 'type': REALTIME[b], 'channel': None,
                'data1': None, 'data2': None, 'raw': [b],
            })
            continue

        if b >= 0x80:
            # Byte de status nuevo
            status_high = b & 0xF0
            if status_high in STATUS_NAMES:
                running_status = b
                pending = [b]
                expected = STATUS_NAMES[status_high][1]
            else:
                # Status raro (Sysex, etc.) — no soportado en este script
                msgs.append({
                    't': t, 'type': f'Unhandled 0x{b:02X}', 'channel': None,
                    'data1': None, 'data2': None, 'raw': [b],
                })
                running_status = None
                pending = []
        else:
            # Byte de datos
            if running_status is None:
                msgs.append({
                    't': t, 'type': 'Orphan data', 'channel': None,
                    'data1': b, 'data2': None, 'raw': [b],
                })
                continue
            if not pending:
                # Running status: re-usa el status anterior
                pending = [running_status]
            pending.append(b)
            if len(pending) - 1 == expected:
                status = pending[0]
                status_high = status & 0xF0
                channel = (status & 0x0F) + 1
                name = STATUS_NAMES[status_high][0]
                d1 = pending[1] if len(pending) > 1 else None
                d2 = pending[2] if len(pending) > 2 else None
                msgs.append({
                    't': pending[0] if isinstance(pending[0], float) else t,
                    'type': name, 'channel': channel,
                    'data1': d1, 'data2': d2,
                    'raw': list(pending),
                })
                pending = []  # listos para otro running-status frame

    return msgs


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    p = argparse.ArgumentParser()
    p.add_argument('csv', type=Path)
    p.add_argument('--raw-bytes', action='store_true',
                   help='Listar los bytes UART crudos además de los mensajes MIDI')
    p.add_argument('--invert', action='store_true',
                   help='Invertir la señal (úsalo si la línea idle es 0)')
    p.add_argument('--verbose', '-v', action='store_true')
    p.add_argument('--max-show', type=int, default=50,
                   help='Cuántos mensajes mostrar como máximo (default 50)')
    args = p.parse_args()

    if not args.csv.exists():
        print(f"Error: no existe {args.csv}", file=sys.stderr)
        sys.exit(1)

    print(f"Leyendo {args.csv}...")
    df = pd.read_csv(args.csv, comment='#')
    # Ser tolerante con nombres de columnas: tomar las dos primeras
    t = df.iloc[:, 0].to_numpy()
    level = df.iloc[:, 1].to_numpy()

    # Si la señal es analógica (no 0/1), umbralizar a la mitad
    if level.max() > 1.5 or level.min() < -0.1:
        threshold = (level.max() + level.min()) / 2
        print(f"Señal analógica detectada (rango {level.min():.2f}..{level.max():.2f}), "
              f"umbralizando a {threshold:.2f}")
        level = (level > threshold).astype(np.int8)
    else:
        level = level.astype(np.int8)

    duration = t[-1] - t[0]
    print(f"Duración: {duration:.3f} s, {len(t)} samples")

    # Auto-detectar inversión: en MIDI la línea está casi siempre a 1 (idle),
    # solo baja durante start bits y bits 0. Si la media es < 0.5, probablemente
    # está invertida.
    invert = args.invert
    if not invert and level.mean() < 0.3:
        print("[AUTO] Señal mayoritariamente a 0 — invirtiendo automáticamente.")
        invert = True

    # ---- Decodificar UART ----
    print("\nDecodificando UART 31250 baud 8N1...")
    raw_bytes, framing_errors = decode_uart(t, level, invert=invert,
                                            verbose=args.verbose)
    print(f"  Bytes recibidos: {len(raw_bytes)}")
    print(f"  Framing errors: {framing_errors}")

    if args.raw_bytes:
        print("\nBytes UART crudos (primeros 100):")
        for ts, b in raw_bytes[:100]:
            print(f"  t={ts*1e3:8.3f} ms  0x{b:02X}  ({b:3d})  {bin(b)[2:].zfill(8)}")

    # ---- Decodificar MIDI ----
    print("\nDecodificando MIDI...")
    msgs = decode_midi(raw_bytes)
    print(f"  Mensajes MIDI: {len(msgs)}")

    # Conteo por tipo
    type_counts = {}
    for m in msgs:
        type_counts[m['type']] = type_counts.get(m['type'], 0) + 1

    print("\nResumen por tipo:")
    for k, v in sorted(type_counts.items(), key=lambda x: -x[1]):
        print(f"  {k:25s}  {v:5d}")

    # ---- Análisis de mensajes CC (lo más relevante para Faderbay) ----
    cc_msgs = [m for m in msgs if m['type'] == 'CC']
    if cc_msgs:
        print(f"\nAnálisis de mensajes CC ({len(cc_msgs)}):")
        # Distribución por canal y CC
        by_cc = {}
        for m in cc_msgs:
            key = (m['channel'], m['data1'])
            by_cc.setdefault(key, []).append(m)
        print(f"  Pares (canal, CC) únicos: {len(by_cc)}")
        print(f"  Detalle:")
        print(f"  {'Canal':6s} {'CC':4s} {'Count':6s} {'Min val':8s} {'Max val':8s} {'Rate (msg/s)':12s}")
        for (ch, cc), lst in sorted(by_cc.items()):
            vals = [m['data2'] for m in lst]
            t_first = lst[0]['t']
            t_last = lst[-1]['t']
            rate = (len(lst) - 1) / (t_last - t_first) if t_last > t_first else 0
            print(f"  {ch:6d} {cc:4d} {len(lst):6d} {min(vals):8d} {max(vals):8d} {rate:12.1f}")

    # ---- Primeros y últimos mensajes ----
    print(f"\nPrimeros {min(args.max_show, len(msgs))} mensajes:")
    for m in msgs[:args.max_show]:
        raw_hex = ' '.join(f'{b:02X}' for b in m['raw'])
        if m['channel'] is not None:
            print(f"  t={m['t']*1e3:9.3f} ms  [{raw_hex:12s}]  "
                  f"{m['type']:18s} ch={m['channel']:2d} "
                  f"data1={m['data1']} data2={m['data2']}")
        else:
            print(f"  t={m['t']*1e3:9.3f} ms  [{raw_hex:12s}]  {m['type']}")

    # ---- Análisis temporal: gaps entre mensajes ----
    if len(msgs) > 1:
        times = np.array([m['t'] for m in msgs])
        gaps = np.diff(times) * 1000  # ms
        print(f"\nAnálisis temporal entre mensajes:")
        print(f"  Gap mínimo:  {gaps.min():.3f} ms")
        print(f"  Gap medio:   {gaps.mean():.3f} ms")
        print(f"  Gap mediano: {np.median(gaps):.3f} ms")
        print(f"  Gap máximo:  {gaps.max():.3f} ms")

    print("\nListo.")


if __name__ == '__main__':
    main()
