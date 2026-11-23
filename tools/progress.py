#!/usr/bin/env python3
"""progress.py: decompilation progress card (docs/progress.svg) and numbers (docs/progress.json).

Every code entry ("C addr name") of src/NAME/NAME.syms is one function of NAME.EXE, in
address order. Its status:

  matched  defined in a C source of the link (src/NAME.lnk) and the EXE rebuilds identical
  attempt  reserved for a function with a draft in src/NAME/work/ but not yet in the link
  open     in the EXE but in neither a C nor an assembly source of the link
  aside    in a linked assembly module (startup, Lattice library, Origin library): not game
           code, left out of the percentage as in the r/decomps cards

The percentage is matched / (matched + attempt + open). The source scan needs only the
repository (no gamedata, no DOSBox); the byte-identical rebuild is checked by build.py.

    python3 tools/progress.py            # writes docs/progress.svg and docs/progress.json
    python3 tools/progress.py --check    # exit 1 if the committed files are stale
"""
import argparse, json, os, re, sys
from html import escape

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXES = ['gen', 'ultima', 'mondain', 'space', 'out']
NAME = 'u1-decompiled'
PER_TILE = 1

COLORS = dict(matched='#2a8c3c', attempt='#b8860b', open='#64748b', aside='#b94a53')
LABELS = dict(matched='Matched', attempt='Attempt', open='Open', aside='Aside')


def read(path):
    with open(path, encoding='latin-1', newline='') as f:
        return f.read()


def link_files(exe):
    """Source files of a link, in order, as paths relative to the repo."""
    base = os.path.join(ROOT, 'src', exe)
    out = []
    for line in read(os.path.join(base, exe + '.lnk')).splitlines():
        line = line.split(';')[0].strip()
        if line:
            out.append(os.path.normpath(os.path.join(base, line)))
    return out


C_DEF = re.compile(
    r'^(?!(?:extern|typedef|static|if|while|for|switch|return|else)\b)'
    r'(?:[A-Za-z_]\w*[ \t*]+)*\**([A-Za-z_]\w*)[ \t]*\(([^()]*)\)[ \t]*'
    r'(?!;)((?:[^;{}()=]*(?:\([^()]*\)[^;{}()=]*)*;[ \t]*\n?)*)\s*\{', re.M)


def strip_comments(text):
    return re.sub(r'/\*.*?\*/', lambda m: re.sub(r'[^\n]', ' ', m.group(0)), text, flags=re.S)


def c_functions(text):
    """Names of the functions defined (K&R style, at column 0) in a C source."""
    return {m.group(1) for m in C_DEF.finditer(strip_comments(text))}


ASM_LABEL = re.compile(r'^([A-Za-z_@$?][\w@$?]*)(?::|\s+PROC\b)', re.I)


def asm_labels(text):
    return {m.group(1) for m in map(ASM_LABEL.match, text.splitlines()) if m}


def classify(exe):
    files = link_files(exe)
    cnames, anames = set(), set()
    for p in files:
        if p.upper().endswith('.C'):
            cnames |= c_functions(read(p))
        elif p.upper().endswith('.ASM'):
            anames |= asm_labels(read(p))
    work = set()
    wdir = os.path.join(ROOT, 'src', exe, 'work')
    # names drafted in work files but not (yet) in the link count as attempts
    if os.path.isdir(wdir):
        for fn in os.listdir(wdir):
            if fn.endswith('.C'):
                work |= c_functions(read(os.path.join(wdir, fn)))
    funcs = []
    for line in read(os.path.join(ROOT, 'src', exe, exe + '.syms')).splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[0] == 'C':
            name = parts[2]
            if name in cnames:
                st = 'matched'
            elif name in anames:
                st = 'aside'
            elif name in work:
                st = 'attempt'
            else:
                st = 'open'
            funcs.append((int(parts[1], 16), name, st))
    funcs.sort()
    return funcs


def collect():
    data = {}
    for exe in EXES:
        data[exe] = [dict(addr='%04X' % a, name=n, status=s) for a, n, s in classify(exe)]
    return data


def summary(rows):
    c = {k: 0 for k in COLORS}
    for r in rows:
        c[r['status']] += 1
    total = c['matched'] + c['attempt'] + c['open']
    return dict(c, total=total, percent=round(100.0 * c['matched'] / total, 1) if total else 100.0)


