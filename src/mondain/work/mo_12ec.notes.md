# mo_12ec.C notes (MONDAIN F_12EC..F_2230, 23 functions)

## Status (all 23 MATCH with lc.py; data layout/strings not verified)

| function | status | purpose |
|---|---|---|
| F_12EC | MATCH | defeat screen, save inuse.u1, execl out.exe (never returns) |
| F_138D | MATCH | player dies: hp and food = 0, then F_12EC |
| F_13A3 | MATCH | redraw the 19x9 battle map and the player |
| F_1415 / F_1447 | MATCH | x / y step for direction 1..4 |
| F_1477 | MATCH | per-tick animation (spell effect, tile bounce, player tile) |
| F_1565 | MATCH | F_05B1(random 1..255 - 128) |
| F_1589 | MATCH | move player by (dx,dy) if the target square is free |
| F_161E | MATCH | player takes n damage |
| F_1664 | MATCH | Mondain melee attack |
| F_16E0 | MATCH | Mondain spell (magic missile / mind blaster / psionic shock) |
| F_186F | MATCH | monsters' turn (chant, melee, spell, move) |
| F_19C8 | MATCH | Mondain's phase by hp (D_012E), spawn magic square, death |
| F_1B46 | MATCH | read key (upper-cased) with idle animation; returns char |
| F_1B9B, F_1CAF, F_1D53, F_1D94 | MATCH | the four story text screens |
| F_1DE6 | MATCH | the four screens with pauses |
| F_1E03 | MATCH | load a file (n bytes) and plot it as a 1-bit 320-wide picture |
| F_1EA8 | MATCH | victory screen, wait for key |
| F_1F31 | MATCH | end of game: victory, nif.bin picture, restart message |
| F_1853 | MATCH | the C main (name in .syms is F_1853 but lives at 1F85): setup + key loop |

## Globals (D_ names; types as declared)
Initialised (DATA): D_0126 D_0128 (int: Mondain x,y, 3,4), D_012A D_012C (player x,y, 15,4),
D_012E (int: Mondain hp, 1000), D_0130 (int: spell-effect frame, -1), D_0156/D_0184/D_018A
(int[3]: file-name pointers and sizes per video mode), D_0190 D_0192 (int: bouncing tile),
D_0194 (unsigned char [19][9] battle map), D_0240 (Mondain tile), D_0242, D_0244
(Mondain state 20/40/30), D_0246/D_0248/D_024A (int flags), D_08BB (unsigned char[8] bit masks).
BSS: D_0DA6 D_0DA8 (int: colours), D_2046 (text colour), D_2048 (int: is-CGA flag), D_204A,
D_204C, D_204E (redraw flag), D_2054 D_2056 (int: magic square x,y), D_2058 (char[]: player
record, 0x334 bytes), D_206E (int[]: [0]=hp, 1..6 stats), D_2074 D_2078 D_207A D_2086
(int: player stats), D_2080 (food), D_2090, D_208A, D_082C (int: 0=CGA 1=EGA), D_238C (char[]),
D_2E0C D_2E0E (char *: argv[1], argv[2]), D_2E10 (unsigned char[]: file buffer).

## String literals (offset in MONDAIN DGROUP)
04FA "THOU ART DEAD!", 0509/0527/0541 defeat lines, 055D "inuse.u1", 0566 "out.exe", 056E "M",
0576 "Attacked by Mondain!", 057F-0597 spell strings (see file), 05DF "You hear a strange chanting!",
05FC "Mondain is dead!", 060D "...or is he?", 061A..08A4 story text (F_1B9B..F_1D94),
08C3 "THOU ART VICTORIOUS!", 08D8 "nif.bin", 08E0 "Press CONTROL-ALT-DEL to Restart".
The `""` in F_19C8 is the empty string at D_02AF (first module part; with one module -cs
makes it shared).

## Compiler lessons
- A hidden expression temp (2 bytes per int result held across a call, 4 for cdq/idiv) sits at
  [bp+0] BEFORE declared locals. An int "local" that is really an intermediate (F_1664:
  `if (F_4069() > expr) ... else F_161E(D_206E[0]/32 + F_4069())`) is NOT a variable: delete it.
- `(int)D_2058` as an argument yields `mov ax,addr; mov bx,const; push bx; push ax` and adds a
  temp slot; plain `D_2058` does not.
- `if (a == 0 || a == 3)` chain, not a switch, for 2-3 values (switch uses a 2-byte temp).
- `if (D_0246 != 0 && ..)` vs `if (D_0246 && ..)`: only the second compiled to the original after
  a leading `n = 0` (immediate store instead of xor ax,ax).
- Early `return;` statements give inline epilogues; the original used if/else nesting (one
  shared epilogue). Chained `c[1] = c[2] = 1` stores c[2] first.
- 30-case switch -> cs: compare loop over (value,addr) pairs, table in REVERSE source order.
