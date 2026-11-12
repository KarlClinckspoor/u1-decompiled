# MONDAIN integration (done)

`MONDAIN.C` (one module, `-ms -v -w -cs`) replaces the stand-in MONDAIN.ASM; `build.py mondain`
gives IDENTICAL (sha1 9456ed0a...). All 75 game functions and DATA 0124-0902 match
(`lc.py src/mondain/MONDAIN.C --exe gamedata/MONDAIN.EXE --syms src/mondain/mondain.syms --data 124`).

- Merged from work/mo_03e9.C, mo_03e9_b.C, mo_12ec.C in address order; one module is enough
  (the empty string at 02AF is shared by F_0AD9 and F_19C8; LC has enough memory).
- F_05DB is `unsigned char` now.
- Initialised globals come first (0124-0256: `_stack`, positions, file-name tables, the 19x9 field,
  Mondain state); D_08BB sits just before F_1E03 (its literals follow it in DATA).
- BSS (2040-48A0 before _IOB) is defined in address order. Unlabelled gaps are `U_xxxx` pads.
- Overlays of the stand-in's labels: D_0216 is inside D_0194 (written `D_0194[14][4]`);
  the player record D_2058[0x334] contains D_206E.. (D_206E is the hp int; the stats loop in F_16E0
  indexes `(&D_206E)[i]`, and D_2070/D_2072/... stay separate public ints).
- MONDAIN.ASM was removed (still in git history).
