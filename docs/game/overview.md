# Ultima I (Origin Systems 1986/87 PC version): programs, chaining, data files

This is the top-level map of the game as seen in the decompiled sources. Details per program:

| document | program |
|---|---|
| [title-and-loader.md](title-and-loader.md) | ULTIMA.EXE (title, story, castle picture, starts GEN) |
| [character-creation.md](character-creation.md) | GEN.EXE (menu, character generation, roster, starts OUT) |
| [space-flight.md](space-flight.md) | SPACE.EXE (space shuttle flight and combat) |
| [final-battle.md](final-battle.md) | MONDAIN.EXE (the fight against Mondain) |
| [save-format.md](save-format.md) | `playerN.u1` / `inuse.u1` (the 820-byte character record) |

OUT.EXE (the game proper: overworld, towns, castles, dungeons, shops) is being integrated separately; this
documentation only touches it where the other programs call it or share its data. Statements taken from
`src/out/OUT.C` (work in progress) are marked "(OUT.C)".

Conventions: `F_xxxx` is the code offset of a function in that EXE, `D_xxxx` a DGROUP offset (see
docs/MAP.md). Certain facts come from the code; interpretations are marked **(inferred)** or
**(uncertain)**. The manual is `gamedata/Ultima_1_manual.pdf`.

## 1. The five programs and how they chain

Each program is a separate DOS EXE built with Lattice C 3.1x (small model). They never return to each other:
the next program is started with Lattice `execl()` and the current one is gone. Only one runs at a time and all
state that must survive is either in the command line or in a file.

```
ULTIMA.EXE  --execl("gen.exe","gen.exe", argv1, 0)-->  GEN.EXE
GEN.EXE     --execl("out", "out", mode, slot, 0)---->   OUT.EXE        (slot = '1'..'4')
OUT.EXE     --execl("space.exe",   p, argv1, argv2, 0)-> SPACE.EXE     (boarding a Shuttle, map tile 17)
OUT.EXE     --execl("mondain.exe", p, argv1, argv2, 0)-> MONDAIN.EXE   (boarding a Time Machine, tile 18,
                                                                       only with all four gems)
SPACE.EXE   --execl("out.exe","out.exe", argv1, argv2, "S", 0)-> OUT.EXE   (landed, or died)
MONDAIN.EXE --execl("out.exe","out.exe", argv1, argv2, "M", 0)-> OUT.EXE   (died; victory never returns)
```

Sources: ULTIMA F_0A27 (`EXECL("gen.exe", ...)`), GEN F_1643 (`execl(D_01A0 = "out", ...)`), OUT F_1135 and F_118D
(OUT.C), SPACE F_0EBD, MONDAIN F_12EC.

Every `execl` sits in an endless loop: when it returns (the file was not found, e.g. wrong diskette) the program
asks for the disk and tries again. ULTIMA prints "Insert ULTIMA I disk and press RETURN" (waits for Return);
GEN, SPACE, MONDAIN and the file routines print "Insert Ultima I diskette / Hit space to continue..." (waits for
Space). Note that GEN's literal is `"out"` without extension, ULTIMA's `"gen.exe"`, SPACE/MONDAIN's `"out.exe"`.

### Command line (all five programs)

