#!/usr/bin/env python3
"""Find the 16-bit immediates in a piece of code that are addresses, by comparing two EXEs.

    python3 tools/addrimm.py EXE START END OTHER.EXE [-o FILE.imm]
    python3 tools/addrimm.py EXE --data D0 D1 OTHER.EXE OD0 [-o FILE.ptr]

The code EXE:START-END is looked for in OTHER.EXE with every 16-bit immediate, displacement and
call/jump target masked. An immediate that differs between the two copies moved with the data
layout, so it is an address (a string, a table); one that is the same is a constant. Prints the
instruction offsets (in EXE) of the address immediates; -o writes them in the NAME.imm format
tools/genasm.py reads. A plain number in an instruction cannot otherwise be told from an address:
no relocation marks DGROUP offsets in a small-model EXE.

With --data, EXE's DGROUP D0-D1 (a module's data) is compared with OTHER's from OD0: the words
that differ are near pointers, written in the NAME.ptr format (DGROUP offsets in EXE).
"""
import os, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import capstone
from xmap import load, find

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
md.detail = True


def pattern(code, base):
    """must-match mask: everything but 16-bit immediates, displacements and rel16 targets"""
    m = bytearray([1]) * len(code)
    for i in md.disasm(code, base):
        o = i.address - base
        if i.mnemonic in ('call', 'jmp') and i.size == 3 and code[o] in (0xE8, 0xE9):
            m[o + 1:o + 3] = b'\0\0'
        if i.disp_size == 2:
            m[o + i.disp_offset:o + i.disp_offset + 2] = b'\0\0'
        if i.imm_size == 2:
            m[o + i.imm_offset:o + i.imm_offset + 2] = b'\0\0'
    return bytes(code), m


def data_pointers(exe, d0, d1, other, od0):
    _, _, img, ds = load(exe)
    _, _, oimg, ods = load(other)
    a, b = img[ds + d0:ds + d1], oimg[ods + od0:ods + od0 + d1 - d0]
    out, k = [], 0
    while k < len(a) - 1:
        if a[k:k + 2] != b[k:k + 2] and k % 2 == 0:     # pointers are words at even offsets
            out.append(d0 + k)
            k += 2
        else:
            k += 1
    bad = [d0 + k for k in range(len(a)) if a[k] != b[k] and not any(p <= d0 + k < p + 2 for p in out)]
    return out, bad


def main():
    a = sys.argv[1:]
    out = a[a.index('-o') + 1] if '-o' in a else None
    a = [x for x in a if x not in ('-o', out)]
    if a[1] == '--data':
        ptrs, bad = data_pointers(a[0], int(a[2], 16), int(a[3], 16), a[4], int(a[5], 16))
        print('%d pointers%s' % (len(ptrs), ('; bytes that differ outside them: ' + ' '.join('%04X' % x for x in bad[:10])) if bad else ''))
        if out:
            open(out, 'w').write(' '.join('%04X' % x for x in ptrs) + '\n')
        return
    exe, s0, s1, other = a[0], int(a[1], 16), int(a[2], 16), a[3]
    _, _, img, ds = load(exe)
    _, _, oimg, ods = load(other)
    f = find(oimg[:ods], *pattern(img[s0:s1], s0))
    if len(f) != 1:
        raise SystemExit('%04X-%04X found %d times in %s' % (s0, s1, len(f), other))
    p = f[0]
    hits = []
    for x, y in zip(md.disasm(img[s0:s1], s0), md.disasm(oimg[p:p + s1 - s0], p)):
        if x.size != y.size:
            break
        if x.imm_size == 2 and not x.mnemonic.startswith(('j', 'call', 'loop')):
            vx = struct.unpack('<H', bytes(x.bytes[x.imm_offset:x.imm_offset + 2]))[0]
            vy = struct.unpack('<H', bytes(y.bytes[y.imm_offset:y.imm_offset + 2]))[0]
            if vx != vy:
                hits.append(x.address)
                print('%04X  %-6s %-28s | %04X %s' % (x.address, x.mnemonic, x.op_str, y.address, y.op_str))
    print('%d address immediates (%s copy at %04X)' % (len(hits), os.path.basename(other), p))
    if out:
        open(out, 'w').write(' '.join('%04X' % h for h in hits) + '\n')


if __name__ == '__main__':
    main()
