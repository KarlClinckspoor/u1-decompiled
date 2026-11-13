# OUT integration (done)

The game code of OUT.EXE (03E9-908B, 194 functions) is C now: `OUTA.C`, `OUTB.C`, `OUTC.C` (`-ms -v -w -cs`)
plus `OUTBSS.C` and `OUTSYM.ASM` replace the stand-ins OUT1-3.ASM (removed, still in git history).
`build.py out` gives IDENTICAL (sha1 e4048b57...); `lc.py` says MATCH for every function and for DATA of each
module, with no WRONG TARGETS:

    lc.py src/out/OUTA.C --exe gamedata/OUT.EXE --syms src/out/out.syms --data 124
    lc.py src/out/OUTB.C ... --data 7EC
    lc.py src/out/OUTC.C ... --data 1164

- Source: the work files `work/ou_*.C` merged in address order (all 194 functions), one set of externs in
  every part. The work files stay as they were.
- Why three parts: LC1 runs out of memory ("Not enough memory", a hard limit) at about 2250 lines; OUT.C in one
  piece is 4700. The original is one module as far as the data shows: 345 distinct string literals, none twice, in
  order of first use through all functions (`-cs` would have stored them twice if there were a boundary).
  The cuts are where the DATA offset is even (LINK word-aligns each module's DATA): OUTA = F_03E9-F_3240
  (DATA 0124-07EC), OUTB = F_32F8-F_63D4 (07EC-1164), OUTC = F_63F2-F_1853 (1164-17A4).
- Literals used in more than one part (40 of them, e.g. "", "!", "?", "Missed!") are named arrays,
  `char D_xxxx[] = "..."` with xxxx the DGROUP address, defined in the first part that uses them (all literals of
  that function that are new get one, to keep the order) and `extern`ed in the others; every use is the name.
  These are in out.syms.
- Initialised globals: `_stack` (3072), `D_014E` (three Tiles file names), `D_0172`, `D_0180` ("playerx.u1"),
  `D_0182` (flag, was `D_0180[1]` in ou_7c31.C), all at the top of OUTA.C; `D_16CC` (spawn chance by experience)
  sits between F_7E69 and F_7E80 in OUTC.C. All other DATA is string literals.
- Tables of names/messages (DGROUP 267C-39E8: weapon/armour/monster/spell/place names, the "blocked" messages,
  entity templates D_31DC...) are NOT game data modules: they belong to the Origin text module
  `src/origin/OM4437.ASM` (shared with MONDAIN and SPACE) and have only the labels D_0CD2, D_0D72, D_0DF8,
  D_0F04, D_0F5C, D_0FA2, D_0FD8, D_1DC8 there. The static copies in ou_03e9.C (D_37AE, D_368C) and ou_5469.C
  (D_307C, D_3174) must not be defined again. The unlabelled tables the code uses are given absolute names in
  `OUTSYM.ASM` (`D_31DC EQU 31DCH`, public; LINK resolves them as plain DGROUP offsets): D_307C D_3174 D_31CC D_31DC
  D_368C D_3698 D_36E8 D_36F8 D_3708 D_37AE D_3808 D_3814 D_3930. They are in out.syms too. (A cast of
  `D_0FD8 + n` does not fold the way the original does in F_20FC, an extern array does.)
- BSS (3A70-B124): `OUTBSS.C` defines every label of the stand-in as `char NAME[size]` in address order (size = distance
  to the next label, so overlays like the 820-byte player record D_2058 are several small objects). It is a module of
  its own so that the parts can declare the variables with the types they use (`int D_206E[]`,
  `struct cell D_AD56[][11]`, `struct mon D_3B2C[]`, ...). No other module has UDATA before _IOB.
- Type conflicts between the work files were settled in the merge: `D_206E` is an array (`D_206E[0]` = hit points),
  `D_3B2C` a `struct mon` array (ou_7c31.C used `int [][8]`: `MT[m][k]` macro there), `D_0FD8`/`D_0D72`/`D_0DF8`
  `unsigned char *[]`, `D_1DC8` `int[]`, `F_33BB`/`F_3862`/`F_39C5` return `unsigned char`.
- Still addresses as names (F_xxxx, D_xxxx); `F_3F7F_O` and `F_125C`/`F_1853` (main) keep their out.syms names.
