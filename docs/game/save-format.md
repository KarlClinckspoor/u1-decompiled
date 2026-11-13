# Character record: `player1.u1`..`player4.u1` and `inuse.u1`

All character data of Ultima I live in one block of **820 bytes (0x334)**, stored as a plain binary file with no
header, no checksum and no version field. The same format is used by

* `player1.u1`..`player4.u1`: the four roster slots. Created by GEN (new character); rewritten by OUT's Save
  command (F_5B0E: "saving game...", "saved.", after copying the turn counter into the record) when the player
  saves (OUT.C); read by OUT at start, and the first 15 bytes (the name) by GEN's roster scan.
* `inuse.u1`: the "current" character handed between programs. OUT writes it before running SPACE.EXE or
  MONDAIN.EXE (F_1135); SPACE (F_0EBD) and MONDAIN (F_12EC) read it at start and write it when they leave; OUT reads it
  at start when started with a third argument (overview.md).

Integers are 16-bit little endian words (`int` in Lattice small model, 8086). Only GEN, SPACE and MONDAIN are
analysed here; OUT-only facts come from `src/out/OUT.C` and are marked "(OUT)". Offsets are into the file/record.

## 1. Where the record lives in memory

| program | address of offset 0 | how it is read/written |
|---|---|---|
| GEN | `D_0A30` (BSS 0A30..0D64) | `fwrite(&D_0A30, 0xB4, 1)` then `fwrite(&D_0AE4, 0x280, 1)` to `playerN.u1` |
| MONDAIN | `D_2058` (2058..238C) | F_3E2F/F_3E85 on `inuse.u1`, 0x334 bytes |
| SPACE | 5ADC (named `D_2058`) | same |
| OUT | 3A78 (named `D_2058`) | same, `playerx.u1` and `inuse.u1` |

GEN's name for offset `o` is `D_0A30 + o`; the SPACE/MONDAIN/OUT sources use `D_2058 + o` (the name is the MONDAIN
address, not the real address, in SPACE and OUT). Both name sets are given below.

The file is written in two fwrite calls in GEN (180 + 640 bytes) but the two buffers are adjacent, so the file is the
simple concatenation.

## 2. Field table

Source column: G = written by GEN (value for a new character), S = used by SPACE, M = used by MONDAIN, O = used by
OUT (OUT.C).

