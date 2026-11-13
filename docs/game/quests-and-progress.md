# OUT.EXE: quests and progress (kings' quests, gems, space ace, princess, time machine)

Sources: `OUTB.C` (F_4BD6, F_50C6, F_5085), `OUTC.C` (F_7526, F_767D, F_769D, F_7738, F_78AF, F_794E), `OUTA.C` (F_08EE, F_0953), `OUTC.C` F_118D/F_1135 (boarding), the data tables of
`OUT.EXE`. Addresses are real DGROUP addresses; record offsets are relative to the start of the character record (save-format.md). Level = experience / 1000 + 1 (F_40E9).

## 1. The path to the end, as OUT enforces it

1. The time machine (map object 18) does **not exist** at the start. The only code that creates one is the princess reward F_4BD6, and only for a player who has already destroyed at least 20 enemy vessels in space
   (`D_20FE >= 20`, record 0xA6, maintained by SPACE.EXE; the manual calls it becoming an "ace") **and** has `experience >= 7000` (level 8). The time machine then appears in the north-west part of the quarter you are in
   (towns-shops-castles.md section 8), at most nine times in total (`D_3B1C < 9`).
2. Boarding the time machine (Board, F_118D, tile 18) needs all four gems; without them: "Thou canst not determine how to operate the craft at this time." With them OUT saves `inuse.u1` and starts `mondain.exe` (final-battle.md).
3. The gems come from the four **even kings** (castles 34, 36, 38, 40) after you have killed a particular monster in a dungeon for them (below). The **odd kings** (castles 33, 35, 37, 39) reward visiting a monument with Strength.
4. The space part (SPACE.EXE, entered by boarding the shuttle, tile 17, which must be bought in a transport shop after 3000 turns) is described in space-flight.md.

The tavern gossip (towns-shops-castles.md 2.6) hints at exactly this: "Thou must destroy at least 20 enemy vessels to become an ace", "the princess will give great reward ... and an extra gift to an 8th level ace", "thou must go back in time",
"thou should destroy the evil gem".

## 2. The quest state words `D_3AB2[0..8]` (3AB2, record 0x3A..0x4B)

Nine 16-bit words, index 0 unused, index `k` = king number 1..8 (the king of castle `c` is `k = c - 32`). Values:

| value | meaning | set by |
|---|---|---|
| 0 | no quest (initial, or the quest was completed and rewarded) | GEN (new character), F_78AF |
| -1 | the king has given you the quest and it is not done | F_7526 (the king, Transact > Service) |
| 1 | the objective has been achieved; go back to the king | F_5085 (called by the kill code or the monument code) |

* `F_767D()` = (`D_3AB2[k] == -1`): the king answers "Thou art on a quest for me already!" instead of giving it again.
* `F_5085(c)`: `if (D_3AB2[8 - c] == -1) { D_3AB2[8 - c] = 1; "A quest has been completed!" (sound 5) }`. It only counts if the quest was **accepted first**.
* `F_78AF()` (first thing in "Transact with king", after the checks "not here" and "rejects thine offer"): if `D_3AB2[k] == 1`: sets it to 0, shows "Ah, <name>!  Thou hast completed thy quest." and pays the reward (below) -- then returns 1 and the menu is skipped.
* After a reward the quest can be accepted again (value 0 -> -1) and rewarded again: the rewards can be farmed. Gem counts then exceed 1 (a non-zero count is all that is tested).

## 3. The eight quests

The text the king shows when you choose Service (F_7526; frame, "Go ... " lines, "Do not return until thy quest is done!", then "Press Space to continue:"):

| k | castle (place) | quarter | quest | objective code | reward |
|---|---|---|---|---|---|
| 1 | The Castle of Lord British (33) | NW | "Go forth and find the Grave of the Lost Soul" | visit place 48 (SE), F_50C6 with `c = 7` -> F_5085(7) | Strength |
| 2 | The Castle of the Lost King (34) | NW | "Go now and kill a Gelatinous Cube" | kill a dungeon monster of id 9 (levels 3-4) -> F_08EE(9) -> F_5085(6) | red gem |
| 3 | The Castle Barataria (35) | NE | "Go forth and find the Southern Sign Post" | visit place 46 (SW), `c = 5` | Strength |
| 4 | The Castle Rondorin (36) | NE | "Go now and kill a Carrion Creeper" | monster id 14 (levels 5-6) -> F_5085(4) | green gem |
| 5 | The Castle of Olympus (37) | SW | "Go forth and find the Pillar of Ozymandias" | visit place 44 (NE), `c = 3` | Strength |
| 6 | The Black Dragon's Castle (38) | SW | "Go now and kill a Lich" | monster id 19 (levels 7-8) -> F_5085(2) | blue gem |
| 7 | The White Dragon's Castle (39) | SE | "Go forth and find the Tower of Knowledge" | visit place 42 (NW), `c = 1` | Strength |
| 8 | The Castle of Shamino (40) | SE | "Go now and kill a Balron" | monster id 24 (levels 9-10) -> F_5085(0) | white gem + storeroom permission |

