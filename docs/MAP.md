# Map of the executables

## Common layout (all five EXEs)
Small model, linked by Microsoft LINK 3.x in this order:

| part | what | source |
|---|---|---|
| code 0000 | Lattice C 3.1x startup CXINIT (1001 bytes, identical in all EXEs) | src/lattice/C.ASM |
| code | the program's own modules | src/NAME/*.C |
| code | Lattice C library modules pulled from LCS.LIB | src/lclib/ |
| code | Origin's support library modules (graphics, keyboard, sound, timer) | src/origin/ |
| DGROUP 0000 | startup data (segment table, argv/env, 8087 flags, abort messages), 0x123 bytes | C.ASM |
| DGROUP | DATA of the modules above, word aligned, in link order | |
| DGROUP | UDATA (BSS) in link order; Lattice's _IOB (20 FILEs) last | src/lclib/_IOB.ASM |
| XSTACK | 0x1400-byte stack segment (header SS:SP points at its top) | C.ASM |
| FAR_BSS | empty; marks the end of the program | C.ASM |

The header's 37-39 relocations are all segment references: the startup's DGROUP/UDATA/XSTACK/
FAR_BSS frames, ONERROR's code frame, and Origin's interrupt handlers loading DGROUP.

## GEN.EXE (character creation) - 20112 bytes, sha1 af21ad12e999d63d36d2e34381c857f55e57c14d
DGROUP paragraph 0x426.

| code | | data |
|---|---|---|
| 0000-03E9 | startup | 0000-0123 |
| 03E9-1948 | game code, main = F_1853 | 0124-0692, BSS 0A30-0DB0 |
| 1948-34BF | Lattice library: EXECL/EXECV, FCLOSE, FOPEN/FREOPEN, FREAD, FWRITE, _MAIN, ONERROR, RBRK and the memory pool, CLOSE, EXIT, OPEN, _FILBF, _FLSBF, CXSERR, GETFC, CHKUFB, GETPF, READ, WRITE, _GETBF, DOS call wrappers, DUNIQUE, _EXIT, LSEEK, STCGFE, STRMFE, _EXEC, GETFA, STRCPY, STRLEN, STCCPY, ... | 0692-07C0 |
| 34BF-4254 | Origin support library, 17 modules (below) | 07C0-0A30 |
| | _IOB | BSS 0DB0-0EC8 |

17 of the library modules are byte-identical to Lattice 3.00g's LCS.LIB; the rest are a later
version (FOPEN sets errno, _FLSBF/_FILBF/_EXEC/OPEN/READ/WRITE changed), which with the startup
places the library at Lattice 3.10 or 3.20 (1986).

## Origin support library (src/origin)
Origin's routines were a library: each EXE links the modules it needs, in the same order, and in
OUT they sit among the Lattice library modules. Comparing which of GEN's routines are next to
each other in the other four EXEs gives the module boundaries; each module's data share follows
from the data it uses (data lies in the same module order). Only four routines are C (Lattice
frame `push bp; sub sp,N; mov bp,sp`); the rest are assembly (`push bp; mov bp,sp; push si;
push di; push ds` or no frame).

| module | code (GEN) | data (GEN) | | also in |
|---|---|---|---|---|
| OR34BF | 34BF 352C 3547: detect Tandy (ROM string), set text cursor, CGA mode 4 | 07C0-07C6 "Tandy" | asm | all |
| OR357C | 357C: put character | | asm | OUT SPACE MONDAIN |
| OR3599 | 3599 35CA: fill rectangle | 07C6-07D8 | asm | all |
| OR3862 | 3862: toupper | | asm | all |
| OR387E | 387E 3920: arrows over a text cell | | C | OUT SPACE MONDAIN |
| OR39C5 | 39C5: wait for a key | | C | OUT SPACE MONDAIN |
| OR39FC | 39FC 3A1D: draw line | 07D8-07F6 | asm | all |
| OR3AD7 | 3AD7: game screen frame | | C | GEN only |
| OR3C8B | 3C8B 3D23: print number, BIOS teletype | 07F6-080A | asm | OUT SPACE MONDAIN |
| OR3D41 | 3D41 3D47 3D61: plot pixel | 080A-0832 | asm | all |
| OR3E5E | 3E5E 3E77 TIMERISR: tick wait, timer hook | 0832-0836 | asm | all |
| OR3EC4 | 3EC4: random number from the DOS clock | | asm | OUT SPACE MONDAIN |
| OR3EED | 3EED: scroll the message area up a line | | asm | OUT SPACE MONDAIN |
| OR3F62 | 3F62: play sound n (10 PC-speaker routines) | 0836-084A jump table | asm | OUT SPACE MONDAIN |
| OR4071 | 4071: fill the 200-line address table | 084A-09EC | asm | OUT (at B231); others unchecked |
| OR40EE | 40EE 4115: Tandy mode 9 / EGA mode 0Dh, with palette | 09EC-09FE | asm | all |
| OR413C | 413C 41D5 4224 4234: key input with cursor | 09FE-0A30 | asm | all |

The 12 zero bytes at code 4254 are LINK's padding: DATA is paragraph aligned (as in Lattice's
own C.ASM), so DGROUP starts at paragraph 0x426.

D_0830 holds the video mode in use: 1 EGA, 2 CGA, 4 Tandy.

