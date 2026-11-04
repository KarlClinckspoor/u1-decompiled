#!/usr/bin/env python3
"""Unpack the toolchain from the archives in tools/ into build/tc/.

    python3 tools/setup.py [--archives DIR]

Needs `7z` (p7zip) on the PATH; the floppy images inside are read with tools/fat12.py.
The archives are the user's own copies (never committed):

  Lattice iAPX32 C compiler v3.10.zip     LC.EXE 3.10, LC1 3.11, LC2 3.10  -> build/tc/LC
  Lattice C 3.00g (5.25).7z               headers, CS.OBJ, LCS.LIB, C.ASM  -> build/tc/INC, LIB300, SRC300
  Lattice C 3.30 ... (Installed).7z       C.ASM and library sources (reference only) -> build/tc/SRC330
  Microsoft C Compiler 2.03 ....7z        LINK 2.40                        -> build/tc/LINK240
  MASM 3.0 image (u6-decompiled)          MASM 3.00, LINK 3.00, LIB        -> build/tc/MASM3
  Microsoft Macro Assembler 4.0 ....7z    MASM 4.00, LINK 3.05, LIB        -> build/tc/MASM4
"""
import argparse, glob, os, shutil, subprocess, sys, tempfile, zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
TC = os.path.join(ROOT, 'build', 'tc')
sys.path.insert(0, HERE)


SEVENZ = shutil.which('7z') or next((p for p in (r'C:\Program Files\7-Zip\7z.exe',
                                                  r'C:\Program Files (x86)\7-Zip\7z.exe')
                                     if os.path.exists(p)), '7z')


def sh7z(archive, out):
    subprocess.run([SEVENZ, 'x', '-y', '-o' + out, archive], check=True, stdout=subprocess.DEVNULL)


def fat(img, out):
    subprocess.run([sys.executable, '-I', os.path.join(HERE, 'fat12.py'), img, out], check=True,
                   stdout=subprocess.DEVNULL)


def find(arch, pat):
    m = glob.glob(os.path.join(arch, pat))
    if not m:
        raise SystemExit('missing archive: ' + pat)
    return m[0]


def copy(srcdir, names, dst):
    os.makedirs(dst, exist_ok=True)
    have = {n.upper(): n for n in os.listdir(srcdir)}
    for n in names:
        shutil.copy(os.path.join(srcdir, have[n.upper()]), os.path.join(dst, n.upper()))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--archives', default=os.path.join(ROOT, 'tools'))
    ap.add_argument('--masm3', help='MASM 3.0 floppy image (default: search ../u6-decompiled/tools/masm300)')
    a = ap.parse_args()
    arch = a.archives
    tmp = tempfile.mkdtemp()
    os.makedirs(TC, exist_ok=True)

    # Lattice 3.10 compiler
    with zipfile.ZipFile(find(arch, 'Lattice iAPX32 C compiler v3.10.zip')) as z:
        z.extractall(os.path.join(tmp, 'l310'))
    d = os.path.join(tmp, 'l310')
    copy(os.path.join(d, 'L'), ['LC.EXE', 'LC1.EXE'], os.path.join(TC, 'LC'))
    copy(d, ['LC2.EXE'], os.path.join(TC, 'LC'))

    # Lattice 3.00g: headers, libraries, sources
    sh7z(find(arch, 'Lattice C 3.00g*.7z'), os.path.join(tmp, 'l300'))
    for i in sorted(glob.glob(os.path.join(tmp, 'l300', '*', 'disk0*.img'))):
        fat(i, os.path.join(tmp, 'l300x', os.path.basename(i)[:-4]))
    x = os.path.join(tmp, 'l300x')
    inc = os.path.join(TC, 'INC')
    os.makedirs(inc, exist_ok=True)
    for f in glob.glob(os.path.join(x, 'disk01', '*.H')):
        shutil.copy(f, inc)
    for f in glob.glob(os.path.join(d, '*.H')):          # 3.10's STDIO.H/STDLIB.H win
        shutil.copy(f, inc)
    copy(os.path.join(x, 'disk01'), ['OMD.EXE'], os.path.join(TC, 'LC'))
    copy(os.path.join(x, 'disk03'), ['CS.OBJ', 'LCS.LIB', 'SM8086.MAC'], os.path.join(TC, 'LIB300'))
    src = os.path.join(TC, 'SRC300')
    os.makedirs(src, exist_ok=True)
    for f in glob.glob(os.path.join(x, 'disk02', '*.*')):
        if not f.upper().endswith('.EXE'):
            shutil.copy(f, src)
    shutil.copy(os.path.join(x, 'disk01', '8087.MAC'), src)

    # Lattice 3.30: reference sources
    sh7z(find(arch, 'Lattice C 3.30*.7z'), os.path.join(tmp, 'l330'))
    z = glob.glob(os.path.join(tmp, 'l330', '*', '*.zip'))[0]
    with zipfile.ZipFile(z) as zz:
        zz.extractall(os.path.join(tmp, 'l330x'))
    shutil.copytree(os.path.join(tmp, 'l330x', 'SOURCE'), os.path.join(TC, 'SRC330'), dirs_exist_ok=True)
    shutil.copy(os.path.join(tmp, 'l330x', 'S', 'DOS.MAC'), os.path.join(TC, 'SRC330'))

    # LINK 2.40 from MS C 2.03
    sh7z(find(arch, 'Microsoft C Compiler 2.03*.7z'), os.path.join(tmp, 'msc203'))
    i = glob.glob(os.path.join(tmp, 'msc203', '*', 'disk03.img'))[0]
    fat(i, os.path.join(tmp, 'msc203x'))
    copy(os.path.join(tmp, 'msc203x'), ['LINK.EXE'], os.path.join(TC, 'LINK240'))

    # MASM 3.0 (LINK 3.00, LIB)
    m3 = a.masm3 or (glob.glob(os.path.join(ROOT, '..', 'u6-decompiled', 'tools', 'masm300', '*.img')) or
                     glob.glob(os.path.join(arch, 'masm300', '*.img')) or [None])[0]
    if m3:
        fat(m3, os.path.join(tmp, 'm3'))
        copy(os.path.join(tmp, 'm3'), ['MASM.EXE', 'LINK.EXE', 'LIB.EXE'], os.path.join(TC, 'MASM3'))
    else:
        print('warning: MASM 3.0 image not found (--masm3)')

    # MASM 4.0 (LINK 3.05, LIB, EXEMOD)
    sh7z(find(arch, 'Microsoft Macro Assembler 4.0*.7z'), os.path.join(tmp, 'm4'))
    i = glob.glob(os.path.join(tmp, 'm4', '*', '*.img'))[0]
    fat(i, os.path.join(tmp, 'm4x'))
    copy(os.path.join(tmp, 'm4x'), ['MASM.EXE', 'LINK.EXE', 'LIB.EXE', 'EXEMOD.EXE'], os.path.join(TC, 'MASM4'))

    shutil.rmtree(tmp)
    for r, ds, fs in sorted(os.walk(TC)):
        print(os.path.relpath(r, TC), len(fs), 'files')


if __name__ == '__main__':
    main()