* `argv[1]` is a one-letter video mode: **C** = force CGA, **E** = EGA, **T** = Tandy. Every program does
  `D_2048 = (toupper(argv[1][0]) == 'C')`; the Origin routine F_3F58 (and GEN's own F_03E9) then forces CGA
  colours and `D_082C = 0` when that flag is set. The letter is passed along unchanged.
* The GOG launcher runs plain `ultima.exe` (`gamedata/dosboxULTIMA1*.conf`), so ULTIMA receives no `argv[1]`
  (presumably a NULL pointer, if Lattice's `_MAIN` terminates `argv` as ANSI does; not checked in `_MAIN`.
  Then reading `argv[1][0]` reads DGROUP offset 0, and `D_4A2A = argv[1] = NULL` ends the argument list of the
  following `execl`). GEN therefore also starts without argument. The video mode is in any
  case auto-detected (below); GEN's F_1643 then **creates** the letter for OUT: it starts as `"C"` and becomes
  `'E'` (D_082C == 1) or `'T'` (D_082C == 2).
* `argv[2]` (OUT, SPACE, MONDAIN) is the character slot as a character, `'1'`..`'4'`: GEN's `D_01A8` is the
  string `"0"` and F_1643 adds the slot number to its first byte.
* `argv[3]` (OUT only): any non-NULL value means "returning from SPACE ("S") or MONDAIN ("M")"; OUT's F_03E9 then
  reads `inuse.u1` instead of `playerN.u1` (OUT.C: `if (D_8B8C == 0) read D_0180 else read "inuse.u1"`). The
  letter itself is not examined.

SPACE and MONDAIN keep `argv[1]`, `argv[2]` pointers (SPACE `D_5E10/D_5E12`, MONDAIN `D_2E0C/D_2E0E`) only to pass
them back to OUT.

## 2. Files

### Character files (details in save-format.md)

| file | written by | read by | content |
|---|---|---|---|
| `player1.u1`..`player4.u1` | GEN (new character), OUT (Save command F_5B0E) | GEN (names only), OUT at start | 820-byte character record |
| `inuse.u1` | OUT before `execl` to SPACE/MONDAIN; SPACE, MONDAIN before returning to OUT | SPACE, MONDAIN at start; OUT when `argv[3]` is set | same record: the "current" character handed from program to program |

The record is always read/written as one block of `0x334` = 820 bytes, by the Origin routines F_3E2F (read, mode
`"rb"`) and F_3E85 (write, mode `"wb"`). Both loop forever on any DOS error: they call the "Insert Ultima I
diskette" prompt (F_125C) and retry (`while (_OSERR != 0)`). GEN uses `fopen/fread/fwrite` directly and
treats failures differently (see character-creation.md).

### Game data (all read-only; names are matched case-insensitively by DOS)

| file | size | read by | content |
|---|---|---|---|
| `CASTLE.4` | 16384 | ULTIMA | title picture, CGA screen image (ULTIMA reads 0x4000 bytes) |
| `CASTLE.16` | 32000 | ULTIMA | title picture, 16-colour (EGA/Tandy), 4 bits per pixel, 320x200 |
| `{CGA,EGA,T1K}SPACE.BIN` | 7296 | SPACE | ship sprites: 12 sprites (3 types x 4 facings) x 608 bytes |
| `{CGA,EGA,T1K}FIGHT.BIN` | 3192 | SPACE | enemy sprites of the front view: 7 sizes x 456 bytes |
| `{CGA,EGA,T1K}MOND.BIN` | 1216 (CGA) / 2432 | MONDAIN | 19 tiles of the final battle |
| `{CGA,EGA,T1K}TILES.BIN` | 3328 (CGA) / 6656 | OUT; MONDAIN (first 11 tiles) | overworld tile set (52 tiles; CGA 64 bytes/tile, EGA/T1K 128) |
| `{CGA,EGA,T1K}TOWN.BIN` | 816 / 1632 | OUT (reads 1632 bytes) | (OUT.C) town/castle graphics |
| `NIF.BIN` | 6720 | MONDAIN | the ending picture, 1 bit per pixel, 320 wide (6720*8/320 = 168 rows) |
| `MAP.BIN` | 13104 | OUT | overworld map. 13104 = 168 x 156 / 2: 4 bits per cell **(inferred from OUT.C's quadrant function F_40FE, which splits at x > 83, y > 77)** |
| `TCD.BIN` | 6840 | OUT | (OUT.C) `D_8BAE[10][38][18]`: ten 38x18 maps (towns/castles) |

`C*` = CGA 4-colour, `E*` = EGA 16-colour planar, `T1K*` = Tandy 1000 16-colour. SPACE and MONDAIN pick the
name by `D_082C` from a three-entry pointer table (SPACE `D_0166[]`/`D_0194[]`, MONDAIN `D_0156[]`/`D_0184[]`).
The graphic formats of the sprites are explained where they are used (space-flight.md, final-battle.md).

## 3. Video modes, colours and Origin support routines

`D_082C` (set by the Origin routine F_34BF "detect adapter", which also returns it) holds the mode:

| value | meaning | detection (F_34BF) |
|---|---|---|
| 0 | CGA, 320x200, 4 colours (video segment B800) | default; also PCjr (BIOS model byte F000:FFFE = FDh) |
| 1 | EGA, 320x200x16, planar (segment A000, mode 0Dh) | INT 10h AH=12h BL=10h reports an EGA with colour monitor |
| 2 | Tandy 1000 (16 colours, segment B800) | model byte FFh and the string "Tandy" found in ROM A000h..FFFFh |
| -1 | monochrome: ULTIMA exits with "Ultima I requires a Color Graphics Adapter." | EGA present but mono monitor |

(For a model byte FFh without "Tandy" the EGA test is applied as for other models.) The "force CGA" letter `C`
overrides the result: `D_082C = 0`.

The text/graphics colours are four globals set by the Origin routine F_3F58 (GEN: its own F_03E9, which differs
slightly):

| global | CGA | EGA / Tandy | used as |
|---|---|---|---|
| `D_0DA6` | 3 | 15 | main foreground (frames, text) |
| `D_0DA8` | 2 | 1 | border/accent colour |
| `D_2044` | 1 | 12 | alert/warning colour (also fuel/shield in SPACE) |
| `D_2046` | 3 | 11 | message text colour |

F_3F58(setmode) also calls F_4071 (fill the 200-entry row address table `D_084A`) and, when `setmode != 0`, the
mode switch (F_3547 CGA mode 4, F_4115 EGA 0Dh, F_40EE Tandy). SPACE and MONDAIN call it with 0: the screen is
already in graphics mode when they are started (the previous program left it that way).

### Shared Origin routines used by the four documented programs

Roles come from how the routines are called and from the assembly in `src/origin/`. All are `near` C-callable.

| routine | role | confidence |
|---|---|---|
| F_352C(col,row) | set the text cursor (column 0..39, row 0..24; 8x8 cells) | certain |
| F_3599(str,colour) | print a string at the cursor | certain |
| F_357C(ch,colour) | print one character | certain |
| F_3B46(str) | message line: print `str` in `D_2046`, scroll the message area (F_3EED), cursor to (1,24) | certain |
| F_3AE2() | "Press Space to continue:" at (1,24), wait for Return/Space, erase | certain |
| F_3C8B(n,width,pad,colour,flag) | print a number | certain; `flag` meaning unknown (1 = left aligned **(inferred)**) |
| F_35CA(x0,y0,x1,y1,col) | fill a rectangle | certain |
| F_39FC(x0,y0,x1,y1,col) / F_3812(x,y,col) | line / line from the last point | certain |
| F_3D41(x,y,col) | plot a pixel | certain |
| F_387E(col,row) / F_3920 | draw the left/right command-arrow markers around a text cell (also used as frame ornaments) | **(inferred)** |
| F_39C5(colour) | wait for a key (blinking cursor) | certain |
| F_413C(colour) | poll a key with animated cursor, 0 = none. Arrow keys return 1 left, 2 right, 3 up, 4 down; Esc 1Bh, Return 0Dh, Backspace 8; F1-F10 return C9h-D2h; others ASCII | certain (OR413C.ASM) |
| F_4127() | F_413C(D_2046) upper-cased, then waits 3 ticks if no key (1 if key) | certain |
| F_4161() | key wait: up to 1500 polls of F_4127; 0 on timeout | certain |
| F_3862(ch) | toupper | certain |
| F_4224() | flush the keyboard buffer (non-PC models) | certain |
| F_3E5E(n) | wait n timer ticks (timer ISR hook) | certain; tick length not measured |
| F_3CF1(n) | short delay (MONDAIN F_1B46, SPACE F_3330) | **(uncertain)** |
| F_3EC4() | pseudo-random value from the DOS clock | certain |
| F_37FD(seed) | seed the generator `D_0C70` | certain |
| F_4069(lo,hi) | random number in [lo,hi] (hi is clamped to 255; result is a byte). The generator: `x = ror3(x + 9248h); r = x & 3FFh`, then `r` is reduced by repeated subtraction of `hi-lo` until `r <= hi-lo`, result `r + lo` (so roughly uniform, 0 and `hi-lo` slightly over/under-represented) | certain (OM4069.ASM) |
| F_3F62(n) | play PC-speaker sound n (0..9; a jump table of 10 routines) | certain |
| F_3E2F(name,buf,n) / F_3E85(name,buf,n) | read / write file (see above) | certain |
| F_4437 / F_44A2 / F_450D / F_4558 | redraw the status line fields "Hits:", "Food:", "Exp.:", "Coin:" from the record | certain from the strings `D_0CBA..D_0CCC` |
| F_45A3() | draw the status panel (all four) at start | **(inferred)** |
| F_43C8 | the Ready command (weapon/armour/spell); F_478E the Ztats screen | **(inferred)**, both are called by `R` and `Z` |
| F_40A8(ch) | is `ch` a vowel (for "a/an") | certain |
| F_40E9() | level = experience / 1000 + 1 | certain |
| F_3BD5(n) | ULTIMA only: memory check/reservation (title-and-loader.md) | |
| F_1D0D(1) | Lattice `ONERROR` module routine, called first in every `main` | certain it is Lattice's |

The sound numbers used by the four programs (arguments of F_3F62; roles **(inferred)** from where they are
played): 0 blocked/crash, 1 beep (refused or unknown command), 2 damage taken / hit, 3 monster melee, 4 footstep,
5 spell/magic, 6 spell failed, 7 weapon attack, 8 laser (SPACE), 9 thruster hum (SPACE, once at each end of
every star-field frame). The Noise command toggles a flag stored in the character record (offset 0x38).

## 4. Naming caveat for the record fields

SPACE, MONDAIN and OUT all contain the 820-byte character record in BSS, each at a different address, but the
sources name its fields with the **MONDAIN** addresses (`D_2058` = record start, `D_206E` = hit points, ...,
`D_20FE` = kills). The real addresses are:

| program | record start | hit points | `D_5B8C` / shuttle fuel |
|---|---|---|---|
| GEN | `D_0A30` | `D_0A46` | `D_0AE0` |
| MONDAIN | `D_2058` | `D_206E` | - |
| SPACE | 5ADC (named `D_2058`) | 5AF2 (`D_206E`) | 5B8C |
| OUT | 3A78 (named `D_2058`) | 3A8E (`D_206E`) | 3B28 (`D_3B28`) |

Record offset = (name address - 0x2058) in SPACE/OUT/MONDAIN source names, = (address - 0xA30) in GEN. The full
field table is in save-format.md.
