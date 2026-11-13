# ULTIMA.EXE: title sequence and loader (src/ultima/ULTIMA.C)

ULTIMA.EXE is the program the player starts. It checks the video adapter and memory, shows the title
animation, the story lines and the castle picture in an endless loop, and as soon as a key is pressed it shows
the credits screen and starts GEN.EXE with `execl`. It never exits normally. The game part is code
03E9-0E4D (18 functions) of a 36880-byte EXE whose tail holds a copy-protection envelope (docs/MAP.md); the
envelope is not analysed here.

## 1. Start-up: `main` = F_1853(argc, argv)

```
F_1D0D(1);                                   /* Lattice ONERROR */
D_4A2A = argv[1];                            /* passed on to gen.exe */
D_2048 = (toupper(argv[1][0]) == 'C');       /* force CGA */
F_04D0();  F_0516();  F_0586();  F_0AF7();
if (D_082C == 1) F_3CE9();                   /* EGA: convert the castle picture to planes */
for (;;) { F_0DB3(); F_0C08(); }             /* title, then text/castle, forever */
```

(With the GOG launcher there is no argument: see overview.md for what that means.)

* **F_04D0 adapter check.** `v = F_34BF()` (sets `D_082C`). `v == -1` (monochrome): print "Ultima I requires a
  Color Graphics Adapter." and `EXIT(1)`. Otherwise `F_3BD5(1)` if `v == 2` (Tandy) and not forced CGA, else
  `F_3BD5(0)`.
* **F_3BD5(n) memory** (module OU3BD5): asks DOS (INT 21h/48h, BX = FFFFh) for the largest free block and
  stores its size in paragraphs in `D_4996`. For Tandy (n = 1) it subtracts 0401h paragraphs, then allocates
  that block, allocates a 0400h-paragraph (16 KB) block (segment in `D_4994`, also written to the word at
  0050h:0010h), and frees the big one: it carves a 16 KB video buffer out of the top of memory. **(inferred
  purpose: Tandy video RAM.)**
* **F_0516 memory check.** If `D_4996 < 0x1900` (6400 paragraphs = 100 KB): "Not enough memory." and
  `EXIT(1)`.
* **F_0586 banner.** `F_3F58(1)` (colours, video mode set, see overview.md), six 1-pixel lines in `D_0DA8`
  (rows 0x3A-0x3C and 0x4A-0x4C, x 112..200), then the texts "Ultima I" at (16,8), "The First Age of Darkness"
  at (8,11), "Copyright (C) 1987 Origin Systems, Inc." at (0,21), all in `D_0DA6`.
* **F_0AF7** loads the castle picture into the 0x7D00-byte buffer `D_4A2C`: `castle.4` (0x4000 bytes) when
  `D_082C == 0`, else `castle.16` (0x7D00 bytes). Uses Lattice `FOPEN/FREAD/FCLOSE` ("rb"); no error handling
  (a missing file leaves the buffer as it is).
* **F_3CE9** (EGA only, once): converts the 4-bits-per-pixel picture in `D_4A2C` to four bit planes (it allocates
  a 0x800-paragraph work block, builds plane after plane: for each plane and each group of 4 source bytes
  (8 pixels) one output byte, test of the plane bit with the mask tables `D_49D0`/`D_49D4`, 8000 bytes per
  plane) and copies the 32000 bytes back. (OU3C6C.ASM; details of the mask tables not checked.)

## 2. The endless loop

```
F_0DB3():  k = F_0D4C();                    /* picture reveal; returns a key pressed during it */
           if (k == 0) { cursor (14,13); wait 5 ticks; print "Presents..." (D_2046); wait 45 ticks; }
           else F_0A27();                   /* credits + start gen.exe, never returns */
F_0C08():  text pages and castle (below); if any key was pressed: clear screen, F_0A27()
```

Timings are in `F_3E5E` ticks (the Origin timer wait).

### The picture reveal (F_0D4C, F_08BF, F_0628, F_0744)

The title picture is 64 rows by about 276 columns of text characters (`'*'` = lit pixel), stored as two
pointer tables `D_3262[64]` (left 199 columns, 62 rows used) and `D_4320[64]` (right 77 columns) that sit between
F_0586 and F_0A27 in the original DATA. (It is the Ultima I title logo; not rendered here.)

F_0D4C: clear the screen, colour 9 (EGA/Tandy) or `D_2044` (CGA), `F_08BF(D_3262, D_4320, 0x15, 0x14, 0x129,
0x53, col)`, then plot the single pixel (0x59, 0x33) (a detail pixel of the logo **(inferred)**), return the key.

F_08BF computes the geometry for the rectangle (x0,y0)-(x1,y1) = (21,20)-(297,83):

```
w = (x1-x0)/4 + (x1-x0)%2     = 69        /* columns per quarter */
h = (y1-y0-1)/2 + (y1-y0-1)%2 = 31        /* half the rows */
```

F_0628(t1,t2,h,w,x,y,col) reveals the picture in pseudo-random order using a **Galois LFSR**:

