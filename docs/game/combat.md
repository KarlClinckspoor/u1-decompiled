# OUT.EXE: combat (overworld, towns and castles, dungeons), weapons, armour, spells, death

Source: `src/out/OUTA.C` (F_0953, F_0BAA, F_0C52, F_0CF3, F_0FEF, F_1072, F_10C2, F_12F2, F_13A8, F_1769, F_18FC, F_19FF, F_286B, F_294E,
F_2B6D), `OUTC.C` (F_81FC, F_8194, F_873D, F_8600, F_8AC9), `src/origin/OOC47C.ASM` (death), data read from `OUT.EXE`.
`rand(lo,hi)` is `F_4069(lo,hi)`: a number in [lo, hi] with `hi` clamped to 255 (overview.md, OM4069). "Level" means the experience
level `F_40E9() = experience / 1000 + 1`; "dungeon level" is `D_AD50`.

## 1. The attributes that matter

The record holds (offsets from the start of the record, C names in parentheses): hit points `D_206E[0]` (0x16), then
`D_206E[1..6]` = **Strength** 0x18 (`D_3A90`), **Agility** 0x1A (`D_3A92`), **Stamina** 0x1C (`D_3A94`), **Charisma** 0x1E (`D_3A96`),
**Wisdom** 0x20 (`D_3A98`), **Intelligence** 0x22 (`D_3A9A`). Class `D_206A` (0x12: 0 Fighter, 1 Cleric, 2 Wizard, 3 Thief), race `D_2068`
(0x10), sex `D_206C` (0x14). Readied weapon `D_2082` (0x2A), spell `D_2084` (0x2C), armour `D_2086` (0x2E), transport `D_2088` (0x30).

Which stat does what in OUT:

| stat | used for |
|---|---|
| Strength | weapon damage `rand(2, Strength + 8 * weapon)`; the reward for odd-king quests raises it |
| Agility | chance to hit with weapons (`Agility + 50` percent); chest traps (`rand(1,75) > Agility` springs a trap); chance a stealing jester's theft is announced (`rand(1,255) < Agility + 128`) |
| Stamina | chance that monsters miss: see below; number of ales you can drink before the wench takes you (`Stamina / 4`) |
| Charisma | **selling** prices of weapons and armour |
| Wisdom | chance that a spell works (`rand(1,100) < Wisdom + 50`, never fails for clerics); spell shop prices; lowered by the "seduced" event |
| Intelligence | magic damage `rand(1, Intelligence)`; **buying** prices of weapons, armour, food, transport; halved by the Mind Whipper |

