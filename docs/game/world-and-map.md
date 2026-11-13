# OUT.EXE: the world, MAP.BIN, tiles, towns, castles, signs and places

Source: `src/out/OUTA.C`, `OUTB.C`, `OUTC.C`, `OUTBSS.C`, `out.syms`, the Origin modules in `src/origin/OO*.ASM`
and the data read from `gamedata/OUT.EXE` and `gamedata/MAP.BIN`, `gamedata/TCD.BIN`. Function names are
code offsets (`F_xxxx`). **Address convention**: the C files keep old names for some data (`D_0DA6`, `D_2058`,
`D_0FD8`, ... are the names the same object has in MONDAIN, see overview.md section 4). This document gives the
**real DGROUP address of OUT.EXE** in hex whenever it matters; out-state.md has the full table name -> address.

Terms used throughout: *mode* = `D_204A` (DGROUP 8B9A): 0 overworld, 1 town, 2 castle, 3 dungeon (4 is tested in
two places, F_19FF and F_7CF2, but nothing in OUT sets it).

## 1. The overworld map (MAP.BIN)

* `MAP.BIN` is 13104 bytes = 168 x 156 cells at **4 bits per cell**, two cells per byte, row-major
  (84 bytes per row). It is read once by main (F_1853, `F_3E2F("map.bin", D_3DAC, 13104)`) into DGROUP 3DAC..70DC.
* Cell lookup: F_A37D (called by F_A363 *terrain at (x,y)*): offset = `y*84 + (x>>1)`; **even x = high nibble, odd x =
  low nibble**. The raw nibble is then converted to a *terrain code*: raw 0..4 unchanged, raw 5 -> 6, 6 -> 7,
  7 -> 9 (computed as `if (v > 4) { v++; if (v == 8) v++; }`).

The raw nibbles in the file (counts over the 26208 cells, checked against the file):

| raw | terrain code | count | meaning | evidence |
|---|---|---|---|---|
| 0 | 0 | 16074 | water (sea) | "You can't walk on water!" for t == 0; sea monsters spawn on 0 |
| 1 | 1 | 6324 | grass / plains | land monsters spawn on 1; "Rafts/Frigates like water!" for t != 0 |
| 2 | 2 | 2528 | woods | "Aircars can't pass woods!" for t == 2 |
| 3 | 3 | 1198 | mountains | "Mountains are impassable!" for t == 3 (every vehicle) |
| 4 | 4 | 8 | castle (8 places 33-40) | every castle place lies on raw 4 |
| 5 | 6 | 8 | pillar / sign post / grave (places 41-48) | all 8 lie on raw 5 |
| 6 | 7 | 32 | town (places 1-32) | all 32 lie on raw 6 |
| 7 | 9 | 36 | dungeon entrance (places 49-84) | all 36 lie on raw 7 |

Raw 8..15 never occur. Terrain code 5 and 8 never come out of F_A363: they are the second animation frames of
castle (4 -> 5) and town (7 -> 8) used by the idle animation F_AF57. Land monsters may step on terrain 4..9, i.e. on
places (F_80A2: `res = k >= 4 && k <= 9`).

### Size, wrapping and the four continents

* The player's world position is `D_3AAC` (x, DGROUP 3AAC) and `D_3AAE` (y, 3AAE); x is 0..167, y is 0..155.
  (Both are part of the saved record, offsets 0x34 and 0x36.)
* The map is four 84 x 78 quarters. **F_40FE(x,y)** returns the quarter: `(y > 77) * 2 + (x > 83)`:
  0 NW, 1 NE, 2 SW, 3 SE. Lord British's land is the NW quarter; the names (`D_3174` at 3174, printed by Inform
  with "You are in the lands") are 0 "of Lord British", 1 "of the Feudal Lords", 2 "of the Dark Unknown",
  3 "of Danger and Despair". Each quarter has 8 towns, 2 castles, 2 shrines/signs and 9 dungeons (by coordinates,
  table below). Crossing a quarter boundary on foot makes `F_066F` call F_86E1(old quarter), which deletes all
  monsters of the quarter that was left.
