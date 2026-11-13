# GEN.EXE: character generation (src/gen/GEN.C)

GEN is started by ULTIMA.EXE after the title sequence. It shows the main menu, creates characters (four slots
`player1.u1`..`player4.u1`), lets the player pick one and then starts OUT.EXE. All 29 functions are C and the
EXE rebuilds byte-identical. Function names below are code offsets (`F_xxxx`); data are `D_xxxx` DGROUP offsets.

## 1. Program flow

`main` = F_1853(argc, argv):

1. `F_1D0D(1)` (Lattice ONERROR); `D_0DAE = (toupper(argv[1][0]) == 'C')` (force CGA; see overview.md).
2. F_13B2: scan the roster (below).
3. F_03E9(0): choose colours and tables for the machine (`F_34BF()` = adapter type, which also sets `D_082C`):

   | adapter | `D_0DA6` | `D_0DA8` | `D_0DAA` | `D_0DAC` | `D_082E` |
   |---|---|---|---|---|---|
   | 0 CGA | 3 | 2 | 1 | 3 | 6 |
   | 1 EGA, 2 Tandy | 15 | 1 | 12 | 11 | 7 |

   `D_0DA6` frame/title colour, `D_0DA8` border fill, `D_0DAA` error colour, `D_0DAC` normal text colour. With
   the force-CGA flag: `D_082C = 0`, `D_0DA6 = 3, D_0DA8 = 2, D_0DAA = 1` (`D_0DAC` unchanged). Then F_4071 (row
   table) and, if the argument `setmode` is non-zero, the video mode switch (here 0: the screen is already in
   mode from ULTIMA). `D_082E` is written but not read by GEN (it belongs to an Origin routine).
4. Clear the screen, `D_0830 = 0`, draw the border (F_05B2: filled bars of `D_0DA8`, an inner line in `D_0DA6`,
   rounded corners from F_048A).
5. Loop forever: F_06BB (main menu) returns the key; `A` -> create a character, `B` -> continue game; any other
   key (including `Q`, which F_06BB accepts) redisplays the menu. After A/B the inside of the frame is cleared.

### Main menu (F_06BB)

Text: `*** Ultima I ***` at column 12 row 6, then "from darkest / dungeons, to / deepest space!" (rows 9-11),
`a) Generate new character`, `b) Continue previous game`, `Thy choice: `. Keys are upper-cased with F_3862;
only A, B or Q leave the loop.

### "Generate new character" (key A)

* F_1423: first free slot = first `i` with `D_01AA[i] == 0` (0..3), else -1.
* If the roster is full (F_14BB, title "Character Replacement"): list the four names (F_1455) and say "The roster
  is full. Type the number of the character to wish to replace with a new one. Press ESCAPE to return to the
  main menu..." Keys `1`..`4` pick the slot to overwrite (`c - '1'`), Escape returns -1 (nothing happens).
* F_106A(slot) does the generation (section 2); F_0670 redraws the title bar when back.

### "Continue previous game" (key B), F_16DC

* No saved characters (`D_0DA4 == 0`): "There are no Ultima I characters saved on this disk. Press RETURN to
  generate a character or ESCAPE to return to the main menu..." Return starts F_106A(0) (slot 0).
* Else F_1455 lists the four slots and the prompt "Type number of character you wish to use in Ultima I. Press
  ESCAPE to go to the main menu..." A key `1`..`4` whose slot exists (`D_01AA[c-'1'] != 0`) calls F_1643(c - '0');
  an empty slot is ignored; Escape returns.

### Starting the game, F_1643(n)

* `*D_01A4 = 'E'` if `D_082C == 1`, `'T'` if 2 (otherwise it stays `'C'`); `*D_01A8 += n` ("0" becomes "1".."4").
* Clear the screen, draw the frame (F_3AD7, the game screen frame; GEN-only Origin routine), print "Please wait
  whilst" / "thy game loads".
* `execl("out", "out", "C|E|T", "1".."4", 0)` in a loop; if it returns, F_15B3 asks "Insert Ultima I diskette /
  Hit space to continue..." (waits for Space) and the `execl` is retried.

## 2. Creating a character, F_106A(slot)

The whole routine is a `do { ... } while (!done)` loop: answering `N` to "Save this character?" starts over
(attributes reset to 10, 30 points again). Escape during the attribute stage (only there) leaves to the menu.

### Stage 1: attributes (F_0D18, F_0EF6, ..., F_0ECA)

Title " Character Generation ". "Points left to distribute: " is printed at (32,4) as a 3-digit number
(`D_0DA0`, initially **30**). Six lines (rows 6..11, value printed at column 28 by F_07A8) all start at **10**:
`D_0A46[1..6]` = Strength, Agility, Stamina, Charisma, Wisdom, Intelligence (`D_0A46[0]` is hit points). Help
text: "Move cursor with up and down arrows; increase and decrease attributes with left and right arrows. Press space
bar when finished, or escape to return to the main menu."

