# u1-decompiled

Source code for the PC version of **Ultima I: The First Age of Darkness** (Origin Systems, 1986/87,
conversion by John Fachini), rebuilt with the original tools into executables identical to the
GOG release, byte for byte.

## Status

| executable | what | status |
|---|---|---|
| GEN.EXE | character creation | **rebuilds identical**. Game code in C (src/gen/GEN.C, 29 functions); Origin's support library as 17 modules (3 in C, 14 in assembly); Lattice library as 49 generated assembly modules |
| ULTIMA.EXE | loader, title | not started (has a protection-envelope remnant, see docs/MAP.md) |
| OUT.EXE | the game | not started |
| SPACE.EXE | space flight | not started |
| MONDAIN.EXE | final battle | not started |

The startup module (src/lattice/C.ASM) is shared by all five and already matches them all.

## Toolchain

The game was compiled with **Lattice C 3.1x** (small model, `lc -ms -v -w -cs`), its startup and
library a release between 3.00g and 3.30 for which no copy is known: the startup was rebuilt
from the executables by hand, the library modules are generated from them. Assembly: MASM 3.00.
Link: Microsoft LINK 3.05 (2.40 and 3.00 give the same file).

The repository contains neither the game nor the tools. Put the GOG files in `gamedata/` and
your toolchain archives in `tools/` (Lattice C 3.10 compiler zip, Lattice C 3.00g and 3.30 disks,
Microsoft C 2.03 for LINK 2.40, MASM 3.0 image, MASM 4.0 disk), then:

```sh
python3 tools/setup.py          # unpacks the toolchain into build/tc (needs 7z)
python3 tools/build.py gen      # builds build/gen/GEN.EXE and compares it with gamedata/GEN.EXE
```

Builds run the DOS tools in headless DOSBox: `dosbox` on the PATH, or the one named by the
`DOSBOX` environment variable; on Windows the GOG copy in `gamedata/DOSBOX/` is used when
neither is set (7-Zip is looked for in `Program Files` too). The disassembly tools need
`capstone` (`pip install capstone`, in a venv if the system Python refuses).

## Layout

- `src/lattice/C.ASM`: Lattice startup (CXINIT), shared by every executable
- `src/origin/`: Origin's support library (graphics, text, keyboard, sound, timer), one file per
  module, named after its first function (`OR34BF.ASM`, `OR387E.C`, ...); every EXE links a subset
- `src/lclib/`: the Lattice C 3.1x library modules the EXEs link, one file per module
- `src/gen/`: GEN.EXE: `gen.lnk` (link order), `GEN.C`,
  `gen.syms` (names of every function and global by address)
- `tools/`: build, compare, disassembly and generation tools (docs/MATCHING.md explains the loop)
- `docs/HANDOFF.md`: state and next steps; `docs/MAP.md`: layout of the executables;
  `docs/MATCHING.md`: how Lattice C 3.10 code reads