(That buying uses Intelligence and selling Charisma is exactly what the code does: `F_63B3`, `F_67F1`, `F_6DE3`, `F_7034` read `D_3A9A`, while
`F_63D4`, `F_680C` read `D_3A96`. The manual only says price varies with the armour's protection; no claim is made about intent.)

Hit points, food, experience and pence are capped at 9999 for display (F_4437, F_44A2, F_450D, F_4558 clamp the variable); food and
hit points below 100 are printed in the warning colour `D_2044`.

## 2. Weapons

Names `D_0D72` (DGROUP 277E, display) and `D_0DF8` (2804, lower case short names), range `D_0CD2` (26DE, one int per weapon):

| # | weapon | range | max damage `Str + 8*#` extra | notes |
|---|---|---|---|---|
| 0 | Hands | 1 | +0 | always owned (count -1) |
| 1 | Dagger | 1 | +8 | starting weapon (count 2) |
| 2 | Mace | 1 | +16 | |
| 3 | Axe | 1 | +24 | |
| 4 | Rope & Spikes | 0 | | cannot attack ("Attack with rope?" and a beep) |
| 5 | Sword | 1 | +40 | |
| 6 | Great Sword | 1 | +48 | |
| 7 | Bow & Arrows | 3 | +56 | |
| 8 | Amulet | 0 | | magic item: multiplies Magic Missile by 3/2 when readied |
| 9 | Wand | 0 | | x2 |
| 10 | Staff | 0 | | x3 |
| 11 | Triangle | 1 | +88 | magic item (x3) and a (short) sword |
| 12 | Pistol | 3 | +96 | |
| 13 | Light Sword | 1 | +104 | |
| 14 | Phazor | 3 | +112 | |
| 15 | Blaster | 3 | +120 | |

Damage of one attack with weapon `w`: `rand(2, Strength + 8*w)` (the upper limit is clamped to 255 by `F_4069`; so with high
Strength the top weapons all give `rand(2,255)`). Hit chance: `Agility + 50` percent as `rand(1,100) <= Agility + 50` (100 % once
Agility >= 50). The same formulas are used in all four modes (F_0C52 dungeon, F_0FEF overworld, F_0CF3 -> F_2B6D towns).

Where weapons come from: shops (towns-shops-castles.md), stealing (random 1..15), castle donations, the Pillars of the Argonauts, chests of
the quest rewards... The armour table: index `D_2086`: 0 Skin, 1 Leather armor, 2 Chain mail, 3 Plate mail, 4 Vacuum suit, 5 Reflect suit
(`D_0F5C` at 2968). Armour reduces the monsters' hit chance by **8 per armour index** (below); there is no other armour effect in OUT.
Counts are in `D_20AC[0..5]` (index 0 = -1 = skin) and `D_20B8[0..15]`.

## 3. Overworld: the player attacks

`A` / `F`: F_10C2 -> F_1072 -> F_0FEF(c, k) -> F_0CF3(c, snd, range, dmg, hit, tile). F_0FEF sets:
* k = 7 (weapon): `range = D_0CD2[weapon]`, `hit = Agility + 50`, `dmg = rand(2, Strength + 8*weapon)`, tile 50 (0x32);
* k = 8 (ship's weapon from **Fire**): `range = 3`, `hit = 80`, `dmg = rand(1, 10 * transport) + 30` where transport is 4 (frigate: 31..70) or
  5 (aircar: 31..80).

F_0CF3 (projectile; `snd` = k is the sound): step `i` = 1, 2, ... up to `range`: draw the projectile tile at the cell `i` squares away in
direction `c` (sound at the first step, a short delay afterwards), redraw what is really there, stop when the cell holds a monster
(type >= 19) or is a mountain (3). If no monster was reached: "Miss!". If one was: when `rand(1,100) <= hit` it is hit, else "Missed the
<name>!" (the messages: "Hit the <name>" / "Killed the <name>" ...):
* the monster loses `dmg` hit points (its table entry's `f8`);
* alive: "Hit the <name>! <dmg> damage" (sound 2);
* dead (hp <= 0): "Killed the <name>" -- the entry is removed (F_B58E), `D_8B90--`, and the rewards are
  `gold = rand(1, K*4) + 10` pence and `experience += rand(2, K*5)` where `K = D_368C[(type - 19)/2]` (see monsters.md for the table; K is 1..10),
  then the pence are announced by F_0761 right after the name: "Killed the Bear! 14 copper! 1 silver!" (no "Thou dost find:" here, that
  prefix is printed only by the dungeon and chest code; the pence added are capped so that the purse stays <= 9999). F_0761 prints the units digit as copper, the tens
  digit as silver and `n / 1000` as gold, so the hundreds digit is not mentioned (while the Ztats screen counts 100 pence per gold crown); this is how the code is written.

Path geometry: the path is orthogonal only (directions 1-4) and passes over every monster-free cell, even over water and
vehicles. A monster can be hit by a weapon with range 1 only if it is orthogonally adjacent.

**Magic Missile / Kill (outdoors)**: F_18FC. After "Cast <spell>" it prints ": " and reads a direction (F_4161; a key that is not an arrow gives "Nothing" and
`D_20D8[D_2084]++` gives the charge back, so a cancelled cast costs nothing).
Then `"DELCIO-ERE-UI"` (Magic Missile, damage `F_12F2()`) or `"INTERFICIO-NUNC!"` (Kill, damage 9999) is printed; the spell works if the player
is a cleric (`D_206A == 1`, always) or `rand(1,100) < Wisdom + 50`; otherwise "Failed!" + failure sound. On success sound 5 and
F_0CF3(c, 7, range 3, dmg, hit 101, tile 0x31): it cannot miss (101%). `F_12F2` = `rand(1, Intelligence)` times 2 if the readied weapon is the
Wand (9), times 3/2 (integer) for the Amulet (8), times 3 for the Staff (10) or Triangle (11).

**Prayer (outdoors)** (F_1769): prints the spell name; if hit points < 15 they are set to 15 ("Shazam!"); else if food < 15 it is set to 15;
else with probability 24/100 (`rand(1,100) < 25`) the first monster in the table that can reach the player (F_8194: within 1 square, 3 for ranged
types) is removed with no reward ("Monster removed!"); otherwise "Hmmmm... no effect!". In a dungeon Prayer casts a random dungeon spell:
`i = rand(1,10)`, 4 becomes 5, then F_13A8(i, 0).

## 4. Overworld: monsters attack

Every turn (F_8600, called from F_8CEB): for each monster entry 1..`D_8BA6` of type 19..47: `dx = playerX - monsterX`, `dy = playerY - monsterY`.
`F_8194(type, dx, dy)` gives its *reach*: 3 for types 23 (Dragon turtle), 25 (Pirate ship), 31 (Hidden archer), 47 (Wandering warlock), else 1 -- and
returns 0 if `max(|dx|,|dy|)` exceeds it. Monsters in reach are queued (up to 10 entries, `q[]`) and attack **after** all the others have moved
(F_8598); the others try to move one step towards the player (monsters.md). After the moves F_C3CA redraws the view; then the queue is
processed last-to-first while hit points > 0: `F_81FC(m, dx, dy, reach)`.

F_81FC: prints "<name> attacks!"; animates a projectile (tile 50) from the monster towards the player, one cell per step towards the player
(a step changes x and y each by 1 toward 0, so it is a straight line only for orthogonal and exact diagonal shots), at most `reach` steps; the
shot stops at a mountain (3). `hit = rand(1,255) < 200 - Stamina/2 - 8*armour`. If the shot did not arrive on the player's square (blocked, or the
geometry did not line up) or the roll failed: " Missed!" (redraws the player). Otherwise sound 2, damage
`dmg = rand(1, 2*K + 1)` with `K = D_368C[(type-19)/2]` (monsters.md), "Hit! <dmg> damage", hit points `-= dmg`.
(So the hit chance is `(199 - Stamina/2 - 8*armour)/255`: with Stamina 20 and no armour 74 %; with Reflect suit 58 %.)

## 5. Towns and castles

The player attacks with F_2B6D(dir, range, damage, chance) (called from F_0CF3 in modes 1 and 2). It prints the sound 7, follows the
direction up to `range` cells (stops at the first entity or non-free cell), finds the entity there (F_22F3) and: no entity, or
`rand(1,100) > chance` -> "Missed!". Else "Hit " + who ("King!", "Guard!", "Merchant!", "Princess!", "Bard!" in a town or "Jester!" in a castle,
"Wench!" for a male player or "Lecher!" for a female one) and sets the **hostility flag `D_AD46 = 1`** (guards now attack, shops refuse, see
towns-shops-castles.md), sound 2. If `damage >= entity hp`: "Killed!" -- experience +1 (but for the princess `D_8B9E = 0` and no experience),
the entity becomes type -1 (dead), its square is redrawn as floor, guards give another +14 experience, and a jester killed in a castle
drops "Thou hast found a key!" (`D_8BA0 = 1`). Else the entity loses `damage` hit points (**not** the king of the castle of Lord British, place 33:
`D_8BA2 != 0` makes the king immune) and "<damage> damage!" is printed. Entity hit points: guards 500, kings 2000, everything else 1 (the bard of map 9: 5).
No gold is paid; experience is the only reward.

Guards when hostile (F_2A73 -> F_286B, F_294E): a guard (type 17) whose distance is >= 2 in either axis and `|dx|+|dy| < 13` steps towards you (x
first, then y); guards that are adjacent (|dx| < 2 and |dy| < 2) attack, in the order they were queued, each one: "Attacked by guard!" + sound 7; they
miss if `rand(1,255) < Stamina/2 + 8*armour + 56` ("Missed!"), else "Hit!... <dmg> damage!" with
`dmg = rand(2, HP/128 + 15)` (`HP` = your **current** hit points, integer division), sound 2, hit points `-= dmg`, and the loop stops when hit points <= 0.
The princess (type 22) runs away when hostile (monsters.md).

## 6. Dungeons

Shots: `F_0C52` (Attack): prints a blank line, sound 7, `r = F_0BAA(range)` = distance (1..range) to the first monster in the facing direction (0
if the first non-empty cell is a wall/force field or nothing is in range: "Nothing"); then `hit = Agility + 50`, `dmg = rand(2, Strength + 8*weapon)` and
`F_0953(2, hit, dmg, x, y)` against the cell at distance `r`. F_0BAA scans cells `i = 1..range` ahead and stops at a monster (`b != -1`), wall (1, 2), force
field (8) or door (3); it returns 0 for "wall/field/no monster", else `i`.

`F_0953(snd, hit, dmg, x, y)` (also used by the spells): the attack happens only if `rand(1,100) <= hit`, the player's cell is not a wall type 1/2/8 and, if the
player stands in a doorway (type 3), the target cell is floor/ladder (type 0, 6, 7); otherwise "Missed!". Then sound `snd`, "Hit! " (not for the Kill spell, which
passes `dmg = 10000`), and
* monster hit points `d <= dmg`: print its name (F_3C31; "Mimic" for id 11) + " killed!" (" destroyed!" for dmg 10000); `F_08EE(id)` (quest bookkeeping for the four king-quest
  monsters); reward **pence** `rand(2, 3*L + 10 + id)` ("Thou dost find: ..."), **experience** `rand(2, L*L + 10)`; the monster's slot is freed (`D_B11E[id % 5] = 0`),
  the bonus `D_AD54 += 2 * experience gained` (paid as hit points when leaving the dungeon, dungeons.md); then `F_4602` puts a new random monster on the level;
* else the monster loses `dmg` hit points: "<dmg> damage!".
(L = dungeon level `D_AD50`.)

**Dungeon spells** (F_13A8, damage via F_0953 with hit 101 so they always hit): Magic Missile `F_0BAA(5)` range 5, damage `F_12F2()`; Kill hits the cell directly ahead if it
holds a monster, destroying it (`dmg 10000`). Others are in dungeons.md.

**Dungeon monsters attack** (F_8AC9, F_873D): every cell of the 9 x 9 interior (`i`, `j` = 1..9) that holds a monster not already moved this turn (`c < 999`): with Manhattan distance
`d = |dx| + |dy|` to the player: `d == 1` -> attack F_873D(dx,dy); `d < 7` -> step towards the player (monsters.md). After a monster's row cell the loop `break`s if hit
points <= 0 or food <= 0 (note: a `break` leaves only the inner loop).

F_873D(dx,dy): nothing happens if the player's cell is a force field (type 8). The attack is also suppressed when the monster stands in a doorway (cell type 3) while the
player's cell is a doorway or a wall-type cell (3, 1 or 2): the code tests `monsterCell.a != 3 || (playerCell.a != 3 && playerCell.a != 1 && playerCell.a != 2)`. Otherwise: "Attacked by <name>!"
(sound 3). With `t` = the monster's id:
* **miss** if `rand(1,255) <= Stamina/2 + 8*armour + 56`: "Missed!" (e.g. Stamina 20 with leather: 74/255 = 29 % miss);
* else `dmg = L + t*t` (if that exceeds 255: `rand(t+1, 255)`) and special cases by monster:
  * t = 9 Gelatinous cube with armour readied (armour != skin): "Armor destroyed!", one piece of armour is lost (`D_20AC[armour]--`), readied armour becomes skin, **no damage**;
  * t = 16 Gremlin: "A gremlin stole some food!": food halved, no damage;
  * t = 21 Mind Whipper and a second `rand(1,255) < 128`: "Mental attack!": `Intelligence = Intelligence/2 + 5`, no damage;
  * t = 2 Thief: also steals the first weapon (index 1..15, lowest index first) that you own and do not have readied: "Thief stole a(n) <weapon>" (count -1), and the damage is dealt as well;
  * all others, and the cases above that do not take the "no damage" branch: "Hit! <dmg> damage!" (sound 2), hit points `-= dmg`.
Damage grows fast with the id: Giant rat (3) at level 1: 10; Cyclops (8) at level 3: 67; Ettin (10) at level 5: 105; every id >= 16 (16*16 = 256) gives `rand(t+1, 255)` at any level.

A chest trap (F_7B63) costs `D_AD50` hit points (dungeon level) only.

## 7. Magic summary

| spell # | name | where it works | effect |
|---|---|---|---|
| 0 | Prayer | overworld, dungeon | above |
| 1 | Open | dungeon | opens a coffin in your cell, pence `rand(3, L*L+9)` |
| 2 | Unlock | dungeon | opens a chest in your cell |
| 3 | Magic Missile | overworld (range 3), dungeon (range 5) | `F_12F2` damage |
| 4 | Steal | nowhere (a dungeon cast only plays sound 5; outdoors "Failed, dungeon spell only!") | the spell shop skips it and no prize list contains it, so OUT never gives the player this spell |
| 5 | Ladder Down | dungeon | ladder in the current cell |
| 6 | Ladder Up | dungeon | |
| 7 | Blink | dungeon | random teleport |
| 8 | Create | dungeon | force field ahead |
| 9 | Destroy | dungeon | removes a force field ahead |
| 10 | Kill | overworld (range 3), dungeon (adjacent) | kills outright |

Each cast of a spell (except Prayer) uses up one charge (`D_20D8[spell]`, 0..255). A failed attempt prints "Failed!" and beeps (sound 6) but the charge is gone.

## 8. Order of events in a turn

1. the player's command (a hit or a spell takes effect at once);
2. overworld: `F_A554`, spawn check; the turn counter;
3. the monsters' turn (non-attackers move first, then the attackers of that turn fire; in towns the entities move and then the adjacent guards attack);
4. food use (overworld only);
5. death check at the start of the next loop iteration.

## 9. Death and resurrection (F_C47C, `src/origin/OOC47C.ASM`)

Checked at the start of every loop iteration: `HP <= 0 || food <= 0` (running out of food kills). The sequence:
* mode = 0, food = 0, hit points = 0, pence = 0; the status panel updated; the tombstone picture F_AB9D drawn in the view window (colour `D_0DA6`); sound 2 three times;
* the character name (`D_2058`) and ", thou art dead" are printed, then "Attempting resurrection!" after a wait of 80 ticks;
* hit points are counted up from 1 to 99 (one per tick wait) and each time shown; then `F_86E1(0..3)` deletes every monster in all four quarters;
* food is counted up from 1 to 99;
* if the character was riding a transport the count of that transport in `D_20EE[]` is decreased by one, the transport is gone and the player is on foot (type 10);
* **all weapons are lost** (`D_20B8[1..15] = 0`), the readied weapon, armour and spell are reset to 0 (hands, skin, Prayer); armour pieces and spell charges, gems,
  the quest flags and experience are kept; **pence are 0**;
* the player is moved to a random square of the same quarter that is grass or woods (F_8C20: offsets -10..+10 in x and y, retried until the square is in the same quarter and its terrain/object value is 1 or 2), the view is redrawn (F_C3CA).
Result: you come back with 99 hit points and 99 food.
