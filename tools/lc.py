#!/usr/bin/env python3
"""Compile a C file with Lattice C 3.10 and compare each function with the original EXE.

    python3 tools/lc.py FILE.C [--exe gamedata/GEN.EXE] [--syms src/gen/gen.syms] [-v] [--data OFF]

Options for LC come from a line "/* opts: ... */" in the file (default: -ms -v).
Every public function in the object is looked up by name in the symbol table (C lines) and
compared with the original bytes from that address, fixup bytes masked. With --data OFF the
DATA segment is compared with DGROUP:OFF. Exit status 0 when everything matches.
"""
import os, re, shutil, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import dos, omf, cmp, modgen


def compile_c(path, opts=None, keep=None):
    src = open(path, 'rb').read().decode('latin1')
    if opts is None:
        m = re.search(r'/\*\s*opts:\s*(.*?)\s*\*/', src)
        opts = m.group(1) if m else '-ms -v'
    tmp = keep or tempfile.mkdtemp(prefix='lc', dir=os.path.join(ROOT, 'build'))
    os.makedirs(tmp, exist_ok=True)
    stem = os.path.splitext(os.path.basename(path))[0].upper()[:8]
    shutil.copy(path, os.path.join(tmp, stem + '.C'))
    inc = os.path.dirname(os.path.abspath(path))
    for f in os.listdir(inc):
        if f.upper().endswith('.H'):
            shutil.copy(os.path.join(inc, f), os.path.join(tmp, f.upper()))
    obj = os.path.join(tmp, stem + '.OBJ')
    if os.path.exists(obj):
        os.unlink(obj)
    dos.run(tmp, ['LC %s %s > %s.LOG' % (opts, stem, stem)])
    log = open(os.path.join(tmp, stem + '.LOG'), 'rb').read().decode('latin1').replace('\r', '')
    return obj if os.path.exists(obj) else None, log, tmp


def check_fixups(m, k, off, end, at, img, ds, csym, dsym, byname, bydname):
    """Fixups to external symbols must land where the original points: the masked bytes are
    checked against the symbol tables (code names for self-relative, DGROUP names otherwise).
    Fixups to the module's own segments (string literals, its own data) are not checked."""
    import struct
    bad = []
    code = m['data'][k]
    for p, n, lt, mm, frame, target, disp in m['fix'][k]:
        if not (off <= p < end) or n != 2 or lt not in (1, 5):
            continue
        tm, ti = target
        if tm != 2:                      # only EXTDEF targets
            continue
        name = m['exts'][ti - 1].upper()
        here = at + (p - off)
        content = struct.unpack('<H', bytes(code[p:p + 2]))[0]
        orig = struct.unpack('<H', img[here:here + 2])[0]
        if mm == 0:                      # self-relative (near call/jump)
            if name not in byname:
                bad.append('%04X: %s not a code symbol' % (here, name))
                continue
            want = (byname[name] + disp + content - (here + 2)) & 0xffff
            if want != orig:
                tgt = (orig + here + 2) & 0xffff
                bad.append('%04X: calls %s, original calls %s' % (here, name, csym.get(tgt, '%04X' % tgt)))
        else:
            base = bydname.get(name)
            if base is None:
                base = byname.get(name)
                if base is None:
                    bad.append('%04X: %s not in the symbol table' % (here, name))
                    continue
            want = (base + disp + content) & 0xffff
            if want != orig:
                near = max([a for a in dsym if a <= orig] or [0])
                lab = dsym.get(near, '?')
                bad.append('%04X: %s%+d = %04X, original %04X (%s%+d)' % (
                    here, name, (disp + content) if (disp + content) < 0x8000 else (disp + content) - 0x10000,
                    want, orig, lab, orig - near))
    return bad


def main():
    a = sys.argv[1:]
    def opt(name, default):
        if name in a:
            k = a.index(name)
            v = a[k + 1]
            del a[k:k + 2]
            return v
        return default
    exe = opt('--exe', os.path.join(ROOT, 'gamedata', 'GEN.EXE'))
    syms = opt('--syms', os.path.join(ROOT, 'src', 'gen', 'gen.syms'))
    doff = opt('--data', None)
    verbose = '-v' in a
    a = [x for x in a if x != '-v']
    obj, log, tmp = compile_c(a[0])
    errs = [l for l in log.split('\n') if re.search(r'Error|error', l)]
    for l in log.split('\n'):
        if 'Error' in l or 'Warning' in l and verbose:
            print(l)
    if not obj:
        print(log)
        sys.exit(2)
    csym, dsym = modgen.load_syms(syms)
    byname = {v.upper(): k for k, v in csym.items()}
    bydname = {v.upper(): k for k, v in dsym.items()}
    img, ds = cmp.load_exe(exe)
    ok = True
    for m in omf.read_obj(obj):
        progs = [k for k, s in enumerate(m['segs'], 1) if s[1] == 'PROG']
        for k in progs:
            pubs = sorted((off, nm) for nm, si, off in m['pubs'] if si == k)
            ln = m['segs'][k - 1][2]
            for j, (off, nm) in enumerate(pubs):
                end = pubs[j + 1][0] if j + 1 < len(pubs) else ln
                if nm.upper() not in byname:
                    print('%-12s not in symbol table' % nm)
                    ok = False
                    continue
                at = byname[nm.upper()]
                r, msg = cmp.compare(m['data'][k][off:end], m['mask'][k][off:end], img[at:at + end - off], verbose)
                bad = check_fixups(m, k, off, end, at, img, ds, csym, dsym, byname, bydname)
                if r and bad:
                    r = False
                    msg = 'WRONG TARGETS (code matches, %d fixup(s) point elsewhere)' % len(bad)
                print('%-12s %04X %s' % (nm, at, msg.replace('PROG: ', '')))
                for b in bad[:12 if not verbose else 60]:
                    print('    ' + b)
                ok &= r
        if doff:
            for k, s in enumerate(m['segs'], 1):
                if s[0] == 'DATA' and s[2]:
                    d0 = int(doff, 16)
                    r, msg = cmp.compare(m['data'][k], m['mask'][k], img[ds + d0:ds + d0 + s[2]], verbose, 'DATA')
                    print(msg)
                    ok &= r
    shutil.rmtree(tmp, ignore_errors=True)
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
