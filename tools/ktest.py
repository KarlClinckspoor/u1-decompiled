#!/usr/bin/env python3
"""Run an Ultima I program under DOSBox with scripted keys and memory pokes/dumps (KTEST.ASM).

    from ktest import Script, run
    s = Script(); s.poke(0x3A78, bytes) ; s.keys("abc\r"); s.ext(0x4B); s.wait(50); s.dump(0, 0x3A78, 820); s.end()
    out = run('out', s)         # -> dict {0: bytes, ...} (dumps), runs build/NAME/NAME.EXE or gamedata copies
"""
import os, sys, shutil, struct
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import dos

class Script:
    def __init__(s): s.b = bytearray()
    def key(s, k): s.b += bytes([0, k if isinstance(k, int) else ord(k)])
    def keys(s, t):
        for c in t: s.key(c)
    def ext(s, sc): s.b += bytes([1, sc])
    def wait(s, n):
        while n > 0: s.b += bytes([2, min(n, 255)]); n -= 255
    def poke(s, off, data):
        for i in range(0, len(data), 200):
            c = data[i:i+200]; s.b += bytes([3]) + struct.pack('<H', off+i) + bytes([len(c)]) + c
    def dump(s, idx, off, n): s.b += bytes([4, idx]) + struct.pack('<HH', off, n)
    def end(s): s.b += bytes([5])

def build_ktest():
    w = os.path.join(ROOT, 'build', 'ktest'); os.makedirs(w, exist_ok=True)
    exe = os.path.join(w, 'KTEST.EXE')
    if not os.path.exists(exe) or os.path.getmtime(exe) < os.path.getmtime(os.path.join(HERE, 'ktest', 'KTEST.ASM')):
        src = open(os.path.join(HERE, 'ktest', 'KTEST.ASM'), 'rb').read().replace(b'\r\n', b'\n').replace(b'\n', b'\r\n')
        open(os.path.join(w, 'KTEST.ASM'), 'wb').write(src)
        dos.run(w, [r'D:\MASM3\MASM KTEST.ASM,KTEST.OBJ; > M.LOG', r'D:\MASM4\LINK KTEST.OBJ,KTEST.EXE,NUL,; > L.LOG'])
    return w

def run(prog, script, files=(), extra_cmds=(), workname=None, prefix=''):
    """prog: 'out' | 'gen' | 'space' | 'mondain' | 'ultima'. files: extra game data files to copy from gamedata."""
    kw = build_ktest()
    wd = os.path.join(ROOT, 'build', 'run_' + (workname or prog)); shutil.rmtree(wd, ignore_errors=True); os.makedirs(wd)
    shutil.copy(os.path.join(kw, 'KTEST.EXE'), wd)
    open(os.path.join(wd, 'KEYS.BIN'), 'wb').write(bytes(script.b))
    gd = os.path.join(ROOT, 'gamedata')
    for f in os.listdir(gd):
        p = os.path.join(gd, f)
        if os.path.isfile(p) and f.upper().endswith(('.BIN', '.EXE', '.4', '.16')) or f.upper().startswith('CASTLE'):
            shutil.copy(p, wd)
    for f in files: shutil.copy(f, wd)
    dos.run(wd, list(extra_cmds) + ['KTEST', prog.upper() + ' ' + prefix])
    return {int(f[1]): open(os.path.join(wd, f), 'rb').read() for f in os.listdir(wd) if len(f) == 6 and f[0] == 'D' and f.endswith('.BIN') and f[1].isdigit()}, wd
