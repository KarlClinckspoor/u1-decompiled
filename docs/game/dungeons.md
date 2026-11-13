# OUT.EXE: dungeons (generation, first-person view, ladders, chests, traps, spells)

Sources: `OUTB.C` (F_33B0-F_3F7F view drawing, F_4013-F_4BB0 view + generation + movement, F_55B1, F_56F7, F_57DF, F_581A, F_58F8, F_59D7),
`OUTA.C` (F_0BAA, F_0C52, F_0953, F_13A8, F_1351), `OUTC.C` (F_7B63 chest), `src/origin/OOAC91.ASM` (feature sprites). DGROUP addresses are
real addresses of OUT.EXE. There are 36 dungeon entrances (places 49-84, world-and-map.md) but **one** generator: the place number is only used to print the name.

## 1. State

| variable | address | meaning |
|---|---|---|
| `D_204A` | 8B9A | mode, 3 inside a dungeon |
| `D_AD50` | AD50 | dungeon level 1..10 (0 = outside) |
| `D_AD3E`, `D_AD40` | AD3E, AD40 | player x, y in the level (0..10) |
| `D_AD52` | AD52 | facing: 1 West, 2 East, 3 North, 4 South |
| `D_AD54` | AD54 | accumulated bonus hit points (twice the experience earned in this dungeon) |
| `D_AD56` | AD56..B11E | the level: `struct cell {int a, b, c, d;} D_AD56[11][11]` (11 x 11 x 8 = 968 bytes, address `AD56 + x*88 + y*8`) |
| `D_B11E` | B11E (5 bytes) | which of the five monster types of the level are present |
| `D_204E` | AD44 | "redraw the header" flag |
| `D_8BA8` | 8BA8 | turn counter (still counted in dungeons) |

Cell fields: `a` = structure type (below), `b` = monster id or -1, `c` = 4 (chest) / 5 (coffin) or 0, `d` = the monster's hit points. During the monsters' turn `c` temporarily
carries +1000 as a "moved" mark.

Structure types `a`: 0 floor; 1 wall; 2 secret door (looks like a wall; Inform turns it into a door); 3 door; 6 ladder down; 7 ladder up; 8 force field ("three rungs" picture; blocks like a wall;
made and removed by the spells Create/Destroy). Types 1, 2 and 8 block movement; a door (3) can be entered unless the player already stands in a door (F_3F7F_O). The interior is
x, y = 1..9, the outer ring (0 and 10) is wall.

## 2. Entering and leaving

* **Enter** on a dungeon place: F_4BB0: mode 3, `D_AD50 = D_AD3E = D_AD40 = 1`, facing South (4), `D_AD54 = 0`, then F_4761. The start square (1,1) is a "ladder up" (type 7).
* The whole dungeon uses no persistent storage: the level is regenerated every time the player changes level.
* **Leaving**: Climb up (K) on an up ladder at level 1 -> level 0: if `D_AD54 + HP > 9999` the bonus is cut to `9999 - HP`; a non-zero bonus is added: "Thou dost gain N hit points";
  mode 0, the map frame is redrawn (F_3C46, F_3BF6) and you stand on the dungeon's entrance square on the overworld.
* Food is **not** consumed in dungeons (F_7E80 runs only in mode 0), but Gremlins halve it.

## 3. Generating a level (F_4761)

Called on entry and after each ladder. Steps, with `L = D_AD50`:

1. F_456D: clears the header and prints " Level  N" (arrows at columns 15 and 26 of row 0, number at (23,0)).
2. **Seed**: F_45C8 seeds the random generator with `F_37FD(D_3AAA + 5*x + 3*y + 17*L)` where `x`, `y` are the *world* coordinates of the entrance (`D_3AAC`, `D_3AAE`) and
   `D_3AAA` is a per-character seed stored in the record (offset 0x32, set by GEN to clock + Wisdom + class). Therefore the walls, doors, force fields and ladders of a
   level are always the same for the same dungeon and level of one character.
