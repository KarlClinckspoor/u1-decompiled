#!/usr/bin/env python3
"""Generate assembly stand-ins for modules of an EXE and make them assemble to the original bytes.

    python3 tools/genasm.py EXE SYMS OUTDIR NAME CODE0 CODE1 [DATA0 DATA1 [BSS0 BSS1]]

Runs tools/modgen.py, assembles with MASM 3.00, compares code and data with the EXE (fixups
masked) and writes every instruction MASM encodes differently as bytes (keeping symbolic
operands), until the object matches. Leaves OUTDIR/NAME.ASM and OUTDIR/NAME.OBJ.
OUTDIR/NAME.imm, if present, lists (hex) the instructions whose 16-bit immediate is a DGROUP
offset; they are written OFFSET DGROUP:name (tools/addrimm.py finds them).
"""
import os, subprocess, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import omf, dos, modgen
import capstone

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)


def build(exe, syms, outdir, name, code, data, bss, force, addrimm=()):
    g = modgen.Gen(exe, syms, name, code, data, bss, force)
    g.addrimm = set(addrimm)
    src = g.emit()
    open(os.path.join(outdir, name + '.ASM'), 'w', newline='').write(src)
    for f in (name + '.OBJ', name + '.LOG'):
        p = os.path.join(outdir, f)
        if os.path.exists(p):
            os.unlink(p)
    dos.run(outdir, ['D:\\MASM3\\MASM %s.ASM,%s.OBJ; > %s.LOG' % (name, name, name)])
    log = open(os.path.join(outdir, name + '.LOG'), 'rb').read().decode('latin1')
    if not os.path.exists(os.path.join(outdir, name + '.OBJ')) or 'Severe' in log and '\n  0 ' not in log:
        pass
    return log


def errors(log):
    lines = [l for l in log.replace('\r', '').split('\n') if l.strip()]
    for k, l in enumerate(lines):
        if l.strip().startswith('0') and k > 0 and 'Errors' in lines[k - 1]:
            pass
    tail = ' '.join(lines[-2:])
    nums = [int(x) for x in tail.split() if x.isdigit()]
    return sum(nums) if nums else 1


def mismatches(obj, img, ds, code, data):
    bad_code, bad_data = set(), []
    for m in omf.read_obj(obj):
        for k, (sn, cn, ln, attr) in enumerate(m['segs'], 1):
            if ln == 0:
                continue
            b, mask = m['data'][k], m['mask'][k]
            if cn == 'PROG' and code:
                orig = img[code[0]:code[0] + ln]
                if ln != code[1] - code[0]:
                    raise SystemExit('%s: PROG is %d bytes, expected %d' % (obj, ln, code[1] - code[0]))
                diffs = [i for i in range(ln) if not mask[i] and b[i] != orig[i]]
                if diffs:
                    starts = []
                    a = 0
                    for ins in md.disasm(bytes(orig), code[0]):
                        starts.append(ins.address - code[0])
                    import bisect
                    for x in diffs:
                        s = starts[bisect.bisect_right(starts, x) - 1]
                        bad_code.add(code[0] + s)
            elif sn == 'DATA' and data:
                orig = img[ds + data[0]:ds + data[0] + ln]
                diffs = [i for i in range(ln) if not mask[i] and b[i] != orig[i]]
                if diffs or ln != data[1] - data[0]:
                    bad_data.append((ln, diffs[:5]))
    return bad_code, bad_data


def main():
    a = sys.argv[1:]
    exe, syms, outdir, name = a[:4]
    code = modgen.rng(a[4], a[5]) if len(a) > 5 else None
    data = modgen.rng(a[6], a[7]) if len(a) > 7 else None
    bss = modgen.rng(a[8], a[9]) if len(a) > 9 else None
    os.makedirs(outdir, exist_ok=True)
    d = open(exe, 'rb').read()
    h = struct.unpack('<14H', d[:28])
    img = d[h[4] * 16:]
    ds = struct.unpack('<H', img[1:3])[0] * 16
    force = set()
    ffile = os.path.join(outdir, name + '.force')
    if os.path.exists(ffile):
        force = {int(x, 16) for x in open(ffile).read().split()}
    ifile = os.path.join(outdir, name + '.imm')    # instructions whose immediate is an address
    addrimm = {int(x, 16) for x in open(ifile).read().split()} if os.path.exists(ifile) else set()
    for it in range(8):
        log = build(exe, syms, outdir, name, code, data, bss, force, addrimm)
        if errors(log):
            sys.stdout.write(log)
            raise SystemExit('%s: assembly errors' % name)
        bc, bd = mismatches(os.path.join(outdir, name + '.OBJ'), img, ds, code, data)
        if bd:
            raise SystemExit('%s: data mismatch %s' % (name, bd))
        if not bc:
            open(ffile, 'w').write(' '.join('%04X' % x for x in sorted(force)) + '\n')
            print('%s: MATCH (%d instructions written as bytes)' % (name, len(force)))
            return
        new = bc - force
        if not new:
            raise SystemExit('%s: still differs at %s' % (name, ' '.join('%04X' % x for x in sorted(bc))))
        force |= new
    raise SystemExit('%s: no convergence' % name)


if __name__ == '__main__':
    main()
