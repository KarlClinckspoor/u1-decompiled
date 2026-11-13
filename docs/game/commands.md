# OUT.EXE: the command loop and every command key

Source: `main` = F_1853 in `src/out/OUTC.C` (the `switch (c)` at the end), and the functions it calls. Messages are
quoted from the program's strings. "Beep" = `F_0476(1)` (sound 1, only if sound is on, see video-sound-input.md);
"mode" = `D_204A` (0 overworld, 1 town, 2 castle, 3 dungeon).

## 1. The loop (F_1853)

Start-up, in order: `F_1D0D(1)` (Lattice ONERROR hook); `D_2048 = (toupper(argv[1][0]) == 'C')` (force-CGA flag);
`D_8B88/8A/8C = argv[1..3]` (kept to be passed on to SPACE/MONDAIN); `F_3F58(0)` (video set-up); `F_03E9(argv[2][0])`
(read `playerN.u1`, or `inuse.u1` if `argv[3]` is non-NULL: save-format.md); `F_0441` (read the tile graphics:
`CgaTiles.Bin` 0xD00 bytes if `D_082C == 0`, else `EgaTiles.Bin`/`T1KTiles.Bin` 0x1A00 bytes, into `D_238C`); read
`map.bin` (13104 bytes), `tcd.bin` (6840 bytes) and `cgatown.bin`/`egatown.bin`/`t1ktown.bin` (1632 bytes into
DGROUP A666); `D_8BA6 = D_3B22` and `D_8BA8 = D_3B24` (monster count and turn counter come out of the record);
`D_8B8E = 1` (food counter); mode variables cleared; `D_8B90 = F_B5E1()` (monsters), `D_8B92 = F_B617()`
(vehicle objects); `F_37FD(D_3AAA + F_3EC4() % 999)` seeds the random generator (world seed plus clock);
if hit points <= 0 or food <= 0 the death/resurrection routine F_C47C runs now, otherwise F_C3CA draws the view;
`F_45A3` draws the status panel.

Then forever:

1. `if (HP <= 0 || food <= 0) F_C47C();` (death and resurrection, combat.md section 9).
2. `F_387E(0,24)`: draw the right-pointing command arrow at text column 0, row 24 (the prompt).
3. Key wait: `do { c = F_4127(); cnt++; if (mode == 0) F_AF57(cnt); } while (c == 0 && cnt < 1500);`
   F_4127 polls the keyboard (upper-cases the key; waits 3 timer ticks when no key was pressed, 1 when one was); on
   the overworld the view is redrawn and animated at every poll (F_AF57). After 1500 empty polls the command becomes
   a space (**Pass**); at 3 ticks of about 1/18.2 s per poll that is about four minutes.
4. The command is executed (below).
5. Post-command turn processing (**every** command, valid or not, costs a turn):
   * mode 0: `F_A554()` (water animation step); if `D_8B90 <= 10` then `F_7F0F()` (maybe spawn a monster);
   * `D_8BA8++` (turn counter, 32 bits; saved in the record, drives shop stock and time-dependent unlocks);
   * `F_8CEB()`: monsters' turn: mode 0 `F_8600`, mode 3 `F_8AC9`, modes 1/2 `F_2A73`;
   * `F_7E80()`: food use (mode 0 only);
   * mode 3: `F_414A()` redraws the first-person view;
   * `D_8B9C = D_204A` (previous mode, so a change of mode is noticed by F_2A73).

## 2. Key table

Arrow keys are delivered by F_413C as 1 = left (West), 2 = right (East), 3 = up (North), 4 = down (South). F1-F10 would come as
C9h-D2h but are not handled (they give "Huh?"). Lower-case keys are upper-cased.

