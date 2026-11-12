# OUT.EXE F_2B6D-F_3F7F (36 functions; nothing lies between F_2A73 and F_2B6D)

Files: ou_2b6d.C (F_2B6D), ou_2b6d_b.C (F_2E22-F_32F8), ou_2b6d_c.C (F_33B0-F_3F7F_O).

## Status
All 36 MATCH. Caveat: `D_3808`, `D_3814`, `D_3930` have no entry in out.syms, so lc.py reports
WRONG TARGETS for the functions using them (code bytes match). Checked by compiling a copy with
`D_3808 = D_1DC8+26 ints`, `D_3814 = D_1DC8+32`, `D_3930 = D_1DC8+174`: all MATCH, targets ok.

| function | purpose |
|---|---|
| F_2B6D | ranged/directional attack (dir, range, damage, chance): "Hit King!/Guard!/..", kill/damage |
| F_2E22 | step the player one square in dir 1-4 (or leave the map / "Blocked!") |
| F_2F3B F_2FF6 F_30C0 F_31B1 F_3240 F_32F8 | shop sign n: transport, armour, weapon, magic, food, pub (return 7/6 for the first three) |
| F_33B0 F_3430 | dungeon wall edge lines, left / right, depth step i |
| F_34B0 | front wall rectangle at depth i |
| F_355A F_35DC | left / right wall outline at depth i |
| F_365E F_368F F_36C0 F_36F1 | side-wall feature (F_AC91 tile 26/27 and 28/29) at depth i |
| F_3722 | three rungs (ladder) across the wall at depth i |
| F_37DB F_390D | left / right side door at depth i |
| F_3A54 | front wall with doorway at depth i |
| F_3B25 F_3B65 | vertical wall edge line left / right |
| F_3BA5 F_3BF5 F_3C13 F_3C31 | name+draw a dungeon feature kind (D_3930 names), coffin, chest, print name |
| F_3C64 | sign(x) |
| F_3C8E F_3CBB | dx / dy of facing D_AD52 (1 W, 2 E, 3 N, 4 S) |
| F_3CE8 | print the compass direction word in the dungeon status line |
| F_3D6C F_3DF5 | draw dungeon cell (x,y) at depth i (walls, doors, coffin/chest, features) |
| F_3F19 F_3F4C | wall pieces by side (1,2 / 3 / other): F_3430,F_390D,F_35DC / F_33B0,F_37DB,F_355A |
| F_3F7F_O | is the cell at (D_AD3E+a, D_AD40+b) passable (1) or not (0) |

## Globals
D_AD56 `struct cell{a,b,c,d}[][11]` (BSS, 8 bytes per cell, 0x58 per row): dungeon view map;
a = wall type (1,2 wall, 3 door, 6/7 side feature, 8 ladder), b = feature kind (-1 none; index
into D_3930 names), c = 4 chest / 5 coffin. D_AD52 int facing (1 W, 2 E, 3 N, 4 S).
D_3808[], D_3814[] int[] (initialised): perspective x / y offsets per depth step (0,0x48,0x6c,
0x7e,0x87,0x90 / 0,0x24,0x36,0x3f,0x44,0x48); D_3930 char*[] (initialised) feature names;
D_1DC8 char*[] (initialised) direction messages. D_0DA6 / D_2044 colour ints, D_0DA8 int colour,
D_2046 text colour. D_ACC6 struct ent{f0 type,f2 x,f4 y,f6 hp}[] (BSS) monsters, others as in ou_1769.notes.md.
D_8B9E D_8BA0 D_8BA2 ints (castle flags: princess alive / key found / king invulnerable?), D_206C int, D_207E int (experience-like count).

## Strings (DGROUP)
1E1 "Missed!", 4E0 "Hit ", 4E5 "King! ", 4EC "Guard! ", 4F4 "Merchant! ", 4FF "Princess! ", 50A "Bard! ",
511 "Jester! ", 51A "Wench! ", 522 "Lecher! ", 52B "Killed!", 533 "Thou hast found a key!", 1D8 " damage!",
54A "Blocked!", 553-84A the shop sign names (printed in order in F_2F3B.. F_32F8), 2E1 "Coffin", 2E8 "Chest",
85D "Mimic", 863 " North ", 86B " South ", 873 " East  ", 87B " West  ".

## Compiler lessons
- Functions taking an argument and calling with computed arguments get a hidden `sub sp,2N` of
  temp slots; don't declare dummy locals to imitate them (shop signs: no locals matched).
- A switch with a sparse case set and unsigned argument: `cmp si,8; jb` jump table; with cases 1,2,3
  mixed, the compare chain tests the LAST source case first: write `case 2: case 1:` to get 3,1,2.
- Common subexpression `a + 16 + a`: writing both args as `a + 16 + a + D_3808[i-1]` made the compiler
  share `a+16+a` in a register (F_37DB/F_390D); a different operand order did not.
- `d = expr; d = ABS(d / 4);` with ABS(x) ((x)<0?-(x):(x)) gives the idiv/neg/re-idiv pattern.
- 2D table of 8-byte structs: `struct cell D_AD56[][11]` gives `imul 0x58` + `shl cl,3`; indexing
  a monster struct array by `m` gives the `shl ax,cl` (3) form, int array `m*4` gives shl ax,1 x3.
- Calls to functions of the same file are resolved at compile time: sizes/order of earlier functions
  must match before later ones compare (1-byte call offset differences cascade).
