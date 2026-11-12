# Brief: matching C for a range of Ultima I game code

Repo: C:\Users\karl9\Downloads\Claude\u1-decompiled (Windows; use the Bash tool = Git Bash).
It rebuilds the five DOS executables of Ultima I (1986, Lattice C 3.10, small model) byte for
byte. GEN.EXE's game code is already C (src/gen/GEN.C, the model to follow). The game code of
the other four EXEs is still generated assembly "stand-ins" (src/NAME/NAME.ASM, or OUT1-3.ASM),
which are fully labelled with the symbol names you must use. Your job: write C for your
address range that compiles to the exact original bytes.

Read first: docs/MATCHING.md (how Lattice C 3.10 code reads; essential), the top of
src/gen/GEN.C (style, K&R declarations, extern block for the Origin library and its return
types), src/origin/OR39C5.C and OR387E.C (Origin C routines).

## Tools (Python is the venv: `build/venv/Scripts/python`; plain python lacks capstone)

    PY=build/venv/Scripts/python
    $PY tools/lc.py src/EXE/work/FILE.C --exe gamedata/EXE.EXE --syms src/EXE/EXE.syms [-v]

compiles with Lattice C in headless DOSBox (~3 s; parallel runs are fine) and compares every
public function in the file with the original at the address its name has in the .syms file.
It prints MATCH, the differing instructions, or WRONG TARGETS (the code bytes match but a call
or a global reference points to a different symbol/offset than the original: fix it). Fixups
to string literals (your module's own DATA) are not checked: verify string contents yourself.

    $PY tools/disasm.py gamedata/EXE.EXE 0423 60       # disassemble (but the stand-in .ASM is nicer)
    $PY tools/dump.py gamedata/EXE.EXE 0124 80 --ds    # DGROUP data
    $PY tools/dump.py gamedata/EXE.EXE --str 0156      # NUL-terminated string at DGROUP:0156

The first line of a C file `/* opts: -ms -v -w -cs */` sets the compiler options (use those).

## Rules

- Name every function exactly as in the .syms file (`F_0423`), all public (no `static`),
  K&R definitions (`F_0423(c) int c; { ... }`), no prototypes. Globals by their .syms names
  (`D_2046`), declared `extern` with your best type (int, unsigned char, char arrays, long,
  pointer, arrays: `extern int D_2044[];`). Element offsets: `D_2044[3]` on an int array is
  D_2044+6; lc.py's WRONG TARGETS tells you if you picked the wrong base or index.
- Use real string literals for strings (look them up with dump.py --str); do not reference
  string data by D_ name. Note in your notes file which D_ address each literal came from.
- Origin library and Lattice library functions: extern them with the names from the .syms
  (F_352C, F_39FC, STRCPY, ...). Infer return types from use (`test al,al` after the call =
  char return; `xor ah,ah` = unsigned char).
- Work only in your own files: `src/EXE/work/PREFIX*.C` and `src/EXE/work/PREFIX.notes.md`
  (PREFIX is given below). You may split into several .C files (PREFIX_a.C, PREFIX_b.C) if one
  gets long. Do not edit anything else (no .syms, .lnk, .ASM, tools, docs), do not run
  build.py, do not commit. Other agents are working on other ranges at the same time.
- Every function in your range must be attempted. Keep going until it MATCHes. Try the
  variations MATCHING.md lists (statement order, a[n++] vs a[n]=..;n++, nested ifs vs &&,
  register locals, char vs unsigned char, int vs unsigned, loop forms for/while/do, switch
  vs if chains, temporaries). If one still differs after a serious effort (~15 distinct
  attempts), keep your closest version in the file, mark it `/* NONMATCHING: what differs */`,
  and move on; come back to it at the end.
- If a routine is clearly not compiled C (no Lattice frame `push bp; [sub sp,N;] mov bp,sp`
  with `mov sp,bp` after calls, uses CS-relative data, interrupt handler, odd register
  conventions), do not write C for it: list it as ASM in the notes.
- Comment each function with one line saying what it does (as in GEN.C), from what you can
  see (what it draws, which keys it handles, which globals it updates). Do not rename
  anything yet; that is a later pass.

## Notes file (src/EXE/work/PREFIX.notes.md), keep it short

1. Status table: function, MATCH / NONMATCHING / ASM, one-line purpose.
2. Globals used: address, type you chose, what it seems to be (e.g. `D_2046 int: text colour`).
   Mark initialised data (non-zero in the EXE's DATA) vs BSS.
3. String literals used: D_ address and text, in the order the functions use them.
4. Anything surprising about the compiler you had to learn (for the shared notes).

## Report back (final message)

Counts (MATCH / NONMATCHING / ASM out of total), the list of non-matching and ASM functions
with a few words each, and any compiler lesson worth sharing. Keep it under 25 lines.
