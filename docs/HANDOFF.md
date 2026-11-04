# Handoff: Ultima I decompilation (u1-decompiled)

State as of 2026-11-04. Read this first, then README.md, docs/MAP.md and docs/MATCHING.md.

## Where things stand

| executable | status |
|---|---|
| GEN.EXE (character creation) | **Rebuilds byte-identical** (sha1 af21ad12e999d63d36d2e34381c857f55e57c14d) |
| MONDAIN.EXE (final battle) | **Rebuilds byte-identical** (sha1 9456ed0ae86a5cd6336de1b670538f1ebe0f8741): shared modules plus generated stand-ins (`src/mondain/`) |
| SPACE.EXE (space flight) | **Rebuilds byte-identical** (sha1 fdd68f5bbeb9abeb7524b385e372daad5a6d02a3): shared modules plus generated stand-ins (`src/space/`) |
| ULTIMA.EXE (loader, title) | **Rebuilds byte-identical** (sha1 6589c21ebd80a03dec5b1d0d07068a638f450ee0): shared modules plus stand-ins; `src/ultima/postlink.py` appends the envelope's remains from gamedata at build time |
| OUT.EXE (the game) | **Rebuilds byte-identical** (sha1 e4048b57ea64c0bdf51c7ec74f939002711a9475): shared modules plus stand-ins (`src/out/OUT1-3.ASM`, split because MASM 3.00 runs out of memory on 36 KB) |

What GEN.EXE is built from (link order in `src/gen/gen.lnk`):

| module | source | state |
|---|---|---|
| startup CXINIT | `src/lattice/C.ASM` | Hand-written MASM. Matches all five EXEs (only symbol addresses differ) |
| game code 03E9-1948 | `src/gen/GEN.C` | C, all 29 functions. Data 0124-0692 and BSS 0A30-0DB0 defined in the file |
| Lattice library 1948-34BF | `src/lclib/*.ASM` | 49 modules, generated MASM (tools/genasm.py) with symbolic addresses; 14 are byte-identical to 3.00g's |
| Origin library 34BF-4254 | `src/origin/OR*.ASM`, `OR*.C` | 17 modules in library order (docs/MAP.md). 3 are C (387E/3920, 39C5, 3AD7; `-ms -v -w -cs`), 14 are generated MASM with symbolic data and code addresses |
| _IOB | `src/lclib/_IOB.ASM` | Lattice's FILE table (118h bytes of UDATA); must stay last (its UDATA ends BSS) |

Names are still addresses (`F_xxxx` code, `D_xxxx` DGROUP); `src/gen/gen.syms` maps every
address to its name. GEN.C functions carry a one-line comment saying what they do.

