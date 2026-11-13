# MONDAIN.EXE: the final battle (src/mondain/MONDAIN.C)

MONDAIN.EXE is started by OUT.EXE when the player boards a **Time Machine** (map tile 18) while owning all four
gems (OUT.C F_118D: `D_20A4 && D_3ACA && D_3AC8 && D_3AC6`; otherwise OUT prints "Thou canst not determine how
to operate the craft at this time."). It tells the story in four screens and then runs a small real-time arena
fight against Mondain. If the player dies it returns to OUT (with argument "M"); if the player wins it shows
the ending picture and never returns. One C module, 75 functions, rebuilds byte-identical; the code is 03E9-2230.
Record field names (`D_206E`, ...) are the MONDAIN addresses; see overview.md section 4 and save-format.md.

## 1. Start-up, `main` = F_1853

```
F_1D0D(1); D_2048 = (toupper(argv[1][0]) == 'C'); D_2E0C = argv[1]; D_2E0E = argv[2];
F_3F58(0);                                   /* colours; mode already set */
sz = 64 (CGA) or 128 (EGA/Tandy, D_082C != 0) /* bytes per 16x16 tile */
D_238C[0..127] = 0;                          /* tile 0: blank */
F_3E2F("inuse.u1", D_2058, 0x334);           /* the character */
F_3E2F(D_0184[mode], D_238C + sz*10, D_018A[mode]);   /* CgaTiles/EgaTiles/T1KTiles.Bin, first 0x2C0 / 0x580 bytes = 11 tiles */
F_3E2F(D_0156[mode], D_238C + sz,    0xA80 - sz);     /* CgaMond/EgaMond/T1KMond.Bin: 19 tiles at slots 1..19 */
D_204A = 5;  F_45A3();                       /* mode 5 (OUT uses 0..3), draw the status panel */
F_37FD(D_208A + F_3EC4());                   /* seed = record seed + clock */
F_1DE6();  F_13A3();                         /* four story screens, draw the arena */
```

Tile table (`D_238C`, 21 slots of `sz` bytes): the overworld tile file is loaded first at slot 10..20 and then
`*Mond.Bin` over slots 1..19, so what remains of `Tiles.Bin` is **slot 20 = its tile 10, the player's avatar**
(OUT's player-on-foot tile is also 10, OUT.C). `*Mond.Bin` is 1216 bytes (CGA, 19 x 64) / 2432 (EGA, T1K,
19 x 128), shorter than the requested `0xA80 - sz` bytes (2624 / 2560); `fread` is asked for one item of that size, so it
returns 0 but the file's bytes are in the buffer **(uncertain: depends on Lattice's partial-read behaviour, which
evidently works since the game does)**.

Tile slots used by the game (draw routine F_355D(tile, x, y), cell coordinates 0..18 / 0..8):

| slot | content |
|---|---|
| 0 | blank floor |
| 1 | blocked square ("Blocked!") |
| 2, 3 | Mondain, first form (two animation frames, `D_0240 + D_0242`) |
| 4 | Mondain dead (also the player's picture on the victory screen) |
| 5 | wall of force (burns when touched; also what the Create spell makes) |
| 6..10 | the evil gem (animation: `D_0190 + 5`, `D_0190` bounces 1..5) |
| 11, 12, 13 | Mondain, second form (`11 + D_0242`, `D_0242` cycles 0,1,2) |
| 14..19 | magic square animation (`14 + D_0130`, `D_0130` 0..5) |
| 20 | the player |

## 2. The story screens (F_1DE6, F_1B9B, F_1CAF, F_1D53, F_1D94)

Each is drawn in a framed window (F_047D, colour `D_0DA8`) and followed by F_3AE2 ("Press Space to continue:",
Space or Return).

1. "Entering the craft, thou dost remark upon four holes marked: R G B W. The proper gems fit in each hole.
   Further examination leads thee to two discoveries: There is a button marked LAUNCH, and thou art locked in, with
   nothing to do but press it!" (the four holes are drawn with character 4 in colours 12, 10, 9, 15 on EGA/Tandy,
   2, 1, 1, 3 on CGA: red, green, blue, white).
2. "As soon as thou hast pressed the large black button, lights begin to dance across the control screens.
   Generators shake the very floor beneath thy feet. The small craft begins to tremble and rattle as thy
   journey through time begins..."