Keys (F_39C5(0) values):

| key | code | action |
|---|---|---|
| left arrow | 1 | F_0E32: attribute -1 if it is > 10 (then `D_0DA0++`) |
| right arrow | 2 | F_0E61: attribute +1 if it is < 25 and `D_0DA0 != 0` (then `D_0DA0--`) |
| up arrow | 3 | F_0E9E: cursor to the previous attribute, wraps 1 -> 6 |
| down arrow | 4 | F_0ECA: next attribute, wraps 6 -> 1 |
| Space | 20h | finish if `D_0DA0 == 0`, otherwise beep (F_0D42 = sound 1) |
| Escape | 1Bh | back to the main menu |

Cursor markers are drawn with F_387E/F_3920 at columns 11 and 30 (F_0E07), erased by F_0DC2. So before race and
class bonuses each attribute is 10..25 and exactly 30 points are spent (total 90).

### Stage 2: race (F_07E2), sex (F_0995), class (F_0A91)

Each screen lists the choices `a) .. d)` and reads a key (upper-cased); other keys are ignored. The choice is
printed ("Race: Human" at (14,13), "Sex: Male" at (15,14), "Class: Fighter" at (13,15)). Bonuses are added directly
to the attribute values (and the shown value is refreshed), **not capped at 25**:

| race (value stored) | key | adjustment |
|---|---|---|
| Human (0) | A | Intelligence +5 |
| Elf (1) | B | Agility +5 |
| Dwarf (2) | C | Strength +5 |
| Bobbit (3) | D | Wisdom +10, Strength -5 (can go below 10, to 5) |

| class (value stored) | key | adjustment |
|---|---|---|
| Fighter (0) | A | Strength +10, Agility +10 |
| Cleric (1) | B | Wisdom +10 |
| Wizard (2) | C | Intelligence +10 |
| Thief (3) | D | Agility +10 |

Sex: A Male (0), B Female (1); no game effect is visible in the four programs. The race and class names are
`D_013C[]` (Human, Elf, Dwarf, Bobbit) and `D_0160[]` (Fighter, Cleric, Wizard, Thief). This matches the
manual: humans have "a natural intellect higher than any other race", elves "superior natural agility", dwarves
"an inordinate endowment of natural strength", bobbits are "naturally weak, yet possess a serenity indicative of
profound wisdom"; the fighter becomes "stronger, more agile", the cleric gains "a good deal of wisdom", the
wizard "hones the intellect", the thief is "endowed with exceptional agility". Stored values: race at record
offset 0x10, class 0x12, sex 0x14 (save-format.md).

Class numbers matter later: MONDAIN's spell-failure test treats class 1 (Cleric) as never failing ("at no time
will the proper spell cast by a cleric fail", manual p.3), and OUT tests class 3 (Thief) for chest traps.

### Stage 3: name (F_0C2C)

"Enter thy name: ". Up to 14 characters go into `D_0A30[0..13]` (16-byte field, zero terminated):

* a character `> ' '` is accepted if `len < 14`; a space only if `len != 0` and `len < 14` (no leading space);
  the character is echoed with F_357C and keeps its case (no upper-casing);
* left arrow (code 1) or Backspace (8) with `len != 0`: `len--` and the cursor moves back (the old glyph stays
  on screen until overwritten, `D_0A30` is correct);
* Return (with `len != 0`) terminates the name (`D_0A30[len] = 0`) and ends the stage. The name is then printed
  at (12,4).

### Stage 4: confirm and write (F_0D50)

"Save this character? (Y-N) ": `Y` prints "Yes" and continues, `N` returns 0 (restart the stage-1 loop).

On Yes the record is completed (exact formulas):