3. All 121 cells: `a = 0, c = 0, b = -1, d = 0`.
4. The outer ring (x or y equal to 0 or 10): `a = 1`.
5. For `x = 2, 4, 6, 8` and `y = 1..9`: `a = rand(1,4) - 1`, i.e. each cell in the even columns is floor, wall, secret door or door with probability 1/4 each. (The code first
   sets 16 pillars at (even, even) and then overwrites them in this step, so they play no role.) The odd columns 1, 3, 5, 7, 9 stay open: the level is five long
   north-south corridors joined through the even columns.
6. **Extra obstacles**: `m = 2L` ; for `k = 0..m` (that is `2L + 1` times): `d = 8` (force field) if `rand(1,255) > 160` (37 %), else 1 (wall);
   `i = rand(1,16) - 1`; the cell is `(D_1DC8[38+i], D_1DC8[54+i])` on odd levels and the transposed cell on even levels, from the 16 positions
   x = 4 8 5 2 8 1 5 4 6 1 3 7 3 9 2 6, y = 1 1 2 3 3 4 4 5 5 6 6 6 8 8 9 9 (tables at DGROUP 37D4 + 76 and + 108).
7. **Re-seed with the clock** (`F_37FD(F_3EC4() + 0x6F1)`): everything after this point differs on every visit.
8. **Chests and coffins**: `L + 1` attempts: `c = (rand(1,100) & 1) + 4` (4 chest or 5 coffin), at a random interior cell `(rand(10,99)/10, rand(10,99)/10)` (1..9, 1..9) if its
   type is not 1, 2 or 8.
9. **Ladders** (these override whatever is there): cell (1,2) is cleared to floor; on **odd** levels (7,3) = up ladder (7) and (3,7) = down ladder (6); on **even** levels (7,3) = down (6) and (3,7) =
   up (7). On level 10 the down ladder at (7,3) is removed (cleared). On level 1 the up ladder at (7,3) is removed and (1,1) becomes the up ladder (7), the exit.
   (Since the player keeps his coordinates when changing level, a ladder trip always arrives on the matching ladder of the next level.)
10. `D_B11E[0..4] = 0` and **three monsters** are placed by F_4602 (monsters.md).

The variables `D_ADB6`, `D_ADBE`, `D_AE96`, `D_AFD6` that the C source uses in step 9 are just the `a` fields of the cells (1,1), (1,2), (3,7), (7,3) (addresses `ADB6 = AD56+0x60`, `ADBE = +0x68`, `AE96 = +0x140`, `AFD6 = +0x280`).

## 4. Movement

Arrow keys in a dungeon (F_4B73): up (3) = **Forward** (F_4B16), down (4) = **Turn around** (F_4A38), left (1) = **Turn left** (F_4A82), right (2) = **Turn right** (F_4ACC); each prints its
name ("Forward", "Turn left", ...). Forward asks F_3F7F_O(dx,dy) for the cell ahead (it returns 1 = blocked): blocked if its type is 1, 2 or 8; if it is a door (3) while the player already stands in a door; or if it
holds a monster. A blocked move prints " - path blocked!" with sound 0; a successful one clears the line and moves (no sound, no message). Every command costs a turn and runs the monsters (combat.md).

Other commands in dungeons: Attack (weapon shot ahead), Cast (all spells), Ready, Z, Inform (search), Open (coffin), Unlock (chest), K-Limb (ladder), Noise, Quit refused ("only allowed outdoors!").
Enter, Board, X-it, Drop, Get, Steal, Transact, Fire answer with "?"/a beep.

## 5. Ladders

* **K-Limb** (F_581A) must be used on a cell with type 6 or 7 *facing north or south* ("Thou must face the ladder!" otherwise; on a plain floor cell " what?").
* Down (F_57DF): `D_AD50++`, " down to level N", regenerate (F_4761). Up (F_56F7): `D_AD50--`, " up to level N"; at level 0 the exit above, else regenerate and clear the monster in the player's cell.
* Spells can make ladders (below). Level 10 allows no down ladder (the spell refuses, generation removes it). There is no other limit on levels: the dungeon is 10 levels deep.
* The ladders drawn in the view are the side-ladder features (see 7).

