# OUT.EXE: how the program uses DGROUP, the character record, the files and its screens

Sources: `src/out/OUTBSS.C` (the BSS in address order, with the real addresses in comments), the extern blocks and initialised data of `OUTA.C`..`OUTC.C`, `src/out/out.syms`, `src/origin/*.ASM`, `docs/MAP.md`.
**Naming caveat** (also overview.md section 4): the C sources call many OUT objects by the *MONDAIN* address (`D_2058`, `D_206E`, `D_0FD8`, `D_1DC8`, `D_0DA6`, ...). Section 2 gives the real address of every BSS object; the first column of each table below is the real
DGROUP address in hex (the DGROUP of OUT.EXE is paragraph 0xD48; offsets are from its start). Initialised tables whose C name differs from their real address are listed with both.

## 1. Command line and chaining

`main` = F_1853(argc, argv): `argv[1]` is a video-mode letter ("C" CGA, "E" EGA, "T" Tandy; only "C" is examined: `D_2048 = (toupper(argv[1][0]) == 'C')` forces CGA), `argv[2]` the character slot `'1'..'4'` (a bad value: F_03E9 calls F_5643 -- text mode 3 -- and `EXIT(1)`),
`argv[3]` non-NULL when OUT is started back by SPACE or MONDAIN (then `inuse.u1` is read instead of the player file). The three pointers are saved in `D_8B88` (8B88), `D_8B8A` (8B8A), `D_8B8C` (8B8C) and handed on:
Board on a shuttle / time machine: `execl("space.exe" / "mondain.exe", the same name, argv[1], argv[2], 0)` (F_1135). GEN starts OUT with `execl("out", "out", mode, slot, 0)`.

## 2. Real addresses of the BSS (3A70..B124)