Steps 1-3 of "Next steps" are done and all five EXEs link identical (step 4, first part): the
startup, library and Origin modules are shared sources; each game's own code is still a
generated stand-in (GEN's is C). Next: the game code in C, EXE by EXE (step 4.3).

## Toolchain facts (proven)

- Compiler: Lattice C 3.10 (LC.EXE 3.10, LC1 3.11, LC2 3.10 from Karl's
  "Lattice iAPX32 C compiler v3.10.zip", which is really the 8086 compiler). GEN.C options:
  `lc -ms -v -w -cs` (small model, no stack checks, even frames, identical strings once).
  K&R C. 3.00g's compiler produced the same code for every function tried.
- Startup and library are from a Lattice release between 3.00g and 3.30 that Karl could not
  find. The startup was rebuilt by hand; the library is generated assembly. 17-27 modules per
  EXE are byte-identical to 3.00g's LCS.LIB (tools/libmatch.py lists them).
- Assembler: MASM 3.00 (MASM 4.00 gives identical objects).
- Linker: LINK 3.05 (MASM 4.0) is used. LINK 2.40 and 3.00 give the same GEN.EXE, so the
  original version is still undetermined. LINK 1.10 cannot read Lattice 3 libraries.
- Lattice 3.1x assembly modules have PARA DATA, even when empty: ONERROR, CXSERR, _EXIT, BUFSIZ
  and DUNIQUE (empty, it pads SPACE's data to 26C0) show it; the C modules are WORD.
- DATA is `PARA` aligned in C.ASM (as in Lattice's own C.ASM); LINK then pads the code up to
  DGROUP's paragraph (it used to be 12 explicit zero bytes at the end of ORIGIN.ASM).
- The header checksums of GEN, OUT, SPACE and MONDAIN are valid MS LINK checksums; ULTIMA.EXE's
  is not (see below).

## How to build and check

```sh
python3 tools/setup.py --archives <dir with Karl's .7z/.zip> [--masm3 <MASM 3.0 .img>]
python3 tools/build.py gen          # -> build/gen/GEN.EXE, compared with gamedata/GEN.EXE
```

Needs `dosbox` (0.74 headless; `apt-get install dosbox p7zip-full` works in the cloud
workspace) and `7z`. On Karl's Windows machine it runs natively: tools/dos.py falls back to
`gamedata/DOSBOX/DOSBox.exe` (or `DOSBOX=...`), setup.py to `C:\Program Files\7-Zip\7z.exe`;
capstone is in a venv: `build/venv/Scripts/python tools/...` (the system Python refuses pip).
A GEN build takes about 3 s. py7zr cannot open the archives that use BCJ2 (Lattice 2.12, MS C 1.03/1.04).
setup.py looks for the MASM 3.0 image in `../u6-decompiled/tools/masm300/`; in the cloud it is
staged from Karl's u6-decompiled folder. `gamedata/` must hold the GOG EXEs.

The matching loop for C (details in docs/MATCHING.md):

```sh
python3 tools/lc.py FILE.C [-v] [--data OFF]   # compiles, compares each function by its .syms name
python3 tools/disasm.py gamedata/GEN.EXE 1643 60
python3 tools/dump.py gamedata/GEN.EXE 124 80 --ds      # or --str OFF
```

For assembly stand-ins:

```sh
python3 tools/genasm.py EXE SYMS OUTDIR NAME CODE0 CODE1 [DATA0 DATA1 [BSS0 BSS1]]
```

It generates MASM with symbols, assembles, compares, and rewrites instructions MASM encodes
differently as bytes until it matches; force lists are kept in OUTDIR/NAME.force.
In a .syms file, `C` = code entry, `L` = code label on data (never disassembled from, e.g.
`L 2FC0 XLOADER`), `D` = DGROUP data.

`xmap.py EXE -o NAME.syms --map` finds GEN's shared modules (src/lclib, src/origin) in another EXE,
using the fixups of build/gen/*.OBJ as the mask (run `build.py gen` first), names their code and
data with GEN's names, adds F_/D_ names for the rest and prints the module map.

`addrimm.py EXE START END OTHER.EXE -o OUT/NAME.imm` finds the 16-bit immediates that are
addresses (they differ between the two EXEs' copies of the code); genasm.py reads NAME.imm and
writes them `OFFSET DGROUP:name` (every target needs a symbol in the .syms file first).
`addrimm.py EXE --data D0 D1 OTHER.EXE OD0 -o OUT/NAME.ptr` does the same for near pointers in a
module's data (written `DW name`). A shared module needs both before another EXE can link it.

Other tools: `exediff.py` (header, relocations, image), `cmp.py` (object vs EXE),
`omf.py` (object/library reader), `fmap.py` (function map), `libmatch.py`, `libfuzzy.py`,
`libsim.py`, `libdir.py` (library identification), `rdis.py` (recursive disassembly).

## Conventions

- git author: Karl <30571394+karlclinckspoor@users.noreply.github.com>; commit messages end
  with the Co-Authored-By/Claude-Session lines. Never commit gamedata or tool archives
  (`.gitignore` covers `gamedata/`, `build/`, `tools/*.7z`, `tools/*.zip`).
- Work now happens directly in `C:\Users\karl9\Downloads\Claude\u1-decompiled\` on Windows (Git
  Bash and PowerShell). From a cloud session, the old route still applies: tar the repo (`.git`
  plus tracked files) into /mnt/user-data/outputs and untar it there with the device shell.
- Git Bash's `sed -i` drops CRs. MASM 3.00 needs CRLF in .ASM files (the C files are LF), so
  edit .ASM files with Python (binary mode) or the Edit tool.
- Do not name a tool `dis.py` in tools/: it shadows Python's `dis` module and breaks capstone.
- Building emu2 or downloading prebuilt binaries was blocked by the safety classifier, so
  DOSBox from apt is the DOS runner (about 1.5 s per compile).

## Code-shape findings worth knowing (full list in docs/MATCHING.md)

- Frame: `push bp; sub sp,N; mov bp,sp`; locals upward from bp+2 in declaration order;
  arguments at bp+N+4; the compiler sometimes takes a 2-byte slot of its own.
- Key codes and `argv` are `unsigned char` (`xor ah,ah` rather than `cbw`).
- `{ register unsigned char *p; p = G; *p += n; }` avoids a pointer reload that `*G += n`
  produces; disjoint block-local variables share a frame slot.
- Nested `if (a && b) { if (c) ... }` differs from `if (a && b && c)`.
- Initialised pointer arrays: their strings come right before them in DATA; strings follow in
  order of use; UDATA follows definition order.

## Next steps (in order)

1. ~~Origin routines to C~~ **done.** Only 4 routines were C (Lattice frame); the rest is
   hand-written assembly. The library is split into its 17 modules in `src/origin/` (method in
   docs/MAP.md). Left for the readable pass: comments in the assembly modules.
2. ~~Split LCLIB.ASM into Lattice modules~~ **done**: 49 modules in `src/lclib/` (table and
   method in docs/MAP.md). The other EXEs link different subsets in a different order: write
   their .lnk from the modules, adding the modules GEN lacks (generate them from that EXE the same
   way, symbolic addresses included). Open: the 14 modules identical to 3.00g could link the
   real objects (`LIB LCS *NAME`) instead.
3. ~~ULTIMA.EXE~~ **done** (identical link with stand-ins, postlink.py). Was: same method; then a post-link step for the protection remnant. The header
   CS:IP is 088F:0000; at image 0x88F0 a stub restores SS:SP and RETFs to the startup; 0x500
   encrypted-looking bytes follow. Append the stub and tail from gamedata/ULTIMA.EXE at build
   time, patch CS:IP/SS:SP/checksum, and never commit the bytes.
4. **MONDAIN, SPACE, OUT** (OUT is 54 KB of code; split it by subsystem among parallel
   agents). Identical links are done for all of them; their game code is still generated
   stand-ins (src/NAME/NAME.ASM; OUT1-3.ASM), to be replaced by C. Suggested order: MONDAIN
   (7.7 KB), ULTIMA (2.6 KB), SPACE (14.8 KB), OUT (36 KB, in parallel by subsystem). Find the
   real C module boundaries first (data order, string order, frame style). Per EXE:
   1. `build.py gen`, then `xmap.py EXE -o src/NAME/NAME.syms --map`: shared modules placed and
      named, gaps listed. Give each gap a module (generate with genasm.py; new Origin modules are
      named OM/OS/OO+address after the EXE they were first found in), place data-only modules by
      their data, and check the data each new module owns from the references (fmap.py).
   2. Make generated stand-ins and get an identical link first (as was done for GEN).
   3. Replace the stand-ins with C function by function, re-linking after each.
5. **Readable names**: rename F_/D_ via .syms plus the sources (the U6 readable pass is the
   model); ScummVM's Ultima 1 engine is a naming reference.

Running agents in parallel: give each a disjoint address range and its own work file in
`src/gen/work/` (or the EXE's equivalent), point it to docs/MATCHING.md, and integrate their
files yourself. Data layout is only checked at link time. Agents should use real string
literals, not D_ references to strings.

## Elsewhere

abedegno's Exhume (https://github.com/abedegno/Exhume, also in
`../underworld-exhumed-enhanced/exhume/`) is the same method for Borland (Ultima Underworld II).
No Lattice profile, so not used for matching here. Worth reading when the time comes: its
`skills/orchestrate`, `match-file`, `verify-and-merge` (parallel agents, for OUT.EXE),
`docs/readability.md` and `skills/readability-pass` with `syncnames.py`/`structrec.py` (step 5),
and `runtime/port` + `replay` (a port checked against recorded DOS sessions).

## Loose ends

- MONDAIN's Origin modules (src/origin/OM*.ASM) are split where SPACE/OUT/ULTIMA show a boundary;
  runs that sit together everywhere (OM4437 = 4437-4A57) may still be several modules.
- Names: library functions first found in MONDAIN/SPACE/ULTIMA carry their Lattice names (CXC33,
  MEMSET, PUTS, CXNM8, CXD22/CXD33, _PFMT/_PF, FPUTS, FPUTC, STCD_I, STCL_H, STCL_O, STCUL_D,
  STRUPR, MEMCPY, ISDIGIT, TOUPPER, ISLOWER). Address names (F_/D_xxxx) are per EXE, so two can
  meet in one link: build.py now fails on LINK's "defined more than once", and xmap gives a
  clashing address name the EXE's initial as suffix (SPACE's game data D_07FA_S).
- Unsplit runs: ULTIMA's LU1222 (two functions), LU27B9/LU2846 are probably STCI_H/STCI_O.
- The OO* Origin modules (OUT only) keep their address immediates as numbers and their data
  pointers as bytes: fine while only OUT links them. The game stand-ins likewise.
- BSS ownership: each stand-in owns all of its EXE's BSS before _IOB. Some of it is probably
  UDATA of Origin modules (the shared globals D_0DA6.. sit at the start of MONDAIN's BSS but at
  the end of GEN's); sort it out when the game code becomes C.
- MONDAIN.C defines `_stack` itself (3000 at DGROUP 0124), so the STACK module is not linked
  there; GEN uses the library's (0800h).

- `GEN.EXE.map.json` in Karl's folder root is a leftover (now untracked; fmap writes to build/).
  `u1-repo.tgz` there is the transfer archive. Both can be deleted.
- docs/MAP.md's ULTIMA section and the per-EXE tables need filling as EXEs are done.
- Project doc `claude/u1-decompilation-plan.md` (claude.ai project "Ultimas") has the same
  status summary; keep both in step.
