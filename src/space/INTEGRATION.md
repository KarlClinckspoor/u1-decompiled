# SPACE.EXE integration (C in place of the SPACE.ASM stand-in)

`python tools/build.py space` reports IDENTICAL (sha1 fdd68f5bbeb9abeb7524b385e372daad5a6d02a3).

Link order (space.lnk), game part:

| module | contents | DATA |
|---|---|---|
| SPACEA.C | F_03E9-F_31D7 (work files sp_03e9, sp_16d3, sp_2b7b up to F_31D7), globals | 0124-0E02 |
| SP3330.ASM | F_3330, original bytes cut from the old stand-in (the C version in work/sp_2b7b.C does not match) | - |
| SPACE4.C | F_34A8-F_1853 (code 3ADE, main) | 0E02-0F7E |
| SPACE5.C | F_3DD6 (own module: its string literal starts word aligned, 0F7F is padding) | 0F80-0FA0 |
| SPBSS.C | all BSS 26C0-6002 as `int`/`char[]` definitions in address order | - |

Notes
- The three work files compile as ONE module (SPACEA.C): "Not enough fuel!", "", "inuse.u1" are shared
  by their functions in the original DATA. Only F_3330 forces a module break, so SPACE4.C reuses
  literals of SPACEA.C by address: `(char *)0xC07` ("Crunch!"), `0xC0F` ("Thy shield is drained!"),
  `0x19A` (""), and `extern char D_0B1E[]` ("inuse.u1", defined in SPACEA.C right where its literal
  sat, F_0EBD uses it; a `(char *)0xB1E` cast changes the argument load order).
- SPACEA.C defines the initialised globals in DATA order (tables with their strings, the three
  pictures, D_0AEE/D_0B06); D_0BD2 before F_16D3, D_0C92-D_0C9E before F_2003, D_0DE0/D_0DE2 before
  F_31D7; SPACE4.C starts with D_0E18 (its strings "left".."bottom" come first).
  `_STACK` is `int _STACK = 0x800;` (0130), 0132 is an unnamed int (D_0132), D_01F4 follows D_01F2.
- D_2C30..D_2C5A are really the sector (3,3) of the galaxy D_26C0 (26C0 + 24*0x3A); SPBSS keeps them
  as separate symbols because the code (SPACEA.C) addresses them by name. D_2C5C is `int` in SPACEA.C
  (F_0D88 stores 0 to it; a char-array cast changes the code) and `char[]` in SPACE4.C.
- space.syms: 10FA is now D_07FA (the Origin module OR3C8B's variable); D_07FA_S is only 07FA (picture).
  D_0B1E added.
- SPACE.ASM (old generated stand-in) is no longer linked; kept for reference.
- Open: F_3330 still not matched in C (register choice, see work/sp_2b7b.notes.md); when it matches,
  merge SP3330 back into SPACEA/SPACE4 (then the address-cast literals become ordinary literals).