3. "After but a few moments thou dost feel a strong magic pulling thee from thy craft..."
4. "...A moment later, thou art face to face with the evil Mondain himself! Good luck, this is it!"

## 3. The arena

The field is `D_0194[19][9]`, indexed `[x][y]` (x = 0..18 across, y = 0..8 down). Initial contents (tile numbers):

```
x=2,y=4: 1 (blocked)            player D_0126,D_0128 = (3,4), tile 20 stored at [3][4]
x=5..9:  5 at y=2 and y=6       (two walls of force, 5 squares long each)
x=11:    5 at y=1,2,3 and y=5,6,7
x=12,13: 5 at y=1 and y=7
x=14,y=4: 6 (the gem)           Mondain D_012A,D_012C = (15,4)
x=15,16: 5 at y=1 and y=7
x=17:    5 at y=1,2,3 and y=5,6,7
```

(Mondain's own square is not stored in the field; he is a separate object. `D_0126/D_0128` = player,
`D_012A/D_012C` = Mondain; the source comments for these are the right way round, the notes in `work/` are not.)

State variables: `D_012E` Mondain's hit points (initially **1000**); `D_0240` his tile (2 or 11 or 4), `D_0242` its
frame, `D_0244` his behaviour state (20 first form, 40 second form, 30 dead); `D_0246` "fight on" flag;
`D_0248` gem destroyed; `D_024A` victory; `D_0190/D_0192` gem animation (0 = not animating); `D_0130` magic
square frame (-1 = none), `D_2054/D_2056` its position; `D_204E` redraw flag; `D_204C` Escape flag (never read).

### Turn structure

```
for (;;) {
    n = 0;  F_387E(0,24);
    do { key = F_1B46();  n++;  F_1477(); } while (key == 0 && n < 1500);   /* animate while waiting */
    if (key == 0) key = ' ';           /* idle = Pass: the world moves on */
    dispatch(key);
    if (D_204E) F_13A3();              /* redraw whole arena */
    F_19C8();                          /* Mondain's state, magic squares, victory check */
    if (D_024A) { F_1F31(); D_204C = 1; }
    F_186F();                          /* monsters' turn */
    if (D_206E > 0 && D_2080 > 0) continue;
    F_138D();                          /* dead: HP and food = 0, defeat screen */
}
```

F_1B46 reads a key (upper-cased `F_413C`) and waits 1 tick (plus 1-2 more when idle; F_3CF1(4) while the gem animates);
F_1477 animates: the magic square frame, the gem (`D_0190`: 1..5 and back, tile `D_0190 + 5`), and Mondain's tile
(form 1: `D_0242 = (rnd(1,200) < 100)`, i.e. random 0/1; form 2: frame cycles 0,1,2).

Directions (codes 1..4 from the arrow keys): 1 left/West, 2 right/East, 3 up/North, 4 down/South (`D_1DC8[]` =
"", "West", "East", "North", "South"); F_1415 / F_1447 give the step: `dx`: 1 -> -1, 2 -> +1, 3/4 -> 0; `dy`:
3 -> -1, 4 -> +1, 1/2 -> 0. The numeric keys `8 2 6 4` act as up/down/right/left only for the Attack command.

### Commands