## Lattice library (src/lclib)
GEN's library code and data split into the 49 modules LINK took from the library, in link order
(src/gen/gen.lnk). LINK pulls library modules in the order the references resolve, so the order
differs between the EXEs (EXIT follows EXECV in ULTIMA and OUT); module contents come from
Lattice 3.00g's LCS.LIB directory (`tools/libdir.py`), split further where the other EXEs show
a boundary (the 3.00g IOS0V2 module of DOS calls is several modules here, since OUT has DSEEK
next to DWRITE). Functions that sit together in every EXE and share no evidence stay in one
module (DOPEN = DOPEN DCLOSE DREAD DSEEK; L2C5F = IOCTL 4401h, DCREAT, DCREATX; L33F9).

Data-only modules (no code) are placed where their data lies. The DATA of ONERROR, CXSERR,
_EXIT and BUFSIZ (and Origin's OR34BF) is paragraph aligned: their data starts on a paragraph in
all of GEN, OUT, SPACE and MONDAIN, and the zero bytes before it are LINK's padding. Lattice
3.00g's DSEG macro says WORD, so this is a 3.1x change (or these modules were written without it).
"3.00g" marks the modules whose code is byte-identical to 3.00g's LCS.LIB (fixups masked).

| module | code | functions | data | |
|---|---|---|---|---|
| EXECL | 1948-197D | EXECL, EXECV |  | 3.00g |
| FCLOSE | 197D-19F0 | FCLOSE |  |  |
| FOPEN | 19F0-1B9B | FOPEN, FREOPEN |  |  |
| FREAD | 1B9B-1C09 | FREAD |  |  |
| FWRITE | 1C09-1C75 | FWRITE |  |  |
| _MAIN | 1C75-1D0B | _MAIN |  | 3.00g |
| STACK |  |  | 0692-0694 |  |
| _MNEED |  |  | 0694-0698 |  |
| _POOL |  |  | 0698-06A0 |  |
| ONERROR | 1D0B-1DA6 | ONERROR, F_1D0D, F_1D65 | 06A0-06C0 PARA | 3.00g |
| ERROR |  |  | 06C0-06C2 |  |
| RBRK | 1DA6-1E2C | RBRK |  |  |
| L1E2C | 1E2C-1EB9 | F_1E2C |  |  |
| L1EB9 | 1EB9-1ED5 | F_1EB9 |  |  |
| CLOSE | 1ED5-1F1D | CLOSE |  | 3.00g |
| EXECLE | 1F1D-1F8C | EXECLE, EXECVE |  |  |
| EXIT | 1F8C-20A0 | EXIT |  |  |
| OPEN | 20A0-22F7 | OPEN, CREAT |  |  |
| _FILBF | 22F7-2491 | _FILBF |  |  |
| _FLSBF | 2491-2769 | _FLSBF | 06C2-06C6 |  |
| _FMODE |  |  | 06C6-06C8 |  |
| D06C8 |  |  | 06C8-06E0 |  |
| CXSERR | 2769-27D4 | CXSERR, CXSERZ | 06E0-0738 PARA | 3.00g |
| GETFC | 27D4-27E8 | GETFC |  | 3.00g |
| CHKUFB | 27E8-2831 | CHKUFB |  | 3.00g |
| GETPF | 2831-28AA | GETPF | 0738-0740 |  |
| _IOMODE |  |  | 0740-0742 |  |
| _UFBS |  |  | 0742-0794 |  |
| READ | 28AA-2971 | READ |  |  |
| WRITE | 2971-2A93 | WRITE |  |  |
| L2A93 | 2A93-2C04 | F_2A93 |  |  |
| _GETBF | 2C04-2C5F | _GETBF |  | 3.00g |
| L2C5F | 2C5F-2C8F | F_2C5F, DCREAT, DCREATX |  |  |
| DOPEN | 2C8F-2CE6 | DOPEN, DCLOSE, DREAD, DSEEK |  |  |
| DUNIQUE | 2CE6-2CF7 | DUNIQUE |  | 3.00g |
| DWRITE | 2CF7-2D20 | DWRITE |  |  |
| _EXIT | 2D20-2D3A | _EXIT | 07A0-07A2 PARA | 3.00g |
| BUFSIZ |  |  | 07B0-07B2 PARA |  |
| ONEXIT | 2D3A-2D47 | ONEXIT |  |  |
| L2D47 | 2D47-2DEA | F_2D47, F_2DDE |  |  |
| LSEEK | 2DEA-2EBF | LSEEK |  | 3.00g |
| STCGFE | 2EBF-2F40 | STCGFE |  |  |
| STRMFE | 2F40-2FC0 | STRMFE |  |  |
| _EXEC | 2FC0-336E | XLOADER, _EXEC |  |  |
| GETFA | 336E-3382 | GETFA |  | 3.00g |
| STRCPY | 3382-3397 | STRCPY |  | 3.00g |
| STRLEN | 3397-33AE | STRLEN |  | 3.00g |
| STCCPY | 33AE-33F9 | STCCPY |  |  |
| L33F9 | 33F9-34BF | F_33F9, F_3407, F_3415 |  |  |

Address immediates are symbolic (`OFFSET DGROUP:_IOB`, `OFFSET DGROUP:_UFBS`, ...), _MAIN uses
`_IOB+n` for the fields of the FILE table, D06C8's heap pointer is `DW D_06CC`, and _EXEC's LEAs
into its loader are `DW XLOADER+n`, so the modules can be linked into the other EXEs.

## ULTIMA.EXE
Header CS:IP 088F:0000 points at a stub after the program (image 0x88F0) that restores SS:SP
and jumps to the startup at load:0000; 0x500 bytes after it look encrypted and the header
checksum is wrong: the remains of a removed protection envelope. To be handled by a post-link
step that appends the stub and tail and patches the header.
