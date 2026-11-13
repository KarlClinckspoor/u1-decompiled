# OUT.EXE: monsters (overworld, dungeons, town entities): tables, spawning, movement

Sources: `OUTC.C` (F_7F0F spawn, F_80A2, F_8194, F_8479-F_8600 movement, F_86E1, F_873D, F_8A04, F_8AC9), `OUTA.C` (F_0742 names,
F_252C-F_2A73 entities, F_4602), `OUTB.C` (F_4761, F_4602, F_58F8), data tables read from `OUT.EXE` (addresses are real DGROUP addresses).
Combat numbers (hit chances, damage taken) are in combat.md; this file has the tables and behaviour.

## 1. Overworld monsters: the table and where it lives

Every overworld object is an entry of `D_3B2C` (DGROUP 3B2C, 40 entries x 16 bytes, saved with the character record): fields (words)
`type, terrain, x, y, hp, (3 unused)`. Entry 0 is the player; monsters have `type >= 19` (odd numbers 19..47; the next even number is the
second animation frame). `F_B502(type, terrain, x, y, hp)` appends an entry (if `D_8BA6 < 39`), `F_B58E(x,y)` removes one. A monster
is created with `terrain` = the terrain it stands on (F_8479 updates it when it moves). Names are looked up with `F_0742(type) =
D_0FD8[106 + (type-19)/2]`, the pointer table at **2AB8**.

The 15 monsters (`index = (type - 19) / 2`):

| idx | type (tile pair) | name | lives on | spawn pool (`lvl`) | reach | HP at creation (L = your level) | K (`D_368C[idx]`) | gold on kill | exp on kill | damage to you |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 19/20 | Ness creature | water | 0 | 1 | L + 41 | 10 | 11..50 | 2..50 | 1..21 |
| 1 | 21/22 | Giant squid | water | 1 | 1 | L + 61 | 5 | 11..30 | 2..25 | 1..11 |
| 2 | 23/24 | Dragon turtle | water | 2 | 3 | L + 81 | 4 | 11..26 | 2..20 | 1..9 |
| 3 | 25/26 | Pirate ship | water | 3 | 3 | L + 101 | 3 | 11..22 | 2..15 | 1..7 |
| 4 | 27/28 | Hood | grass/woods | 4 | 1 | L + 11 | 2 | 11..18 | 2..10 | 1..5 |
| 5 | 29/30 | Bear | grass/woods | 5 | 1 | L + 21 | 1 | 11..14 | 2..5 | 1..3 |
| 6 | 31/32 | Hidden archer | **woods only** | 6 | 3 | L + 41 | 4 | 11..26 | 2..20 | 1..9 |
| 7 | 33/34 | Dark knight | grass/woods | 7 | 1 | L + 61 | 6 | 11..34 | 2..30 | 1..13 |
| 8 | 35/36 | Evil trent | **woods only** | 8 | 1 | L + 81 | 8 | 11..42 | 2..40 | 1..17 |
| 9 | 37/38 | Thief | grass/woods | 9 | 1 | L + 21 | 10 | 11..50 | 2..50 | 1..21 |
| 10 | 39/40 | Orc | grass/woods | 10 | 1 | L + 41 | 1 | 11..14 | 2..5 | 1..3 |
| 11 | 41/42 | Knight | grass/woods | 11 | 1 | L + 61 | 2 | 11..18 | 2..10 | 1..5 |
| 12 | 43/44 | Necromancer | grass/woods | 12 | 1 | L + 81 | 4 | 11..26 | 2..20 | 1..9 |
| 13 | 45/46 | Evil ranger | grass/woods | 13 | 1 | L + 91 | 6 | 11..34 | 2..30 | 1..13 |
| 14 | 47/48 | Wandering warlock | grass/woods | 14 | 3 | L + 101 | 8 | 11..42 | 2..40 | 1..17 |

* "Spawn pool" is the value `lvl` of F_7F0F (below); `lvl` also indexes the difficulty table `D_3698` (3698: 4 6 8 10 | 1 2 4 6 8 | 2 4 6 8 9 10),
  the HP formula is `hp = F_40E9() + 1 + 10 * D_3698[lvl]` with `F_40E9() = experience/1000 + 1`; so the table column "L + n" is `L + 1 + 10 * D_3698`.
