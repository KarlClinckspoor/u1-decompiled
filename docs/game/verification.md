# Checking the docs against the running game

The docs in this directory were written from reading the code. `tools/ktest.py` (with `tools/ktest/KTEST.ASM`)
runs the real executables under headless DOSBox with a **test and cheat layer**: a small resident program that
hooks INT 21h and answers the game's key poll (AH=06h, DL=FFh, used by `getkey`) from a script. The script can
also **poke** bytes into the game's DGROUP (the cheat layer: teleport, set hit points, ...) and **dump** DGROUP
ranges to files, so state can be compared with what the docs predict. The layer is not part of any game binary;
it only exists in the test harness.

```python
import ktest
s = ktest.Script()
s.wait(150)                       # polls with "no key"
s.keys('A'); s.ext(0x4D)          # a key; an arrow key (scan code: 4B left, 4D right, 48 up, 50 down)
s.poke(0x3A78 + 0x16, b'\0\0')    # cheat: hit points = 0
s.dump(0, 0x3A78, 820)            # dump the OUT character record
s.end()
out, workdir = ktest.run('out', s, files=['PLAYER1.U1'], prefix='C 1')   # out[0] = the dump
```

`run()` copies `gamedata/` (data files and EXEs) to `build/run_NAME/`; files written by the game stay there.

## Confirmed by running

| claim (doc) | result |
|---|---|
| GEN: 30 points to distribute, each attribute starts at 10, 5 right-arrows each gives 15 (character-creation.md) | confirmed |
| GEN: Human +5 Intelligence, Fighter +10 Strength and Agility; stored race/class/sex 0/0/0 | confirmed: attributes 25 25 15 15 15 20 |
| GEN: new record: hit points 150, pence 100, experience 0, food 200, dagger readied, leather armour, Prayer, on foot, position (49,40), sound on, owned arrays, first object (10,1,49,40), offset 0xA8 = -1 | confirmed field by field; dagger count really is 2 |
| GEN writes an 820-byte `player1.u1`, name at 0 | confirmed |
| OUT loads `playerN.u1` into 3A78; turn counter at 8BA8, monster count at 8BA6 | confirmed |
| OUT: every command costs a turn (turn counter +1 per Pass, per move and per Quit) | confirmed (30 passes: 30; 3 moves: 33) |
| OUT: an arrow key moves one square | confirmed (49 -> 52) |
| OUT: food on foot goes down by one every two turns (`foodtick`: counter reaches `foodper[veh]`) | confirmed (200 -> 185 after 30 turns) |
| OUT: Quit saves the file and the game continues | confirmed: file rewritten, program keeps running, Quit costs a turn; the saved record has the state before that turn's food use |
| OUT: death (hit points 0): 99 hit points, 99 food, pence 0, all weapons lost, moved to a new square (combat.md section 9) | confirmed (position (60,155) -> (70,146); weapon counts 0; hands kept) |

## Not confirmable

* The wrap of the world coordinates (east of x = 167): every land square of `MAP.BIN`'s first and last column is
  water (no grass or woods in columns 0 and 167), so the player cannot walk over the edge; the wrap is only
  reachable through the place tables or ships.
