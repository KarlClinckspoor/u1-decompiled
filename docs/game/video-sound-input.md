# OUT.EXE: video, keyboard, timing, sound and file I/O as seen from the game (Origin support library)

Sources: the Origin library modules linked into OUT (`src/origin/OR*.ASM`, `OM*.ASM`, `OO*.ASM`; their roles are tabulated in docs/MAP.md), `OUTA.C`..`OUTC.C`, `F_3F58` in `OM3F58.ASM`.
The same library is used by GEN, SPACE and MONDAIN (overview.md section 3 lists the routines with their semantics); this file adds what OUT does with it and the details that were read from the code. Parameters are listed in **C
order** (first argument first); the routines are Lattice-C-callable (arguments on the stack, `ret n` pops them). "(inferred)" marks interpretations not proven by running the program.

## 1. Video adapters and modes

OUT never selects the video mode itself: `F_3F58(0)` (called once from main) gets the adapter type, picks colours and builds the row-address table, but its argument (`setmode`) is 0, so the screen stays in the mode ULTIMA/GEN set up
(they pass the mode on as the letter in `argv[1]`).

* **Adapter detection** `F_34BF` (writes `D_082C`, DGROUP 19CC (named 082C); returns it): reads the BIOS model byte at F000:FFFE: 0xFD (PCjr) -> 0 (CGA tiles/modes); 0xFF (PC): scans ROM from F000:A000 for the string "Tandy" (data at 18B0) -> 2 (Tandy); otherwise EGA test with
  INT 10h AH=12h BL=10h (unchanged CX = no EGA -> 0 CGA; BH bit 0 gives colour EGA -> 1, monochrome EGA -> -1). So `D_082C`: **0 CGA, 1 EGA, 2 Tandy 16-colour** (and 0xFFFF for a monochrome EGA, which nothing handles).
* **Mode set** (not called by OUT, but it defines the formats): CGA `F_3547` = BIOS mode 4 (320 x 200, 4 colours, palette via INT 10h AH=0Bh, port 3D8h), EGA `F_4115` = mode 0Dh (320 x 200, 16 colours, 4 planes), Tandy `F_40EE` = mode 9 (320 x 200, 16 colours, 4 bits/pixel at B800h).
* **Colours** (F_3F58, per adapter; `D_2048` = the "C" argument forces the CGA set and `D_082C = 0`): CGA: `D_0DA6` = 3, `D_0DA8` = 2, `D_2044` = 1, `D_2046` = 3; EGA/Tandy: 15, 1, 12, 11. `D_0DA6` = frame/lines, `D_0DA8` = fill, `D_2044` = highlight/warning, `D_2046` = text.
* `F_4071` builds `D_084A` (DGROUP 24EC, 200 words): the offset of every pixel row in video memory: CGA `(row & 1) * 0x2000 + (row >> 1) * 80`; Tandy `(row & 3) * 0x2000 + (row >> 2) * 160`; EGA `row * 40`. It also sets
  `D_0830` (DGROUP 19D0) = **1 (EGA), 2 (CGA), 4 (Tandy)**, which the blitters add to the byte offset: it equals the width in bytes of 8 pixels, i.e. a left margin of one character cell.
* The playing screen is the 40 x 25 text grid of 8 x 8 cells = 320 x 200 pixels. The map window is x 8..311, y 8..151 (304 x 144): 19 x 9 tiles of 16 x 16 (overworld), 38 x 18 of 8 x 8 (towns/castles), the free-drawn dungeon view; the frame/panel layout is in out-state.md section 5.

## 2. Text

Text is output through the **BIOS teletype** (INT 10h AH=0Eh with the colour in BL) so it uses the ROM 8 x 8 font, also in the graphics modes; the cursor is the BIOS cursor (INT 10h AH=2, page 0).

