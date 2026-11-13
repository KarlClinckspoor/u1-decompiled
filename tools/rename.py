#!/usr/bin/env python3
"""Apply readable names (docs/names.tsv) to the .syms files, .C and .ASM sources.

    python3 tools/rename.py [MAP.tsv ...] [--check] [--stage N] [--list-collisions]

Mapping file: one entry per line, TAB separated: scope, old, new [, comment]
  scope  shared | gen | mondain | space | ultima | out
         shared  the name belongs to a module that several EXEs link (src/origin, src/lclib,
                 src/lattice): it is renamed in that module, and in the .syms and game sources of
                 every EXE that links it (a name in no shared module: in every EXE that has it,
                 for names that mean the same thing everywhere)
         EXE     the name is private to that EXE's own files (src/EXE/*.C|*.ASM|*.syms)
  lines starting with # and blank lines are ignored; '#stage N' lines start a stage (see --stage).

Rules enforced (nothing is written when one fails):
  * Lattice C 3.10 keeps 8 significant characters: new names are at most 8 characters, and the
    first 8 (case-insensitive: LC and MASM upper-case) must differ from every other identifier of
    every EXE that uses the name (including locals and the library's names);
  * not a C keyword; not a MASM mnemonic/register/operator when the name appears in an .ASM file;
  * old name must exist where the scope says; a name used by a shared module must have scope shared;
  * whole-word replacement; string and character literals are never touched.
--check only reports. --stage N applies just the entries of stage N (default: all).
"""
import os, re, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC = os.path.join(ROOT, 'src')
EXES = ['gen', 'mondain', 'space', 'ultima', 'out']

C_KEYWORDS = set('''auto break case char continue default do double else enum extern float for goto if int
long register return short sizeof static struct switch typedef union unsigned void while main_ entry
fortran asm pascal'''.split())
MASM_RESERVED = set('''
AX BX CX DX SI DI BP SP AL AH BL BH CL CH DL DH CS DS ES SS ST
AAA AAD AAM AAS ADC ADD AND CALL CBW CLC CLD CLI CMC CMP CMPS CMPSB CMPSW CWD DAA DAS DEC DIV ESC HLT IDIV
IMUL IN INC INT INTO IRET JA JAE JB JBE JC JCXZ JE JG JGE JL JLE JMP JNA JNAE JNB JNBE JNC JNE JNG JNGE JNL
JNLE JNO JNP JNS JNZ JO JP JPE JPO JS JZ LAHF LDS LEA LES LOCK LODS LODSB LODSW LOOP LOOPE LOOPNE LOOPNZ
LOOPZ MOV MOVS MOVSB MOVSW MUL NEG NOP NOT OR OUT POP POPF PUSH PUSHF RCL RCR REP REPE REPNE REPNZ REPZ RET
RETF ROL ROR SAHF SAL SAR SBB SCAS SCASB SCASW SHL SHR STC STD STI STOS STOSB STOSW SUB TEST WAIT XCHG XLAT
XOR EQ NE LT LE GT GE MOD SHORT TYPE LENGTH SIZE OFFSET SEG PTR DUP BYTE WORD DWORD QWORD TBYTE NEAR FAR
HIGH LOW MASK WIDTH THIS PROC ENDP SEGMENT ENDS ASSUME GROUP NAME PUBLIC EXTRN LABEL END EQU DB DW DD DQ DT
ORG PAGE TITLE SUBTTL COMMENT INCLUDE IF ELSE ENDIF IFE IFDEF IFNDEF IF1 IF2 MACRO ENDM REPT IRP IRPC LOCAL
EXITM PURGE RECORD STRUC EVEN ALIGN RADIX COMMON MEMORY STACK AT PARA PAGE INPAGE BYTE UNIQUE CONST
FWAIT F2XM1 FABS FADD FCHS FCLEX FCOM FCOS FDIV FLD FST FSTP FSUB FMUL FINIT FILD FIST FISTP FSQRT FXCH
DATA PROG UDATA DGROUP CODE'''.split())

IDENT_C = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')
IDENT_A = re.compile(r'[A-Za-z_@$?][A-Za-z0-9_@$?]*')
NAME_OK = re.compile(r'^[A-Za-z][A-Za-z0-9_]*$')


def read(p):
    return open(p, 'rb').read().decode('latin1')


def write(p, t):
    open(p, 'wb').write(t.encode('latin1'))


