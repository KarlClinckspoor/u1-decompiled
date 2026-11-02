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
                print('%-12s %04X %s' % (nm, at, msg.replace('PROG: ', '')))
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
