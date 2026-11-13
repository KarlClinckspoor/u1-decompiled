# SPACE.EXE: space flight and combat (src/space/SPACEA.C, SPACE4.C, SPACE5.C, SP3330.ASM)

SPACE.EXE is started by OUT.EXE when the player boards a Space Shuttle on the overworld (map tile 17). It
is a self-contained flight game: a 7x7 galaxy of sectors (the manual: "The starways are divided into 49
sectors on a 7 x 7 grid"), a top view of the current sector, a front view for combat, docking at star bases,
hyperjumps, and landing back on Sosaria. It ends by writing `inuse.u1` and starting OUT again (landed or dead).
The game part is code 03E9-3DED of a 39824-byte EXE; everything is C except F_3330 (alien turn in combat), which
is kept as the original assembly (the C version does not reproduce its register use). The EXE rebuilds
byte-identical. Naming: see the caveat in overview.md section 4 (`D_2058`, `D_206E`, ... are MONDAIN names for
the record fields; the DGROUP of SPACE has the record at 5ADC).

Manual cross-check (page 9-10 "Starwalking"): thrust and retro, turn left/right in the cockpit, climb/dive in
the front view; "Inform" gives a long-range scan; Hyperjump to the next sector in the direction of travel;
docking "at any of the unused docking ports", a fee, then a "Base Command" query for "the direction toward the next
vehicle"; "Only the shuttle craft has heated shields. Any vehicle will incinerate if it collides with a star";
once combat has begun the pilot cannot return to the top view until the enemies are gone or the pilot hyperjumps;
"Changing from front view mode to top view mode at high speeds will surely result in a fatal collision"; "A ship
without fuel drifts forever and a depleted shield spells certain death." The code confirms all of these except
the "fatal collision" remark (the speed conversion is described below; no special death was found).

## 1. Views and terminology

| manual | code | `D_01F2` |
|---|---|---|
| top view (sector seen from above, ships drawn as sprites) | F_259E draws the top view | 0 |
| front view (cockpit: star field, crosshair, enemy ahead) | F_258D/F_1DDA star field, F_1BE9 crosshair | 1 |

Screen: the cockpit frame F_23F3 (bars at x 8..15 / 304..311, y 8..15 / 144..151, colour 7 or `D_0DA6` on CGA, with
black rivets), status text "Shld:" at text (31,21) and "Fuel:" at (31,22), values at column 36 (F_06FB shield,
F_0682 fuel, printed with 4 digits, colour `D_2044` below 100 else `D_2046`), messages in the bottom line via F_3B46.

## 2. Data structures

### Galaxy: `D_26C0[7][7]` (0xB1A = 2842 bytes at DGROUP 26C0, indexed `[sx][sy]`)

```
struct obj    { int x, y, type, fuel, shield, dir; };            /* 12 bytes */
struct sector { int count;            /* enemies in the sector (0..3)             */
                int star_x, star_y;   /* 0 = no star                              */
                int base_x, base_y;   /* 0 = no star base                         */
                struct obj o[4]; };   /* ships; type -1 = empty slot  (58 = 0x3A)  */
```

In the C sources the `obj` fields are called `f6`, `f8` and the sector fields `f2..f8`; **the C comments that call
`f6` the shield and `f8` the fuel are reversed**: `f6` is fuel (printed beside "Fuel:", spent by F_095F, "Not enough
fuel!") and `f8` is the shield (spent by F_08C0, "Thy shield is drained!"). I use `fuel`/`shield` here. The
sector (3,3) fields are also referenced by the stand-alone symbols `D_2C30..D_2C5A` (26C0 + 0x570).

`dir` (facing): 0 = left (x decreasing), 1 = right, 2 = up (y decreasing), 3 = down. F_063A returns the
opposite direction (0<->1, 2<->3). The current sector is `D_01EE, D_01F0` (initially 3,3), the player's ship is
`o[D_5E14]` of it (F_088E returns its shield, F_092D its fuel).

Positions are in screen pixels. Top view: ships keep x in 20..275, y in 10..130 (F_34A8/F_34D9 wrap at the
edges); a sector's star/base coordinates are 50..200 x 50..120 (random below).

### Ship types

| type | fuel at refuelling (F_0774) | shield (F_07AB) | exhaust sprite function | remarks |
|---|---|---|---|---|
| 0 | 1000 | 1000 | F_281F | the **Space Shuttle**: the only type that may land (F_2731) |
| 1 | 5000 | 1000 | F_2EC1 | |
| 2 | 2500 | 5000 | F_2B7B | |

(The names of types 1 and 2 are not in SPACE.EXE: the string table that names OUT's vehicles lists only "Shuttle"
for the space program.) F_0774/F_07AB return 0 for any other value.

### Docking ports

A star base sits at (`base_x`,`base_y`) and has four docking ports numbered by the directions the pilot gives:
0 left, 1 right, 2 top, 3 bottom (names `D_0E18[] = "left","right","top","bottom"`). A ship sitting in port `k`
has position and facing

```
x   = D_0AEE[k][type] + base_x       D_0AEE = { {-17,-15,-15}, {18,18,18}, {0,1,2}, {0,1,2} }
y   = D_0B06[k][type] + base_y       D_0B06 = { {1,1,1}, {1,1,1}, {-17,-16,-16}, {18,17,17} }
dir = opposite(k)                     (headed into the port)
```

(F_07E2, F_0838, F_063A). F_3741(ship) returns the port number whose position and facing the ship matches
(-1 = not docked); F_37E0(k) returns the index of the ship sitting in port k (-1 = none).

### Sprites

`CgaSpace.Bin` etc. (7296 bytes) is loaded at DGROUP 31DA (right behind the galaxy), `CgaFight.Bin` (3192 bytes)
at 4E5A (SPACE.EXE F_1853: `D_2C5C + 1406`, `D_4C67 + 499`).

* Space bank (Origin routine F_603E(type, dir, x, y), module OS603E): sprite number `type*4 + dir`, each 0x260 =
  608 bytes = 19 rows with pre-shifted copies selected by the low bits of x (CGA 4 shifts x 8 bytes per row, EGA
  8 shifts x 4, Tandy 2 shifts x 16). It **XORs** the sprite onto the screen (so a second call at the same place
  erases it) and returns 1 when it hit pixels that were already lit (collision detection!). The galaxy objects
  are drawn only with this routine.
* Fight bank (F_614E(x, y, kind, draw), same module): 7 kinds (apparent sizes 0..6) x 0x1C8 = 456 bytes, 19
  rows. `draw` 1 = OR onto the screen, 0 = erase.
* F_68F8 (module OS68F8) plots a pixel through the viewport clip; F_27EE(x,y,col) additionally clips to
  20<x<300, 18<y<141. F_281F / F_2EC1 / F_2B7B (called through F_3109) draw a few coloured pixels at the rear
  of a ship by facing: they are the **engine exhaust/thruster lights** (the C comment says "alien ship": wrong).
* The pictures in DATA: `D_022E` 19x18 (the star: values 1/2 = two colours; colours 12/14 or `D_0DA8`/`D_0DA6`
  on CGA, F_0425), `D_04DA` 20x20 one colour (the star base, F_04CA), `D_07FA_S` 21x18 four colours (the planet
  Sosaria, F_0524, colours 1,10,7 or `D_2044,D_0DA8,D_0DA6`).

## 3. Start-up, `main` = F_1853

1. `F_1D0D(1)`; force-CGA flag `D_2048`; remember `argv[1]`, `argv[2]` (`D_5E10`, `D_5E12`).
2. `F_3F58(0)`: colours, no mode set.
3. Load `inuse.u1` into the record (820 bytes), `D_0166[D_082C]` (Space bank), `D_0194[D_082C]` (Fight bank):
   file names `CgaSpace.Bin`/`EgaSpace.Bin`/`T1KSpace.Bin` and `...Fight.Bin`.
4. F_3A4E: lift-off countdown: at (1,24) print 10, 9, ... 1 separated by "..", one step per 18 ticks or until a
   key is pressed; then "Thou hast lifted off!".
5. `D_5AD2` (accent colour of ship sprites) = `D_0DA8` (CGA) or `D_2044`; print "Shld:" / "Fuel:"; `D_5FFE = 2`.
6. F_0D88 builds the galaxy (below), then `F_37FD(seed + F_3EC4())` reseeds randomly, F_1DDA (star field),
   F_259E (draw the top view), F_0682/F_06FB (status values).
7. Main loop (below).

### The galaxy: F_0D88 and F_09AF

The random generator is seeded with `F_37FD(D_5B0E * 3 / 2)` where `D_5B0E` is the **per-character seed in the
record (offset 0x32)**, so a character always meets the same galaxy (the seed is created by GEN, character-
creation.md). Then `MEMSET(D_26C0, 0, 0xB1A)` and F_09AF(sx,sy) for all 49 sectors:

```
o[0..3].type = -1
count = F_4069(0,240) / 40;  if (count > 3) count = 0        /* ~ P(0)=1/2, P(1..3)=1/6 each */
star: if (F_4069(1,200) < 77) { star_x = rnd(50,200); star_y = rnd(50,120); }   /* ~38% */
base: if (F_4069(1,200) < 77) { x = rnd(50,200); y = rnd(50,100);
          if (|x-star_x| < 40 || |y-star_y| < 40) x = y = 0; }                  /* ~38%, else none */
if (base) {                                    /* ships docked at the base */
    n = rnd(0,98) % 3;                         /* 0, 1 or 2 ships */
    if (n) { e = rnd(0,199) % 4;  t = rnd(0,98) % 3;
             o[0] = { type t, fuel F_0774(t), shield F_07AB(t), x,y of port e, dir opposite(e) };
             if (n == 2) { second ship of a different type (loop until != t) in a different port (until != e) } }
}
```

Then F_0D88 **overwrites sector (3,3)** (home sector, Sosaria's orbit) with fixed contents:

| field | value |
|---|---|
| count | 0 |
| star | (140, 70) |
| base | (200, 100) |
| `o[0]` | type 2 docked in port 3 (bottom): x 202, y 117, dir 2, fuel 2500, shield 5000 |
| `o[1]` | type 1 docked in port 0 (left): x 185, y 101, dir 1, fuel 5000, shield 1000 |
| `o[2]` = player ship (`D_5E14 = 2`) | type 0 (Shuttle) at (260, 90), dir 0, **fuel `D_5B8C`, shield `D_5B8E`** (record offsets 0xB0/0xB2: the shuttle's state from the last landing or purchase; OUT sets both to 1000 when the shuttle is bought) |

The planet Sosaria is drawn at (60,75) in sector (3,3) only (F_259E: `F_0524(60,75)`).

### Sector map symbols (Inform, F_142B)

Priority: enemies (`count > 0`: two vertical bars and a crossbar, F_127E) > base (a plus sign, F_12F1) > star (a
diamond, F_1349) > empty (five dots, F_13B3). Cells are 10 pixels apart starting at (132,46) (`px = i*10 + 0xA5 -
0x23 + 2`). F_14E5 ("Inform and Search") clears the window, writes "Sector Scan:", draws the 7x7 map in a frame
and blinks the current sector (erase 8 columns / redraw with 7-tick waits) until a key is pressed; then it
redraws the view.

## 4. The main loop

```
for (;;) {
    if (shield == 0) { D_5B8E = D_5B8C = 0; show; F_0F25(); }           /* dead */
    n = 0;  F_387E(0,24);                                               /* command marker */
    do { F_386B();                       /* one turn of the world */
         c = toupper(F_413C(D_2046));  n++;
         if (D_01F2 == 0) F_3E5E(2);     /* top view: pace of 2 ticks */
    } while (c == 0 && n < 1500);
    if (c == 0) c = ' ';                 /* no key = Pass */
    ... dispatch ...
    if (D_0134) F_259E();                /* redraw view */
    if (D_012E) { D_012E = 0; if (D_012C && D_01F2 == 0) D_0DE0 = !D_0DE0; }
}
```

Commands (key -> function; "refused" = print the name with `?` and beep, F_19BD):

| key | function | effect |
|---|---|---|
| arrows (codes 1 left, 2 right, 3 up, 4 down) | F_1919 | top view: F_16D3 (turn / thrust / retro); front view: move the crosshair |
| `1`..`8` (front view only) | F_165C | set speed n |
| `F` | F_163C | top view: refused "Fire?"; front view: F_2252 fire |
| `H` | F_2003 | Hyperjump |
| `I` | F_14E5 | Inform and Search (sector map) |
| `N` | F_3983 | toggle "Noise" on/off (record offset 0x38) |
| `V` | F_10B3 | View: top <-> front |
| `Z` | F_478E | Ztats screen (shared routine) |
| space (or no key) | F_1A97 | Pass; in front view re-centre the crosshair |
| `A B C D E G K O Q R S T U X` | refused | "Attack?" "Board?" "Cast?" "Drop?" "Enter?" "Get?" "Klimb?" "Open?" "Quit?" "Ready?" "Steal?" "Transact?" "Unlock?" "X-it?" |
| Esc | | sets the word at DGROUP 01F4, then "Huh?" (the word is never read in SPACE) |
| anything else | F_0F0E | "Huh?" + beep |

## 5. Top view mechanics

### One turn, F_386B

If `D_01F2 == 1` it just runs F_3330 (front view). Otherwise, for the player's ship `o` (facing `d`):

```
moved = (D_6000 != d) || D_0128 || D_012A           /* turned or has velocity; D_6000 = facing last drawn */
if (moved || D_012C) {
    if (moved)  r = F_603E(o->type, D_6000, o->x, o->y);          /* XOR-erase old image */
    if (D_012C) F_3109(o, 0);                                      /* erase exhaust */
    if (moved) { o->x = F_34A8(o->x);  o->y = F_34D9(o->y);        /* add velocity, wrap */
                 r = F_603E(o->type, d, o->x, o->y);  D_6000 = d;  /* draw; r = overlap */
                 if (r) F_350A();                                  /* collision */
                 else if (F_3741(D_5E14) != -1) { F_3567(); D_012C = 0; } }   /* docked */
    if (D_012C) F_3109(o, 1);                                      /* redraw exhaust */
}
```

`D_0128`, `D_012A` are the velocity components (pixels per turn) and `D_012C` is a counter of exhaust frames.
F_34A8: `x += D_0128; if (x > 275) x = 20; if (x < 20) x = 275`. F_34D9: `y += D_012A`, wrap 130 -> 10, 10 -> 130.
The ship wraps around the sector window; the sector only changes by Hyperjump.

### Flying: F_16D3(k), k = 1 Counter-Clockwise, 2 Clockwise, 3 Thrust, 4 Retro

```
cost = 5; delta = -1;  print "Counter-Clockwise"/"Clockwise"/"Thrust"/"Retro"  (D_01C4[k])
if (k == 1 || k == 2) cost = 2;
if (fuel == 0)  { "No fuel!!" "Wilt thou drift forever?!?" }
else {
  if (fuel < cost) cost = fuel;
  if (docked && k != 4) { "Crunch!"; sound 0; shield -= shield/4 + 5;      /* F_0910 */
                          if (shield == 0) { "Thy shield is drained!"; F_0F25(); } }
  else {
    fuel -= cost (F_095F);  d = o->dir;
    if (k is a turn) { if (D_012C) F_3109(o,0); o->dir = D_0BD2[k-1][d]; delta = D_012C = 0; }
    else if (k == 3) delta = 1;
    switch (d) { case 2: D_012A -= delta; case 3: D_012A += delta; case 0: D_0128 -= delta; case 1: D_0128 += delta; }
    clamp D_0128, D_012A to -9..9
    if (k == 3) D_012C += 10; else if (k == 4) { F_3109(o,0); D_012C = 0; }
  }
}
```

Turn table `D_0BD2`: counter-clockwise: left->down, right->up, up->left, down->right ({3,2,0,1}); clockwise: left->up,
right->down, up->right, down->left ({2,3,1,0}). Thrust adds 1 to the velocity component along the facing,
retro subtracts 1 (so retro first slows, then reverses). Costs: turn 2, thrust/retro 5 fuel. A turn does not
change the velocity: the ship keeps drifting along the old vector while pointing elsewhere (inertial flight).

### Crashes, stars, planet: F_350A, F_2665, F_2731

On pixel overlap F_350A runs F_2665 and F_2731 first:

* F_2665: if the sector has a star and the ship is within 24 pixels of it in both x and y:
  "Thy ship melts near the star!" and death (F_0F25).
* F_2731: only in sector (3,3): ship within 24 pixels of (60,75). If `type == 0`: "Thou hast landed safely!",
  `D_5B8C = fuel, D_5B8E = shield` (record offsets 0xB0/0xB2) and F_0EBD (leave to OUT). Otherwise: "Only space
  shuttles are heat / shielded for landings!", `D_5B8C = D_5B8E = 0` and death.
* otherwise: "Crunch!", sound 0, shield -= `shield/4 + 5`; if shield is 0 "Thy shield is drained!" and death; else
  both velocity components are negated (bounce).

(Only a pixel overlap triggers any of these; ships overlap each other, the star, the base and the planet through
their sprites.)

### Docking, F_3567

Reached when the ship is exactly in a port position with the matching facing (F_3741) and no overlap. Velocity is
zeroed. `n` = number of ships in the sector.

1. "Docked!  Welcome to base!"
2. If armour `D_2086 < 4` (anything below the Vacuum suit; record offset 0x2E): "The spacedoors open, exposing / thee
   to a complete vacuum!" and death.
3. If coins `D_207C < 500` (record 0x24): "Sorry, but thou canst not / refuel thy ship without / the 500p docking
   fee." and a key wait; no refuelling.
4. Else coins -= 500, the ship is refuelled/repaired: `fuel = F_0774(type)`, `shield = F_07AB(type)`.
5. If `n > 1`: "Choose thy ship: " (key by F_4161: arrow keys give 1..4): `c = key - 1` selects port c;
   the ship sitting there (F_37E0) becomes the player's (`D_5E14`, `D_6000 = its dir`) with "Thou art in the
   <left|right|top|bottom> ship."; "That port is not available." if empty; repeats until valid.
6. Redraws status values; F_4558 (coins).

This is the manual's "Base Command" query. A ship that stays docked cannot turn or thrust (it "Crunch"es) but may retro
away; View "can't be changed whilst in space dock."

### View command, F_10B3

* Top -> front (only if not docked, fuel > 0): `D_0134 = D_01F2 = 1`; speed `D_0126 = ceil(sqrt(vx^2+vy^2))`
  (F_03E9 counts odd numbers 1,3,5,... that fit: the C comment "largest square" is wrong), capped at 8; if the
  velocity is zero the speed becomes 1 and the velocity is set to +-1 along the facing (dir 0: vx=-1, 1: vx=+1,
  2: vy=-1, 3: vy=+1). `D_5E18 = D_0126` remembers the entry speed.
* Front -> top (only if the sector's `count == 0`; else "Thou must eliminate all enemy craft first!"):
  velocities are scaled by `k = D_0126 * 1000 / D_5E18` (F_1031/F_1072): `v = round(v * k) / 1000`, with F_0FFD
  rounding to the nearest 100 then nearest 1000, positive values capped at 6; if both are zero the facing
  gives a velocity of 1. So a speed change made in the front view carries over.

Initial values: `D_0126 = 5` (speed), `D_0128 = D_012A = 0`, crosshair `(D_013A, D_013C) = (0x9F, 0x4F) = (159, 79)`.

## 6. Hyperjump, F_2003

Only in the **front view** ("Hyperjump can only be made / from front view.") and with at least 100 fuel
("Not enough fuel!"). Sequence: `D_0DE2 = 20`; "..."; seed `r = F_3EC4() | 0xBBBB`; remember speed `x`;
`n = 10` if `x < 4` else 15; `D_0C9C = 1` (jumping: no enemy is shown and enemies grow faster). Accelerate:
for speed `j = x..8`, `n+1` frames of F_3330 each. Speed 0, erase the stars, print "Lightspeed!"; reseed with `r`,
new star field, `D_5FFE = 1` (stars are not erased: streaks), 201 frames; reseed, `D_5FFE = 3`, 200 frames;
`D_5FFE = 2`, decelerate `j = 8..x+1` (frame + delay F_3CF1(D_0C9E[j])); crosshair redrawn; " completed.";
fuel -= 100; reseed `F_3EC4() + 1234`; speed restored to `x`; F_4224 (flush keys); then **F_1E1C**:

```
dx = sign(D_0128), dy = sign(D_012A); if both 0 use the facing (0: dx=-1, 1: dx=+1, 2: dy=-1, 3: dy=+1)
new sector = (sx+dx, sy+dy) with wrap 0..6 (the galaxy is a torus)
copy the ship record o[D_5E14] to the first free slot (type == -1) of the new sector; mark the old slot -1
(no free slot: F_3DD6 "Abnormal program termination", _exit(3))
ship position = (260, 90); if the new sector has enemies, F_254B() sets up one
```

`D_012C = D_0C9C = 0`. The speed table `D_0C9E = {0,8,7,6,5,4,3,2,1}` is the frame delay per speed level
(speed 1 = delay 8, speed 8 = delay 1).

## 7. Front view and combat

### Star field, F_1C2A / F_1DDA / F_1B21

40 stars, offsets `(D_5E1A[i], D_5E6A[i])` from the crosshair centre and screen positions `(D_5EBA[i],
D_5F0A[i])`. Each frame the offset grows (`dx += dx/8` (at least 1 step), `dy += dy/6`) so the stars stream
outwards; a star leaving the viewport (20<x<300, 20<y<140) is replaced by F_1B21: quadrant `rnd(0,255)/64`, `dx =
rnd(2,255)/2`, `dy = rnd(4,255)/4`, signs by quadrant (`5000` = off screen if it would start outside). The
crosshair (`D_013A`, `D_013C`, velocities `D_0136`, `D_0138`) moves with the "Left/Right/Climb/Dive" commands:

* F_1919(k) in the front view: print `D_01E4[k]` ("Left","Right","Climb","Dive"); fuel < 2: "Not enough fuel!";
  else fuel -= 2 and the crosshair velocity is set to (-4,0), (4,0), (0,-4) or (0,4) pixels per frame.
* The crosshair stops (velocity 0) when it reaches x >= 300 or <= 20 or y >= 140 or <= 20. Space centres it
  and zeroes both velocities. Sound 9 is played at the start and end of each F_1C2A.

`D_5FFE` (star mode): 2 normal, 1 no erasing (hyperjump streaks), 3 = erase but crosshair fixed **(inferred)**.

### Speed, F_165C(n)

Keys `1`..`8` in the front view: "Speed n", cost `4 * |n - D_0126|` fuel ("Not enough fuel!" if more than
the tank), then `D_0126 = n`. Speed only changes the frame delay (`D_0C9E[speed]`) and the approach rate of
enemies (below).

### The enemy (F_3330, F_254B, F_31D7)

One enemy at a time per sector (the sector's `count` of 1..3 enemies is the number still to come). State:
position `(D_0C92, D_0C94)` (0 = none shown), previous position `(D_0C96, D_0C98)`, **kind** `D_0C9A` 0..6 (its
apparent size, growing as it nears; the Fight bank sprite number), frame counter `D_0DE2`.

F_254B places a new enemy: `x = rnd(40,280)`, `y = rnd(40,120)`, kind 0 (or nothing while hyperjumping).

F_3330 (assembly, from SP3330.ASM; SP3330.ASM is the only authority) each frame:

```
if (D_012C) D_012C--;  D_0DE0 = 0;
F_1C2A();                     /* star field */
F_3CF1(D_0C9E[D_0126]);       /* speed-dependent delay */
if (sector.count == 0) return;
D_0C96 = D_0C92;  D_0C98 = D_0C94;
dx = D_013A - D_0C92;  dy = D_013C - D_0C94;                 /* crosshair minus enemy */
D_0C92 -= sign(dx);  D_0C94 -= sign(dy);                     /* enemy steps one pixel AWAY from the crosshair */
if (D_0C92 > 280 || D_0C92 < 30 || D_0C94 > 125 || D_0C94 < 24) {      /* left the view */
    if (D_0C96) erase old sprite;  F_254B();                  /* new enemy */
} else {
    if (D_0C96) erase old sprite;
    if ((D_0C9C && D_0DE2 >= 8 - D_0126) || D_0DE2 > 15) {    /* time to grow */
        if (rnd(1, (D_0126+3)*10) > 20 || D_0C9C) D_0C9A++;
        if (D_0C9A > 6) F_254B();                              /* flew past: respawn far away */
        D_0DE2 = 0;
    } else D_0DE2++;
}
if (D_0C92) { draw enemy sprite (kind D_0C9A) at (D_0C92, D_0C94);
              F_31D7();                                        /* enemy may fire */
              if (|dx| < 4 || |dy| < 4) redraw crosshair; }
```

(The sign convention is exactly what the assembly does: the enemy moves away from the crosshair while it is on
screen. After a respawn the first frame sees `D_0C92 = 0`, giving `-1`, which also triggers a respawn, so a
new enemy appears one frame after the previous one is gone.) Growth: every 16th frame (outside hyperjumps) the
kind increases with probability about `1 - 20/((speed+3)*10)` (about 50% at speed 1, 80% at speed 8).

**Enemy fire, F_31D7** (every frame): if `rnd(1,200) < 4` (about 1.5%) and kind > 0 and not jumping and an enemy is
shown: "Alien fires!"; `r = rnd(1,255)` picks the aiming point: `r < 64` (159,135) bottom centre, `< 128` (159,20)
top, `> 192` (20,79) left, else (290,79) right; a line (colour `D_2044`) is drawn from the enemy's bottom centre
(`x + width[kind]/2, y + height[kind]`) to that point, sound 8, wait 2 ticks, erased. **Only when `r < 64`**
(about 25% of the shots) it hits: "You've been hit!", sound 2, `shield -= min(shield, 321)`; shield 0:
"Thy shield is drained!" and death.

**Player fire, F_2252** (key `F`, front view only): "Fire!"; fuel < 10: "Thou hast not enough fuel!"; else fuel -= 10;
laser lines (colour `D_2044`) from (20,140) and (300,140) to the crosshair, sound 8. With an enemy on screen the
offset `(px,py) = (D_013A - D_0C92, D_013C - D_0C94)` of the crosshair inside the enemy sprite box is tested
against the per-kind tables

| kind | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|---|
| `D_0212` min px | 1 | 1 | 1 | 1 | 0 | 0 | 0 |
| `D_01F6` max px | 5 | 6 | 8 | 10 | 12 | 16 | 18 |
| `D_0220` min py | 5 | 5 | 4 | 4 | 3 | 2 | 1 |
| `D_0204` max py | 4 | 6 | 8 | 10 | 12 | 14 | 18 |

(Kind 0 can never be hit: its y range 5..4 is empty. The same width/height tables size the enemy's shots.)

A hit: sound 2, "Hit!!!", **experience +100** (record 0x26, `D_207E`, F_450D redraws it), **kill counter +1**
(record 0xA6, `D_20FE`); at exactly 20: the victory screen F_21FC ("Thou hast achieved / the rank of / Space
Ace!", key wait); above 20: "Thou art still a Space ace!". Then `--count`: if still > 0 another enemy is placed
(F_254B), else the field is empty (`D_0C92 = D_0C94 = 0`) and View may switch back to top view. OUT later needs
20 kills and 7000 experience for the princess' "time travel" reward (OUT.C F_4BD6).

## 8. Leaving: F_0EBD and death F_0F25

* Landing (F_2731) or any death goes through F_0EBD: if hit points `D_206E == 0` (death) the current ship's shield
  and fuel are zeroed first (the landing case keeps them; the shuttle's values were already copied to the record
  at offsets 0xB0/0xB2 by F_2731); the record (0x334 bytes) is written to `inuse.u1`; then
  `execl("out.exe","out.exe", D_5E10, D_5E12, "S", 0)` in a loop with the usual "Insert Ultima I diskette" retry.
* Death (F_0F25): `D_2080` (food, record 0x28) and `D_206E` (hit points, 0x16) are set to 0, the status fields
  are redrawn (F_4437, F_44A2), then F_0EBD. OUT sees `HP <= 0 || food <= 0` at start and shows the death/
  resurrection routine. Causes of death in SPACE: shield 0 (collision or alien hits), melting at a star, a
  non-shuttle entering the planet, docking without a Vacuum suit.

Messages unique to SPACE (strings in DATA): "Huh?", "View", "Not enough fuel!", "Noise on/off", "Insert Ultima I
diskette", "Thou hast lifted off!". Fuel is spent by: turn 2, thrust/retro 5, front-view move 2, speed change
4 per step, laser 10, hyperjump 100; the shield by: crash `shield/4 + 5`, alien hit 321.

## 9. Notes on the sources

* The C comments in SPACEA.C that mislabel things: `f6`/`f8` (fuel/shield reversed), F_03E9 ("largest square" =
  ceil sqrt), F_3109 / F_281F / F_2B7B / F_2EC1 ("alien ship" = exhaust), F_0425 "19x18" is 18 rows x 19
  columns (the star).
* F_3330 is the only function not in C. 376 bytes; the register allocation of the compiler could not be
  reproduced (docs/ notes in src/space/work/sp_2b7b.notes.md). SP3330.ASM is byte-exact.
* Text literals reused through address casts in SPACE4.C: `(char *)0xC07` = "Crunch!", `0xC0F` = "Thy shield is
  drained!", `0x19A` = "".
* `D_5E14` is the index of the player's ship; `D_6000` the facing last drawn (so a turn causes a redraw);
  `D_0134` redraw flag; `D_012E` "exhaust toggle" flag; `D_0DE0` exhaust drawn flag.
