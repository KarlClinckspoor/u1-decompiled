# Readable names

The sources used to name everything by address (`F_xxxx` code, `D_xxxx` DGROUP data). `docs/names.tsv`
maps those to readable names and `tools/rename.py` applies the map to the `.syms` files and the `.C`/`.ASM`
sources. All five EXEs still rebuild byte-identical after every stage.

## The 8-character rule

Lattice C 3.10 keeps 8 significant characters of every identifier (locals and members too), upper-cases
external names and the assembler does the same. So every new name is **at most 8 characters**, and its first
8 characters (case-insensitive) must differ from every other identifier of every EXE that uses it. Names shared
with assembly modules (Origin library, stand-ins) are used verbatim in the .ASM, so the limit also keeps the
C and assembler views of a symbol equal. `rename.py` enforces this (plus: not a C keyword, not a MASM mnemonic,
register or operator when the name occurs in an .ASM file) and writes nothing when any entry fails.

## Convention

All names are lowercase, no underscores unless needed, verb first for functions:

| group | pattern | examples |
|---|---|---|
| commands | `cmd` + word | `cmdattk`, `cmdcast`, `cmdboard`, `cmdnoise`, `cmdhuh` |
| record fields (the 820-byte character record) | `pl` + field; `rdy` readied, `own` owned, `gem` gem colour | `plname`, `plstat` (hit points then the six attributes), `plcoin`, `rdyweap`, `ownweap`, `gemred` |
| screen colours | `col` + role | `colline`, `colfill`, `colhigh`, `coltext` |
| video / text | verb + noun | `gotoxy`, `putstr`, `fillrect`, `drawline`, `pixel`, `msgline`, `vidmode` |
| sounds | `snd` + role | `sndbuzz` ... `sndhum` (sounds 0..9), `playsnd`, `playif` |
| shops and signs | `sign*`, `weap*`, `arm*`, `food*`, `trans*`, `cntr*` | `signweap`, `weapshop`, `armshop` |
| dungeon | `dung*`, `draw*` | `mkdung`, `dungmons`, `drawdung`, `dunglvl` |
| tables of names | noun + `nam`/`names` | `racenam`, `spellnam`, `wnames`, `placenam` |
| string literals shared between modules | `s` + content | `snothing`, `scopper`, `sempty` |
| per-EXE copies of the same thing | the same name in each EXE | `argv1` in ULTIMA, MONDAIN, SPACE, OUT |

Unclear routines and data keep their address names (`F_xxxx`, `D_xxxx`, `U_xxxx`); so do the Lattice library
internals (`F_1D0D`, `F_2A93`, `D_06A0` ...). The Lattice names (`EXECL`, `FOPEN`, `_IOB`, ...) and the module
names (`OR*`, `OM*`, `OS*`, `OU*`, `OO*`) were not touched.

## Scopes

An address name means the same thing in every EXE only when it belongs to a module several EXEs link. So the
map has a scope column:

* `shared`: Origin library routines and data (`src/origin`, `src/lclib`), plus game symbols that a shared
  module refers to (`D_2046`, `F_125C`, ...). Renamed in the module and in every EXE that links it. A shared
  name with no module is renamed in each EXE that has it (same meaning everywhere).
* `gen` `mondain` `space` `ultima` `out`: private to that EXE's own files. The same address names occur in
  several EXEs with different meanings (`F_03E9` is a different function in each), so these cannot be shared.
  A private entry for a name that a shared module uses is rejected.

Many record fields have an address name that is the **MONDAIN** address in SPACE and OUT (`D_2058` = record
start, `D_206E` = hit points); the map renames them by real field (docs/game/save-format.md), and the fields
that OUT and SPACE name by their own address (`D_3A90` ...) get the same readable names as MONDAIN's.

## Using the tool

```sh
build/venv/Scripts/python tools/rename.py --check            # dry run of docs/names.tsv
build/venv/Scripts/python tools/rename.py [--stage N]        # apply (entries already applied are skipped)
for e in gen mondain space ultima out; do build/venv/Scripts/python tools/build.py $e; done   # must stay IDENTICAL
```

`names.tsv` columns: scope, old, new, comment (TAB separated); `# stage N` lines group the entries:
1 Origin library routines and shared data, 2 game functions of each EXE, 3 game globals (the order the pass
was done in; each stage was rebuilt separately). To rename something later, add a line, run `--check`, then
apply: it refuses collisions (8-character keys against every identifier of the EXEs concerned, old names that
do not exist, names that are used by a shared module with a private scope).

Not renamed by the tool: `src/*/work/` drafts, the `INTEGRATION.md` notes and the other docs still speak in
address names; the stale `.syms` comments do not matter, the name is the third column.