| key | function | effect |
|---|---|---|
| arrows | F_0696 | move one square |
| `A` | F_0A0D(-1) | Attack, then ask a direction (arrow key; any other key = "nothing") |
| `8` `2` `6` `4` | F_0A0D(3/4/2/1) | Attack up/down/right/left at once |
| `C` | F_0ED3 | Cast the readied spell |
| `G` | F_1057 | "Get (Gem)" |
| `I` | F_1124 | Inform and Search (hint about the gem) |
| `N` | F_0576 | toggle Noise (record offset 0x38) |
| `R` | F_43C8 | Ready weapon/armour/spell (shared routine, changes `D_2082/D_2086/D_2084`) |
| `Z` | F_478E | Ztats (shared), arena redrawn afterwards |
| space | F_0551 | "Pass" |
| `H` | F_11CC | "Hyperjump?" refused, **and sets `D_0190 = 1`** (starts the gem's bounce animation; the only place besides F_0696 that does) |
| `B D E F K O U V X` | refused | "Board?" "Drop?" "Enter?" "Fire?" "Klimb?" "Open?" "Unlock?" "View?" "Xit?" (each + beep) |
| `Q` | F_11F6 | "Quit...is not allowed!" |
| `S` | F_1204 | "Steal...he's watching, / That would be foolish!" |
| `T` | F_121B | "Transact...Mondain will / not negotiate!" |
| Esc, other | F_055F | "Huh?" + beep (Esc also sets `D_204C`, never read) |

### Moving, F_0696(dir)

Target `(x,y) = player + step(dir)`. Tile `t`: outside the field (`x < 0 || x >= 19 || y < 0 || y >= 9`) or Mondain's
square -> `t = 2`; else `t = D_0194[x][y]`.

* `t == 0`: print the direction name, move (the field gets tile 20 at the new square, 0 at the old one), sound 4.
* `t == 1`: "Blocked!", sound 0.
* anything else (force wall 5, magic square 14, the gem 6, the edge, Mondain): sound 0, F_4031, "Burned! N damage!"
  with `N = HP/10` (at least 1), sound 2, `HP -= N`, status redraw (F_4437), F_3FF9. You do not move.

Afterwards: if the gem is on one of the eight neighbouring squares (F_0FC7/F_0F7B: tile 6, strictly inside the
field `0 < x < 19`, `0 < y < 9`) and `D_0190 == 0`, then `D_0246 = D_0190 = 1`: **the fight starts and the gem
begins to animate** when the player first stands next to it.

### Attack, F_0A0D / F_09C0 / F_0944 / F_0831

F_0A0D sets `D_0246 = 1` (any Attack starts the fight), prints "Attack with <weapon>" using `D_0DF8[D_2082]`
(the readied weapon's name: hands, dagger, mace, axe, rope, sword, g sword, bow, amulet, wand, staff, triangle,
pistol, L sword, phazor, blaster) and the weapon's range `D_0CD2[D_2082]`:

| index | weapon | range |
|---|---|---|
| 0 | Hands | 1 |
| 1 | Dagger | 1 |
| 2 | Mace | 1 |
| 3 | Axe | 1 |
| 4 | Rope & Spikes | 0 |
| 5 | Sword | 1 |
| 6 | Great Sword | 1 |
| 7 | Bow & Arrows | 3 |
| 8 | Amulet | 0 |
| 9 | Wand | 0 |
| 10 | Staff | 0 |
| 11 | Triangle | 1 |
| 12 | Pistol | 3 |
| 13 | Light Sword | 1 |
| 14 | Phazor | 3 |
| 15 | Blaster | 3 |

Range 0: "?" + beep, no attack. Else ": " and a direction. Then F_0944:

```
range  = D_0CD2[weapon];  chance = Agility + 50;  damage = rnd(2, Strength + 3*weapon)    /* hi clamped to 255 */
F_0831(dir, sound 7, range, damage, chance)
```

(Agility = record 0x1A `D_2072`, Strength = 0x18 `D_2070`.) F_0831 steps `i = 1..range` along the direction
from the player and stops at the first square holding Mondain (his square counts as tile 2), or at the field edge
(`x > 0 && y > 0 && x < 19 && y < 9` must hold). **Walls of force do not stop shots.** If he was reached and
`rnd(1,100) <= chance`: `D_012E -= damage`, "Hit Mondain! N damage!", sound 2; otherwise "Missed!".

### Spells, F_0ED3 (Cast) and friends

The readied spell `D_2084` (record 0x2C; names `D_0F04[]`: 0 Prayer, 1 Open, 2 Unlock, 3 Magic Missile, 4 Steal,
5 Ladder Down, 6 Ladder Up, 7 Blink, 8 Create, 9 Destroy, 10 Kill). "Cast <name>"; if its charge count
`D_20D8[D_2084]` (record 0x80 + 2*n) is 0: "You've used up that spell!". Otherwise **a charge is spent first** (unless
`D_2084 == 0`; Prayer's count is -1 = unlimited), then only spells 3, 7, 8, 9, 10 do anything (F_0E56); every other
spell prints "Failed!" and beeps (so Open, Unlock, Steal, the ladders and even Prayer are charge-wasting no-ops
here). A spell that returns 0 ends with "Failed!" and sound 6.

Spell success test F_0618: `rnd(1,249) < Intelligence + 160` **or class == Cleric (1)** (so Intelligence = record
0x22, `D_207A`; clerics never fail; manual p.3). The C comment "agility" is wrong.

| spell | function | effect |
|---|---|---|
| 3 Magic Missile | F_0D08 | asks a direction; needs success **and** Mondain within Manhattan distance < 7 (F_064A); `F_0831(dir, 5, 5, damage, 500)`: range 5, always hits when it reaches him; `damage = rnd(2, Intelligence)` scaled by the readied weapon: Wand x2, Amulet x1.5, Staff or Triangle x3 (F_0A7A; the four magic artifacts of the manual) |
| 7 Blink | F_0D88 | on success: moves the player to a random empty square (`x = rnd(1,255) % 19`, `y = rnd(1,255) % 9`, until the field square is 0) |
| 8 Create | F_0B07 | asks a direction; on success puts a wall of force (5) on an empty square next to the player (the check `x != D_012A && y != D_012C` is stricter than needed: both coordinates must differ from Mondain's) |
| 9 Destroy | F_0BE4 | on success removes a wall of force (5) from the square in that direction |
| 10 Kill | F_0CAC | on success and Mondain within distance 7: `"INTERFICIO-NUNC!" The spell doth seem to make him stronger!` and `D_012E *= 2` if it is below 15000. **The Kill spell strengthens Mondain.** |
| 0 Prayer | F_0AD9 | never reached (see above). If it were: always "Hmmm...no effect!" because the test `if (F_0618)` tests the function's address (an original bug) |

An ordinary direction prompt reads one key; anything but an arrow gives "nothing" + sound 6.

### The gem, F_1057 (Get), F_1124 (Inform)

* `G`: needs the gem on one of the 8 neighbouring squares and not yet destroyed (`D_0248 == 0`), else "..'tis nothing
  here!". Then: `D_0248 = D_204E = D_0246 = 1`, tile at [14][4] cleared, `D_0190 = 0`; the player suffers
  **three quarters of the current hit points** ("... N damage!", `HP -= HP*3/4`, five sounds); "The Gem is
  DESTROYED!". If Mondain is already dead (`D_0244 == 30`) the victory flag `D_024A` is set.
* `I`: with the gem destroyed: "Mondain's magical aura doth / seem substantially diminished / in the absence of the
  gem."; otherwise "...it looks / as if he is creating the evil / gem!"

## 4. Mondain's behaviour

### State, F_19C8 (once per player turn)

1. **Magic squares.** If the fight is on, no square is active (`D_0130 == -1`) and `rnd(1,200) < 36` (about 17.5%):
   try up to 100 times to pick `x = (rnd(1,255)+rnd(1,255)) % 19`, `y = (rnd(1,255)+rnd(1,255)) % 9` on an empty
   square; accept it only if x differs from both the player's and Mondain's x and y differs from both their y's.
   The square gets tile 14 (burns when stepped on) and its animation `D_0130 = 0..5` (tiles 14..19) runs one step
   per animation tick (F_1477); at frame 6 it is cleared again.
2. **Form by hit points** (`D_012E`): `> 500`: first form (`D_0240 = 2`, `D_0244 = 20`). `1..500` and not yet in
   form 2: second form (`D_0240 = 11`, `D_0244 = 40`, frame 0). `<= 0` and not yet dead: "Mondain is dead!", then
   if the gem is intact "...or is he?" else `D_024A = 1` (victory); `D_0240 = 4`, `D_0244 = 30`.

So **both** the gem must be destroyed and Mondain reduced to 0 hit points to win, in either order. If he
is killed first he lies there (tile 4) and, while the gem still exists, **regains 25 hit points per turn**
(F_186F `D_0240 == 4`), coming back as form 2 and then form 1 once above 500: the player has to get to the gem
and take it (losing 3/4 of his hit points; keep them high) in that time.

### Monster turn, F_186F

```
if (!D_0246)  { if (rnd(1,200) < 17) "You hear a strange chanting!"; }      /* before the fight: idle */
else if (D_0240 == 4)  D_012E += 25;                                        /* dead: regenerates */
else {
   dx = D_012A - D_0126;  dy = D_012C - D_0128;  dist = |dx| + |dy|;       /* Mondain minus player */
   if (dist == 1 && D_0244 == 20)                       F_1664();           /* melee */
   else if (dist < 7 && rnd(1,255) < 128 && D_0244 == 20) F_16E0();         /* spell */
   else {
      if (D_0244 == 40) { if (dx == 0) dx = F_1565(); if (dy == 0) dy = F_1565(); }   /* second form: random sign */
      if (rnd(1,255) & 1) { moved = F_1589(0, sign(dy)); if (!moved) moved = F_1589(sign(dx), 0); }
      else                { moved = F_1589(sign(dx), 0); if (!moved) moved = F_1589(0, sign(dy)); }
      if (!moved && D_0244 == 40) D_012E += 5;          /* a stuck second form heals */
   }
}
```

F_1589(ddx,ddy) moves **Mondain** (not the player) by one square if the target is inside the field and its tile is
0. Note the sign convention as compiled: `dx`, `dy` are *Mondain minus player*, so these moves go **away from
the player**: the first form retreats (against the walls) unless it is melee-adjacent or casts; the second form
neither melees nor casts (it needs `D_0244 == 20`) but wanders and heals when blocked. (This is what the
byte-exact code does; whether the original design intended pursuit is unknown.) F_1565 returns `sign(rnd(1,255) -
128)` (0 for exactly 128).

* Melee, F_1664: "Attacked by Mondain!", sound 3. Hit unless `rnd(1,255) > (185 - Stamina/2 + 8*armour) * 2`
  (Stamina = record 0x1C `D_2074`, armour index = record 0x2E `D_2086`; with normal stats the right side exceeds
  255, i.e. he practically always hits). Damage `HP/32 + rnd(1,20)` via F_161E ("Hit! N damage!", sound 2, `HP -= N`).
* Spell, F_16E0: "Mondain casts ", `kind = rnd(1,238) % 3`:
  * 0 magic missile: sound 5; hit if `rnd(1,255) < (195 - (Wisdom + Intelligence/2)) * 2` (Wisdom = 0x20 `D_2078`);
    damage `rnd(1,100)`.
  * 1 mind blaster: sound 5; 30% (`rnd(1,200) > 140`): "Hit!  Stats are reduced!": for the six attributes
    (record 0x18..0x22): if > 16: `-= value/8`; else if >= 2: `-= 2`. **The reduction is permanent (saved in the
    record).**
  * 2 psionic shock: screen effect (F_4031/F_3FF9), sound 5; 30%: damage `HP/32` (`-254` if it exceeded 255).
  * a miss prints "Missed!".

Hit points and food are tested after every turn: `HP <= 0 || food <= 0` leads to F_138D. MONDAIN never reduces food.

## 5. Ending the program

* **Defeat**: F_138D sets `D_206E = D_2080 = 0` (HP, food), redraws them (F_4437, F_44A2) and calls F_12EC: "THOU ART
  DEAD!", the window text "Thou hast been defeated by Mondain the Wizard! ... THE UNIVERSE IS DOOMED!", wait 50
  ticks, write `inuse.u1`, and `execl("out.exe","out.exe", D_2E0C, D_2E0E, "M", 0)` in the usual retry loop. OUT then
  takes its death/resurrection path (`HP <= 0 || food <= 0` at start).
* **Victory**: `D_024A` (see above). F_1F31: F_1EA8 shows Mondain's remains (tile 4), a pixel at (50,7), forces
  sound on, "THOU ART VICTORIOUS!" at (11,8), three sound-5 beeps, waits 10 ticks, flushes the keyboard and waits
  for a key (10000 polls); then clears the screen and F_1E03(0x1A40, "nif.bin") loads `NIF.BIN` (6720 bytes) and plots
  it as a 1-bit-per-pixel 320-wide picture (MSB first; set bit = colour 15, or 3 on CGA; clear bit = 0; 168
  rows); waits 75 ticks and prints "Press CONTROL-ALT-DEL to Restart" at (4,24). **There is no code path back to
  OUT or to DOS after this** (the loop of F_1853 continues but nothing leaves the program; **uncertain** exactly how
  the original idles: `F_3E5E(0)` is called last and then the main loop runs on with `D_204C = 1`).
  The game does not write the record on victory.

## 6. Open points

* Sound numbers per event are listed in overview.md (all inferred from use).
* The meaning of `D_204A = 5` and of F_45A3 (status panel) is only inferred; both are Origin/OUT shared code.
* MONDAIN's `D_0190` initial value 0 means the gem is a static tile (6) until the player nears it, attacks or
  presses `H`.
