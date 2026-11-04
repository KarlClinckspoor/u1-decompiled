# Matching C with Lattice C 3.10

How to write C (or assembly) that compiles to the original bytes of an Ultima I executable.
Everything here was learned on GEN.EXE.

## Toolchain
- Lattice C 3.10 (LC.EXE 3.10, LC1 3.11, LC2 3.10), small model: `lc -ms -v -w` (-v: no
  stack-check calls; -w: even frames). K&R C only: no prototypes, `f(a, b) int a; char b; { ... }` definitions.
- MASM 3.00 for assembly. Microsoft LINK 3.05 (LINK 2.40 and 3.00 give the same EXE).
- `python3 tools/setup.py` unpacks the toolchain into build/tc (once). DOSBox runs headless.

## Loop for one function
1. Find it: `src/gen/gen.syms` names every function `F_XXXX` (its code offset) and global `D_XXXX`
   (its DGROUP offset); library functions have their Lattice names (STRCPY, FOPEN, ...).
   `python3 tools/disasm.py gamedata/GEN.EXE 0670 80` disassembles,
   `python3 tools/dump.py gamedata/GEN.EXE 124 60 --ds` dumps data, `--str OFF` prints a string.
   `tools/modgen.py` prints any range as labelled MASM (see src/origin/*.ASM, src/gen/LCLIB.ASM).
2. Write C in a work file, one or more functions, named exactly as in gen.syms (`F_0670()`),
   calling other functions and using globals by their gen.syms names, declared `extern`.
3. `python3 tools/lc.py FILE.C` compiles and compares every function in the file with the
   original (fixup bytes masked). `-v` shows more differing instructions.
4. Iterate until every function says MATCH.

## What the code tells you about the source (observed)
- GEN.C is compiled with **`-ms -v -w -cs`**: `-w` rounds the frame to an even size (a char as
  last local otherwise gives `sub sp,3`); `-cs` stores identical string literals once.
- Frame: `push bp; sub sp,N; mov bp,sp` ... `mov sp,bp; add sp,N; pop bp; ret`. Locals are laid
  out in declaration order upward from `[bp+2]`; sometimes the compiler adds a 2-byte slot of its
  own at bp+2 (locals then start at bp+4). A function with arguments but no locals still gets
  `sub sp,2`; with neither, no `sub`. Arguments start at `[bp+N+4]`.
- Small switches become compare chains in descending case order; four or more contiguous cases
  become a jump table.
- Key codes are `unsigned char` (giving `xor ah,ah`); plain char gives `cbw`.
- `a[n++] = c;` and `a[n] = c; n++;` compile differently; try both.
- Each `return` inside a loop gets its own epilogue.
- After every call: `mov sp,bp` (caller pops with BP). Arguments pushed right to left, often
  through AX/BX/CX; a value already in a register is pushed again (`push ax; push ax`), so
  repeated constants in a call are one source expression each, not a variable.
- `char` locals/args are bytes; widening to int is `xchg bx,ax; cbw; xchg bx,ax` for non-AX regs.
- The optimizer keeps values in registers across statements and delays stores, so statement
  order in the source can differ slightly from the store order. Try both orders.
- `test ax,ax` for comparisons with 0; `xor ah,ah` (30 E4) for unsigned char widening.
- `switch` is a jump table in the code segment (`jmp cs:table[si]`) after range checks;
  the table entries are code offsets.
- String literals go to the module's DATA segment in order of first use; an initialised
  pointer array's strings come right before the array.
- Names are upper-cased by LC, so case in C names does not matter for matching.

## Data layout
Function matching masks every address, so a function can match before its data is right.
Data is checked when the module is linked (`python3 tools/build.py gen`). Each module's DATA
and UDATA are word aligned and laid out in link order (src/gen/gen.lnk).

## Assembly
Functions without a C frame, interrupt handlers and code that uses CS-relative data were
written in assembly. `tools/genasm.py` generates a matching MASM module for a range
(see src/origin/); clean it up by hand and keep it matching. Immediates that are
addresses stay numbers (no relocation marks them in an EXE): make them `OFFSET DGROUP:D_xxxx`
or `OFFSET label` by hand, and tables of code addresses in DATA `DW label`, or the module only
fits the one EXE it was generated from.

## Tricks that were needed in GEN
- A pointer used for a read-modify-write was a block-local `register` copy:
  `{ register unsigned char *num; num = D_01A8; *num += n; }` (the plain `*D_01A8 += n` reloads
  the pointer); block-local variables of disjoint blocks share a frame slot.
- `if (a && b) { if (c) f(); }` and `if (a && b && c) f();` differ: the nested form makes the
  compiler keep a widened char in a stack temporary at [bp+0].
- `argv` was `unsigned char **` (`xor bh,bh` rather than `cbw` when passing argv[1][0]).