class File:
    def __init__(self, path):
        self.path = path
        self.kind = path.rsplit('.', 1)[-1].upper()
        self.text = read(path)

    # --- scanning: yields (is_code, ident_or_None) via a callback-based rewriter
    def rewrite(self, sub):
        """sub(word, in_comment) -> replacement or None. Returns new text."""
        t = self.text
        if self.kind == 'C':
            return self._rw_c(t, sub)
        if self.kind == 'ASM':
            return self._rw_asm(t, sub)
        return self._rw_syms(t, sub)

    def _rw_c(self, t, sub):
        out = []
        pat = re.compile(r'/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|[A-Za-z_][A-Za-z0-9_]*', re.S)
        pos = 0
        for m in pat.finditer(t):
            out.append(t[pos:m.start()])
            s = m.group(0)
            if s.startswith('/*'):
                out.append(re.sub(r'[A-Za-z_][A-Za-z0-9_]*', lambda w: sub(w.group(0), True) or w.group(0), s))
            elif s[0] in '"\'':
                out.append(s)
            else:
                out.append(sub(s, False) or s)
            pos = m.end()
        out.append(t[pos:])
        return ''.join(out)

    def _rw_asm(self, t, sub):
        out = []
        for line in t.split('\n'):
            # split code / comment at the first ';' outside quotes
            q = None
            cut = len(line)
            for i, ch in enumerate(line):
                if q:
                    if ch == q:
                        q = None
                elif ch in '\'"':
                    q = ch
                elif ch == ';':
                    cut = i
                    break
            code, com = line[:cut], line[cut:]
            res = []
            pos = 0
            pat = re.compile(r'"[^"]*"|\'[^\']*\'|[A-Za-z_@$?][A-Za-z0-9_@$?]*')
            for m in pat.finditer(code):
                res.append(code[pos:m.start()])
                s = m.group(0)
                res.append(s if s[0] in '"\'' else (sub(s, False) or s))
                pos = m.end()
            res.append(code[pos:])
            com = re.sub(r'[A-Za-z_][A-Za-z0-9_]*', lambda w: sub(w.group(0), True) or w.group(0), com)
            out.append(''.join(res) + com)
        return '\n'.join(out)

    def _rw_syms(self, t, sub):
        out = []
        for line in t.split('\n'):
            if line.startswith(';') or line.startswith('#') or not line.strip():
                out.append(line)
                continue
            out.append(re.sub(r'[A-Za-z_][A-Za-z0-9_]*', lambda w: sub(w.group(0), False) or w.group(0), line))
        return '\n'.join(out)

    def words(self):
        """identifiers in code (not comments, not literals)"""
        found = []
        def sub(w, in_comment):
            if not in_comment:
                found.append(w)
            return None
        self.rewrite(sub)
        return found


class Tree:
    def __init__(self):
        self.files = {}
        self.game = {}      # exe -> [paths] (game sources + syms)
        self.mods = {}      # exe -> [paths] shared modules linked
        for e in EXES:
            d = os.path.join(SRC, e)
            g = []
            for f in sorted(os.listdir(d)):
                if f.upper().endswith(('.C', '.ASM')) or f.endswith('.syms'):
                    g.append(os.path.join(d, f))
            self.game[e] = g
            m = []
            for line in open(os.path.join(d, e + '.lnk')):
                line = line.split(';')[0].strip()
                if line:
                    p = os.path.normpath(os.path.join(d, line))
                    if p not in g:
                        m.append(p)
            self.mods[e] = m
        for l in list(self.game.values()) + list(self.mods.values()):
            for p in l:
                if p not in self.files:
                    self.files[p] = File(p)
        self.shared_files = sorted({p for e in EXES for p in self.mods[e]})
        self.linked_by = collections.defaultdict(list)
        for e in EXES:
            for p in self.mods[e]:
                self.linked_by[p].append(e)

    def exe_files(self, e):
        return self.game[e] + self.mods[e]


def load_map(paths):
    entries = []
    stage = 0
    for p in paths:
        for n, line in enumerate(open(p, encoding='latin1'), 1):
            line = line.rstrip('\r\n')
            m = re.match(r'#\s*stage\s+(\d+)', line)
            if m:
                stage = int(m.group(1))
                continue
            if not line.strip() or line.startswith('#'):
                continue
            f = line.split('\t')
            if len(f) < 3:
                sys.exit('%s:%d: need scope TAB old TAB new' % (p, n))
            entries.append((stage, f[0].strip(), f[1].strip(), f[2].strip(), '%s:%d' % (os.path.basename(p), n)))
    return entries