| routine | signature | action |
|---|---|---|
| F_352C | (col, row) | position the cursor |
| F_357C | (ch, colour) | print one character |
| F_3599 | (string, colour) | print a string; **no** newline, no wrapping |
| F_3C8B | (value, width, pad, colour, advance) | print an unsigned number: 5 digits computed with the place values 10000..1 (table at 24DE-); leading zeros replaced by `pad` (a space or '.') up to `width` characters; if `advance == 0` the last digit is written with INT 10h AH=9 so the cursor does not move |
| F_3B46 | (string) | print a line in `D_2046`, scroll the message window up one line (F_3EED), cursor to (1,24) |
| F_3EED | () | scroll the message area (columns 0..29, rows 21..24): EGA/Tandy with BIOS scroll-up (INT 10h AH=6); CGA by moving 30 words per pixel row and clearing the last 8 rows |
| F_387E / F_3920 | (col, row) | draw the right / left arrow glyph (characters 10h / 11h plus lines) at a cell: used for the command prompt and frame headings |
| F_353A | (string) | strlen |
| F_33BB, F_3862 | (ch) | tolower, toupper |
| F_40A8 | (ch) | is it a vowel (A E I O U) -- used for "a"/"an" |
| F_3AE2 | () | prints "Press Space to continue: " at (1,24), waits for Space or Return, clears the rectangle x 0..230, y 192..199 (the text part of row 24) |
| F_B0B5 | (x, y) | number entry at a screen position: up to 4 digits (a leading 0 is ignored; Backspace edits; the first non-digit ends) |

## 3. Graphics

| routine | signature | action |
|---|---|---|
| F_35CA | (x0, y0, x1, y1, colour) | filled rectangle (corners may be given in either order); mode-specific inner loops for CGA (4 pixels per byte), Tandy, EGA (planar via port 3C4/3CE) |
| F_39FC | (x0, y0, x1, y1, colour) | line (Bresenham-style, F_3A1D); also sets the "pen" `D_07F2/D_07F4` |
| F_3812 | (x, y, colour) | line from the pen to (x, y) |
| F_3D47 / F_3D41 / F_3D61 | (x, y, colour) | plot a pixel (3D47 also sets the pen; 3D41 sets a "write" flag `D_082A`) |
| F_355D | (tile, col, row) | draw overworld tile (16 x 16 pixels) at tile column/row; tile data at `D_238C` (70DC), 64 bytes per tile (CGA, 16 rows x 4 bytes) or 128 (EGA/Tandy) |
| F_B34F | (tile, x, y) | draw a town/castle tile (8 x 8) at map cell (x, y); data at A666, 16 / 32 bytes per tile |
| F_3578 | (tile, col, row) in registers | the blitter behind F_355D |
| F_AC91 | (kind, depth, colour) | draw the vector sprite of a dungeon monster/feature scaled for depth 1..5 |
| F_AB9D | (colour) | draw the tombstone bitmap (from masks at 19EC-) |
| F_3BF6, F_3C46 | () | frame bars: F_3BF6 the lower divider (x 80..216, y 152..159, fill `D_0DA8`, lines in `D_0DA6`), F_3C46 the top bar (x 80..216, y 0..8) |
| F_418A, F_41CF | () | F_418A: redraw both bars, clear the map window and print a heading between arrows; F_41CF: the narrow top bar (x 127..199, y 0..7) and `D_204E = 1` |

Screen addresses: CGA B800h (interleaved rows); Tandy B800h; EGA A000h, one plane at a time via the sequencer map-mask register (port 3C5h after index 2) -- planes 1, 2, 4, 8 -- and the graphics controller (3CEh). The pixel/tile routines
compute the row start from `D_084A[row]` plus `D_0830` plus the byte column.
Animation of the overworld is done by redrawing: F_AF57 repaints every visible tile at each idle poll of the main loop (world-and-map.md); F_A554 rotates the water tile data (inferred) so that redrawing produces the shimmer.

## 4. Keyboard

* Keys are read with **DOS direct console I/O** (INT 21h AH=06h, DL=FFh), not through the BIOS: the Origin routine `F_413C(colour)` returns 0 when no key is waiting, otherwise the key: extended codes are mapped to 1 = left (scan 4Bh), 2 = right (4Dh), 3 = up (48h), 4 = down (50h); F1..F10 (3Bh..44h) to C9h..D2h; other
  extended keys are dropped (0); ordinary keys >= 20h and Esc (1Bh), Return (0Dh), Backspace (08h) are returned as is; other control codes are dropped. After a key it calls F_4234, which sets the BIOS keyboard buffer head to its tail (a type-ahead flush) -- but only when the BIOS model byte at F000:FFFE is **not** FFh (so not on the original PC).
* While waiting, F_413C animates a **blinking cursor** (4 frames from `D_09FE` at 3A24) at the BIOS cursor position in the given colour; with colour 0 no cursor is drawn.
* `F_4127()` = `toupper(F_413C(D_2046))` plus a delay: 3 timer ticks if no key, 1 tick if a key. `F_39C5(colour)` waits for a key (Origin C module, `OR39C5.C`) with the same ticks. `F_4161()` waits for a key up to 1500 polls (used for directions).
* The game itself accepts only the keys of commands.md; Esc has no effect except setting a flag.

