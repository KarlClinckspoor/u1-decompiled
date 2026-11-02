#!/usr/bin/env python3
"""Minimal Intel OMF reader for Lattice/MASM objects and libraries.

    python3 tools/omf.py FILE.OBJ|FILE.LIB      # list segments, publics, externs

read_obj(path) -> list of modules; a module is a dict:
  name, segs [(name, class, length, attr)], data {segindex: bytearray},
  mask {segindex: bytearray} (1 where a fixup writes), fix {segindex: [(offset, size, kind, target)]},
  pubs [(name, segindex, offset)], exts [name], groups
"""
import struct, sys


def _idx(b, i):
    v = b[i]
    if v & 0x80:
        return ((v & 0x7f) << 8) | b[i + 1], i + 2
    return v, i + 1


LOCSIZE = {0: 1, 1: 2, 2: 2, 3: 4, 4: 1, 5: 2, 9: 4, 11: 6, 13: 4}


def _parse_fixupp(r, threads):
    """yield (loc, size, ltype, m, frame(meth,idx), target(meth,idx), disp)"""
    k = 0
    while k < len(r):
        b = r[k]
        if not b & 0x80:                      # THREAD
            k += 1
            d = (b >> 6) & 1
            meth = (b >> 2) & 7
            num = b & 3
            ix = None
            if (d == 0 and meth < 4) or (d == 1 and meth < 3):
                ix, k = _idx(r, k)
            threads[(d, num)] = (meth, ix)
            continue
        m = (b >> 6) & 1
        ltype = (b >> 2) & 0xf
        loc = ((b & 3) << 8) | r[k + 1]
        k += 2
        fd = r[k]
        k += 1
        if fd & 0x80:
            frame = threads.get((1, (fd >> 4) & 3))
        else:
            fm = (fd >> 4) & 7
            fi = None
            if fm < 3:
                fi, k = _idx(r, k)
            frame = (fm, fi)
        if fd & 0x08:
            t = threads.get((0, fd & 3))
            tm = t[0] & 3
            target = (tm, t[1])
            hasdisp = not (fd & 0x04)
        else:
            tm = fd & 3
            ti, k = _idx(r, k)
            target = (tm, ti)
            hasdisp = not (fd & 0x04)
        disp = 0
        if hasdisp:
            disp = struct.unpack('<H', r[k:k + 2])[0]
            k += 2
        yield loc, LOCSIZE.get(ltype, 2), ltype, m, frame, target, disp


def read_obj(path):
    d = open(path, 'rb').read()
    mods = []
    i = 0
    pg = None
    if d[0] == 0xF0:
        pg = struct.unpack('<H', d[1:3])[0] + 3
        i = pg
    while i < len(d):
        if d[i] == 0xF1:
            break
        if d[i] != 0x80:
            if pg:
                i = (i // pg + 1) * pg
                continue
            break
        mod = dict(name=None, segs=[], data={}, mask={}, fix={}, pubs=[], exts=[], groups=[], lnames=[''])
        lnames = mod['lnames']
        last = None
        threads = {}
        j = i
        while j < len(d):
            t = d[j]
            L = struct.unpack('<H', d[j + 1:j + 3])[0]
            r = d[j + 3:j + 2 + L]
            if t == 0x80:
                mod['name'] = r[1:1 + r[0]].decode('latin1')
            elif t == 0x96:
                k = 0
                while k < len(r):
                    n = r[k]
                    lnames.append(r[k + 1:k + 1 + n].decode('latin1'))
                    k += 1 + n
            elif t == 0x98:
                attr = r[0]
                k = 1
                if (attr >> 5) == 0:
                    k += 3
                ln = struct.unpack('<H', r[k:k + 2])[0]
                if attr & 2:
                    ln = 0x10000
                ni, k = _idx(r, k + 2)
                ci, k = _idx(r, k)
                mod['segs'].append((lnames[ni], lnames[ci], ln, attr))
                s = len(mod['segs'])
                mod['data'][s] = bytearray(ln)
                mod['mask'][s] = bytearray(ln)
                mod['fix'][s] = []
            elif t == 0x9a:
                gi, k = _idx(r, 0)
                segs = []
                while k < len(r):
                    k += 1
                    si, k = _idx(r, k)
                    segs.append(si)
                mod['groups'].append((lnames[gi], segs))
            elif t == 0x90:
                k = 0
                gi, k = _idx(r, k)
                si, k = _idx(r, k)
                if si == 0:
                    k += 2
                while k < len(r):
                    n = r[k]
                    nm = r[k + 1:k + 1 + n].decode('latin1')
                    off = struct.unpack('<H', r[k + 1 + n:k + 3 + n])[0]
                    k += 3 + n
                    _, k = _idx(r, k)
                    mod['pubs'].append((nm, si, off))
            elif t == 0x8c:
                k = 0
                while k < len(r):
                    n = r[k]
                    mod['exts'].append(r[k + 1:k + 1 + n].decode('latin1'))
                    k += 1 + n
                    _, k = _idx(r, k)
            elif t == 0xa0:
                si, k = _idx(r, 0)
                off = struct.unpack('<H', r[k:k + 2])[0]
                b = r[k + 2:]
                mod['data'][si][off:off + len(b)] = b
                last = (si, off, 'L')
            elif t == 0xa2:
                si, k = _idx(r, 0)
                off = struct.unpack('<H', r[k:k + 2])[0]
                b, _ = _lidata(r, k + 2)
                mod['data'][si][off:off + len(b)] = b
                last = (si, off, 'I')
            elif t == 0x9c and last:
                si, base, _ = last
                for loc, n, lt, m, frame, target, disp in _parse_fixupp(r, threads):
                    p = base + loc
                    for z in range(n):
                        if p + z < len(mod['mask'][si]):
                            mod['mask'][si][p + z] = 1
                    mod['fix'][si].append((p, n, lt, m, frame, target, disp))
            j += 3 + L
            if t in (0x8a, 0x8b):
                break
        mods.append(mod)
        if pg:
            i = j if j % pg == 0 else (j // pg + 1) * pg
        else:
            break
    return mods


def _lidata(r, k):
    rep = struct.unpack('<H', r[k:k + 2])[0]
    cnt = struct.unpack('<H', r[k + 2:k + 4])[0]
    k += 4
    if cnt == 0:
        n = r[k]
        out = r[k + 1:k + 1 + n] * rep
        return out, k + 1 + n
    out = b''
    for _ in range(cnt):
        b, k = _lidata(r, k)
        out += b
    return out * rep, k


def read(path):
    """compatibility: first module, segs/data like the old reader"""
    m = read_obj(path)[0]
    return dict(segs=m['segs'], data=m['data'], pubs=m['pubs'], lnames=m['lnames'], mask=m['mask'], fix=m['fix'])


if __name__ == '__main__':
    for m in read_obj(sys.argv[1]):
        print('module', m['name'])
        for k, s in enumerate(m['segs'], 1):
            print('  seg', k, s, 'fixups', len(m['fix'][k]))
        for g in m['groups']:
            print('  group', g)
        print('  pubs', m['pubs'])
        print('  exts', m['exts'])
