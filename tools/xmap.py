#!/usr/bin/env python3
"""Find GEN's shared modules (src/lclib, src/origin) in another executable and name its symbols.

    python3 tools/xmap.py EXE [-o NAME.syms] [--map]

Each library or Origin module linked into GEN (src/gen/gen.lnk) is looked for in EXE by its
code bytes, with operands that hold addresses masked. For every module found exactly once:
  - its code symbols get their GEN names at the new address;
  - GEN's copy and EXE's copy are disassembled side by side, and every address operand that
    differs (data, or code outside the module) maps a GEN address to an EXE address, so the
    GEN names of data symbols (ERRNO, _UFBS, D_07C6, ...) are carried over too.
The rest of EXE's code gets F_xxxx names from tools/rdis.py (function entries), and every DGROUP
address it references gets D_xxxx. With -o the result is written as a .syms file; --map prints
where each module is and the code that no module covers.
"""
import bisect, glob, os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import capstone
from capstone import x86
import modgen

REFS = ['gen', 'mondain', 'space', 'ultima']   # builds whose modules are known, in order of trust

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
md.detail = True


def load(path):
    d = open(path, 'rb').read()
    h = struct.unpack('<14H', d[:28])
    img = d[h[4] * 16:]
    return d, h, img, struct.unpack('<H', img[1:3])[0] * 16


def masked(code, base):
    """bytes and a must-match mask: rel16 call/jmp targets, 16-bit displacements and 16-bit
    immediates >= 0x100 may differ"""
    m = bytearray([1]) * len(code)
    for i in md.disasm(code, base):
        o = i.address - base
        if i.mnemonic in ('call', 'jmp') and i.size == 3 and code[o] in (0xE8, 0xE9):
            m[o + 1:o + 3] = b'\0\0'
        if i.disp_size == 2:
            m[o + i.disp_offset:o + i.disp_offset + 2] = b'\0\0'
        if i.imm_size == 2 and struct.unpack('<H', code[o + i.imm_offset:o + i.imm_offset + 2])[0] >= 0x100:
            m[o + i.imm_offset:o + i.imm_offset + 2] = b'\0\0'
    return bytes(code), m


def find(img, code, m):
    """start offsets in img where code matches wherever the mask says it must"""
    out = []
    at = next((j for j in range(len(code) - 3) if all(m[j:j + 4])), None)   # 4 must-match bytes
    if at is None:
        return out
    anchor = bytes(code[at:at + 4])
    k = img.find(anchor)
    while k >= 0:
        s = k - at
        if s >= 0 and s + len(code) <= len(img) and all(not m[j] or img[s + j] == code[j] for j in range(len(code))):
            out.append(s)
        k = img.find(anchor, k + 1)
    return out


def ref_modules(ref):
    """(name, code start, code end) of the shared modules with code in reference build REF
    (src/REF/REF.lnk), at their addresses in REF.EXE, in address order"""
    mods = []
    for l in open(os.path.join(ROOT, 'src', ref, ref + '.lnk')):
        l = l.split(';')[0].strip()
        if 'lclib' in l or 'origin' in l or 'lattice' in l:
            mods.append(os.path.normpath(os.path.join(ROOT, 'src', ref, l)))
    first = []
    for p in mods:
        t = open(p, encoding='latin1', newline='').read()
        if p.upper().endswith('.C'):
            a = re.findall(r'^(?:char )?F_([0-9A-F]{4})\(', t, re.M)
        else:   # first instruction address inside a PROG SEGMENT ... ENDS block
            a = []
            for blk in re.findall(r'^PROG\tSEGMENT\r\n(.*?)^PROG\tENDS', t, re.M | re.S):
                a += re.findall(r'; ([0-9A-F]{4})\b', blk)[:1]
        if os.path.basename(p).upper() == 'C.ASM':
            a = ['0000']               # the startup, hand-written: CXINIT at 0
        if a:
            first.append((int(a[0], 16), os.path.splitext(os.path.basename(p))[0]))
    first.sort()
    gd, gh, gimg, gds = load(os.path.join(ROOT, 'gamedata', ref.upper() + '.EXE'))
    end = max(a for a in range(gds) if gimg[a] != 0) + 1     # code end before DGROUP padding
    return [(n, a, first[k + 1][0] if k + 1 < len(first) else end) for k, (a, n) in enumerate(first)]


def operands(i):
    """address-like operands: (kind, byte offset in the instruction, value)"""
    out = []
    if i.mnemonic in ('call', 'jmp') and i.size == 3 and i.bytes[0] in (0xE8, 0xE9):
        out.append(('code', 1, int(i.op_str, 16) & 0xffff))
    if i.disp_size == 2:
        seg = None
        for op in i.operands:
            if op.type == x86.X86_OP_MEM:
                seg = op.mem.segment
        kind = {x86.X86_REG_CS: 'cs', x86.X86_REG_ES: 'es', x86.X86_REG_SS: 'es'}.get(seg, 'data')
        out.append((kind, i.disp_offset,
                    struct.unpack('<H', bytes(i.bytes[i.disp_offset:i.disp_offset + 2]))[0]))
    if i.imm_size == 2 and not (i.mnemonic.startswith(('j', 'call', 'loop')) or i.mnemonic in ('ljmp', 'lcall')):
        v = struct.unpack('<H', bytes(i.bytes[i.imm_offset:i.imm_offset + 2]))[0]
        if v >= 0x100:
            out.append(('imm', i.imm_offset, v))
    return out