def svg(data):
    """The card: one tile per function, EXEs in turn, address order within each."""
    flat = [dict(r, exe=e) for e in EXES for r in data[e]]
    s = summary(flat)
    n = len(flat)
    W, PAD = 840, 20
    inner = W - 2 * PAD
    cols = 1
    # tile size: pick the column count that makes ~square tiles of 8-14 px
    for cols_try in range(20, 200):
        if inner / cols_try < 11:
            break
        cols = cols_try
    cell = inner / cols
    rows = -(-n // cols)
    grid_h = rows * cell
    top = 160
    H = int(top + grid_h + 22)
    out = []
    a = out.append
    a('<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %d %d" '
      'font-family="Roboto, Helvetica, Arial, sans-serif" role="img" aria-label="%s">'
      % (W, H, W, H, escape('Decompilation progress: %d of %d functions matched' % (s['matched'], s['total']))))
    a('<rect width="%d" height="%d" rx="14" fill="#0f172a"/>' % (W, H))
    a('<text x="%d" y="38" font-size="21" fill="#ffffff">Decompilation progress</text>' % PAD)
    a('<text x="%d" y="38" font-size="15" fill="#ffffff" text-anchor="end">%s</text>' % (W - PAD, escape(NAME)))
    a('<text x="%d" y="62" font-size="15" fill="#e5e7eb">%s / %s functions matched (%.1f%%)</text>'
      % (PAD, format(s['matched'], ','), format(s['total'], ','), s['percent']))
    # bar
    a('<rect x="%d" y="74" width="%d" height="18" rx="5" fill="#334155"/>' % (PAD, inner))
    if s['total']:
        a('<rect x="%d" y="74" width="%.2f" height="18" rx="5" fill="%s"/>'
          % (PAD, inner * s['matched'] / s['total'], COLORS['matched']))
    # legend
    lx = [PAD, 232, 430, 628]
    for x, k in zip(lx, ['matched', 'attempt', 'open', 'aside']):
        a('<rect x="%d" y="104" width="10" height="10" fill="%s"/>' % (x, COLORS[k]))
        a('<text x="%d" y="113" font-size="12" fill="#e5e7eb">%s: %s</text>'
          % (x + 14, LABELS[k], format(s[k], ',')))
    a('<text x="%d" y="140" font-size="11" fill="#e5e7eb">Function map: %d function(s) per tile, '
      'in address order (GEN, ULTIMA, MONDAIN, SPACE, OUT)</text>' % (PAD, PER_TILE))
    for i, r in enumerate(flat):
        x = PAD + (i % cols) * cell
        y = top + (i // cols) * cell
        a('<rect x="%.2f" y="%.2f" width="%.2f" height="%.2f" fill="%s" stroke="#0f172a" stroke-width="1">'
          '<title>%s %s %s (%s)</title></rect>'
          % (x, y, cell, cell, COLORS[r['status']], r['exe'].upper(), r['addr'], escape(r['name']), r['status']))
    a('</svg>')
    return '\n'.join(out) + '\n'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--check', action='store_true')
    args = ap.parse_args()
    data = collect()
    result = {e: dict(summary(data[e])) for e in EXES}
    allrows = [r for e in EXES for r in data[e]]
    result['all'] = summary(allrows)
    js = json.dumps(result, indent=1) + '\n'
    sv = svg(data)
    paths = {os.path.join(ROOT, 'docs', 'progress.json'): js, os.path.join(ROOT, 'docs', 'progress.svg'): sv}
    if args.check:
        stale = [p for p, t in paths.items() if not os.path.exists(p) or read(p) != t]
        if stale:
            print('stale:', *stale)
            sys.exit(1)
        return
    for p, t in paths.items():
        with open(p, 'w', encoding='utf-8', newline='\n') as f:
            f.write(t)
    for e in EXES + ['all']:
        r = result[e]
        print('%-8s matched %4d  attempt %d  open %d  aside %4d  -> %.1f%% of %d'
              % (e, r['matched'], r['attempt'], r['open'], r['aside'], r['percent'], r['total']))


if __name__ == '__main__':
    main()
