# Map of the executables

## Common layout (all five EXEs)
Small model, linked by Microsoft LINK 3.x in this order:

| part | what | source |
|---|---|---|
| code 0000 | Lattice C 3.1x startup CXINIT (1001 bytes, identical in all EXEs) | src/lattice/C.ASM |
| code | the program's own modules | src/NAME/*.C |
| code | Lattice C library modules pulled from LCS.LIB | src/NAME/LCLIB.ASM |
| code | Origin's support routines (graphics, keyboard, sound, timer) | src/NAME/ORIGIN.ASM / .C |
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
| 34BF-4260 | Origin support routines | 07C0-0A30 |
| | _IOB | BSS 0DB0-0EC8 |

17 of the library modules are byte-identical to Lattice 3.00g's LCS.LIB; the rest are a later
version (FOPEN sets errno, _FLSBF/_FILBF/_EXEC/OPEN/READ/WRITE changed), which with the startup
places the library at Lattice 3.10 or 3.20 (1986).

## ULTIMA.EXE
Header CS:IP 088F:0000 points at a stub after the program (image 0x88F0) that restores SS:SP
and jumps to the startup at load:0000; 0x500 bytes after it look encrypted and the header
checksum is wrong: the remains of a removed protection envelope. To be handled by a post-link
step that appends the stub and tail and patches the header.