* The names of the plain data: Ness creature, Giant squid, Dragon turtle, Pirate ship, Hood, Bear, Hidden archer, Dark knight, Evil trent, Thief,
  Orc, Knight, Necromancer, Evil ranger, Wandering warlock (the "Hood" is a rogue peasant; see the manual's bestiary).
* `K = D_368C[idx]` (DGROUP 368C): **caution**: the table at 368C has only 6 words of its own (10 5 4 3 2 1); the code indexes it with `idx` = 0..14, so for idx 6..14 it reads
  the following bytes, which are the first nine words of the HP-factor table `D_3698` (4 6 8 10 1 2 4 6 8). The K column above is what the code actually reads
  (10 5 4 3 2 1 4 6 8 10 1 2 4 6 8). The resulting ordering (Ness creature and Thief have the highest rewards, Bear and Orc the lowest) looks accidental, but it is
  what the program does. The data after the 15 words of `D_3698` hold a 25-entry table (2 2 3 3 3 4 4 4 5 5 5 6 6 6 7 7 7 8 8 8 9 9 9 10 10) that nothing in OUT references.
* Gold on kill `rand(1, 4K) + 10`, experience `rand(2, 5K)` (F_0CF3, only for player kills), damage to the player `rand(1, 2K + 1)` (F_81FC).

Other tables:
* `D_36E8` (36E8: 100 50 33 100 50 25 10 10): per transport (foot, horse, cart, raft, frigate, aircar, shuttle, time machine) the **percentage chance that a
  monster's step succeeds** (see movement). The faster your transport, the slower the monsters.
* `D_16CC` (16CC): spawn thresholds, below.

## 2. Spawning (F_7F0F, overworld only)

Each turn in which the player is on the overworld and `D_8B90 <= 10` (at most 10 monsters), F_7F0F is called:
1. `r = rand(1,255)`; the turn spawns something if `r < D_16CC[experience / 1000]` where `D_16CC` = {35, 55, 75, 95, 115, 135, 155, 175, 195, 205}
   (index = thousands of experience, 0..9): chance per turn 13 % at the start growing to 80 % above 9000 experience.
2. `lvl = rand(0,149) / 10` (0..14, nearly uniform), `id = 2*lvl + 19`; the terrain `t` the monster needs: `lvl < 4` water (0), `lvl < 9` woods (2), else grass (1)
   -- but note that the pool value decides the *spawn square*; afterwards a "grass/woods" monster may walk on either.
3. The square: `x = rand(10,15)`, `y = rand(5,15)`, each negated with probability 1/2 (parity of `rand(1,16)`); this is an offset from the player's screen centre
   (9,4) and converted to world coordinates with wrap-around by F_C2EC/F_C31C, i.e. 10..15 columns and 5..15 rows away from the player, outside the visible area.
   The square is accepted if its terrain/object value equals `t` and it is inside the playable strip (F_7EB3: x mod 84 in 9..83; F_7EE1: y mod 78 in 6..77). Only
   one square is tried (the retry loop `while (n < 5 && k != t && !ok)` never increments `n` and ends as soon as *either* test passes; the monster is created only if both pass).
4. `F_B502(id, t, x, y, hp)`; `D_8B90++`.
Monsters are removed when killed, when the player leaves their quarter of the map (F_86E1 in F_066F) and on death (F_86E1 for all four).

## 3. Movement and attack choice (F_8600)

For each monster `i` from 1 to `D_8BA6` with type 19..47 (turn order = table order):
* `dx = playerX - x`, `dy = playerY - y`; if `F_8194(type,dx,dy) != 0` (within reach 1, or 3 for types 23, 25, 31, 47) the monster does **not** move but is queued to attack;
* otherwise `F_8598(i, dx, dy)` moves it: it tries one axis first, then the other. The axis order is chosen by `F_91D4(1,10) & 1`, the Lattice `rand()` whose seed is the
  fixed initial value 1 (`D_17A4`; OUT never calls srand) and whose low bit **alternates 0,1,0,1 on every call** (`seed = (seed * 0x0E5D + 3) & 0x7FFF`), so over the game the
  choices alternate globally. A step in the y direction (F_84D3) or x direction (F_8536) is attempted only if the offset on that axis is non-zero; it succeeds if `F_80A2` says so.
* `F_80A2(m, dx, dy)` (can monster `m` move so that its offset to the player becomes (dx, dy)?): first `rand(1,100) > D_36E8[transport]` fails the step (so your foot speed 100 %, horse 50 %, cart
  33 %, raft 100 %, frigate 50 %, aircar 25 %, shuttle/time machine 10 %). Then by type: types 19..25 (the four sea monsters) may enter only water (0); types 31 and 35 only woods (2);
  all others woods (2) or grass (1), and also any place square (terrain 4..9). The destination must be free (any monster/vehicle/the player there makes the value of F_C2AF(..,0) a type >= 10, which fails),
  and (except the place squares) inside the playable strip of the quarter (F_7EB3/F_7EE1). Mountains (3) block everybody.
* After everyone has moved, F_C3CA redraws the view and the queued attackers shoot in reverse queue order (combat.md section 4). A monster that moved this turn does not attack in the same turn.

## 4. Dungeon monsters

Each of the 11 x 11 cells of a dungeon level (`D_AD56`, 8-byte cells `{a,b,c,d}`) has `b` = monster id or -1, `d` = its hit points (`a` is the wall/floor type, `c` a chest/coffin).
Names are the pointer table `D_3930` (DGROUP 3930; the same table is used for the first-person view caption and for the kill/attack messages):

| id | name | levels where it appears | notes |
|---|---|---|---|
| 0 | Ranger | 1-2 | |
| 1 | Skeleton | 1-2 | |
| 2 | Thief | 1-2 | steals a weapon when it hits |
| 3 | Giant rat | 1-2 | |
| 4 | Bat | 1-2 | |
| 5 | Spider | 3-4 | |
| 6 | Viper | 3-4 | |
| 7 | Orc | 3-4 | |
| 8 | Cyclops | 3-4 | |
| 9 | Gelatinous cube | 3-4 | destroys armour; see-through in the view (F_4013); **quest monster of king 2** |
| 10 | Ettin | 5-6 | |
| 11 | Chest (a Mimic) | 5-6 | drawn as a chest; messages say "Mimic" |
| 12 | Lizard man | 5-6 | |
| 13 | Minatour (sic) | 5-6 | |
| 14 | Carrion creeper | 5-6 | **quest monster of king 4** |
| 15 | Tangler | 7-8 | |
| 16 | Gremlin | 7-8 | steals half the food, no damage |
| 17 | Wandering eyes | 7-8 | |
| 18 | Wraith | 7-8 | |
| 19 | Lich | 7-8 | **quest monster of king 6** |
| 20 | Invisible seeker | 9-10 | see-through in the view |
| 21 | Mind whipper | 9-10 | halves Intelligence (+5) with probability 1/2 per hit |
| 22 | Zorn | 9-10 | |
| 23 | Daemon | 9-10 | |
| 24 | Balron | 9-10 | **quest monster of king 8** |

The rule (F_4602, F_58F8): the monster id is `q = (L - 1) / 2 * 5 + cnt` with `cnt = rand(0,49) / 10` (0..4), so each pair of levels uses a group of five ids as shown; `L` = dungeon
level `D_AD50`. The flags `D_B11E[0..4]` (B11E, 5 bytes) remember which of the five types of the current level are already on the board: a type is not placed twice
(F_4602 loops until it finds a free flag; F_58F8 gives up after 50 tries); F_4761 clears the flags and places **three** monsters; each kill clears `D_B11E[id % 5]` and places one new monster
(F_4602); opening a coffin may put another one next to you (dungeons.md). Hit points at creation: `rand(1, L*L + 1) + 10 + id`. The placement square (F_4602) is a random cell of the interior (`rand(0,242) % 9 + 1` for x and y,
up to 500 tries) that has no monster, is not a wall/force-field (a = 1, 2, 8) and has no chest (c != 4), and is not the player's cell.

Dungeon monsters have **no ranged attacks and no special movement**. Their turn (F_8AC9) goes over the cells with x = 1..9 (outer), y = 1..9 (inner): for every monster not moved this turn
(`c < 999`; moving adds 1000 to the destination's `c`, removed at the end of the turn) with Manhattan distance `d = |dx| + |dy|` from the player: `d == 1` attacks, `1 < d < 7` steps one square towards the player:
first along x (`F_8A04(i, j, sign(dx), 0)`), and if that did not succeed along y; a step needs the target cell not to be a wall (1), secret door (2) or force field (8) and to hold no monster (doors, ladders and chests
are fine). Quirk: the variable that remembers whether a step succeeded is not reset between monsters, so after one monster moved, the next monster with `dx == 0` does not try its y step. After processing a monster's turn the
inner loop breaks if hit points or food are <= 0.

## 5. Town and castle entities

The 15 slots `D_ACC6[i] = {type, x, y, hp}` (loaded by F_20FC from the level's template at 31DC, world-and-map.md) are moved by `F_2A73`, called from the monsters'
turn, **only** while the previous mode equals the current mode (the first turn after entering just draws them):

| type | who | behaviour (function) |
|---|---|---|
| 17 | guard (hp 500) | stands still. If `D_AD46` (hostile) is set: moves towards the player until adjacent (F_286B; |dx|+|dy| must be < 13), then attacks (F_294E, combat.md) |
| 19 | bard (town, "Iolo the Bard") / jester (castle, "Gwino the jester") | F_252C: if not hostile, moves randomly (each of dx, dy = `rand(1,215) % 3`, 2 meaning -1) onto free squares. If it tries to move onto the player's square it **steals**: the highest-numbered weapon (index 15 downward) that you own and do not have readied loses one piece; the loss is announced ("Iolo stole something!" / " The jester stole something!") only if `rand(1,255) < Agility + 128`. If it stole nothing it sings with probability 15/255: "Iolo the Bard sings: Ho eyoh he hum!" or "Gwino the jester sings: I've got the key!" |
| 20 | king (hp 2000) | stands on his throne square (castle) |
| 21 | merchant (hp 1) | stands still |
| 22 | princess (hp 1) | F_2776: not hostile: wanders randomly; hostile: moves away from the player (x first or y first with equal probability) |
| 50 | wench (male player) / lecher (female player) | F_266C: wanders randomly (a type-50 entity next to the player is what makes the pub's "seduced" event possible, F_619C) |

Entities move onto cells for which `F_23E7` returns -1 (free floor, tile 1 or a special square) or 63; the player is type 18 and blocks. F_2203 gives -1/0/1 with roughly equal probability.
Nothing spawns in towns and castles; killed entities (type set to -1, x set to 127 for the F_2B6D kills) are gone until the next visit.
