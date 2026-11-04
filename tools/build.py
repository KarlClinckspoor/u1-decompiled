#!/usr/bin/env python3
"""Build an executable from its sources with the original tools, and compare it with the original.

    python3 tools/build.py gen [--keep]

Reads src/NAME/NAME.lnk: one module per line, in link order ("file [; comment]"); paths are
relative to src/NAME. .C files are compiled with Lattice C 3.10 (LC, options from their
"/* opts: */" line, default -ms -v), .ASM files assembled with MASM 3.00; the objects are
linked with Microsoft LINK 3.05 (MASM 4.0) into build/NAME/NAME.EXE, which is then compared
with gamedata/NAME.EXE. Exit status 0 when identical. If src/NAME/postlink.py exists it is run on
the linked EXE first (ULTIMA.EXE: the protection envelope's remains, copied from the original).
"""
import hashlib, os, re, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import dos


def main():
    name = sys.argv[1].lower()
    src = os.path.join(ROOT, 'src', name)
    out = os.path.join(ROOT, 'build', name)
    shutil.rmtree(out, ignore_errors=True)
    os.makedirs(out)
    mods = []
    for line in open(os.path.join(src, name + '.lnk')):
        line = line.split(';')[0].strip()
        if line:
            mods.append(line)
    cmds = []
    objs = []
    for m in mods:
        p = os.path.normpath(os.path.join(src, m))
        stem = os.path.splitext(os.path.basename(p))[0].upper()
        ext = os.path.splitext(p)[1].upper()
        shutil.copy(p, os.path.join(out, stem + ext))
        if ext == '.C':
            s = open(p, 'rb').read().decode('latin1')
            mo = re.search(r'/\*\s*opts:\s*(.*?)\s*\*/', s)
            cmds.append('LC %s %s >> BUILD.LOG' % (mo.group(1) if mo else '-ms -v', stem))
        else:
            cmds.append('D:\\MASM3\\MASM %s.ASM,%s.OBJ; >> BUILD.LOG' % (stem, stem))
        objs.append(stem)
    for d in (src, os.path.join(ROOT, 'src', 'include')):
        if os.path.isdir(d):
            for f in os.listdir(d):
                if f.upper().endswith('.H'):
                    shutil.copy(os.path.join(d, f), os.path.join(out, f.upper()))
    exe = name.upper()
    open(os.path.join(out, 'LINK.RSP'), 'w', newline='').write(
        '+\r\n'.join(objs) + '\r\n%s\r\n%s/M\r\n;\r\n' % (exe, exe))
    cmds.append('D:\\MASM4\\LINK @LINK.RSP >> BUILD.LOG')
    dos.run(out, cmds)
    log = open(os.path.join(out, 'BUILD.LOG'), 'rb').read().decode('latin1').replace('\r', '')
    bad = []
    for l in log.split('\n'):
        m = re.match(r'^\s*(\d+)\s+(\d+)\s*$', l)
        if (m and int(m.group(1)) + int(m.group(2))) or re.search(r'Error \d|error L|Unresolved|Undefined|more than once', l):
            bad.append(l)
    for o in objs:
        if not os.path.exists(os.path.join(out, o + '.OBJ')):
            bad.append('no object for ' + o)
    built = os.path.join(out, exe + '.EXE')
    if not os.path.exists(built) or bad:
        print(log)
        print('\n'.join(bad))
        sys.exit(2)
    post = os.path.join(src, 'postlink.py')     # an EXE-specific step after LINK (ULTIMA)
    if os.path.exists(post):
        r = subprocess.run([sys.executable, post, built, os.path.join(ROOT, 'gamedata', exe + '.EXE')])
        if r.returncode:
            sys.exit(r.returncode)
    h = hashlib.sha1(open(built, 'rb').read()).hexdigest()
    print('%s.EXE sha1 %s' % (exe, h))
    orig = os.path.join(ROOT, 'gamedata', exe + '.EXE')
    if os.path.exists(orig):
        r = subprocess.run([sys.executable, os.path.join(HERE, 'exediff.py'), built, orig])
        sys.exit(r.returncode)


if __name__ == '__main__':
    main()
