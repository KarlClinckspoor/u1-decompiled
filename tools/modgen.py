#!/usr/bin/env python3
"""Generate a MASM module that assembles to a given piece of an EXE, with symbols.

    python3 tools/modgen.py EXE SYMS NAME CODE0 CODE1 [DATA0 DATA1] [BSS0 BSS1] > NAME.ASM

EXE    the original executable (small model: code at image 0, DGROUP after it)
SYMS   symbol table, lines "C|D OFFSET NAME" (hex; C = code offset, D = DGROUP offset).
       Names of symbols inside this module's ranges become PUBLIC labels; references to
       symbols outside become EXTRN. Unnamed targets get local labels.
CODE0 CODE1 / DATA0 DATA1 / BSS0 BSS1: the module's ranges (hex, end exclusive; '-' for none).

The code is written instruction by instruction in MASM syntax. An instruction MASM would
encode differently is written as DB/DW with its operand still symbolic; tools/asmfix.py does
that after a first assembly. Absolute DS operands become NAME or NAME+k (nearest symbol at
or below). Immediates stay numbers unless the instruction carries a relocation (DGROUP or
PROG frame). Forward and backward jumps keep their original short/near form.
"""
import re, struct, sys, os, bisect
import capstone
from capstone import x86

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
md.detail = True


def hexm(v):
    if v < 10:
        return '%d' % v
    s = '%X' % v
    if s[0] in 'ABCDEF':
        s = '0' + s
    return s + 'H'


NOSEED = set()


def load_syms(path):
    """C = code entry point, L = code label on data (not disassembled from), D = DGROUP data"""
    code, data = {}, {}
    for line in open(path):
        p = line.split()
        if len(p) < 3 or line.startswith('#'):
            continue
        if p[0] == 'L':
            NOSEED.add(int(p[1], 16))
        (code if p[0] in 'CL' else data)[int(p[1], 16)] = p[2]
    return code, data


def c0lab(g, t):
    return g.code and g.code[0] <= t < g.code[1]


def rng(a, b):
    return None if a == '-' else (int(a, 16), int(b, 16))