| offset | size | GEN name | name in SPACE/MONDAIN/OUT | meaning | new character | seen in |
|---|---|---|---|---|---|---|
| 0x00 | 16 | `D_0A30[16]` | `D_2058` | character name, zero terminated, at most 14 characters, case as typed | | G O |
| 0x10 | 2 | `D_0A40` | `D_2068` | race: 0 Human, 1 Elf, 2 Dwarf, 3 Bobbit | choice | G |
| 0x12 | 2 | `D_0A42` | `D_206A` | class: 0 Fighter, 1 Cleric, 2 Wizard, 3 Thief | choice | G M O |
| 0x14 | 2 | `D_0A44` | `D_206C` | sex: 0 male, 1 female | choice | G |
| 0x16 | 2 | `D_0A46[0]` | `D_206E` | hit points | 150 | G S M O |
| 0x18 | 2 | `D_0A46[1]` | `D_2070` | Strength | 10..25 + bonuses | G M O |
| 0x1A | 2 | `D_0A46[2]` | `D_2072` | Agility | same | G M O |
| 0x1C | 2 | `D_0A46[3]` | `D_2074` | Stamina | same | G M O |
| 0x1E | 2 | `D_0A46[4]` | `D_2076` | Charisma | same | G |
| 0x20 | 2 | `D_0A46[5]` | `D_2078` | Wisdom | same | G M |
| 0x22 | 2 | `D_0A46[6]` | `D_207A` | Intelligence | same | G M |
| 0x24 | 2 | `D_0A54` | `D_207C` | coins ("Coin:" in the status line, pence) | 100 | G S O |
| 0x26 | 2 | `D_0A56` | `D_207E` | experience points ("Exp.:"); level = exp/1000 + 1 (F_40E9) | 0 | G S O |
| 0x28 | 2 | `D_0A58` | `D_2080` | food ("Food:") | 200 | G S M O |
| 0x2A | 2 | `D_0A5A` | `D_2082` | readied weapon (index 0..15, table below) | 1 (Dagger) | G M O |
| 0x2C | 2 | `D_0A5C` | `D_2084` | readied spell (index 0..10) | 0 (Prayer) | G M |
| 0x2E | 2 | `D_0A5E` | `D_2086` | readied armour (index 0..5) | 1 (Leather armor) | G S M O |
| 0x30 | 2 | `D_0A60` | `D_2088` | vehicle in use (index 0..7 of the vehicle table; 0 = on foot) | 0 | G O |
| 0x32 | 2 | `D_0A62` | `D_208A` | random seed of the character: `clock + Wisdom + class` at creation | random | G S M O |
| 0x34 | 2 | `D_0A64` | `D_208C` | overworld position x | 49 | G O |
| 0x36 | 2 | `D_0A66` | `D_208E` | overworld position y | 40 | G O |
| 0x38 | 2 | `D_0A68` | `D_2090` | sound ("Noise") on/off | 1 | G S M O |
| 0x3A | 18 | `D_0A6A[9]` | `D_2092..` | nine words; GEN clears words 1..8, word 0 stays BSS-zero. OUT indexes it as `D_3AB2[]` (3AB2 = 3A78+0x3A) with values -1/0/1 (per-place state flags **(inferred)**) | 0 | G O |
| 0x4C | 8 | `D_0A7C[4]` | `D_20A4`, `D_3AC6`, `D_3AC8`, `D_3ACA` | the four gems (counts); order Red, Green, Blue, White **(inferred from the manual and OUT)** | 0 | G O |
| 0x54 | 12 | `D_0A84[6]` | `D_20AC[]` | armour owned (counts) | -1, 1, 0, 0, 0, 0 | G O |
| 0x60 | 32 | `D_0A90[16]` | `D_20B8[]` | weapons owned (counts) | -1, 2, 0 ... | G O |
| 0x80 | 22 | `D_0AB0[11]` | `D_20D8[]` | spell charges | -1, 0, ... | G M O |
| 0x96 | 16 | `D_0AC6[8]` | `D_20EE[]` | vehicles owned (counts) | -1, 0, ... | G O |
| 0xA6 | 2 | `D_0AD6` | `D_20FE` | enemies destroyed in space ("Space Ace" at 20) | 0 | G S O |
| 0xA8 | 2 | `D_0AD8` | (OUT `D_3B20`) | last shrine/sign visited (a visit twice in a row gives no gain) **(inferred)** | -1 | G O |
| 0xAA | 2 | `D_0ADA` | (OUT `D_3B22`) | saved copy of OUT's `D_8BA6` (copied in F_1135 and F_5B0E) | 0 | G O |
| 0xAC | 4 | `D_0ADC` (long) | (OUT `D_3B24`) | saved copy of OUT's long counter `D_8BA8` (incremented once per command: the game clock) | 0 | G O |
| 0xB0 | 2 | `D_0AE0[0]` | SPACE `D_5B8C`, OUT `D_3B28` | **shuttle fuel** (set to 1000 when OUT sells a shuttle; SPACE reads it at launch and writes it at landing) | 0 | S O |
| 0xB2 | 2 | `D_0AE0[1]` | SPACE `D_5B8E`, OUT `D_3B2A` | **shuttle shield** (same) | 0 | S O |
| 0xB4 | 640 | `D_0AE4`, `D_0AE6`, `D_0AE8`, `D_0AEA`, `D_0AEC[316]` | (OUT `D_3B2C[40]`) | 40 records of 16 bytes (8 words) of the overworld object table; record 0 is the player (OUT: `D_3B2C[0].type` is the tile drawn, 10 on foot and 11..18 on a vehicle, `.x`, `.y`); other records are monsters/objects and are zero in a new file | rec 0: type 10, terrain 1, x 49, y 40 | G O |

Total 0xB4 + 0x280 = 0x334. A new file therefore has nothing after the first record's four words except zeros.
GEN's `D_0AE8`/`D_0AEA` repeat position (49,40) of offsets 0x34/0x36; OUT's `D_3AAC/D_3AAE` are the position copy
in the first block (0x34/0x36).

Name conventions: attribute order Strength, Agility, Stamina, Charisma, Wisdom, Intelligence is the order of the
GEN screen (rows 6..11) and of OUT's/MONDAIN's formulas below. OUT's source comments call 0x22 "charisma" in its
price formulas (`(255 - D_3A9A)...`); by GEN's order 0x22 is Intelligence and 0x1E (OUT `D_3A96`) Charisma. Treat
that as an open naming question when OUT is integrated.