def main():
    args = sys.argv[1:]
    check = '--check' in args
    stage = None
    if '--stage' in args:
        i = args.index('--stage')
        stage = int(args[i + 1])
        del args[i:i + 2]
    maps = [a for a in args if not a.startswith('--')] or [os.path.join(ROOT, 'docs', 'names.tsv')]
    entries = load_map(maps)
    if stage is not None:
        entries = [x for x in entries if x[0] == stage]
    tree = Tree()
    errors = []
    # words per file (code identifiers), original
    words = {p: set(f.words()) for p, f in tree.files.items()}
    anywords = {p: set(w for w in re.findall(r'[A-Za-z_][A-Za-z0-9_]*', f.text)) for p, f in tree.files.items()}

    perfile = collections.defaultdict(dict)       # path -> {old: new}
    exe_new = collections.defaultdict(dict)       # exe -> {new_upper: old}
    seen_old = {}
    for st, scope, old, new, where in entries:
        def err(msg):
            errors.append('%s: %s %s -> %s: %s' % (where, scope, old, new, msg))
        if scope not in EXES + ['shared']:
            err('bad scope')
            continue
        if not NAME_OK.match(new):
            err('bad name syntax')
            continue
        if len(new) > 8:
            err('longer than 8 characters')
            continue
        if new.lower() in C_KEYWORDS:
            err('C keyword')
            continue
        key = (scope, old)
        if key in seen_old:
            err('old name mapped twice (also %s)' % seen_old[key])
            continue
        seen_old[key] = where
        if scope == 'shared':
            owners = [p for p in tree.shared_files if old in anywords[p]]
            if owners:
                exes = sorted({e for p in owners for e in tree.linked_by[p]})
            else:       # not in a shared module: same meaning in every EXE that has it
                exes = [e for e in EXES if any(old in anywords[p] for p in tree.game[e])]
                if not exes:
                    if any(new in anywords[p] for p in tree.files):
                        continue        # already applied
                    err('not found anywhere')
                    continue
            targets = list(owners)
            for e in exes:
                targets += [p for p in tree.game[e]]
            others = [e for e in EXES if e not in exes and any(old in anywords[p] for p in tree.game[e])]
            if others:
                err('also used by game files of %s which do not link the module' % others)
                continue
            if new.upper() in MASM_RESERVED:
                err('MASM reserved word')
                continue
            affected = exes
        else:
            e = scope
            inshared = [p for p in tree.mods[e] if old in anywords[p]]
            if inshared:
                err('used by shared module %s: scope must be shared' % os.path.basename(inshared[0]))
                continue
            targets = list(tree.game[e])
            if not any(old in anywords[p] for p in targets):
                if any(new in anywords[p] for p in targets):
                    continue            # already applied
                err('not found in %s files' % e)
                continue
            if any(p.upper().endswith('.ASM') and old in anywords[p] for p in targets) and new.upper() in MASM_RESERVED:
                err('MASM reserved word (name is used in an .ASM file)')
                continue
            affected = [e]
        for p in targets:
            if old in anywords[p]:
                perfile[p][old] = new
        for e in affected:
            if new.upper() in exe_new[e]:
                err('duplicate of %s in %s' % (exe_new[e][new.upper()], e))
            exe_new[e][new.upper()] = old
    if errors:
        print('\n'.join(errors))
        print('%d error(s); nothing written' % len(errors))
        sys.exit(1)

    # collision check, per EXE
    bad = 0
    for e in EXES:
        if not exe_new[e]:
            continue
        olds = set()
        for p in tree.exe_files(e):
            olds |= set(perfile[p])
        newset = {n.upper() for n in exe_new[e]}
        # identifiers of the EXE's files after the rename, with their 8-char keys
        idents = collections.defaultdict(set)
        stay = set()
        for p in tree.exe_files(e):
            for w in words[p]:
                w2 = perfile[p].get(w, w)
                idents[w2.upper()[:8]].add(w2.upper())
                if w2 == w:
                    stay.add(w.upper())
        for n in sorted(newset):
            k = n[:8]
            if idents[k] != {n} or n in stay:
                bad += 1
                print('collision in %s: %s clashes with %s' % (e, n, sorted(idents[k] - {n}) or 'an existing identifier of the same name'))
    if bad:
        print('%d collision(s); nothing written' % bad)
        sys.exit(1)
    nf = 0
    for p, mp in perfile.items():
        if not mp:
            continue
        f = tree.files[p]
        new = f.rewrite(lambda w, c: mp.get(w))
        if new != f.text:
            nf += 1
            if not check:
                write(p, new)
    print('%d entries; %d files %s' % (len(entries), nf, 'would change' if check else 'rewritten'))


if __name__ == '__main__':
    main()
