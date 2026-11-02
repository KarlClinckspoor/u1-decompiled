# Matching C with Lattice C 3.10

How to write C (or assembly) that compiles to the original bytes of an Ultima I executable.
Everything here was learned on GEN.EXE.

## Toolchain
- Lattice C 3.10 (LC.EXE 3.10, LC1 3.11, LC2 3.10), small model: `lc -ms -v` (-v: no stack-check
  calls). K&R C only: no prototypes, `f(a, b) int a; char b; { ... }` definitions.
- MASM 3.00 for assembly. Microsoft LINK 3.05 (LINK 2.40 and 3.00 give the same EXE).
- `python3 tools/setup.py` unpacks the toolchain into build/tc (once). DOSBox runs headless.

## Loop for one function
1. Find it: `src/gen/gen.syms` names every function `F_XXXX` (its code offset) and global `D_XXXX`
   (its DGROUP offset); library functions have their Lattice names (STRCPY, FOPEN, ...).
   `python3 tools/disasm.py gamedata/GEN.EXE 0670 80` disassembles,
   `python3 tools/dump.py gamedata/GEN.EXE 124 60 --ds` dumps data, `--str OFF` prints a string.
   The generated stand-in `src/gen/GENG.ASM` (or ORIGIN.ASM) shows each function with labels.
2. Write C in a work file, one or more functions, named exactly as in gen.syms (`F_0670()`),
   calling other functions and using globals by their gen.syms names, declared `extern`.
3. `python3 tools/lc.py FILE.C` compiles and compares every function in the file with the
   original (fixup bytes masked). `-v` shows more differing instructions.
4. Iterate until every function says MATCH.

## What the code tells you about the source (observed)
- Frame: `push bp; sub sp,N; mov bp,sp` ... `mov sp,bp; add sp,N; pop bp; ret`. N is the size
  of the locals plus 4. Locals are laid out in declaration order upward from `[bp+4]`
  (first declared at bp+4). Arguments start at `[bp+N+4]`. No frame bytes (`push bp; mov bp,sp`)
  when there are no locals.
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
(see src/gen/ORIGIN.ASM); clean it up by hand and keep it matching.
