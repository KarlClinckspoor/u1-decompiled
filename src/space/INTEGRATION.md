# SPACE.EXE integration (C in place of the SPACE.ASM stand-in)

`python tools/build.py space` reports IDENTICAL (sha1 fdd68f5bbeb9abeb7524b385e372daad5a6d02a3).

Link order (space.lnk), game part:

| module | contents | DATA |
|---|---|---|
| SPACE.C | F_03E9-F_1853 (code 03E9-3ADE+main incl. fightfrm F_3330), globals | 0124-0F7E |
| SPACE5.C | F_3DD6 (own module: its string literal starts word aligned, 0F7F is padding) | 0F80-0FA0 |
| SPBSS.C | all BSS 26C0-6002 as `int`/`char[]` definitions in address order | - |

Notes
- One module (SPACE.C, formerly SPACEA.C + SPACE4.C + SP3330.ASM, ~50 KB; LC1 has enough memory).
  fightfrm (F_3330) matches in C: the old position is copied into the locals dx/dy first, then each is
  subtracted from the crosshair (see work/sp_3330_match.C). Because it is C now, the shared literals
  ("Crunch!", "Thy shield is drained!", "", "inuse.u1") are ordinary string literals / `inusefil`.
- SPACE.C defines the initialised globals in DATA order (tables with their strings, the three
  pictures, D_0AEE/D_0B06); D_0BD2 before F_16D3, D_0C92-D_0C9E before F_2003, D_0DE0/D_0DE2 before
  F_31D7; `portname` ("left".."bottom") follows fightfrm (D_0E18).
  `_STACK` is `int _STACK = 0x800;` (0130), 0132 is an unnamed int (D_0132), D_01F4 follows D_01F2.
- D_2C30..D_2C5A are really the sector (3,3) of the galaxy D_26C0 (26C0 + 24*0x3A); SPBSS keeps them
  as separate symbols because the code addresses them by name. D_2C5C is `int` in SPACE.C
  (F_0D88 stores 0 to it) and `char[]` in SPBSS.C.
- spacebnk is `extern int` in SPACE.C (F_0D88 stores 0), but main needs its address plus 1406:
  `&spacebnk + 703` (int pointer arithmetic) compiles to the original constant; `(char *)&spacebnk + 1406`
  does not (it emits a runtime add). fightbnk is `extern char[]`.
- space.syms: 10FA is now D_07FA (the Origin module OR3C8B's variable); D_07FA_S is only 07FA (picture).
  D_0B1E added.
- SPACE.ASM (old generated stand-in) is no longer linked; kept for reference.
