# Map of the executables

## Common layout (all five EXEs)
Small model, linked by Microsoft LINK 3.x in this order:

| part | what | source |
|---|---|---|
| code 0000 | Lattice C 3.1x startup CXINIT (1001 bytes, identical in all EXEs) | src/lattice/C.ASM |
| code | the program's own modules | src/NAME/*.C |
| code | Lattice C library modules pulled from LCS.LIB | src/NAME/LCLIB.ASM |
| code | Origin's support library modules (graphics, keyboard, sound, timer) | src/origin/ |
| DGROUP 0000 | startup data (segment table, argv/env, 8087 flags, abort messages), 0x123 bytes | C.ASM |
| DGROUP | DATA of the modules above, word aligned, in link order | |
| DGROUP | UDATA (BSS) in link order; Lattice's _IOB (20 FILEs) last | IOB.ASM |
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

## ULTIMA.EXE
Header CS:IP 088F:0000 points at a stub after the program (image 0x88F0) that restores SS:SP
and jumps to the startup at load:0000; 0x500 bytes after it look encrypted and the header
checksum is wrong: the remains of a removed protection envelope. To be handled by a post-link
step that appends the stub and tail and patches the header.