| key | function | summary |
|---|---|---|
| arrows (1..4) | F_066F | move / turn (section 3) |
| A | F_10C2(-1) | Attack, asks for the direction |
| 8 2 6 4 | F_10C2(3,4,2,1) | Attack north, south, east, west without asking (digits of the numeric keypad) |
| B | F_118D | Board a horse/vehicle standing on your square |
| C | F_19FF | Cast the readied spell |
| D | F_204D | Drop pence/weapon/armour (towns and castles only) |
| E | F_52AB | Enter a town/castle/sign/dungeon |
| F | F_53DA | Fire ship cannons / aircar lasers |
| G | F_5550 | Get (castle storeroom items) |
| H | F_559A | "HyperJump?" and a beep (works only in SPACE) |
| I | F_564B | Inform (name of place; in dungeons: search for secret doors) |
| K | F_581A | K-Limb a dungeon ladder |
| N | F_58BD | Noise on/off |
| O | F_59D7 | Open a coffin (dungeon) |
| Q | F_5B54 | Quit = save the game (overworld only; the program does not end) |
| R | F_43C8 | Ready weapon / armour / spell |
| S | F_5DB1 | Steal (at a shop counter or castle storeroom) |
| T | F_7B17 | Transact (shops in towns, the king in castles) |
| U | F_7CF2 | Unlock (chests in dungeons, doors in castles) |
| V | F_7B4C | "View?" and a beep (never available in OUT) |
| X | F_7D6B | X-it a vehicle |
| Z | F_478E | Ztats (character sheet), then `D_204E = 1` |
| space or no key | F_7E5B | prints "Pass" |
| Esc (1Bh) | sets `D_0182 = 1` then falls into "Huh?" | `D_0182` (DGROUP 0182) is never read by OUT |
| anything else | F_7E69 | "Huh?" and a beep |

Keys that are *not* commands: there is no key for Look, Pray (it is the spell Prayer, key C), Talk (use T), Climb (K), Use, Wait
(space), Hyperjump (H, SPACE.EXE only), View (V; in SPACE).

## 3. Movement (arrow keys)

`F_066F(c)`:
* **town or castle (mode 1/2)**: F_2E22 (towns-shops-castles.md): step one square if free ("Blocked!" + sound 0/1 otherwise),
  leaving the 38 x 18 map ends the visit.
* **dungeon (mode 3)**: F_4B73: **up = Forward** (F_4B16), **down = Turn around**, **left = Turn left**, **right = Turn right**
  (dungeons.md). The facing is `D_AD52` (1 W, 2 E, 3 N, 4 S).
* **overworld**: `F_04AF(c)` decides whether the step is allowed (below); if so: sound 4 when the transport is foot, horse
  or cart (`D_2088 <= 2`), print the direction word (`D_1DC8[c]` = "West" ...), scroll the map (F_B665 north, F_B70F south,
  F_B7CD east, F_B884 west; these also update `D_3AAC/D_3AAE` with the world wrap-around, world-and-map.md), copy the new
  position into the player's monster-table entry, and if the quarter of the map changed call `F_86E1(old quarter)`
  (delete the monsters there).

