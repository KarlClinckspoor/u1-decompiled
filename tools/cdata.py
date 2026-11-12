#!/usr/bin/env python3
"""Draft C definitions for a DGROUP range of an EXE (initialised data of a C module).

    python3 tools/cdata.py EXE SYMS D0 D1 [--refs ASM ...]

Splits the range at every D label of the .syms file and prints one C definition per label:
strings as string literals (several NUL-terminated strings under one label become a
`char *NAME[]` only when a following word table points at them), word tables whose values
point to string starts inside the range as `char *NAME[] = { "..", .. }` (Lattice puts the
strings of an initialised pointer array right before it), everything else as int or char
arrays. Unlabelled runs of strings (literals of the functions) are printed as comments with
the functions that reference them first when --refs lists the module's assembly stand-in(s).

The draft is a starting point: types come from how the code uses the data.
"""
import re, struct, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import modgen


def load(exe):
    d = open(exe, 'rb').read()
    h = struct.unpack('<14H', d[:28])
    img = d[h[4] * 16:]
    ds = struct.unpack('<H', img[1:3])[0] * 16
    return img, ds


def cstr(b):
    out = []
    for c in b:
        if c == 0x22:
            out.append('\\"')
        elif c == 0x5c:
            out.append('\\\\')
        elif c == 10:
            out.append('\\n')
        elif c == 13:
            out.append('\\r')
        elif 32 <= c < 127:
            out.append(chr(c))
        else:
            out.append('\\%03o' % c)
    return '"' + ''.join(out) + '"'


def strings_at(blob):
    """split blob into NUL-terminated strings if it is all text, else None"""
    if not blob or blob[-1] != 0:
        return None
    parts = blob[:-1].split(b'\0')
    for p in parts:
        if any(not (32 <= c < 127 or c in (10, 13)) for c in p):
            return None
    return [p for p in parts]


def refs(asm_files):
    """first function referencing each D label, from stand-in assembly"""
    first = {}
    for path in asm_files:
        cur = None
        for line in open(path, encoding='latin1'):
            m = re.match(r'^(F_[0-9A-F]{4}|[A-Z_][A-Z0-9_]*):', line)
            if m and m.group(1).startswith('F_'):
                cur = m.group(1)
            for d in re.findall(r'\bD_[0-9A-F]{4}\b', line):
                if cur and d not in first and not line.startswith('D_'):
                    first[d] = cur
    return first


def main():
    a = sys.argv[1:]
    ref_files = []
    if '--refs' in a:
        k = a.index('--refs')
        ref_files = a[k + 1:]
        a = a[:k]
    exe, syms, d0, d1 = a[0], a[1], int(a[2], 16), int(a[3], 16)
    img, ds = load(exe)
    _, dsym = modgen.load_syms(syms)
    first = refs(ref_files) if ref_files else {}
    labs = sorted(x for x in dsym if d0 <= x < d1)
    if not labs or labs[0] != d0:
        labs = [d0] + labs
    labs.append(d1)
    starts = set()
    for i in range(len(labs) - 1):
        blob = img[ds + labs[i]:ds + labs[i + 1]]
        ss = strings_at(blob.rstrip(b'\0') + b'\0') if blob else None
        if ss is not None:
            o = labs[i]
            for s in ss:
                starts.add(o)
                o += len(s) + 1
    for i in range(len(labs) - 1):
        o, e = labs[i], labs[i + 1]
        blob = img[ds + o:ds + e]
        name = dsym.get(o, 'D_%04X' % o)
        who = (' /* first used by %s */' % first[name]) if name in first else ''
        ss = strings_at(blob)
        pad = ''
        if ss is None and blob.endswith(b'\0\0'):
            ss2 = strings_at(blob[:-1])
            if ss2 is not None:
                ss, pad = ss2, ' /* + 1 pad byte */'
        if ss is not None:
            if len(ss) == 1:
                print('char %s[] = %s;%s%s  /* %04X */' % (name, cstr(ss[0]), pad, who, o))
            else:
                print('/* %04X %s: %d strings%s */%s' % (o, name, len(ss), pad, who))
                for s in ss:
                    print('    %s' % cstr(s))
            continue
        if len(blob) % 2 == 0 and len(blob) >= 2:
            ws = struct.unpack('<%dH' % (len(blob) // 2), blob)
            if all(w in starts for w in ws):
                print('char *%s[] = {%s  /* %04X: %d pointers to strings above */' % (name, who, o, len(ws)))
                for w in ws:
                    s = img[ds + w:img.index(b'\0', ds + w)]
                    print('    %s,  /* %04X */' % (cstr(s), w))
                print('};')
                continue
            print('int %s[%d] = {%s  /* %04X */' % (name, len(ws), who, o))
            for k in range(0, len(ws), 12):
                print('    ' + ', '.join('0x%X' % w if w > 9 else str(w) for w in ws[k:k + 12]) + ',')
            print('};')
            continue
        print('unsigned char %s[%d] = {%s  /* %04X */' % (name, len(blob), who, o))
        for k in range(0, len(blob), 16):
            print('    ' + ', '.join('0x%02X' % c for c in blob[k:k + 16]) + ',')
        print('};')


if __name__ == '__main__':
    main()