```
n = h*w (2139); bits = F_0722(n-1) = number of significant bits (12)
tap = D_43A0[bits-2]  ( = 0xCA0 for 12 bits)
s = 1;
do { q = s / w; r = s % w;                 /* unsigned long arithmetic via CXD22 */
     if (q < h) F_0744(t1,t2,q,r,x-1,y,col,w,h);
     s = (s & 1) ? (s >> 1) ^ tap : s >> 1;
     key = F_413C(0);
} while (s != 1 && key == 0);
return key;
```

`D_43A0[]` holds the standard maximal-length taps for widths 2..16: 3, 6, 0Ch, 14h, 30h, 60h, 0B8h, 110h,
240h, 500h, 0CA0h, 1B00h, 3500h, 6000h, 0B400h (plus a 16th entry 2000h). A 12-bit LFSR visits 1..4095, so every
cell index 1..2138 appears once (cells with `q >= h` are skipped). Each visit calls F_0744, which plots up to
eight pixels: the picture pixels in rows `q` and `q+h` at the four columns `r, r+w, r+2w, r+3w` (r+w > 198 is
read from the right table). Pressing a key during the reveal aborts it (returns the key).

F_0926 draws the same picture instantly (two rows `i` and `63-i` at a time) and is used for the credits screen.

### Text pages and castle (F_0C08)

1. Clear rows around (14,13) (11 blanks, colour `D_2046`); print "...a new release of the best / selling personal
   computer / role-playing adventure..." at (5,12),(5,13),(5,14); then up to 75 times: wait 1 tick, poll a key.
2. If no key: clear the rectangle (0,90)-(319,179), print "...Lord British's original / fantasy masterpiece..." at
   (6,12),(6,13); again 75 x (wait, poll).
3. If still no key: F_0B65, the castle: clear the screen; for EGA/Tandy F_3C6C sets the castle palette (INT 10h
   AX=1002h with the table `D_49BC`); show the picture (`D_082C`: 0 F_3CCD copies 0x4000 bytes to B800:0, 1
   F_3D6A writes the four planes to A000 through the EGA map-mask, 2 F_3CA2 copies 200 rows of 160 bytes through
   the row-address table); then 100 times: wait 3 ticks and draw a random one of three 7x7 bitmaps
   (`D_0124`, `D_0164`, `D_01A4`; F_0473 picks `F_4069(1,0xEF)/80`: 0, 1 or 2) at x=0x7B and x=0xC4, row 0x37
   (two small animated shapes on the picture; **(inferred: flickering torch/sparkle)**), stopping on a key
   (F_0473 returns `F_413C(0)`). Then clear and restore the normal palette (F_3C87).
4. A key at any point: clear the screen and F_0A27.

### Credits and launch (F_0A27)

Draw the picture (F_0926), print in `D_0DA6`: "ULTIMA and LORD BRITISH are registered" (1,17), "trademarks of Richard
Garriott and" (2,18), "Origin Systems, Inc." (11,19), "Conversion by: John Fachini" (6,23); wait 60 ticks; then

```
for (;;) { EXECL("gen.exe", "gen.exe", D_4A2A, 0);       /* does not return on success */
           print "Insert ULTIMA I disk and press RETURN" at (1,21) in D_2044; wait for Return (0Dh);
           blank the line; }
```

F_053B ("Please insert the ULTIMA I disk...") exists but is never called.

## 3. Data and variables

| address | name | meaning |
|---|---|---|
| 0124, 0164, 01A4 | 3 x char[8][8] | the three 7x7 flicker bitmaps (rows of 8 bytes, values 0/1) |
| 02B9.. | `D_3262[]`, `D_4320[]` | the picture rows and their pointer tables |
| 43A0 | `D_43A0[]` | LFSR taps |
| 4994 / 4996 | `D_4994` / `D_4996` | Tandy buffer segment / free paragraphs (OU3BD5) |
| 49BC | `D_49BC` | castle palette table (16 entries for INT 10h/1002h) |
| 4A20.. | `D_0DA6, D_2044, D_0DA8, D_2046` | colours (CGA 3,1,2,3 / EGA 15,12,1,11 as in overview.md) |
| 4A28 | `D_2048` | force-CGA flag |
| 4A2A | `D_4A2A` | `argv[1]` (NULL when started without arguments) |
| 4A2C | `D_4A2C[0x7D00]` | castle picture buffer |

Key codes returned by `F_413C` are described in overview.md; during the title any key (including Escape) counts.
The credits text and the "Conversion by: John Fachini" line are the only credits inside the programs; the manual
credits Dav Holle (lead programmer and team manager), Dr. Cat, Steve Meuse, Dallas Snell, John Miles and Richard
Garriott.

## 4. Notes

* The colour globals sit at 4A20..4A28 in BSS in the order `D_0DA6, D_2044, D_0DA8, D_2046, D_2048`; the other
  EXEs order them differently (SPACE/MONDAIN: `D_0DA6, D_0DA8, D_2044, D_2046, D_2048`).
* In the file image the protection envelope overlays the start of ULTIMA's BSS (docs/MAP.md), so it costs no
  memory at run time.