* **World wrap-around**: moving north from y = 0 goes to y = 155 (F_B64D) and south from 155 to 0 (F_B6F0), x
  unchanged. Moving east from x = 167 (F_B79A) sets x = 0 **and shifts y by 77** (`if (y >= 77) y -= 77 else
  y += 77`); moving west from x = 0 (F_B852) sets x = 167 and shifts y the same way. So walking round the world
  east/west lands you in the other half; two laps bring you back (the sign "GO EAST TO GO EAST!" alludes to this). The
  *view* (F_C2EC / F_C31C, which turn screen column/row offsets into world coordinates) wraps plainly modulo
  168 / 156 without the shift, so near the seam the picture can disagree with where you really are.
* The visible area is **19 x 9 cells** (tiles of 16 x 16 pixels) with the player at column 9, row 4; the screen
  copy of the view is the byte array `D_8ADC` (DGROUP 8ADC..8B87, index = column*9 + row). F_C3CA refills it from the
  map (F_C2AF(x,y,0) = terrain *or* the monster/vehicle standing there), F_B665 / F_B70F / F_B7CD / F_B884 scroll it
  one step north / south / east / west and fetch only the new row/column from the plain terrain (F_A363).

### Objects on the overworld: the monster table

Every moving thing is an entry of the 16-byte-per-entry table `D_3B2C` (3B2C..3DAC, 40 entries; see out-state.md):
entry 0 is the player, the others are monsters, horses, carts, boats, aircars, shuttles and time machines. F_B545(x,y,
start) searches entries from *start* for one at (x,y); `F_C2AF(x,y,start)` returns that entry's type, or the terrain
code if none. `start` = 1 skips the player's own entry, 0 includes it (the view uses 0, so the player is drawn from
his entry). Entry types (also tile numbers of TILES.BIN):

| type | what |
|---|---|
| 10 | the player on foot (`D_3B2C[0].type`) |
| 11 horse, 12 cart, 13 raft, 14 frigate (15 is its second animation frame), 16 aircar, 17 shuttle, 18 time machine | unoccupied vehicles standing on the map, and the player's glyph while he rides one. Bought vehicles are placed on a neighbouring tile (F_6F58); X-it puts the vehicle back (F_7D6B). |
| 19, 21, 23 ... 47 (odd numbers; the even number after each is the second frame) | the 15 overworld monsters, see monsters.md |
| 49 (0x31) | magic-missile bolt / "Monster removed!" flash tile; 50 (0x32) projectile tile for arrows and monster attacks |

`D_8BA6` (8BA6) is the index of the last used entry (limit 39, F_B502 refuses to add beyond it), `D_8B90` the number
of monsters (type >= 19; recomputed at start by F_B5E1), `D_8B92` the number of entries of type <= 17 (player, horses ...
shuttle; F_B617). Entries are removed by F_B58E (shifting the later ones down).

### Tile animation (F_AF57, called every idle poll of the command loop)

F_AF57(cnt) redraws the whole 19 x 9 view from `D_8ADC`: water (0) is redrawn after F_A554 has rotated the tile 0
pixel data in the graphics buffer (shimmering water; inferred from the code, which moves 8/4 words of the first
tile around inside the buffer at 70DC); castle (4) flips to 5 and town (7) flips to 8 on some counter values
(`cnt % 6 == 0` / `cnt % 3 == 0` for 4, every call for 7); frigate (14) and pirate ship (25) alternate with the next
tile each call; every monster tile (19..47) is drawn as tile or tile+1 at random each call.

## 2. Places (F_A3BA)

`F_A3BA(x, y)` returns the place number 1..84 for the exact world coordinate (x, y) or 0. It scans two 84-byte
tables in DGROUP (x low bytes at `D_18C8` = DGROUP 18C8, y at `D_191C` = 191C; they sit with the data of the Origin module OOA363). Everything that depends on
place numbers uses this: Enter (F_52AB), Inform (F_564B), the transport shop. Names are the pointer table
`D_307C` (DGROUP 307C, 85 entries, entry 0 is "?"). The names are printed exactly as in the data, including the
misspelling "Britian".

* 1..32: towns ("The city of X"); 33..40: castles; 41..48: pillars, towers, sign posts and the grave; 49..84: dungeons.
* A town's layout and shops depend only on `(number - 1) % 8`: towns 1, 9, 17, 25 are the same map (below).
* The place a quest asks for is one of 42, 44, 46, 48 (see quests-and-progress.md).