| field (GEN name / record offset) | value |
|---|---|
| `D_0A62` (0x32) | `F_3EC4() + Wisdom + class` (random-number seed of this character; Wisdom is the final value after bonuses) |
| `D_0A46[0]` (0x16) hit points | **150** |
| `D_0A54` (0x24) coins | **100** |
| `D_0A56` (0x26) experience | 0 |
| `D_0A58` (0x28) food | **200** |
| `D_0A5A` (0x2A) readied weapon | 1 (Dagger) |
| `D_0A5C` (0x2C) readied spell | 0 (Prayer) |
| `D_0A5E` (0x2E) readied armour | 1 (Leather armor) |
| `D_0A60` (0x30) vehicle | 0 (on foot) |
| `D_0A64`, `D_0A66` (0x34, 0x36) | map position (49, 40) |
| `D_0A68` (0x38) | 1 (sound on) |
| `D_0A6A[1..8]` (0x3C..0x4A) | 0 (nine words from 0x3A; word 0 is left at BSS zero) |
| `D_0A7C[0..3]` (0x4C) | 0 (the four gems) |
| `D_0A84[0..5]` (0x54) | armour owned: -1, 1, 0, 0, 0, 0 (Skin, Leather armor) |
| `D_0A90[0..15]` (0x60) | weapons owned: -1, 2, 0, ... (Hands, Dagger) |
| `D_0AB0[0..10]` (0x80) | spell counts: -1, 0, ... (Prayer) |
| `D_0AC6[0..7]` (0x96) | vehicles owned: -1, 0, ... (Foot) |
| `D_0AD6` (0xA6) | 0 (space kills) |
| `D_0AD8` (0xA8) | -1 |
| `D_0ADA` (0xAA), `D_0ADC` (0xAC, long) | 0, 0 (turn counter) |
| `D_0AE0[2]` (0xB0, 0xB2) | not set by GEN: 0 (shuttle fuel, shield) |
| `D_0AE4..D_0AEA` (0xB4..0xBA) | 10, 1, 49, 40 (first overworld object: the player, tile 10, x, y) |
| `D_0AEC[316]` | all 0 (rest of the 640-byte object table) |

A value of -1 in the owned arrays means "unlimited/always owned" (hands, skin, foot, prayer). The "2" for the
Dagger is exactly what the code stores (a count of two **(uncertain: could be a different encoding; OUT decides)**).
The meanings of the later offsets come from OUT/SPACE/MONDAIN (save-format.md). Initial hit points do not depend
on race, class or Stamina.

Writing (all through `<stdio.h>`):

```
fp = fopen(D_0194[slot], "wb");           /* "player1.u1".."player4.u1" */
fwrite(&D_0A30, 0xB4, 1, fp);             /* 180 bytes: name .. D_0AE2 */
fwrite(&D_0AE4, 0x280, 1, fp);            /* 640 bytes: the object table */
fclose(fp);
D_01AA[slot] = 1; D_0DA4++; copy the 15 name bytes into D_0D64[slot]
```

The two blocks are contiguous in DGROUP (0A30..0AE4 and 0AE4..0D64), so the file is the 820-byte image
`D_0A30 .. D_0D63`. On `fopen` failure: "Error writing out character" at (7,18), "Press RETURN" at (14,19) in
`D_0DAA`, wait for Return; the stage-4 loop then ends (`done = 1`), without a saved file.

## 3. The roster

* F_13B2 (called first in `main`): for `i = 0..3` try `fopen(D_0194[i], "rb")`; on success read the first
  **15 bytes** into `D_0D64[i]` (the name), set `D_01AA[i] = 1`, `D_0DA4++` (count). A file that exists but
  is short still counts.
* F_1455 prints `n) name` for each slot at column 15, rows 6..9 (empty slots print the leftover/zero name, i.e.
  nothing).
* Replacing a character when the roster is full overwrites that slot's file; `D_0DA4` is incremented again (it is
  only compared with 0).

## 4. Known data (DGROUP of GEN)

| address | name | content |
|---|---|---|
| 013C | `D_013C[4]` | pointers to "Human", "Elf", "Dwarf", "Bobbit" |
| 0160 | `D_0160[4]` | "Fighter", "Cleric", "Wizard", "Thief" |
| 0194 | `D_0194[4]` | "player1.u1".."player4.u1" |
| 01A0 | `D_01A0` | "out" |
| 01A4 | `D_01A4` | "C" (video letter handed to OUT) |
| 01A8 | `D_01A8` | "0" (slot digit handed to OUT) |
| 01AA | `D_01AA[4]` | which slots exist |
| 0A30-0D63 | record | the character record (save-format.md) |
| 0D64 | `D_0D64[4][15]` | roster names |
| 0DA0 | `D_0DA0` | points left |
| 0DA4 | `D_0DA4` | saved characters |
| 0DA6.. | colours | see section 1 |
| 0DAE | `D_0DAE` | force-CGA flag |

## 5. Notes and open points

* The only random element is the seed `D_0A62`; the generated character is otherwise fully determined by the
  choices. Seeds matter later: SPACE builds the galaxy from this seed (space-flight.md), so the same character
  always meets the same galaxy.
* Attributes saved are final values after the bonuses; nothing is recomputed from race/class later (as far as
  GEN/SPACE/MONDAIN show).
* The Esc-key "return to main menu" behaviour exists only while distributing points. After that a `N` at the
  confirmation is the only way back (it restarts creation, not the menu).
* The routine F_3AD7 (game screen frame, Origin module OR3AD7, C) is only linked into GEN; F_0D42 is just
  `F_3F62(1)`, the beep.