`F_04AF(c)` computes the terrain/object `t` of the target square with `F_C2AF(F_C2EC(dx+9), F_C31C(dy+4), 1)` (screen
position (9+dx, 4+dy); monster entries are included, the player's own entry is skipped) and the transport `lvl = D_2088`
(0 foot, 1 horse, 2 cart, 3 raft, 4 frigate, 5 aircar, 6 shuttle, 7 time machine). The step is allowed unless:

| condition | message (sound 0 each) |
|---|---|
| `19 <= t <= 47`: a monster is there | "Blocked by a" (+"n" if its name starts with a vowel) + monster name + "!" (for the Wandering warlock, t = 47, the message area is scrolled first and the cursor put on row 24 column 1, because the long name would not fit on the line) |
| `t == 3` (mountains) | "Mountains are impassable!" |
| `t == 0` (water) and transport is not raft/frigate/aircar | "You can't walk on water!" |
| `t == 2` (woods) and aircar | "Aircars can't pass woods!" |
| frigate (4) and `t != 0` | "Frigates like water!" |
| raft (3) and `t != 0` | "Rafts like water!" |
| shuttle / time machine (6, 7) | "Can't move on land!" (they only leave the overworld through Board) |

Other terrain (grass, places) is fine for foot, horse and cart; objects 10..18 (vehicles) are walkable. Note the horse/cart can
also enter woods; the aircar can fly over water and grass (and everything but woods and mountains).

## 4. Attack (A, 8 2 6 4, F)

`F_10C2(a)`: prints "Attack with " and the lower-case weapon name (`D_0DF8[readied weapon]`: hands, dagger, mace, axe, rope,
sword, g sword, bow, amulet, wand, staff, triangle, pistol, L sword, phazor, blaster).
* weapon range `D_0CD2[weapon] == 0` (rope & spikes, amulet, wand, staff): prints "?" and beeps;
* in a dungeon: `F_0C52` (direction = facing; see combat.md);
* otherwise ": " and `F_1072(a, 7)`: if `a == -1` the direction is read with `F_4161` (arrow key; waits up to 1500 polls),
  a non-direction answer prints "nothing"; else it prints the direction name and calls `F_0FEF(c, 7)` -> `F_0CF3` (overworld) or
  `F_2B6D` (town/castle). Formulas in combat.md.

**Fire (F)**: not allowed in towns, castles or dungeons ("Fire?" + beep). Otherwise "Fire "; if the transport is not
frigate (4) or aircar (5): "what?" + beep; else "cannons: " (frigate) / "lasers: " (aircar) and `F_1072(-1, 8)`.

## 5. Board (B), X-it (X)

**Board** (F_118D): works only on the overworld ("Board?" + beep in other modes), only on foot ("X-it thy craft first!"), and only
if an object of type 11..18 (a vehicle) is on your square ("Nothing to Board!" otherwise). The vehicle entry is removed from
the monster table and the player takes its tile: `D_2088 = type - 10`, minus 1 if that is > 4 (so types 14 and 15 are both the
frigate, 4; 16 -> 5; 17 -> 6; 18 -> 7). Message: "Mount " + name for horse (11) and cart (12), else "Board " + name (names in
`D_0FA2`: Foot, Horse, Cart, Raft, Frigate, Aircar, Shuttle, Time Machine).
* type 17 (shuttle): `F_1135("space.exe", 17)`;
* type 18 (time machine): needs all four gems (`D_20A4`, `D_3AC6`, `D_3AC8`, `D_3ACA` all non-zero) then `F_1135("mondain.exe", 18)`;
  otherwise the frame shows "Thou canst not determine how to operate the craft at this time." and waits for a key.
`F_1135(prog, tile)` draws the vehicle tile at the player's cell (9,4), copies `D_3B22 = D_8BA6; D_3B24 = D_8BA8`, writes the
record to `inuse.u1` (F_3E85) and does `execl(prog, prog, argv[1], argv[2], 0)` in a loop; if the exec fails it shows the
"Insert Ultima I diskette / Hit space to continue..." prompt (F_125C) and retries.

**X-it** (F_7D6B): "X-it?" + beep unless on the overworld; "X-it what?" + beep when on foot. The player must be standing on
water/grass/woods (terrain `t <= 2`) *and* some non-water cell must lie within +-2 squares (F_A363 != 0 over the 5 x 5 box); otherwise
"Thy canst not leave it here!" (sic) + beep. On success the vehicle object is re-created at the square (F_B502 with the
player's current type), `D_2088 = 0`, the player's entry becomes type 10 and "X-it" is printed.

## 6. Cast (C), Ready (R)

**Cast** (F_19FF). In towns and castles (and mode 4): "Cast -- Hmmmm... no effect!" + failure sound (no spell works there,
not even Prayer). Otherwise "Cast " + spell name (`D_0F04[D_2084]`: Prayer, Open, Unlock, Magic Missile, Steal, Ladder Down, Ladder
Up, Blink, Create, Destroy, Kill):
* spell 0 (Prayer): F_1769, costs nothing;
* count `D_20D8[spell] == 0`: "You've used up that spell!" + failure sound;
* else the count is **decremented first**, and then: in a dungeon F_13A8(spell, 1) executes it; outdoors only Magic Missile (3) and
  Kill (10) work (F_18FC, they ask "Direction:"); every other spell prints "Failed, dungeon spell only!" and is used up anyway.
Spell effects: combat.md (damage), dungeons.md (dungeon spells).

**Ready** (F_43C8): prints " Ready " ... "Ready Weapon,Armour,Spell:" and reads one key (F_4161: arrow or none -> ignored): W, A or S
(upper-cased). It lists the items you own (those with count != 0, counts of hands/skin/Prayer are -1 so index 0 is always
present) as `a) name` ... in a box (F_420B, letters start at `a` for index 0), reads a letter and sets `D_2082`
(weapon, 0..15), `D_2086` (armour, 0..5) or `D_2084` (spell, 0..10). An invalid letter or an item you do not own leaves
the old selection. The chosen name is printed. Any other first key prints "nothing" (nothing is changed).

## 7. Drop (D), Get (G), Steal (S), Transact (T)

* **Drop** (F_204D): only in towns and castles ("Drop?" + beep elsewhere). Prints "Drop Pence,Weapon,Armour:" and reads P, W or A:
  * **P**: "pence: " then a number of up to 4 digits (F_B0B5, input box at column 12, row 24); 0 -> "none"; more than you have ->
    "Thou hast not that much!" + beep; otherwise the pence are subtracted and: in a castle F_1AE4 (see below), in a town only
    standing on a 0x33 square does something. Details: towns-shops-castles.md ("Donations").
  * **W**: weapon list (F_1D71): lists weapons you own, letters `b..` for indices 1..15 (hands cannot be dropped), asks a letter;
    the weapon's name is printed and one is removed; if it was the last of the readied weapon the readied weapon becomes hands.
  * **A**: same for armour (F_1EDF), indices 1..5; dropping the last of the readied armour makes it skin.
  Dropped items are simply gone; nothing appears on the map.
* **Get** (F_5550): in a castle only (town: "Get what?" + beep, elsewhere "Get?" + beep); needs the king's permission.
* **Steal** (F_5DB1): towns and castles; works on the counter squares 0x37 (armour), 0x3B (weapons), 0x39 (food).
* **Transact** (F_7B17): town: F_7377 (shops, pub, transport); castle: F_794E (king); else "Transact?" + beep.
  All details in towns-shops-castles.md.

## 8. Enter (E), Inform (I), K-Limb (K), Open (O), Unlock (U)

* **Enter**: world-and-map.md section 3.
* **Inform** (F_564B): prints "Inform and search". In a dungeon F_55B1 (the cell ahead: "Thou dost find a secret door!" converts a
  type-2 wall to a door (type 3) unless you stand in a doorway; otherwise "Thou dost find nothing"). Elsewhere: names the
  place under the player ("the city of X" for places below 33, else the name); on no place: terrain 0 "You are at sea", 2
  "You are in the woods", else "You are in the lands" + the land name (`D_3174[quarter]`). In towns/castles the world coordinates
  still identify the town, so Inform prints the town's name there.
* **K-Limb** (F_581A): "K-Limb"; outside dungeons "?" + beep; on a floor cell that is not a ladder " what?"; facing east/west
  "Thou must face the ladder!" (north/south works); otherwise up ladder (type 7) -> F_56F7, down ladder (type 6) -> F_57DF
  (dungeons.md).
* **Open** (F_59D7): dungeon coffins only (dungeons.md); "Open?" in other modes.
* **Unlock** (F_7CF2): "Unlock"; dungeon: F_7B63 with `rand(1,75)` (chests); castle: F_7C31 (castle doors, towns-shops-castles.md); overworld:
  "?" + beep; town: " what?" + beep.

## 9. Noise (N), Quit (Q), Ztats (Z), Pass

* **Noise** (F_58BD): "Noise " then toggles `D_3AB0` (the sound flag, saved in the record at offset 0x38): "on" / "off".
* **Quit** (F_5B54): "Quit - "; anywhere but the overworld: "only allowed outdoors!" + beep. Otherwise F_5B0E: "saving game...",
  `D_3B22 = D_8BA6; D_3B24 = D_8BA8`, write the 820-byte record to `playerN.u1` (`D_0180`, whose digit was patched with the slot
  character at start), "saved.". The game goes on.
* **Ztats** (F_478E): see out-state.md (section on the screens): a sheet with name, level (`experience / 1000 + 1`), sex,
  race, class, the six attributes and hit points, money as copper/silver/gold, enemy vessels destroyed, then armour, weapon,
  spell, transport and gem lists. When a list does not fit on the screen a " More " page break appears (F_4645 -> F_3AE2, "Press Space to continue: ", Space or Return).
* **Pass**: prints "Pass"; nothing else (the turn still elapses).

## 10. Messages and sound by command (quick reference)

Beep (sound 1): refused/unknown command, wrong place/mode, missing item. Sound 0 (crash): blocked by terrain/monster in
overworld movement (F_048A), "path blocked" in dungeons. Sound 2: a hit (given or taken). 3: monster attack. 4: footstep. 5:
spell success/shrine. 6: spell failure. 7: weapon use (attack, bolt).