(The "find" places are looked up with `D_307C[42, 44, 46, 48]`; for odd `k` they are `42, 44, 46, 48` for `k = 7, 5, 3, 1`. The kill monster codes: F_08EE(id) maps ids 9, 14, 19, 24 to
`v = 2, 4, 6, 8` and calls `F_5085(8 - v)`, i.e. state index `v` = the even king number.)

How the objective is detected:
* **find**: F_50C6 calls `F_5085(c)` for odd `c` (monuments 42, 44, 46, 48) every time the monument is entered, whether or not it has an effect;
* **kill**: `F_0953` (a dungeon kill) calls `F_08EE(b)` with the killed monster's id; killing it with the spell Kill counts as well.

The rewards (F_78AF):
* odd `k` (F_769D): "For this I will give thee N point(s) of strength." with `N = (99 - Strength) / 8`, at least 1 unless Strength is already 99; `Strength += N`.
* even `k` (F_7738): the gem is chosen by the **quarter in which the castle lies** (`F_40FE` of the world position): 0 red, 1 green, 2 blue, 3 white; `D_20A4` / `D_3AC6` / `D_3AC8` / `D_3ACA` (record 0x4C, 0x4E, 0x50, 0x52) is incremented; and the king adds a hint:
  * NW (red gem): "For this I shall tell thee that it requires all four gems to launch a time machine! The king gives thee a red gem."
  * NE (green gem): "... that the time machine must be used in order to win. ... a green gem!"
  * SW (blue gem): "... the princess will help a space ace through time! ... a blue gem!"
  * SE (white gem): "... to take nine items from my storerooms - but only nine! The king also gives thee a white gem!" and `D_8BA4 = 9` (valid while you are in this castle; entering a castle resets it).
  A 10-tick pause (F_3E5E(10)) and "Press Space to continue:" follow.

## 4. The princess and the time machine

See towns-shops-castles.md section 8. In summary: leave a castle with the princess in a neighbouring cell -> +500 pence, +500 hit points, +500 experience; if you are an 8th-level ace she gives the time-travel hint and the machine appears.
The princess is released by unlocking her room (the key is the jester's). Her names are in `D_31CC`. Rescuing is repeatable (the same castle again after re-entering); each qualifying rescue adds one more machine (up to 9).

## 5. The eight monuments and other one-off bonuses

Entering a monument gives the stat gain described in world-and-map.md section 5 (a 10 % gain, not twice in a row). They also complete the "find" quests above. The Pillars of the Argonauts give a free weapon (the first one you do not own).
Donation squares in castles/towns give pence-for-benefit trades (towns-shops-castles.md 3). There is no experience for exploring: the only sources of experience are kills (overworld: `rand(2, 5K)` per monster; dungeon: `rand(2, L*L+10)`; town entities: +1, guard +15 in total; Space combat in SPACE) and the princess (+500).

## 6. Other counters in the record

| variable | address (record offset) | meaning |
|---|---|---|
| `D_20FE` | 3B1E (0xA6) | "Enemy vessels" destroyed in SPACE (shown on the Ztats screen when non-zero); needs 20 for the ace rank |
| `D_3B1A`, `D_3B1C` | 3B1A, 3B1C (0xA2, 0xA4) | shuttles and time machines owned (entries 6 and 7 of the transport counts `D_20EE[]`) |
| `D_3B20` | 3B20 (0xA8) | last monument visited (-1 at the start); entering the same one again gives no stat gain |
| `D_3B28`, `D_3B2A` | 3B28, 3B2A (0xB0, 0xB2) | shuttle fuel and shield (set to 1000 when the shuttle is bought) |
| `D_3B22`, `D_3B24` | 3B22, 3B24 (0xAA, 0xAC) | saved monster count and the 32-bit turn counter |
| `D_8BA8` (RAM) | 8BA8 | the turn counter in use; shops open stock at 1500 and 3000 turns; aircar and shuttle are sold after 3000 |

New character (set by GEN, from `GEN.C`): hit points 150, pence 100, experience 0, food 200, dagger readied (count 2), leather armour readied (count 1), spell Prayer, on foot, at (49, 40), sound on, `D_3B20 = -1`, all quest words 0, no gems, 0 for
everything else; the monster table holds only the player (`type 10, terrain 1, x 49, y 40`).