| real address | C name | size | meaning |
|---|---|---|---|
| 3A70 | `D_0DA6` | 2 | frame/line colour (CGA 3, EGA/Tandy 15) set by F_3F58 |
| 3A72 | `D_0DA8` | 2 | fill/background colour (CGA 2, else 1) |
| 3A74 | `D_2044` | 2 | highlight/warning colour (CGA 1, else 12) |
| 3A76 | `D_2046` | 2 | normal text colour (CGA 3, else 11) |
| **3A78** | `D_2058` | 820 | **the character record** (3A78..3DAC), see section 3 |
| 3DAC | `D_3DAC` | 13104 | `MAP.BIN` (overworld, 4 bits per cell) |
| 70DC | `D_238C` | 6656 | tile graphics (`CgaTiles.Bin` / `EgaTiles.Bin` / `T1KTiles.Bin`): 52 tiles of 16 x 16 pixels (64 bytes each in CGA, 128 in EGA/Tandy). F_A554 rotates tile 0 inside it (water animation) |
| 8ADC | `D_8ADC` | 171 | the 19 x 9 view buffer (index column*9 + row); `D_8B31` (8B31) is its centre cell (the player's glyph) |
| 8B88, 8B8A, 8B8C | `D_8B88`, `D_8B8A`, `D_8B8C` | 2 each | `argv[1]`, `argv[2]`, `argv[3]` (pointers) |
| 8B8E | `D_8B8E` | 2 | overworld food counter (turns since the last food unit; period = `D_36F8[transport]`: foot 2, horse 3, cart 3, raft 4, frigate 5, aircar 6, shuttle 11, time machine 0) |
| 8B90 | `D_8B90` | 2 | number of monsters (type >= 19) |
| 8B92 | `D_8B92` | 2 | number of vehicle objects (type <= 17, includes the player) |
| 8B94, 8B96, 8B98 | `D_8B94/96/98` | 2 each | number of water / grass / woods cells around the place entered last (the transport shop) |
| 8B9A | `D_204A` | 2 | mode: 0 overworld, 1 town, 2 castle, 3 dungeon |
| 8B9C | `D_8B9C` | 2 | the mode of the previous turn |
| 8B9E | `D_8B9E` | 2 | castle: a door was unlocked / princess dead (never read) |
| 8BA0 | `D_8BA0` | 2 | castle: you carry the jester's key |
| 8BA2 | `D_8BA2` | 2 | castle: this is Lord British's (the king is invulnerable) |
| 8BA4 | `D_8BA4` | 2 | castle: items you may still take |
| 8BA6 | `D_8BA6` | 2 | index of the last used monster-table entry (saved as `D_3B22`) |
| 8BA8 | `D_8BA8` | 4 | the turn counter (long; saved as `D_3B24`) |
| 8BAC | `D_2048` | 2 | force-CGA flag |
| 8BAE | `D_8BAE` | 6840 | `TCD.BIN`: 10 maps x 38 x 18 bytes (`D_8BAE[map][x][y]`) |
| A666 | (in `D_9B31`) | 1632 | town/castle tile graphics (`cgatown.bin`/`egatown.bin`/`t1ktown.bin`), 51 tiles of 8 x 8 pixels (16 / 32 bytes) |
| ACC6 | `D_ACC6` | 120 | the 15 entities of the current town/castle: `{type, x, y, hp}` |
| AD3E, AD40 | `D_AD3E`, `D_AD40` | 2 each | player x, y inside towns/castles (0..37, 0..17) and dungeons (0..10) |
| AD42 | `D_AD42` | 2 | current town/castle map number (castles 0/1, towns 2..9) |
| AD44 | `D_204E` | 2 | redraw flag (header/map) |
| AD46 | `D_AD46` | 2 | hostile flag (towns/castles) |
| AD48 | `D_AD48` | 2 | ales drunk in this town |
| AD4A | `D_AD4A` | 2 | castle number 0..7 (place - 33) |
| AD4C | `D_AD4C` | 2 | king number 1..8 |
| AD4E | `D_AD4E` | 2 | 60 or 61: the lock square that the key fits |
| AD50 | `D_AD50` | 2 | dungeon level |
| AD52 | `D_AD52` | 2 | dungeon facing 1 W, 2 E, 3 N, 4 S |
| AD54 | `D_AD54` | 2 | dungeon bonus hit points |
| AD56 | `D_AD56` | 968 | dungeon level: 11 x 11 cells of 4 words `{a,b,c,d}` |
| B11E | `D_B11E` | 6 | dungeon: monster types present (5 flags) |

(The names `D_ADB6`, `D_ADBE`, `D_AE96`, `D_AFD6` in the C code are cells inside `D_AD56`: dungeons.md.)

## 3. The character record at 3A78 (820 bytes)

The record offsets (`address - 3A78`) are the same as in save-format.md; the C names are given for finding the code. All fields are 16-bit words unless noted.

| real | C name | offset | meaning |
|---|---|---|---|
| 3A78 | `D_2058` | 0x00 | name, 16 bytes |
| 3A88 | `D_2068` | 0x10 | race (0 Human, 1 Elf, 2 Dwarf, 3 Bobbit; names at 283C) |
| 3A8A | `D_206A` | 0x12 | class (0 Fighter, 1 Cleric, 2 Wizard, 3 Thief; 2860) |
| 3A8C | `D_206C` | 0x14 | sex (0 male, 1 female) |
| 3A8E | `D_206E[0]` | 0x16 | hit points (capped 9999) |
| 3A90..3A9A | `D_3A90`..`D_3A9A` = `D_206E[1..6]` | 0x18..0x22 | Strength, Agility, Stamina, Charisma, Wisdom, Intelligence |
| 3A9C | `D_207C` | 0x24 | pence |
| 3A9E | `D_207E` | 0x26 | experience |
| 3AA0 | `D_2080` | 0x28 | food |
| 3AA2 | `D_2082` | 0x2A | readied weapon |
| 3AA4 | `D_2084` | 0x2C | readied spell |
| 3AA6 | `D_2086` | 0x2E | readied armour |
| 3AA8 | `D_2088` | 0x30 | transport (0 foot .. 7 time machine) |
| 3AAA | `D_3AAA` | 0x32 | character seed (dungeon walls) |
| 3AAC, 3AAE | `D_3AAC`, `D_3AAE` | 0x34, 0x36 | overworld position |
| 3AB0 | `D_3AB0` | 0x38 | sound flag |
| 3AB2 | `D_3AB2[0..8]` | 0x3A | quest states (quests-and-progress.md) |
| 3AC4, 3AC6, 3AC8, 3ACA | `D_20A4`, `D_3AC6`, `D_3AC8`, `D_3ACA` | 0x4C..0x52 | red, green, blue, white gem counts |
| 3ACC | `D_20AC[0..5]` | 0x54 | armour counts (index 0 = -1 for skin) |
| 3AD8 | `D_20B8[0..15]` | 0x60 | weapon counts (index 0 = -1 for hands; GEN gives 2 daggers) |
| 3AF8 | `D_20D8[0..10]` | 0x80 | spell charges (index 0 = -1) |
| 3B0E | `D_20EE[0..5]`, then `D_3B1A`, `D_3B1C` | 0x96 | transport counts: foot (-1), horse, cart, raft, frigate, aircar at 3B0E..3B19; **shuttle `D_3B1A` (3B1A, 0xA2), time machine `D_3B1C` (3B1C, 0xA4)** |
| 3B1E | `D_20FE` | 0xA6 | enemy vessels destroyed in SPACE |
| 3B20 | `D_3B20` | 0xA8 | last monument visited (-1 at the start) |
| 3B22 | `D_3B22` | 0xAA | saved `D_8BA6` |
| 3B24 | `D_3B24` (long) | 0xAC | saved `D_8BA8` |
| 3B28, 3B2A | `D_3B28`, `D_3B2A` | 0xB0, 0xB2 | shuttle fuel, shield |
| 3B2C | `D_3B2C[0..39]` | 0xB4 | the overworld object table: 40 entries x 16 bytes `{type, terrain, x, y, hp, 3 unused words}`; entry 0 = the player |

Entry 0's `type` is 10 on foot or the vehicle's type (11..18) while riding; F_7D6B (X-it) copies the type into the `terrain` word before resetting it to 10 (purpose not established).

Use of the record by OUT: it is read once at start (F_03E9: `playerN.u1`, or `inuse.u1`) into 3A78 and never re-read; it is written in two places: Save (F_5B0E, `playerN.u1` via `D_0180`) and F_1135 (`inuse.u1`) -- both first copy `D_8BA6`, `D_8BA8` into 3B22/3B24. Every program that
runs next re-reads the record; after SPACE/MONDAIN OUT reloads it from `inuse.u1` (they write it back before returning). All reads/writes use F_3E2F/F_3E85, which retry forever after any DOS error with the prompt of F_125C
("Insert Ultima I diskette" / "Hit space to continue..."; the pointer-based loops are in `OM3E2F.ASM`).

## 4. Initialised data (real addresses)

| address | C name | content |
|---|---|---|
| 0124 | `_stack` | 3072 (Lattice stack size variable) |
| 0126, 0133, 0140; pointer table 014E | `D_014E[3]` | "CgaTiles.Bin", "EgaTiles.Bin", "T1KTiles.Bin" |
| 0154 / pointer 0172 | `D_0172` | "Thy canst not leave it here!" |
| 0174 / pointer 0180 | `D_0180` | "playerx.u1" (character `[6]` replaced by the slot digit at start) |
| 0182 | `D_0182` | flag, set by Esc, never read |
| 0184 | | "inuse.u1" |
| 018D.. 1749 | | message literals of the game code in order of first use (the notes in `src/out/work/*.notes.md` list them) |
| 16CC | `D_16CC[10]` | monster spawn thresholds |
| 1749, 1762, 177B, 1794, 179C | | "Insert Ultima I diskette", "Hit space to continue...", 24 spaces, "map.bin", "tcd.bin" |
| 17A4 | `D_17A4` | the Lattice `rand()` seed (initial value 1) |
| 18B0..18C6 | | "Tandy" and the rectangle-fill variables of the Origin graphics modules (OR34BF, OR3599) |
| 18C8, 191C | `D_18C8`, `D_191C` | x and y coordinates of the 84 places |
| 19EC..1A14 | | tombstone pixel masks (OOAB9D) |
| 2480, 24BC, 24C6, 24C8 | `D_2480`.. | dungeon monster/feature sprite table and scale table (OOAC91) |
| 24DE | `D_24DE` | 1, 10, 100, 1000 (decimal place values for the number input F_B0B5) |
| 24E6, 24E9 | `D_0AB6/9` | "rb", "wb" |
| 24EC | `D_084A` | the 200-entry row address table for CGA/Tandy/EGA screen rows |
| 267C | `D_0C70` | the seed of `F_4069` (the game's random generator) |
| 267E..26C5 | | " Ready ", ") ", "weapon: ", "armour: ", "spell: ", "Ready Weapon,Armour,Spell:", "nothing" |
| 26C6, 26CC, 26D2, 26D8 | `D_0CBA`.. | "Hits:", "Food:", "Exp.:", "Coin:" |
| 26DE | `D_0CD2[16]` | weapon ranges |
| 277E, 2804 | `D_0D72`, `D_0DF8` | weapon names (display / lower case), 16 pointers each |
| 283C, 2860, 28AA | `D_0E30`, `D_0E54`, `D_0E9E` | race (4), class (4), attribute names (7: Hit Points, Strength, Agility, Stamina, Charisma, Wisdom, Intelligence) |
| 2910, 2968, 29AE | `D_0F04`, `D_0F5C`, `D_0FA2` | spell (11), armour (6), vehicle (8) names |
| 29E4 | `D_0FD8` | block of tables: pointers to "Red Gem" "Green Gem" "Blue Gem" "White Gem" at 29E4; 2AB8: 15 monster-name pointers (`D_0FD8[106..]`); 2B56: 11 spell chants (`D_0FD8[185..]`) |
| 2B6C | | the 85 place names (strings) and the pointer table `D_307C` at 307C |
| 3126..3174 | | land names and pointers `D_3174[4]` |
| 317C, 31CC | | the princess names and pointers `D_31CC[8]` |
| 31DC | `D_31DC[10][15]` | entity templates of the 10 town/castle maps (120 bytes per map) |
| 368C, 3698, 36E8, 36F8, 3708 | | `D_368C` reward/damage factors, `D_3698` hit point factors, `D_36E8` monster move chances, `D_36F8` food periods, `D_3708` monument attributes (monsters.md, world-and-map.md) |
| 3718, 37AE | `D_37AE[7]` | the 7 "blocked" messages |
| 37D4 | `D_1DC8` | direction words (1 West 2 East 3 North 4 South), town file names (index 23..25), perspective tables `D_3808`/`D_3814` at 3808/3814 (index 26 and 32) and the 16 + 16 wall positions (index 38 and 54) |
| 3860.. | | dungeon monster names, pointers `D_3930[25]` at 3930 |
| 3962..39E8 | | " More ", "Ztats", " Inventory ", "Player: ", "A Level ", " Male ", " Female ", "Copper pence.", "Silver pieces", "Gold crowns..", "Enemy vessels" |
| 39FA, 3A0A | | ", thou art dead", "Attempting resurrection!" |
| 39E8, 3A24, 3A4C | `D_09EC`, `D_09FE`, `D_0A26` | EGA/Tandy palette data, the 4 cursor frames (8 bytes each), cursor variables |

## 5. The screens and fixed text positions

Text coordinates are (column, row) of the 40 x 25 text screen (8 x 8 pixel cells); F_352C(col,row) positions the BIOS cursor.

* **Status panel** (F_45A3 at start; F_4437/F_44A2/F_450D/F_4558 update): labels "Hits:", "Food:", "Exp.:", "Coin:" at (31,21)..(31,24); values at (36,21)..(36,24), 4 digits right-aligned; hit points and food below 100 in `D_2044`, all values capped at 9999;
  the cursor is then put at (1,24), where the message line starts. Messages scroll in columns 0..29, rows 21..24 (F_3EED); F_3B46 prints a line and scrolls.
* The command prompt: F_387E(0,24) draws the arrow ">" at the start of row 24.
* **Map window**: x 8..311, y 8..151 (19 x 9 overworld tiles of 16 x 16, or 38 x 18 town tiles of 8 x 8, or the 304 x 144 dungeon view).
* **Frames**: F_21C2 (x 31..287, y 23..127 cleared, outline from F_2168 in the given colour) is the shop/sign/quest box; `F_21E7(col,row,text)` prints inside it. F_1D35 draws the bar at the top of the screen (x 127..199, y 0..6, `D_0DA8`; line y 7 in `D_0DA6`) and sets `D_204E`.
* **Ztats** (F_478E): in a dungeon F_3C46 first clears the top bar; the window is cleared; "Ztats" is printed between the arrows at (13,0)/(25,0); "Player: <name>" at (2,2); "A Level N Male|Female <race> <class>" at (2,3) (level = `F_40E9()`);
  then the attribute list (F_46E5, items 0..6 of `D_206E` with names `D_0E9E`): lines of name, dots up to 13 characters, value as 4 digits padded with dots, in two columns of 13 rows (column 2 and column 21, rows 5..17; F_468F maps the line number to the position);
  then pence: "Copper pence." with `pence % 10` (only if non-zero), "Silver pieces" with `(pence % 100) / 10` (if >= 10 of the remainder), "Gold crowns.." with `pence / 100` (if >= 100); "Enemy vessels" with `D_20FE` if non-zero (the Ztats screen counts 100 pence per gold crown, whereas the pence message F_0761 uses the thousands digit);
  then the lists (armour 1..5 `D_20AC`/`D_0F5C` with the readied armour highlighted in `D_2044`, weapons 1..15, spells 1..10, transport 1..7 with `D_20EE`/`D_0FA2`, then the four gems `D_20A4` array with their names), each item as "name" + dots + count. When more than 25 lines are needed F_4645
  shows " More " between arrows and waits (F_3AE2: "Press Space to continue: ", Space or Return). At the end the bottom frame is redrawn (F_35CA, F_39FC).
* **Death** (F_C47C): tombstone F_AB9D; text at the message line; see combat.md.
* **Header of dungeons**: " Level  N" at row 0 and the compass at row 19 (dungeons.md).

## 6. Function index (by address)

| range | functions |
|---|---|
| 03E9-0476 | F_03E9 read the record, F_0441 read the tile graphics, F_0476 sound if on |
| 048A-0761 | F_048A message+beep, F_04AF can step, F_066F movement dispatch, F_0742 monster name, F_0761 pence message |
| 08EE-1351 | F_08EE kill quests, F_0953 dungeon hit, F_0BAA/F_0C52 dungeon shot, F_0CF3 projectile, F_0FEF/F_1072/F_10C2 attack, F_1135 exec, F_118D Board, F_12F2 spell damage, F_1351 chest/coffin pence |
| 13A8-1D35 | F_13A8 dungeon spells, F_1769 Prayer, F_18FC combat spell, F_19FF Cast, F_1AE4/F_1BC0 donations, F_1CF6/F_1D35 frames |
| 1D71-204D | weapon/armour/pence drop |
| 20FC-2A73 | F_20FC entity load, F_2141 key wait, F_2168-F_21E7 frames/text, F_2203 random -1/0/1, F_2231/F_22B8 map redraw, F_22F3-F_249D entity lookups, F_252C-F_294E entity AI, F_2A73 town turn |
| 2B6D-2E22 | F_2B6D town attack, F_2E22 town step |
| 2F3B-32F8 | shop signs |
| 33B0-3F7F | dungeon wall/feature drawing |
| 4013-4B73 | dungeon visibility, view, generation, movement |
| 4BB0-53DA | dungeon start, princess reward F_4BD6, town/castle entry F_4DBF/F_4E63, signs F_4EA2, quest flag F_5085, monuments F_50C6, Enter F_52AB, Fire F_53DA |
| 5469-5EC9 | king's permission, Get, Inform, ladders, Noise, coffins, Save/Quit, stealing, shop result boxes, gossip pages |
| 616F-680C | pub, price formulas, weapon shop |
| 6823-7B63 | armour/magic/food/transport shops, Transact, king, quests, chests |
| 7C31-8CEB | Unlock, X-it, Pass, "Huh?", food, spawning, monsters' turn, dungeon monsters, random hop F_8C20 |
| 8D1E, 8DAE | F_125C diskette prompt, F_1853 main |
| 9F69-C6D4 | Origin library: see video-sound-input.md |