| # | name | x | y | quarter |
|---|---|---|---|---|
| 1 | Britian | 39 | 39 | NW |
| 2 | Moon | 66 | 41 | NW |
| 3 | Fawn | 25 | 61 | NW |
| 4 | Paws | 46 | 28 | NW |
| 5 | Montor | 52 | 63 | NW |
| 6 | Yew | 18 | 34 | NW |
| 7 | Tune | 70 | 63 | NW |
| 8 | Grey | 64 | 22 | NW |
| 9 | Arnold | 126 | 36 | NE |
| 10 | Linda | 128 | 63 | NE |
| 11 | Helen | 148 | 22 | NE |
| 12 | Owen | 115 | 43 | NE |
| 13 | John | 150 | 49 | NE |
| 14 | Gerry | 121 | 15 | NE |
| 15 | Wolf | 150 | 67 | NE |
| 16 | The Snake | 109 | 61 | NE |
| 17 | Nassau | 42 | 119 | SW |
| 18 | Clear Lagoon | 44 | 92 | SW |
| 19 | Stout | 64 | 133 | SW |
| 20 | Gauntlet | 31 | 112 | SW |
| 21 | Imagination | 66 | 106 | SW |
| 22 | Ponder | 37 | 140 | SW |
| 23 | Wealth | 66 | 88 | SW |
| 24 | Poor | 25 | 94 | SW |
| 25 | Gorlab | 128 | 117 | SE |
| 26 | Dextron | 101 | 119 | SE |
| 27 | Magic | 142 | 139 | SE |
| 28 | Wheeler | 121 | 106 | SE |
| 29 | Bulldozer | 115 | 141 | SE |
| 30 | The Brother | 149 | 112 | SE |
| 31 | Turtle | 97 | 141 | SE |
| 32 | Lost Friends | 103 | 100 | SE |
| 33 | The Castle of Lord British | 40 | 38 | NW |
| 34 | The Castle of the Lost King | 32 | 27 | NW |
| 35 | The Castle Barataria | 125 | 37 | NE |
| 36 | The Castle Rondorin | 114 | 29 | NE |
| 37 | The Castle of Olympus | 41 | 118 | SW |
| 38 | The Black Dragon's Castle | 30 | 126 | SW |
| 39 | The White Dragon's Castle | 127 | 116 | SE |
| 40 | The Castle of Shamino | 135 | 105 | SE |
| 41 | The Pillars of Protection | 36 | 9 | NW |
| 42 | The Tower of Knowledge | 69 | 10 | NW |
| 43 | The Pillars of the Argonauts | 96 | 33 | NE |
| 44 | The Pillar of Ozymandias | 97 | 66 | NE |
| 45 | The Sign Post | 13 | 89 | SW |
| 46 | The Southern Sign Post | 12 | 122 | SW |
| 47 | The Eastern Sign Post | 131 | 87 | SE |
| 48 | The Grave of the Lost Soul | 98 | 88 | SE |
| 49 | The Unholy Hole | 48 | 11 | NW |
| 50 | The Dungeon of Perinia | 18 | 13 | NW |
| 51 | The Dungeon of Montor | 53 | 22 | NW |
| 52 | The Mines of Mt. Drash | 59 | 29 | NW |
| 53 | Mondain's Gate to Hell | 29 | 37 | NW |
| 54 | The Lost Caverns | 13 | 43 | NW |
| 55 | The Dungeon of Doubt | 62 | 49 | NW |
| 56 | The Mines of Mt. Drash II | 39 | 60 | NW |
| 57 | Death's Awakening | 38 | 68 | NW |
| 58 | The Savage Place | 130 | 10 | NE |
| 59 | Scorpion Hole | 100 | 15 | NE |
| 60 | Advari's Hole | 124 | 26 | NE |
| 61 | The Dead Warrior's Fight | 155 | 35 | NE |
| 62 | The Horror of the Harpies | 147 | 36 | NE |
| 63 | The Labyrinth | 98 | 45 | NE |
| 64 | Where Hercules Died | 109 | 50 | NE |
| 65 | The Horror of the Harpies II | 116 | 56 | NE |
| 66 | The Gorgon Hole | 136 | 59 | NE |
| 67 | The Tramp of Doom | 52 | 96 | SW |
| 68 | The Viper's Pit | 32 | 99 | SW |
| 69 | The Long Death | 25 | 105 | SW |
| 70 | The End... | 14 | 110 | SW |
| 71 | The Viper's Pit II | 63 | 119 | SW |
| 72 | The Slow Death | 71 | 120 | SW |
| 73 | The Guild of Death | 40 | 129 | SW |
| 74 | The Metal Twister | 16 | 140 | SW |
| 75 | The Troll's Hole | 46 | 145 | SW |
| 76 | The Skull Smasher | 119 | 89 | SE |
| 77 | The Spine Breaker | 149 | 91 | SE |
| 78 | The Dungeon of Doom | 114 | 100 | SE |
| 79 | The Dead Cat's Life | 108 | 107 | SE |
| 80 | The Morbid Adventure | 138 | 115 | SE |
| 81 | Free Death Hole | 154 | 121 | SE |
| 82 | Dead Man's Walk | 105 | 127 | SE |
| 83 | The Dead Cat's Life II | 128 | 138 | SE |
| 84 | The Hole to Hades | 129 | 146 | SE |
Counts match the raw map: 32 towns, 8 castles, 8 shrines/signs, 36 dungeon entrances (places 49-84 are all
dungeons; *which* dungeon you are in is only used for its name, the generator ignores the number, see dungeons.md).