## 6. Chests, coffins and traps

* A chest (`c == 4`) or coffin (`c == 5`) is shown in the view. It lies in a cell; to use it you must **stand in that cell**.
* **Unlock** (U, F_7CF2 -> F_7B63): `F_7B63(rand(1,75))`: " chest" and " - none here!" + beep if the cell has no chest. If `rand(1,75) > Agility` and the class is not Thief: "Thou hast set off a trap!"
  (sound 2) and `HP -= L`. Else the chest opens: `c = 0`, "Thou dost find: " + pence `rand(3, L*L + 9)` via F_0761. (So with Agility >= 75 or as a thief a chest is always safe.)
* **Open** (O, F_59D7) opens coffins: " coffin" and " - none here!" + beep if there is none. Then with probability `rand(1,255) < 104` (40 %), provided the cell **ahead** is not a wall/secret door and holds no monster, F_58F8
  puts a monster there (monster type from the level's group, hit points as usual; **the coffin stays unopened** and can be tried again); otherwise (and whenever those conditions fail) the coffin is emptied and pays `rand(3, L*L+9)` pence.
  The monster placed by F_58F8 needs the target cell to be floor or ladder type (0, 6, 7).
* Spells **Open** and **Unlock** (below) open a coffin / chest in the current cell without any risk.
* There are **no other traps**: no pits, teleporters or darts; the only risks are monsters and chest traps.
* A monster whose id is 11 ("Chest") is a Mimic: it looks like a chest (kind 11) in the view; the kill message says "Mimic".

## 7. Dungeon spells (F_13A8)

`F_13A8(spell, announce)`: with `announce` the spell's incantation is printed first (`D_0FD8[185 + spell]`, the pointer table at 2B56: "POTENTIS-LAUDIS!" Prayer, "APERTUS!" Open, "PECUNIA!" Unlock, "VASTO!"
Magic Missile, "NUDO!" Steal, "INFERUS!" Ladder Down, "ASCENDO!" Ladder Up, "DUCIS-EDUCO!" Blink, "STRUXI!" Create, "DELIO!" Destroy, "INTERFICIO!" Kill). Then, with `i, j` the cell ahead:

| spell | condition and effect |
|---|---|
| 1 Open | player's cell has a coffin (`c == 5`): prints "Coffin" and F_1351 (" opened!", `c = 0`, "Thou dost find: " pence `rand(3, L*L+9)`); else failure |
| 2 Unlock | chest (`c == 4`): "Chest" and F_1351 |
| 3 Magic Missile | `F_0BAA(5)` finds a monster up to 5 cells ahead (not through walls/doors); damage `F_12F2()` through F_0953 with hit 101 (always hits); failure if none |
| 5 Ladder Down | fails on level 10 or when the cell is a force field; allowed only if `x` or `y` is odd; the cell becomes type 6 "Ladder created!" |
| 6 Ladder Up | allowed if x or y odd and the cell is not a force field; type 7 |
| 7 Blink | picks random `(i, j)` in 1..9 x 1..9 until it is not the current cell, not wall/secret door/force field, and has no monster; moves the player there: "Teleported!" |
| 8 Create | the cell ahead must be floor (0) with no monster and no chest: becomes type 8, "Field created!" |
| 9 Destroy | the cell ahead is a force field: becomes floor, "Field destroyed!" |
| 10 Kill | the cell ahead holds a monster: killed outright (F_0953 with damage 10000); |
| 0, 4 | Prayer/Steal: no case; 4 only produces sound 5 |

Failure: sound 6 and "Failed!". Other spells play sound 5 (the damage spells play it through F_0953). Prayer in a dungeon picks one of these at random for free (combat.md).

## 8. The first-person view (F_414A and helpers)

The window is the rectangle x 8..311, y 8..151 (cleared with F_35CA(8,8,0x137,0x97,0) at each redraw); the compass caption (F_3CE8: " North ", " South ", " East  ", " West  " between the arrows at (16,19) and (24,19)) is
printed on row 19. The view is redrawn after every command (main loop) and the header flag `D_204E` makes it redraw the " Level N" line first.

