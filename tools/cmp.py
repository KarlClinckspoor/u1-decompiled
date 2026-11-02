#!/usr/bin/env python3
"""Compare an object module with the original EXE, fixup bytes masked.

    python3 tools/cmp.py FILE.OBJ GAME.EXE CODEOFF [DATAOFF] [-v]

CODEOFF is the offset of the module's PROG segment in the code segment (image offset; the
code segment starts at image 0), DATAOFF the offset of its DATA segment in DGROUP.
Prints MATCH or the differing instructions. Exit status 0 on match.
"""
import struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import omf
import capstone

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)


def load_exe(path):
    d = open(path, 'rb').read()
    h = struct.unpack('<14H', d[:28])
    img = d[h[4] * 16:]
    ds = struct.unpack('<H', img[1:3])[0] * 16
    return img, ds


def compare(code, mask, orig, verbose=False, label='PROG'):
    n = min(len(code), len(orig))
    bad = [i for i in range(n) if not mask[i] and code[i] != orig[i]]
    if len(orig) < len(code):
        bad.append(len(orig))
    if not bad:
        return True, '%s: MATCH (%d bytes)' % (label, len(code))
    out = ['%s: %d differing bytes of %d, first at %04x' % (label, len(bad), len(code), bad[0])]
    if label == 'PROG':
        a = {i.address: i for i in md.disasm(bytes(code), 0)}
        b = {i.address: i for i in md.disasm(bytes(orig[:len(code) + 16]), 0)}
        shown = 0
        for off in sorted(a):
            i = a[off]
            if any(off <= x < off + i.size for x in bad):
                j = b.get(off)
                out.append('  %04x  %-34s | %s' % (off, i.mnemonic + ' ' + i.op_str,
                                                    (j.mnemonic + ' ' + j.op_str) if j else '?'))
                shown += 1
                if shown > (60 if verbose else 12):
                    break
    else:
        for x in bad[:16]:
            out.append('  %04x  obj %02x  exe %02x' % (x, code[x], orig[x] if x < len(orig) else -1))
    return False, '\n'.join(out)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    verbose = '-v' in sys.argv
    obj, exe, coff = args[0], args[1], int(args[2], 16)
    doff = int(args[3], 16) if len(args) > 3 else None
    img, ds = load_exe(exe)
    ok = True
    for m in omf.read_obj(obj):
        for k, (sn, cn, ln, attr) in enumerate(m['segs'], 1):
            if ln == 0:
                continue
            if cn in ('PROG', 'CODE'):
                r, msg = compare(m['data'][k], m['mask'][k], img[coff:coff + ln], verbose)
            elif sn == 'DATA' and doff is not None:
                r, msg = compare(m['data'][k], m['mask'][k], img[ds + doff:ds + doff + ln], verbose, 'DATA')
            else:
                print('%s: %d bytes (not compared)' % (sn, ln))
                continue
            ok &= r
            print(msg)
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