## 5. Timing

`F_3E5E(n)`: waits `n` ticks of the BIOS timer interrupt (INT 1Ch, 18.2 Hz): it installs its own handler `TIMERISR` for INT 1Ch, counts to `n` (`D_0832`, `D_0834`), and restores the old vector. One tick of 55 ms is therefore the granularity of
every delay (projectile steps use 2 ticks, the death sequence 1 tick per hit point, the tavern pause...). `F_3CF1(n)` (waits `n` vertical retraces on port 3DAh) is linked but used by other programs. There is no other timing: the game is turn-based.

## 6. Sound (PC speaker)

`F_3F62(n)` (n = 0..9) calls routine `n` of the table `D_0836` (DGROUP 24CA). Each routine produces a tone by **bit-banging the speaker bit of port 61h** (toggling bit 1 with delay loops between the toggles; the timer chip is not used): the delay between toggles (the pitch) and the number of toggles
(the length) are fixed per sound, and several routines sweep the delay to make falling/rising tones. OUT checks the sound flag `D_3AB0` in `F_0476(n)` (a no-op when Noise is off).

| n | routine | character (inferred from the loops) | played for (OUT) |
|---|---|---|---|
| 0 | F_3F7F | 16 toggles, delay 1000: short low buzz | crash/blocked move (F_048A, " - path blocked!") |
| 1 | F_3F97 | 48 toggles, delay 355: longer higher beep | refused/unknown command, "?" answers, failures in shops |
| 2 | F_3FB5 | 200 toggles; the delay starts at 7Eh and grows by 5 per toggle (8 bit, so it wraps): repeated falling sweeps | hit given or taken; death (x3) |
| 3 | F_3FBC | the same sweep with 255 toggles: longer | monster attack |
| 4 | F_3FC3 | the same sweep, only 8 toggles: a short click | footsteps |
| 5 | F_3FE3 | warbling nested loops (F_4008 with CH=C0h, CL..) | spell success, shrines, quest completed |
| 6 | F_3FEA | 160 toggles with the delay shrinking: a rising "failure" tone | spell failure, monument without effect |
| 7 | F_4004 | F_4008 with BL=5, BH=0Fh: a short warble | weapon use / shots |
| 8 | F_4038 | a falling-pitch sweep (delay grows each toggle): laser | ships' cannon/laser (also SPACE) |
| 9 | F_4051 | hum | SPACE only |

The Lattice and Origin libraries contain no other sound code.

## 7. Random numbers

* `F_4069(lo, hi)`: the game's generator: state `D_0C70` (267C): `x = ror(x + 0x9248, 3)`; `r = x & 0x3FF`; `hi` is clamped to 255; while `r > hi - lo`: `r -= hi - lo`; result `(r + lo) & 0xFF`. Note: `r` ends in `1..hi-lo` unless it started at 0 or `hi == lo`; the arithmetic is on 16 bits.
* `F_37FD(seed)` sets the state. `F_3EC4()` = a 12-bit number from the DOS clock (INT 21h AH=2Ch: hours, minutes, seconds, hundredths mixed with shifts and XOR 91EBh). OUT seeds at start with `D_3AAA + F_3EC4() % 999`; the dungeon generator re-seeds twice per level (dungeons.md).
* `F_91D4()` = Lattice `rand()` (`seed = (seed * 0x0E5D + 3) & 0x7FFF`, seed 1 at start, never re-seeded): used only for the monsters' choice of axis, where its alternating low bit matters (monsters.md).

## 8. Files, errors, exit

* `F_3E2F(name, buf, n)` / `F_3E85(name, buf, n)`: `fopen(name, "rb"/"wb")` (modes at 24E6/24E9), `fread/fwrite(buf, 1, n)` -- note the argument order: **n bytes in one item** -- `fclose`; on any DOS error (checked through `_OSERR`, DGROUP 0071) they call `F_125C` ("Insert Ultima I diskette / Hit space to
  continue...", waits for the Space key) and **retry forever**. OUT therefore cannot be made to fail by a missing file; it asks for the disk.
* `F_5643()`: switch the CGA to text mode 3 (INT 10h AH=0 AL=3; port 3D8h writes 0 then 29h) -- used before `EXIT(1)` when `argv[2]` is not a slot digit.
* `F_1D0D(1)` is Lattice's ONERROR set-up (`ONERROR.ASM`); OUT has no error handler of its own and never prints Lattice error messages while playing.
* There is no mouse, joystick, serial or printer support, and no copy-protection code in OUT.