## 3. Entering places (the Enter command, F_52AB)

Enter works only on the overworld (mode 0) on a place (F_A3BA != 0); otherwise it prints "Enter?" and beeps.
It then
1. prints "ing..." (the command text was "Enter"), clears `D_AD46` (hostile flag), `D_8B94`, `D_8B96`, `D_8B98` and
   sets the header-redraw flag `D_204E = 1`;
2. counts the terrain of the 3 x 3 cells around the player (F_C2AF(x+i, y+j, 1) over i, j = -1..1, *including the
   place's own cell and any monster standing there as its type*): `D_8B94` = number of water (0) cells, `D_8B96` =
   grass (1), `D_8B98` = woods (2). The transport shop later uses these to decide what it can sell and where the
   purchase may appear;
3. `D_8BA2 = (place == 33)` (Lord British's castle: the king cannot be hurt);
4. prints "The city of " for places < 33 and then the place name;
5. dispatches: 1..32 -> F_4E63 (town), 33..40 -> F_4DBF (castle), 41..48 -> F_50C6 (pillar/sign), 49..84 -> F_4BB0
   (dungeon).

Leaving a town/castle: stepping out of the 38 x 18 map in any direction (F_2E22) sets the mode back to 0; the player
reappears on the overworld at the same world coordinates (they never change inside towns). A castle exit with a
princess (entity 22) in one of the neighbouring cells triggers the rescue reward, quests-and-progress.md.

## 4. Towns and castles (TCD.BIN)

* `TCD.BIN` = 6840 bytes = **10 maps of 38 x 18 cells, one byte per cell, 684 bytes per map** (F_1853 reads it into
  `D_8BAE` = DGROUP 8BAE..A666). Cell address = `D_8BAE[map][x][y]` = `8BAE + map*684 + x*18 + y` (x = column 0..37,
  y = row 0..17). Maps 0 and 1 are the two **castle layouts**, maps 2..9 the **eight town layouts**.
* Castle `c` (33..40): map `(c-1) % 2` (castles 33, 35, 37, 39 use map 0, castles 34, 36, 38, 40 use map 1); the player
  starts at (0, 9) (F_4DBF). Town `c` (1..32): map `(c-1) % 8 + 2` (so 1, 9, 17, 25 share map 2); the player
  starts at (19, 17), the middle of the bottom edge (F_4E63).
* Tile byte values (town tile graphics = the TOWN file, F_B34F draws an 8 x 8 tile at column x, row y of the
  map window):
  * values 0..0x32 (0..50) are **graphics tiles** (51 of them); F_2231 draws the whole map from them;
  * values >= 0x33 (51 and up) are **special floor squares**: drawn as plain floor (tile 1), walkable, with an
    effect when the player stands on them (below);
  * of the graphics tiles only tile **1** is walkable (F_23E7: `t == 1 || t >= 0x33` -> free); everything else
    (walls = tile 0x17 in most towns, water = 0 in castle 0, trees, furniture ...) blocks.
  * 0x3F (63) is returned as 63 by F_23E7 and is treated as walkable by F_2E22 (it occurs 2..4 times per town,
    never in the castles; purpose not established, probably a doorway/exit marker).
* Entity sprites share the tile set: 0x11 (17) guard, 0x12 (18) the player, 0x13 (19) bard (town) / jester (castle),
  0x14 (20) king, 0x15 (21) merchant, 0x16 (22) princess, 0x32 (50) wench (male player) or lecher (female player).
  The per-map entity list is below.
* Doors of the castles: the code sets tile 11 at (35, y) and (31, y) where y = 14 for map 0 and y = 4 for map 1
  (F_4DBF, each time you enter; these are the locked doors); tiles 60 and 61 (single cells) in the castle maps are
  the *lock squares* (see towns-shops-castles.md).

### Special floor squares

| byte | where | effect |
|---|---|---|
| 0x33 (51) | castles 0/1 and towns, in rings and rows | donation squares of the Drop command (towns: pence -> hit points and sometimes a spell; castles: stat increase) |
| 0x34 (52), 0x35 (53) | castle maps | donation squares: random weapon (>= 10 pence) / food (pence x 3) |
| 54, 55 | armour shop counters (towns) / 55 = armour storeroom (castles) | town: Transact; castle: Get / Steal |
| 56, 57 | food shop counters / 57 = food storeroom (castles) | idem |
| 58, 59 | weapon shop counters / 59 = weapon storeroom (castles) | idem |
| 60 | magic shop counter (towns); lock square (castles) | Transact; Unlock |
| 61 | pub counter (towns); lock square (castles) | Transact; Unlock |
| 62 | transport shop counter (towns); the king's throne square (castles) | Transact |
| 63 | ? | walkable, see above |

(Byte 0x37 = 55, 0x39 = 57, 0x3B = 59: F_5DB1 Steal and F_54FB Get test exactly these three values; F_7377 treats 54..62
as "counter" values.)

### Shops present in each town layout

The shop sign functions (F_2F3B transport, F_2FF6 armour, F_30C0 weapon, F_31B1 magic, F_3240 food, F_32F8 pub)
take the town layout number `n = D_AD42 - 2` (0..7) and print one of up to eight names. Which shops exist in a layout
follows the counter bytes found in `TCD.BIN` and agrees with the `case` labels of the sign functions:

| layout n (map) | armour | food | weapons | magic | pub | transport |
|---|---|---|---|---|---|---|
| 0 (map 2): towns 1 9 17 25 | yes | yes | yes | yes | yes | yes |
| 1 (3): towns 2 10 18 26 | yes | yes | yes | yes | yes | no |
| 2 (4): towns 3 11 19 27 | yes | no | yes | yes | yes | no |
| 3 (5): towns 4 12 20 28 | yes | yes | yes | yes | yes | yes |
| 4 (6): towns 5 13 21 29 | no | yes | yes | no | no | no |
| 5 (7): towns 6 14 22 30 | yes | yes | yes | no | yes | yes |
| 6 (8): towns 7 15 23 31 | no | yes | yes | no | yes | no |
| 7 (9): towns 8 16 24 32 | yes | yes | yes | yes | yes | yes |

Sign texts (the shop name is drawn in the framed box of F_21C2):

* Transport: 0 "Scooter's Super Duper Transport, Inc."; 3 "Quality Transport Ltd."; 5 "O.K. New and Used Transport";
  7 "Sly Sam's Transportation Specialists".
* Armour: 0 "The Armour Shop of Lord Eldric D'Charbonneux"; 1 "Max's Armoury"; 2 "The Iron Fist"; 3 "The Hammer -n- Anvil";
  5 "Defense Specialties Unlimited"; 7 "Custom Armor Works".
* Weapons: 0 "The Weapon Shop of Lord Eldric D'Charbonneux"; 1 "The Tempered Steel"; 2 "The Razor's Edge"; 3 "Cold Steel
  Creations"; 4 "The Bloody Blade"; 5 "The Duelo Shop"; 6 "Weaponry Supply"; 7 "Cold Steel Creations".
* Magic: 0 "Mad Chad's Magic Shoppe"; 1 "Mystic Melinda's"; 2 "Words of Power"; 3 "Psychic Sam's Magical Emporium";
  7 "The Unleashed Spell".
* Food: 0 "Li'l Karelia's Finnish Grocery"; 1 "Adventurer's Supply Post"; 3 "The Brown Bag"; 4 "Fresh Food Marketplace";
  5 "Rations Unlimited"; 6 "Fastest Freshest Food Market"; 7 "Exploration Provisioners".
* Pub: 0 "Pub de Varg"; 1 "Ye Olde Local Pub"; 2 "Dr. Cat's Lair"; 3 "Teaser's House"; 5 "Mary's Midway";
  6 "Kurtstable's Korner"; 7 "Dav's House-o-Suds".

(The names are the authors': Dr. Cat, Dav, Kurt, Karelia ... Detailed shop mechanics: towns-shops-castles.md.)

### Entity lists (the data at DGROUP 31DC)

`F_20FC` copies 15 entity records of the current map from `D_31DC` (DGROUP 31DC..368C = 10 maps x 15 x 8 bytes;
unused slots have type -1) into `D_ACC6` (ACC6, 15 x 8 bytes): `{type, x, y, hp}` (4 words). `hp` is the entity's
hit points. Contents (type 17 guard hp 500, 19 jester/bard hp 1, 20 king hp 2000, 21 merchant hp 1, 22 princess hp 1,
50 wench/lecher hp 1):

| map | entities (type @ x,y) |
|---|---|
| 0 (castle, odd castles) | king 20 @ 33,4 (hp 2000); princess 22 @ 35,12; jester 19 @ 35,6; 6 guards @ 1,6 17,6 17,11 25,7 25,12 36,9 |
| 1 (castle, even castles) | king @ 25,4 (hp 2000); princess @ 31,2; jester @ 22,14; 7 guards @ 28,12 34,12 0,8 11,3 22,1 28,1 33,5 |
| 2 | bard 19 @ 15,5; wench 50 @ 34,6; 6 guards; 6 merchants |
| 3 | bard @ 15,9; wench @ 27,3; 5 guards; 5 merchants |
| 4 | bard @ 17,3; wench @ 21,5; 4 guards; 4 merchants |
| 5 | bard @ 19,12; wench @ 24,3; 6 guards; 6 merchants |
| 6 | 4 guards; 2 merchants (no bard, no wench) |
| 7 | wench @ 20,4; 9 guards; 5 merchants |
| 8 | bard @ 31,14; wench @ 27,6; 3 guards; 3 merchants |
| 9 | bard @ 19,10 with hp 5; 6 guards; 6 merchants |

(Exact coordinates of every guard/merchant are in the data; `python3 tools/dump.py gamedata/OUT.EXE 31DC 4B0 --ds`.)
Entity AI is in monsters.md, the hostility rules in combat.md.

## 5. The eight signs and monuments (places 41-48)

Entering one (F_50C6, `c = place - 41`) draws the text of F_4EA2 in the frame and then applies an effect:

| c | place | text | effect |
|---|---|---|---|
| 0 | 41 The Pillars of Protection | "You feel a strong magic surrounding you!" | Agility +10% |
| 1 | 42 The Tower of Knowledge | "You hear someone say, "TURIS-SCIENTIA-MAGNOPERE"" | Intelligence +10%; sets quest flag (below) |
| 2 | 43 The Pillars of the Argonauts | "A sign reads: "FORTES FORTUNA ADIUVAT!"" | gives the first weapon you do not own (index 1..15) |
| 3 | 44 The Pillar of Ozymandias | Shelley's "Ozymandias" ("MY NAME IS OZYMANDIAS, KING OF KINGS: LOOK AT MY WORKS, YE MIGHTY, AND DESPAIR!" ... "You feel a strange force!") | Wisdom +10%; quest flag |
| 4 | 45 The Sign Post | "ULTIMA THULE!" "The sky grows dark, and a strong magic engulfs you!" | Stamina +10% |
| 5 | 46 The Southern Sign Post | "OMNIA MUTANTUR!" | Charisma +10%; quest flag |
| 6 | 47 The Eastern Sign Post | "GO EAST TO GO EAST!" | none (no message at all) |
| 7 | 48 The Grave of the Lost Soul | "The grave is marked: "VAE VICTIS"" | Stamina +10%; quest flag |

Effect details (F_50C6): the stat index per `c` is the table `D_3708` (DGROUP 3708: 2, 6, 0, 5, 3, 4, 0, 3 where the
index is into `D_206E[]`: 1 Strength, 2 Agility, 3 Stamina, 4 Charisma, 5 Wisdom, 6 Intelligence). The gain is
`(stat + 8) / 10` (rounded 10%), capped so the stat does not exceed 99; a stat already at 99 gives no effect.
If `c` equals the previous shrine visited (`D_3B20`, DGROUP 3B20, saved in the record) nothing happens: "Hmmmm...no
effect!" with the failure sound; so the gain needs two different shrines in between. On success the frame flashes and
sound 5 plays. Odd `c` (1, 3, 5, 7) additionally call F_5085, the quest bookkeeping (quests-and-progress.md). Any
key continues (F_2141).

## 6. Dungeon entrances

Places 49..84 start a dungeon (F_4BB0): see dungeons.md. They are all generated by the same code.