### Index tables

The name tables are the Origin data in MONDAIN/OUT (`OM4437`):

* Weapons (16, `D_0DF8`/long names `D_0D72`): 0 Hands, 1 Dagger, 2 Mace, 3 Axe, 4 Rope & Spikes, 5 Sword, 6 Great
  Sword, 7 Bow & Arrows, 8 Amulet, 9 Wand, 10 Staff, 11 Triangle, 12 Pistol, 13 Light Sword, 14 Phazor, 15 Blaster.
  Ranges `D_0CD2`: 1 1 1 1 0 1 1 3 0 0 0 1 3 1 3 3 (final-battle.md).
* Armour (6, `D_0F5C`): 0 Skin, 1 Leather armor, 2 Chain mail, 3 Plate mail, 4 Vacuum suit, 5 Reflect suit.
* Spells (11, `D_0F04`): 0 Prayer, 1 Open, 2 Unlock, 3 Magic Missile, 4 Steal, 5 Ladder Down, 6 Ladder Up, 7 Blink,
  8 Create, 9 Destroy, 10 Kill. (The manual lists the same ten besides Prayer.)
* Vehicles (8, `D_0FA2`): 0 Foot, 1 Horse, 2 Cart, 3 Raft, 4 Frigate, 5 Aircar, 6 Shuttle (offset 0xA2), 7 Time
  Machine (offset 0xA4; OUT gives one more, up to 9, with the princess' reward).
* Attributes (7, `D_0E9E`): Hit Points, Strength, Agility, Stamina, Charisma, Wisdom, Intelligence.
* Races `D_0E30`, classes `D_0E54` as in character-creation.md.

A count of -1 in the "owned" tables is the "always have" marker (Skin armour, Hands, Prayer, Foot). Other entries
count items; GEN's starting kit is a Dagger (count 2) and Leather armor (1), readied.

## 3. Who touches what

| program | reads | writes |
|---|---|---|
| GEN | file names (first 15 bytes of each player file) | the whole record (initial values, section 2) |
| SPACE | sound flag (0x38), seed (0x32), armour (0x2E), coins (0x24), shuttle fuel/shield (0xB0/0xB2), HP (0x16, tested for 0) | coins -500 (docking), experience +100 per hit (0x26), kills +1 (0xA6), shuttle fuel/shield on landing (0xB0, 0xB2), HP and food = 0 on death (0x16, 0x28), sound flag (0x38) |
| MONDAIN | class (0x12), the six attributes (0x18..0x22), weapon (0x2A), spell (0x2C), armour (0x2E), spell charges (0x80..), seed (0x32), HP, food, sound flag | HP (damage), attributes (mind blaster), spell charges (-1 each cast), HP/food = 0 on death, sound flag |

Neither SPACE nor MONDAIN touches the name, race, sex, position, inventory, gems, vehicles, the OUT counters or
the object table; they just carry them through (the record is read whole and written whole).

## 4. Reading and writing behaviour

* GEN: `fopen(name,"wb")`, two `fwrite`s, `fclose`; an `fopen` failure shows "Error writing out character /
  Press RETURN". Roster scan: `fopen(name,"rb")`, one `fread` of 15 bytes (name only), existence = the slot is used.
* All others use F_3E2F (`fopen "rb"`, `fread(buf, 820, 1)`, `fclose`) and F_3E85 (`"wb"`, `fwrite`). Any error
  (`_OSERR != 0` after open, read or write) shows "Insert Ultima I diskette / Hit space to continue..." and retries
  indefinitely, so a missing `inuse.u1` loops until the file appears.
* No endianness/versioning issue: all words are plain DOS `int`; the long at 0xAC is low word first.
* Death is expressed in the record itself: hit points 0 and food 0. OUT tests `HP <= 0 || food <= 0` at start
  (F_1853 in OUT.C) and runs its death/resurrection path (OOC47C: ", thou art dead").

## 5. Open points

* Meaning of offsets 0x3A..0x4B (nine words), 0xA8, and the 0xB4 table is taken from OUT.C and not verified in
  the three programs documented here.
* Whether Dagger = 2 in GEN's initial inventory is a count of two or another encoding (OUT decides).
* The gem order (0x4C..0x53) and which of OUT's four variables is which colour.
