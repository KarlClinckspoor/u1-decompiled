# sp_2b7b notes (SPACE F_2B7B-F_3DED, 18 functions)

File: sp_2b7b.C. Nothing lies between F_281F and F_2B7B in the .syms. Two functions in the range
have names that are not their code addresses: F_125C (code 39BE) and F_1853 (code 3ADE, the
program's main). lc.py 2026-11-12: 16 MATCH, F_3330 NONMATCHING, F_386B differs only by 3 call displacements (F_3330 is 2 bytes too long in mine; it will match when F_3330 does). Data/string layout not checked.

## Status
| fn | status | purpose |
|---|---|---|
| F_2B7B | MATCH | draw ship by facing sel 0..3, design 1 (pixels via F_27EE) |
| F_2EC1 | MATCH | draw ship by facing, design 2 |
| F_3109 | MATCH | draw/erase alien ship object p by type 0/1/2 (F_281F/F_2EC1/F_2B7B), toggles D_0DE0 |
| F_319F | MATCH | random -1/0/1 (F_4069(1,255) vs 125/129) |
| F_31D7 | MATCH | alien fires: laser line, "You've been hit!", shield loss |
| F_3330 | NONMATCHING | alien ship turn in combat: move toward crosshair, redraw, fire (2 bytes longer) |
| F_34A8 / F_34D9 | MATCH | x += D_0128 wrap 20..275 / y += D_012A wrap 10..130 |
| F_350A | MATCH | collision: "Crunch!", shield loss, reverse speeds |
| F_3567 | MATCH | dock at a base: 500p fee, choose ship 1..4 |
| F_3741 / F_37E0 | MATCH | docking slot lookups (return index or -1) |
| F_386B | MATCH* | one turn: combat (F_3330) or move player ship, collisions, docking |
| F_3983 | MATCH | toggle Noise on/off (D_5B14) |
| F_125C | MATCH | "Insert Ultima I diskette" prompt, wait for space |
| F_3A4E | MATCH | lift-off countdown |
| F_1853 | MATCH | main(argc, argv): load files, set up screen, key loop (jump of letter commands) |
| F_3DD6 | MATCH | "Abnormal program termination" + _exit(3) |

## Globals
D_26C0 struct sector[7][7] (see sp_16d3 notes); D_01EE/D_01F0 sector; D_5E14 ship index; D_6000 int
ship facing (last); D_0126 int (speed level / table index); D_0128/D_012A int speed x,y; D_012C int
flag (ship moved / alien turn pending), D_012E int redraw flag, D_0134 int; D_013A/D_013C crosshair;
D_0C92/94 alien pos, D_0C96/98 its previous pos, D_0C9A alien kind, D_0C9C int, D_0C9E int[];
D_0DE0 int (alien-drawn toggle), D_0DE2 int counter; D_01F6/D_0204 int[] alien size tables (init);
D_0E18 char *[] ship names (init); D_0166/D_0194 char *[] data file names (init); D_082C int
graphics mode (0..2); D_2044/2046/2048 int colours/flag; D_2058 char[] (cast to int) = player
record, 820 bytes, loaded from inuse.u1; D_2086, D_207C int player stat / money; D_5AD2 int colour;
D_5B0E, D_5B14 (noise flag), D_5B8C/5B8E, D_5E10/5E12 (argv[1], argv[2]), D_5FFE.
(&D_01F2)[1] is the word at 01F4 (no symbol): set to 1 by the Esc key. D_2C5C+1406 (=31DA) and
D_4C67+499 (=4E5A) are the two graphics buffers (WRONG TARGETS not reported).

## Strings (D_ address)
DE4 Alien fires!; DF1 You've been hit!; C0F Thy shield is drained!; C07 Crunch!; E20 Docked!  Welcome
to base!; E3A The spacedoors open, exposing; E58 thee to a complete vacuum!; E73 Sorry, but thou canst
not; E8D refuel thy ship without; EA5 the 500p docking fee.; EBB Choose thy ship: ; ECD That port is
not available.; EE9 Thou art in the ; EFA " ship."; 19A ""; F01 Noise ; F08 on; F0B off; F0F Insert
Ultima I diskette; F28 Hit space to continue...; F41 24 spaces; F5A ..; F5D Thou hast lifted off!;
B1E inuse.u1; F73 Shld:; F79 Fuel:; F80 \nAbnormal program termination\n.

## Compiler lessons
- `if (a) { a = 0; if (b) if (!c) ... }` (nested ifs at the end of a loop body) matches; the same
  with `continue` statements compiles identical except `a = 0` becomes `mov [a],0` instead of
  `xor ax,ax; mov [a],ax`.
- A far false-branch is `jcc_inverse +3; jmp far` (so no sign of `continue`).
- `a != b || c || d` assigned to a variable gives `xor ax,ax; jmp; xor ax,ax; inc ax`; so does `!x`.
- Passing an `extern char D_2058[]` as `(int)D_2058` gives `mov ax,&D; mov bx,len; push bx; push ax; mov bx,str; push bx`
  (as in the other EXEs); `&D_2058` or the array itself does not.
- Switch on unsigned char with 26 sparse cases: compare chain over (value, target) tables in CS;
  source case order is the reverse of the table order; default label after the last case body
  (`case 27: x = 1; default: f();` falls through).
- Two hidden 2-byte frame slots appear on their own; do not add dummy locals to fill the frame.
- Statement order `done = n = 0` stores n second (store order is right to left).
