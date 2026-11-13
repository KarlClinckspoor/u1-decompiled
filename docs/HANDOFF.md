# Handoff: Ultima I decompilation (u1-decompiled)

State as of 2026-11-12 (C translation integrated). Read this first, then README.md, docs/MAP.md and docs/MATCHING.md.

## Current task (Karl, 2026-11-04): status as of 2026-11-12

All four parts are done except the polish items below.

1. **Pushed** (first commit set). Commits since then are local until Karl says to push.
2. **C translation: done and integrated.** All five EXEs are built from C game code and rebuild
   byte-identical: src/gen/GEN.C, src/ultima/ULTIMA.C, src/mondain/MONDAIN.C,
   src/space/SPACE.C+SPACE5.C+SPBSS.C, src/out/OUTA/B/C.C+OUTBSS.C (+OUTSYM.ASM,
   absolute names for tables that live in the Origin library's data). All game functions are C (SPACE's F_3330, now `fightfrm`, was matched last: copy into locals first, see src/space/INTEGRATION.md). Each src/NAME/INTEGRATION.md says how the modules were cut (LC1 runs out of
   memory at ~2250 lines, so OUT is three modules). src/*/work/ keep the per-range drafts and
   .notes.md (compiler lessons); they still use the old address names.
3. **Readable names: done for what could be justified.** tools/rename.py + docs/names.tsv (855
   names) + docs/NAMES.md (8-significant-character rule). Left as F_/D_/U_ addresses: Lattice
   internals, unclear Origin routines/data, SPACE galaxy sub-fields, some OUT BSS blocks.
   Work files, INTEGRATION.md and the other docs still cite address names; names.tsv maps them.
4. **Mechanics documentation: done** in docs/game/ (15 files: overview, character creation,
   title/loader, space flight, final battle, save format, world/map, commands, combat, monsters,
   dungeons, towns/shops/castles, quests, OUT state, video/sound/input). Written from reading
   the code, never run; uncertain readings are marked. Known oddities from the code are listed
   in the files (e.g. Quit saves without exiting, buying uses Intelligence).

Open polish: name the leftover addresses; the BSS in OUTBSS.C/SPBSS.C is
small char arrays, not the real struct/array types; replace work-file comments' old names;
an eventual `runtime/port` comparison with recorded DOS sessions (see "Elsewhere").

## Where things stand

**Note: the tables below predate the C integration; the "stand-in" game code of MONDAIN/SPACE/ULTIMA/OUT is now C (see "Current task").**

| executable | status |
|---|---|
| GEN.EXE (character creation) | **Rebuilds byte-identical** (sha1 af21ad12e999d63d36d2e34381c857f55e57c14d) |
| MONDAIN.EXE (final battle) | **Rebuilds byte-identical** (sha1 9456ed0ae86a5cd6336de1b670538f1ebe0f8741): shared modules plus generated stand-ins (`src/mondain/`) |
| SPACE.EXE (space flight) | **Rebuilds byte-identical** (sha1 fdd68f5bbeb9abeb7524b385e372daad5a6d02a3): shared modules plus generated stand-ins (`src/space/`) |
| ULTIMA.EXE (loader, title) | **Rebuilds byte-identical** (sha1 6589c21ebd80a03dec5b1d0d07068a638f450ee0): shared modules plus stand-ins; `src/ultima/postlink.py` appends the envelope's remains from gamedata at build time |
| OUT.EXE (the game) | **Rebuilds byte-identical** (sha1 e4048b57ea64c0bdf51c7ec74f939002711a9475): shared modules plus stand-ins (`src/out/OUT1-3.ASM`, split because MASM 3.00 runs out of memory on 36 KB) |

Every EXE is built from the same pieces (link order in `src/NAME/NAME.lnk`, layouts in
docs/MAP.md):

| piece | source | state |
|---|---|---|
| startup CXINIT | `src/lattice/C.ASM` | Hand-written MASM, shared by all five |
| game code | `src/gen/GEN.C`; `src/mondain/MONDAIN.ASM`, `src/space/SPACE.ASM`, `src/ultima/ULTIMA.ASM`, `src/out/OUT1-3.ASM` | GEN: C, all 29 functions. The others: generated MASM stand-ins (each owns its EXE's game data and all BSS before _IOB) |
| Lattice library | `src/lclib/*.ASM` (73 modules) | Generated MASM with symbolic addresses; each EXE links its subset (GEN 49, ULTIMA 69); about 20 are byte-identical to 3.00g's |
| Origin library | `src/origin/*` (47 modules) | OR* first found in GEN, OM* in MONDAIN, OS* in SPACE, OU* in ULTIMA, OO* in OUT. 3 are C (OR387E, OR39C5, OR3AD7; `-ms -v -w -cs`), the rest generated MASM |
| _IOB | `src/lclib/_IOB.ASM` | Lattice's FILE table (118h bytes of UDATA); must stay last (its UDATA ends BSS) |
| ULTIMA's envelope | `src/ultima/postlink.py` | Copies the protection envelope's remains from gamedata at build time |

Names are still addresses (`F_xxxx` code, `D_xxxx` DGROUP) except for library functions and
startup symbols; `src/NAME/NAME.syms` maps every address of NAME.EXE to its name. GEN.C functions
carry a one-line comment saying what they do.

Steps 1-3 of "Next steps" are done and all five EXEs link identical (step 4, first part). Next:
the game code of MONDAIN, ULTIMA, SPACE and OUT in C (step 4.3; started, see "Current task").

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
- The header checksums of all five are the ones LINK writes; ULTIMA.EXE's no longer adds up
  because the envelope was appended after LINK (postlink.py keeps LINK's value).
- Library modules: LINK takes them in the order references resolve, in passes (game, then
  Origin, then the Lattice modules Origin needs), so each EXE has its own order. Data-only
  modules go where their data lies; a module's code and data must keep the same relative order.

## How to build and check

```sh
python3 tools/setup.py --archives <dir with Karl's .7z/.zip> [--masm3 <MASM 3.0 .img>]
python3 tools/build.py gen          # -> build/gen/GEN.EXE, compared with gamedata/GEN.EXE
for e in gen mondain space ultima out; do python3 tools/build.py $e; done   # all five
```

Expected sha1: GEN af21ad12..., MONDAIN 9456ed0a..., SPACE fdd68f5b..., ULTIMA 6589c21e...,
OUT e4048b57... (full values in the table above). build.py fails on assembler errors, LINK
errors and LINK's "defined more than once"; it runs `src/NAME/postlink.py` when there is one.
tools/dos.py runs the DOS commands from a batch file (DOSBox 0.74's autoexec overflows at ~4 KB).

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
differently as bytes until it matches (force list in OUTDIR/NAME.force). It also reads
OUTDIR/NAME.imm and NAME.ptr (below). These files were deleted after generation: the .ASM is
the source, and several have hand edits (OFFSET of code labels, tables of code addresses as DW
labels, PARA DATA), so regenerating one means redoing those. MASM 3.00 runs out of memory
somewhere above ~15 KB of code in one module.
In a .syms file, `C` = code entry, `L` = code label on data (never disassembled from, e.g.
`L 2FC0 XLOADER`), `D` = DGROUP data.

`xmap.py EXE -o NAME.syms --map` finds the shared modules (startup, src/lclib, src/origin) in an
EXE, learning them from every build in its REFS list (gen, mondain, space, ultima, out: build
them first) with the objects' fixups as the mask. It names their code and data with the names
they have in their sources (all of a module's data labels move with it), adds F_/D_xxxx names
for the rest (none inside a placed module's DATA; a clash with a module name gets the EXE's
initial as suffix, e.g. D_07FA_S) and prints the module map with the gaps. Names it cannot
know (a data-only module nobody references, such as OR09DA's D_09DA) are added by hand.

`addrimm.py EXE START END OTHER.EXE -o OUT/NAME.imm` finds the 16-bit immediates that are
addresses (they differ between the two EXEs' copies of the code); genasm.py reads NAME.imm and
writes them `OFFSET DGROUP:name` (every target needs a symbol in the .syms file first).
`addrimm.py EXE --data D0 D1 OTHER.EXE OD0 -o OUT/NAME.ptr` does the same for near pointers in a
module's data (written `DW name`). A shared module needs both before another EXE can link it.

Other tools: `exediff.py` (header, relocations, image), `cmp.py` (object vs EXE),
`omf.py` (object/library reader), `fmap.py` (function map), `libmatch.py`, `libfuzzy.py`,
`libsim.py`, `libdir.py` (library identification), `rdis.py` (recursive disassembly).

## Conventions

- Python: on Windows use `build/venv/Scripts/python` for the tools (the system Python has no
  capstone, and lc.py/cmp.py import it).
- git author: Karl <30571394+karlclinckspoor@users.noreply.github.com>; commit messages end
  with the Co-Authored-By line (and Claude-Session when a session link exists). Never commit
  gamedata or tool archives (`.gitignore` covers `gamedata/`, `build/`, `tools/*.7z`,
  `tools/*.zip`, `.claude/settings.local.json`). `.claude/settings.json` (an allowlist) is
  untracked, Karl's to decide.
- `.gitattributes` keeps .ASM files byte for byte (`-text`): Karl's global core.autocrlf would
  otherwise store them LF and break MASM in a fresh checkout.
- Work now happens directly in `C:\Users\karl9\Downloads\Claude\u1-decompiled\` on Windows (Git
  Bash and PowerShell). From a cloud session, the old route still applies: tar the repo (`.git`
  plus tracked files) into /mnt/user-data/outputs and untar it there with the device shell.
- Git Bash's `sed -i` drops CRs. MASM 3.00 needs CRLF in .ASM files (the C files are LF), so
  edit .ASM files with Python (binary mode) or the Edit tool.
- Do not name a tool `dis.py` in tools/: it shadows Python's `dis` module and breaks capstone.
- In the cloud, building emu2 or downloading prebuilt binaries was blocked by the safety
  classifier, so DOSBox from apt is the DOS runner there (about 1.5 s per compile); on Windows
  it is GOG's DOSBox 0.74 in gamedata/DOSBOX.

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
2. ~~Split LCLIB.ASM into Lattice modules~~ **done**, and extended with the modules the other
   EXEs add: 73 in `src/lclib/` (table and method in docs/MAP.md). Open: the modules identical
   to 3.00g could link the real objects (`LIB LCS *NAME`) instead.
3. ~~ULTIMA.EXE~~ **done**: identical link with stand-ins; `src/ultima/postlink.py` appends the
   envelope's remains (filler from image 88E4, the stub at 88F0 that restores SS:SP and jumps to
   the startup, 0x500 encrypted-looking bytes) from gamedata at build time and sets the header's
   size, SS:SP 09E1:0064 and CS:IP 088F:0000. The bytes are never committed.
4. **MONDAIN, SPACE, OUT** (and ULTIMA's game code). Identical links are done for all of them
   (how: step 4.1-4.2 below); their game code is still generated stand-ins, to be replaced by C.
   Suggested order: MONDAIN (7.7 KB, 03E9-2230), ULTIMA (2.6 KB, 03E9-0E4D), SPACE (14.8 KB,
   03E9-3DED), OUT (36 KB, 03E9-908B, in parallel by subsystem; OUT1-3 are split at arbitrary
   function entries 3430/63B3, not at real module boundaries). First find the real C module
   boundaries (data and string order, where each function's data lies). The C files take the
   stand-ins' place in the .lnk; lc.py checks functions against any EXE
   (`--exe gamedata/NAME.EXE --syms src/NAME/NAME.syms`); build.py checks the whole link.
   The steps that got each EXE to an identical link, for reference:
   1. `build.py` for the reference builds, then `xmap.py EXE -o src/NAME/NAME.syms --map`:
      shared modules placed and named, gaps listed. Give each gap a module (genasm.py; new Origin
      modules are named OM/OS/OU/OO+address after the EXE they were first found in, library
      functions get their Lattice names: fmap.py/libmatch.py/libsim.py identify them), place
      data-only modules by their data, and work out the data each new module owns from the
      references (fmap.py) and from where the next known module's data starts.
   2. A module that another EXE also links needs its address immediates (addrimm.py) and data
      pointers (addrimm.py --data) symbolic; generate it with NAME.imm/NAME.ptr.
   3. Write the .lnk in code order, data-only modules in data order; watch for PARA DATA
      (assembly modules, even empty ones like DUNIQUE: it must come before BUFSIZ).
5. **Readable names**: rename F_/D_ via .syms plus the sources (the U6 readable pass is the
   model); ScummVM's Ultima 1 engine is a naming reference. Names unique in 8 characters;
   shared with assembly: at most 8. Origin library first (shared by all EXEs).
6. **Mechanics documentation** in docs/game/ (see "Current task").

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
- MONDAIN, SPACE and OUT define `_stack` in their own data (MONDAIN 3000 at 0124, SPACE at 0130,
  OUT at 0124), so the STACK module is linked only in GEN and ULTIMA (library value 0800h).
- Uncertain module assignments that give the same bytes either way: OR09DA (the palette) as a
  data-only module rather than part of OR4071; D477E (ULTIMA, read by _PF) as its own module.
- build/exp/ and build/pilot/ hold this session's experiments (ignored by git); the agent
  brief that was in build/pilot is now docs/AGENT_BRIEF.md.
- Project doc `claude/u1-decompilation-plan.md` (claude.ai project "Ultimas") has a status
  summary that was not updated in the 2026-11-04 session (no access from it); bring it in step.