def main():
    a = sys.argv[1:]
    out = a[a.index('-o') + 1] if '-o' in a else None
    showmap = '--map' in a
    exe = [x for x in a if not x.startswith('-') and x != out][0]
    d, h, img, ds = load(exe)
    dlimit = len(img) - ds + h[5] * 16      # DGROUP including BSS (minalloc)
    code, data, notes = {}, {}, []
    placed, seen = [], set()
    exact = {}                     # name -> address from an exact operand (not name+k)
    import omf
    for ref in REFS:
        if not os.path.isdir(os.path.join(ROOT, 'build', ref)):
            continue
        gd, gh, gimg, gds = load(os.path.join(ROOT, 'gamedata', ref.upper() + '.EXE'))
        gcode, gdata = modgen.load_syms(os.path.join(ROOT, 'src', ref, ref + '.syms'))
        gdkeys = sorted(gdata)
        for name, a0, a1 in ref_modules(ref):
            if name in seen:
                continue
            seen.add(name)
            obj = os.path.join(ROOT, 'build', ref, name.upper() + '.OBJ')
            if os.path.exists(obj):      # exact: the bytes LINK fixes up may differ, nothing else
                m = omf.read_obj(obj)[0]
                k = next(k for k, s in enumerate(m['segs'], 1) if s[1] == 'PROG' and s[2])
                pat = (bytes(m['data'][k]), bytes(1 - x for x in m['mask'][k]))
            else:                         # no build yet: guess address operands from the code
                pat = masked(gimg[a0:a1], a0)
            a1 = a0 + len(pat[0])          # the module's own length
            f = find(img[:ds], *pat)
            if len(f) != 1:
                notes.append('%s (%s %04X-%04X): found %d times' % (name, ref.upper(), a0, a1, len(f)))
                continue
            p = f[0]
            placed.append((p, p + a1 - a0, name))
            for k, v in gcode.items():
                if a0 <= k < a1:
                    code[p + k - a0] = v
            src = [q for q in glob.glob(os.path.join(ROOT, 'src', '*', name + '.*')) if q.upper().endswith(('.ASM', '.C'))]
            own = {}                       # this module's own data labels: GEN address -> name
            if src:
                t = open(src[0], encoding='latin1', newline='').read()
                for blk in re.findall(r'^DATA\tSEGMENT\r?\n(.*?)^DATA\tENDS', t, re.M | re.S):
                    for nm in re.findall(r'^(\w+)\t(?:DB|DW)', blk, re.M):
                        own.update({k: v for k, v in gdata.items() if v == nm})
            shift = None
            fix = pat[1]                  # 0 where GEN's object has a fixup (an address)
            gi = list(md.disasm(gimg[a0:a1], a0))
            ti = list(md.disasm(img[p:p + a1 - a0], p))
            for x, y in zip(gi, ti):
                if x.size != y.size:
                    break
                for (kx, ox, vx), (ky, oy, vy) in zip(operands(x), operands(y)):
                    if kx == 'code' and not (a0 <= vx < a1) and vx in gcode:
                        code.setdefault(vy, gcode[vx])
                    elif kx in ('data', 'imm') and fix[x.address - a0 + ox] == 0 and vy < dlimit:
                        j = bisect.bisect_right(gdkeys, vx) - 1
                        if j >= 0 and vx - gdkeys[j] < 0x200 and vy >= vx - gdkeys[j]:
                            data.setdefault(vy - (vx - gdkeys[j]), gdata[gdkeys[j]])
                        if vx == gdkeys[j]:
                            exact.setdefault(gdata[gdkeys[j]], vy)
                            if gdkeys[j] in own:
                                shift = vy - vx
            if shift is not None:         # every label of the module moves with it
                for k, v in own.items():
                    data.setdefault(k + shift, v)
    for adr in [k for k, v in data.items() if v in exact and exact[v] != k]:
        del data[adr]               # a name+k guess that disagrees with an exact operand
    # the rest: function entries and referenced data
    from rdis import analyse
    _, ce, _, funcs, allins = analyse(exe, sorted(code))
    taken = set(code.values()) | set(data.values())   # names the placed modules define
    tag = os.path.basename(exe)[0].upper()

    def fresh(nm):                  # an address name of this EXE must not clash with a module's
        return nm + '_' + tag if nm in taken else nm
    for s in funcs:
        if funcs[s] and s not in code:
            code[s] = fresh('F_%04X' % s)
    for adr, i in allins.items():
        for k, o, v in operands(i):
            if k == 'data' and v < dlimit and v not in data:
                if not any(dv for dv in [v] if dv in data):
                    data[v] = fresh('D_%04X' % v)
    # drop D_ names that fall inside a named object of another module's data (keep exact names)
    iob = next((k for k, v in data.items() if v == '_IOB'), None)
    if iob is not None:               # _IOB is one array of FILEs: its fields are _IOB+n
        for k in [k for k in data if iob < k < iob + 0x118]:
            del data[k]
    if out:
        with open(out, 'w', newline='\n') as f:
            f.write('# %s symbols: C = code offset, D = DGROUP offset (hex); names of GEN\'s shared modules from tools/xmap.py\n' % os.path.basename(exe))
            for k in sorted(code):
                f.write('C %04X %s\n' % (k, code[k]))
            for k in sorted(data):
                f.write('D %04X %s\n' % (k, data[k]))
    if showmap or not out:
        print('%s: code ends %04X (DGROUP paragraph %04X); %d modules placed' % (exe, ds, ds // 16, len(placed)))
        for n in notes:
            print('  not placed: ' + n)
        pos = 0x3E9
        for s, e, n in sorted(placed):
            if s > pos:
                print('  %04X-%04X  %5d bytes not from a known module' % (pos, s, s - pos))
            print('  %04X-%04X  %s' % (s, e, n))
            pos = max(pos, e)
        if pos < ds:
            print('  %04X-%04X  %5d bytes not from a known module' % (pos, ds, ds - pos))


if __name__ == '__main__':
    main()