class Gen:
    def __init__(self, exe, syms, name, code, data, bss, force=None):
        d = open(exe, 'rb').read()
        h = struct.unpack('<14H', d[:28])
        self.img = d[h[4] * 16:]
        self.ds = struct.unpack('<H', self.img[1:3])[0]
        self.rel = {}
        for i in range(h[3]):
            o, s = struct.unpack('<HH', d[h[12] + 4 * i:h[12] + 4 * i + 4])
            self.rel[s * 16 + o] = struct.unpack('<H', self.img[s * 16 + o:s * 16 + o + 2])[0]
        self.csym, self.dsym = load_syms(syms)
        self.name, self.code, self.data, self.bss = name, code, data, bss
        self.dkeys = sorted(self.dsym)
        self.ext_near, self.ext_data, self.pub = set(), set(), []
        self.bare = True
        self.exe = exe
        self.dbytes = set()
        self.force = force or set()     # instruction offsets to write as bytes

    def inmod_data(self, o):
        return any(r and r[0] <= o < r[1] for r in (self.data, self.bss))

    def dref(self, o):
        """name for DGROUP offset o (exact or nearest below), recording EXTRN"""
        k = bisect.bisect_right(self.dkeys, o) - 1
        if k >= 0 and o - self.dkeys[k] < 0x400:
            base = self.dkeys[k]
            nm = self.dsym[base]
            # the nearest symbol must be in the same module range as o, or exact
            if self.inmod_data(o) != self.inmod_data(base) and base != o:
                nm, base = None, None
        else:
            nm, base = None, None
        if nm is None:
            if self.inmod_data(o):
                nm, base = 'D_%04X' % o, o
                self.dsym[o] = nm
                bisect.insort(self.dkeys, o)
            else:
                sys.stderr.write('warning: no symbol for DGROUP:%04X in %s, left as a number\n' % (o, self.name))
                return 'DS:' + hexm(o) if self.bare else hexm(o)
        if not self.inmod_data(base):
            self.ext_data.add(nm)
        return nm if o == base else '%s+%d' % (nm, o - base)

    def cref(self, t):
        c0, c1 = self.code
        if c0 <= t < c1:
            return self.csym.get(t, 'L_%04X' % t)
        if t not in self.csym:
            raise SystemExit('no symbol for code %04X (called from %s)' % (t, self.name))
        self.ext_near.add(self.csym[t])
        return self.csym[t]

    def clab(self, t):
        """label for a code address used as data (CS: operand); label+k inside an item"""
        k = bisect.bisect_right(self.starts, t) - 1
        st = self.starts[k]
        self.cstargets.add(st)
        nm = self.csym.get(st, 'L_%04X' % st)
        return nm if st == t else '%s+%d' % (nm, t - st)

    def operand(self, i):
        o = i.op_str

        def memrepl(mm):
            seg = mm.group(1) or ''
            inner = mm.group(2)
            if re.fullmatch(r'0x[0-9a-f]+|\d+', inner) and seg == 'cs:' and self.code \
                    and self.code[0] <= int(inner, 0) < self.code[1]:
                return 'CS:' + self.clab(int(inner, 0))
            if re.fullmatch(r'0x[0-9a-f]+|\d+', inner):
                self.bare = not seg
                r = self.dref(int(inner, 0))
                if r.startswith('DS:') or re.fullmatch(r'[0-9A-F]+H?', r):
                    return (seg + '[' + r + ']') if seg else r
                return seg + r
            parts = inner.replace(' ', '')
            m2 = re.fullmatch(r'([a-z+]+)([+-])(0x[0-9a-f]+|\d+)', parts)
            if m2 and seg == 'cs:' and m2.group(2) == '+' and self.code and \
                    self.code[0] <= int(m2.group(3), 0) < self.code[1]:
                return 'CS:%s[%s]' % (self.clab(int(m2.group(3), 0)), m2.group(1).upper())
            if m2 and m2.group(1) in ('bx', 'si', 'di', 'bx+si', 'bx+di') and m2.group(2) == '+' \
                    and int(m2.group(3), 0) >= 0x100 and seg not in ('es:', 'ss:'):   # ES/SS: not DGROUP
                return seg + '%s[%s]' % (self.dref(int(m2.group(3), 0)), m2.group(1))
            parts = re.sub(r'0x([0-9a-f]+)', lambda q: hexm(int(q.group(1), 16)), parts)
            return seg + '[' + parts + ']'
        o = re.sub(r'(\b[cdes]s:)?\[([^\]]+)\]', memrepl, o)
        o = o.replace('dword ptr', 'DWORD PTR').replace('word ptr', 'WORD PTR').replace('byte ptr', 'BYTE PTR')
        if i.mnemonic in ('les', 'lds'):
            o = o.replace(', ptr ', ', DWORD PTR ')
        o = re.sub(r'\b0x([0-9a-f]+)\b', lambda q: hexm(int(q.group(1), 16)), o)
        return o.upper().replace('D_', 'D_')

    def insn(self, i):
        m = i.mnemonic
        a = i.address
        relo = [r for r in self.rel if a <= r < a + i.size]
        if a in self.force:
            return None
        if m in ('cdq', 'cwde') and i.size == 1:    # capstone's names for 99h/98h; 16-bit CWD/CBW
            return {'cdq': 'CWD', 'cwde': 'CBW'}[m]
        if m in ('call', 'jmp', 'ljmp', 'lcall') or m.startswith('j') or m.startswith('loop') or m == 'jcxz':
            if not i.op_str.startswith('0x'):
                if m in ('ljmp', 'lcall'):
                    op = self.operand(i)
                    if ':' in i.op_str and '[' not in i.op_str:
                        return None          # direct far jump/call
                    op = re.sub(r'^(DWORD |WORD )?PTR ', '', op)
                    return '%s\tDWORD PTR %s' % (m[1:].upper(), op)
                return '%s\t%s' % (m.upper(), self.operand(i))
            t = int(i.op_str, 16) & 0xffff
            lab = self.cref(t)
            if m == 'jmp':
                if i.bytes[0] == 0xEB:
                    return 'JMP\tSHORT %s' % lab
                if c0lab(self, t):
                    return 'DB\t0E9H\n\tDW\t%s-$-2' % lab
                return 'JMP\t%s' % lab
            return '%s\t%s' % (m.upper(), lab)
        if m in ('rep movsb', 'rep movsw', 'rep stosb', 'rep stosw', 'repne scasb', 'repe cmpsb',
                 'repe cmpsw', 'repne scasw', 'lodsb', 'lodsw', 'stosb', 'stosw', 'movsb', 'movsw',
                 'scasb', 'scasw', 'cmpsb', 'cmpsw'):
            if '[si]' in i.op_str and not i.op_str.count('ds:') and 'es:' in i.op_str and 'movs' not in m \
                    and 'cmps' not in m:
                pass
            if re.search(r'\b(cs|ss|es):\[si\]', i.op_str) and 'movs' in m:
                return None
            return m.upper()
        if relo:
            r = relo[0]
            v = self.rel[r]
            frame = {self.ds: 'DGROUP', 0: 'PROG'}.get(v)
            if frame is None:
                return None
            ops = self.operand(i)
            ops = re.sub(r'\b' + hexm(v) + r'\b', frame, ops) if v else re.sub(r', 0$', ', ' + frame, ops)
            return '%s\t%s' % (m.upper(), ops)
        if m in ('in', 'out', 'int', 'ret', 'retf', 'iret', 'cbw', 'cwd', 'cli', 'sti', 'cld', 'std',
                 'clc', 'stc', 'cmc', 'nop', 'hlt', 'pushf', 'popf', 'lahf', 'sahf', 'xlatb', 'wait',
                 'into', 'aaa', 'aas', 'daa', 'das', 'aam', 'aad'):
            o = re.sub(r'\b0x([0-9a-f]+)\b', lambda q: hexm(int(q.group(1), 16)), i.op_str).upper()
            if m == 'xlatb':
                return 'XLAT'
            return ('%s\t%s' % (m.upper(), o)).rstrip()
        if m.startswith('f'):
            return None
        return ('%s\t%s' % (m.upper(), self.operand(i))).rstrip()

    def forced(self, i):
        """an instruction as DB, with an absolute DS displacement kept symbolic"""
        b = bytes(i.bytes)
        do = getattr(i, 'disp_offset', 0)
        sym = None
        for op in i.operands:
            if op.type == x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and do and \
                    op.mem.segment in (0, x86.X86_REG_DS, x86.X86_REG_ES, x86.X86_REG_SS) and i.disp_size == 2:
                sym = self.dref(op.mem.disp & 0xffff)
                if re.fullmatch(r'(DS:)?[0-9A-F]+H?', sym):
                    sym = None
        if sym is None:
            return 'DB\t' + ','.join(hexm(x) for x in b)
        parts = []
        if do:
            parts.append('DB\t' + ','.join(hexm(x) for x in b[:do]))
        parts.append('DW\t' + sym)
        if do + 2 < len(b):
            parts.append('DB\t' + ','.join(hexm(x) for x in b[do + 2:]))
        return '\n\t'.join(parts)

    def code_lines(self):
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        from rdis import analyse
        c0, c1 = self.code
        if not hasattr(Gen, '_an'):
            Gen._an = analyse(self.exe, sorted(set(self.csym) - NOSEED))
        allins = Gen._an[4]
        ins = []
        a = c0
        while a < c1:
            i = allins.get(a)
            if i is not None and a + i.size <= c1 and a not in self.dbytes:
                i = next(md.disasm(self.img[a:a + i.size], a))
                ins.append(('i', a, i))
                a += i.size
            else:
                ins.append(('db', a, self.img[a:a + 1]))
                a += 1
        self.starts = [a for k, a, i in ins]
        self.cstargets = set()
        targets = set()
        for k, a, i in ins:
            if k == 'i' and (i.mnemonic.startswith('j') or i.mnemonic.startswith('loop') or
                             i.mnemonic in ('call', 'jcxz')) and i.op_str.startswith('0x'):
                t = int(i.op_str, 16) & 0xffff
                if c0 <= t < c1:
                    targets.add(t)
        body = []
        for k, a, i in ins:
            if k == 'db':
                body.append((a, '', True))
                continue
            t = self.insn(i)
            raw = ' '.join('%02X' % x for x in i.bytes)
            if t is None:
                t = self.forced(i)
                body.append((a, '\t%-40s; %04X %s %s' % (t, a, i.mnemonic, i.op_str), False))
            else:
                body.append((a, '\t%-40s; %04X %s' % (t, a, raw), False))
        out = []
        # merge runs of single bytes (data or unreached code in the code segment)
        merged = []
        for a, line, plain in body:
            if merged and plain and merged[-1][2] \
                    and a not in self.csym and a not in targets and a not in self.cstargets \
                    and len(merged[-1][3]) < 12:
                merged[-1][3].append(self.img[a])
                continue
            merged.append([a, line, plain, [self.img[a]] if plain else []])
        for a, line, isdb, bs in merged:
            if a in self.csym:
                out.append('%s:' % self.csym[a])
                if self.csym[a] not in self.pub:
                    self.pub.append(self.csym[a])
            elif a in targets or a in self.cstargets:
                out.append('L_%04X:' % a)
            if isdb:
                out.append('\tDB\t%s\t; %04X' % (','.join(hexm(x) for x in bs), a))
            else:
                out.append(line)
        return out

    def data_lines(self, r, bss=False):
        d0, d1 = r
        base = self.ds * 16
        labs = sorted(o for o in self.dsym if d0 <= o < d1)
        out = []
        cuts = sorted(set([d0] + labs + [d1]))
        for k in range(len(cuts) - 1):
            a, b = cuts[k], cuts[k + 1]
            nm = self.dsym.get(a)
            lab = nm if nm else ''
            if nm and nm not in self.pub:
                self.pub.append(nm)
            if bss:
                out.append('%s\tDB\t%s DUP (?)\t\t; %04X' % (lab, hexm(b - a), a))
                continue
            blob = self.img[base + a:base + b]
            first = True
            for j in range(0, len(blob), 12):
                chunk = blob[j:j + 12]
                out.append('%s\tDB\t%s\t; %04X' % (lab if first else '', ','.join(hexm(x) for x in chunk), a + j))
                first = False
        return out

    def emit(self):
        code = self.code_lines() if self.code else []
        data = self.data_lines(self.data) if self.data else []
        bss = self.data_lines(self.bss, True) if self.bss else []
        L = ['\tNAME\t%s' % self.name, ';', '; Generated by tools/modgen.py from the original executable.', ';',
             '\t.8087',
             "PROG\tSEGMENT\tBYTE PUBLIC 'PROG'", 'PROG\tENDS',
             "DATA\tSEGMENT\tWORD PUBLIC 'DATA'", 'DATA\tENDS',
             "UDATA\tSEGMENT\tWORD PUBLIC 'DATA'", 'UDATA\tENDS',
             'DGROUP\tGROUP\tDATA,UDATA', '\tASSUME\tCS:PROG,DS:DGROUP', '']
        if self.ext_data:
            L += ['DATA\tSEGMENT'] + ['\tEXTRN\t%s:BYTE' % n for n in sorted(self.ext_data)] + ['DATA\tENDS']
        if self.ext_near:
            L += ['PROG\tSEGMENT'] + ['\tEXTRN\t%s:NEAR' % n for n in sorted(self.ext_near)] + ['PROG\tENDS']
        if self.pub:
            for k in range(0, len(self.pub), 6):
                L.append('\tPUBLIC\t' + ','.join(self.pub[k:k + 6]))
        L.append('')
        if code:
            L += ['PROG\tSEGMENT'] + code + ['PROG\tENDS', '']
        if data:
            L += ['DATA\tSEGMENT'] + data + ['DATA\tENDS', '']
        if bss:
            L += ['UDATA\tSEGMENT'] + bss + ['UDATA\tENDS', '']
        L.append('\tEND')
        return ('\n'.join(L) + '\n').replace('\r\n', '\n').replace('\n', '\r\n')


def main():
    a = sys.argv[1:]
    force = set()
    if '--force' in a:
        k = a.index('--force')
        force = {int(x, 16) for x in a[k + 1].split(',') if x}
        del a[k:k + 2]
    exe, syms, name = a[0], a[1], a[2]
    code = rng(a[3], a[4])
    data = rng(a[5], a[6]) if len(a) > 6 else None
    bss = rng(a[7], a[8]) if len(a) > 8 else None
    g = Gen(exe, syms, name, code, data, bss, force)
    sys.stdout.write(g.emit())


if __name__ == '__main__':
    main()