Perspective: `D_3808` (DGROUP 3808, 0 72 108 126 135 144) are the horizontal and `D_3814` (3814, 0 36 54 63 68 72) the vertical inset of the wall rectangle at depth `i` = 0..5 (all in pixels):
at depth `i` the *front wall* rectangle runs from `x = 16 + D_3808[i]` to `303 - D_3808[i]` and from `y = 8 + D_3814[i]` to `151 - D_3814[i]` (so depth 0 is the whole window, depth 1 is 88..231 x 44..115, depth 2 is 124..195 x 62..97 ... depth 5 collapses to a point). Colours:
walls `D_0DA6`, force-field rungs `D_2044`, text `D_2046` (so, e.g., force fields use the highlight colour).

Algorithm (read from F_414A; the order of drawing is far to near):
1. Compute the left and right neighbour offsets from the facing (`a,c` left, `b,d` right) and `dist = F_40D4(dx, dy)`: the number of cells one can see ahead (1..5): scanning cell offsets (dx, dy), 2*(dx, dy), ... F_407E
   stops at a wall, secret door or door (types 1, 2, 3) or at a *visible* monster (any monster except ids 9 Gelatinous cube, 11 Chest/Mimic, 20 Invisible seeker, which F_4013 treats as see-through).
2. If the player stands in a wall-type cell (1 or 2, only by Blink/spell accidents) just the depth-0 wall is drawn. If the cell directly ahead is a wall/door while the player stands in a doorway, the wall is drawn (F_34B0(0)).
   If the player stands in a doorway, a doorway frame is drawn first (F_3A54(0)).
3. For `n = dist` down to 1: the left and right cells at depth `n` are looked at; their wall pieces are drawn with F_3F4C / F_3F19 (wall type 1/2 -> a wall edge: F_33B0 or F_3430 on the correct side; door (3) -> a door frame:
   F_37DB / F_390D; otherwise open: the side wall pieces F_355A / F_35DC); vertical edge lines (F_3B25/F_3B65) are added where the left/right wall appears or disappears between two depths; then the cell in the middle at that depth is drawn by F_3DF5.
4. F_3DF5(depth, x, y): if the cell holds a monster, F_3D6C: the wall behind it if the cell is a wall, a ladder side feature if it is a ladder, and the monster sprite via F_3BA5; otherwise by cell type: 1/2 front wall (F_34B0), 3 doorway (F_3A54),
   8 three rungs (F_3722), 6/7 ladder pieces (F_368F, F_36C0, F_365E, F_36F1 depending on whether you face N/S or E/W); then a coffin (c == 5, F_3BF5) or chest (c == 4, F_3C13) if present.
5. Finally the far end: if the last cell is open the next row's side walls are drawn (F_3F4C, F_3F19 at `dist + 1`), else the corner edges.

F_3BA5(depth, kind) prints the monster's name `D_3930[kind]` on the message line and draws its vector sprite with F_AC91(kind, depth, colour) (only for depth < 5; kind 9 (cube) is drawn as a plain wall outline). The names thus
appear in the message area as you look down a corridor. F_AC91 interprets the byte stream of the kind (`D_2480` table in DGROUP 2480; a byte stream of `x, y` pairs: a plain pair draws a line from the pen to the point, `0x7E x y` moves the pen to the point (and plots it), `0x7F` ends the stream; F_3812 draws the lines, F_3D61 plots), scaling coordinates by a right shift of 0..4 selected by the depth
(`D_24BC`), centred on x = 160 (the tables are in the Origin module OOAC91). Kinds: 0..24 monsters (same numbering as the monster ids), 11 chest, 25 coffin, 26..29 ladder side pieces.

The view uses `F_3F7F_O` (movement) and `F_4013`/`F_407E`/`F_40D4`/`F_411C` (visibility) from the source; the exact stroke data of the sprites is data in `OOAC91.ASM`.
