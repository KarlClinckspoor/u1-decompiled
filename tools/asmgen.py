#!/usr/bin/env python3
"""Draft MASM source for a range of the code segment of an EXE.

    python3 tools/asmgen.py GAME.EXE START END [--syms SYMS.TXT]

Writes MASM 3/4 syntax to stdout: labels L_xxxx for jump/call targets in the range,
absolute DS references as names from SYMS.TXT (lines "OFF NAME", hex, DGROUP offsets) or
D_xxxx, near calls outside the range as F_xxxx. Forward JMPs are written plainly, so MASM's
EB xx 90 (short jump plus NOP) comes out the way the original assembler wrote it. Instructions
whose encoding MASM would not reproduce are left as DB with the disassembly as a comment.
The draft is a starting point: segment refs (relocations) are marked ";RELOC" for hand work.
"""
import re, struct, sys, os
import capstone
from capstone import x86

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
md.detail = True


def hexm(v):
    s = '%X' % v
    if s[0] in 'ABCDEF':
        s = '0' + s
    return s + 'H' if v > 9 else '%d' % v


def main():
    exe = sys.argv[1]
    a0, a1 = int(sys.argv[2], 16), int(sys.argv[3], 16)
    syms = {}
    if '--syms' in sys.argv:
        for line in open(sys.argv[sys.argv.index('--syms') + 1]):
            p = line.split()
            if len(p) >= 2 and not line.startswith('#'):
                syms[int(p[0], 16)] = p[1]
    d = open(exe, 'rb').read()
    h = struct.unpack('<14H', d[:28])
    img = d[h[4] * 16:]
    rel = {struct.unpack('<H', d[h[12] + 4 * i:h[12] + 4 * i + 2])[0]
           for i in range(h[3]) if struct.unpack('<H', d[h[12] + 4 * i + 2:h[12] + 4 * i + 4])[0] == 0}
    ins = list(md.disasm(img[a0:a1], a0))
    targets = set()
    for i in ins:
        if (i.mnemonic.startswith('j') or i.mnemonic in ('call', 'loop', 'jcxz', 'loopne', 'loope')) \
                and i.op_str.startswith('0x'):
            targets.add(int(i.op_str, 16) & 0xffff)

    def lab(t):
        if a0 <= t < a1:
            return 'L_%04X' % t
        return 'F_%04X' % t

    def dname(o):
        return syms.get(o, 'D_%04X' % o)

    out = []
    for i in ins:
        if i.address in targets:
            out.append('L_%04X:' % i.address)
        m = i.mnemonic
        ops = i.op_str
        relo = any(i.address <= r < i.address + i.size for r in rel)
        txt = None
        if m.startswith('j') or m in ('call', 'loop', 'jcxz', 'loopne', 'loope'):
            if ops.startswith('0x'):
                t = int(ops, 16) & 0xffff
                txt = '%s\t%s' % (m.upper(), lab(t))
                if m == 'jmp' and i.bytes[0] == 0xE9 and -128 <= t - (i.address + 2) <= 127:
                    txt = None   # near jmp that MASM would shorten: keep bytes
        elif m in ('rep stosw', 'rep stosb', 'rep movsb', 'rep movsw', 'repne scasb', 'repe cmpsb',
                   'lodsb', 'lodsw', 'stosb', 'stosw', 'movsb', 'movsw', 'scasb', 'cmpsb'):
            if 'es:[di]' in ops or ops == '' or '[si]' in ops:
                txt = m.upper()
        else:
            o = ops
            def memrepl(mm):
                inner = mm.group(2)
                seg = mm.group(1) or ''
                if re.fullmatch(r'0x[0-9a-f]+|\d+', inner):
                    v = int(inner, 0)
                    return (seg if seg else '') + dname(v)
                inner = re.sub(r'0x([0-9a-f]+)', lambda q: hexm(int(q.group(1), 16)), inner)
                inner = inner.replace(' ', '')
                return seg + '[' + inner + ']'
            o = re.sub(r'(\w\w:)?\[([^\]]+)\]', memrepl, o)
            o = o.replace('ptr ', 'PTR ').replace('word PTR', 'WORD PTR').replace('byte PTR', 'BYTE PTR')
            o = o.replace('dword PTR', 'DWORD PTR')
            if m in ('les', 'lds'):
                o = re.sub(r'\bPTR\s+', 'DWORD PTR ', o).replace('DWORD DWORD', 'DWORD')
            o = re.sub(r'\b0x([0-9a-f]+)\b', lambda q: hexm(int(q.group(1), 16)), o)
            txt = ('%s\t%s' % (m.upper(), o)).rstrip()
        b = ' '.join('%02X' % x for x in i.bytes)
        if txt is None:
            txt = 'DB\t' + ','.join(hexm(x) for x in i.bytes)
            out.append('\t%-40s; %04X %s %s' % (txt, i.address, m, ops))
        else:
            out.append('\t%-40s; %04X %s%s' % (txt, i.address, b, ' ;RELOC' if relo else ''))
    print('\n'.join(out))


if __name__ == '__main__':
    main()
