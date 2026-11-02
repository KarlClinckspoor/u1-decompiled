#!/usr/bin/env python3
"""Hex/ASCII dump of the original EXE: code segment (default) or DGROUP.

    python3 tools/dump.py EXE OFF LEN [--ds]       e.g. tools/dump.py gamedata/GEN.EXE 124 80 --ds
    python3 tools/dump.py EXE --str OFF            the NUL-terminated string at DGROUP:OFF
"""
import struct, sys
a = sys.argv[1:]
d = open(a[0], 'rb').read(); h = struct.unpack('<14H', d[:28]); img = d[h[4] * 16:]
ds = struct.unpack('<H', img[1:3])[0] * 16
if a[1] == '--str':
    o = ds + int(a[2], 16); e = img.index(b'\0', o); print(repr(img[o:e].decode('latin1'))); sys.exit()
base = ds if '--ds' in a else 0
o, n = int(a[1], 16), int(a[2], 16)
for k in range(o, o + n, 16):
    b = img[base + k:base + min(k + 16, o + n)]
    print('%04x  %-48s %s' % (k, b.hex(' '), ''.join(chr(c) if 32 <= c < 127 else '.' for c in b)))
