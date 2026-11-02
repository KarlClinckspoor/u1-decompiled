	TITLE	CXINIT - Initiate execution of a Lattice C program
	NAME	CXINIT
;**
;
; Lattice C 3.1x startup module (C.OBJ, S model) as linked into the five Ultima I
; executables. Rebuilt from the executables: no copy of this version of C.ASM is
; known. Its structure follows Lattice 3.00's C.ASM and the data section follows the
; later 3.30 layout (segment table, far BSS, near/far heap), so the names are taken
; from those two where the meaning is the same. Assemble with MASM 3.00 or 4.00.
;
; This module must be first in the link: it defines the segment order
;   PROG (code) | DATA, UDATA, XSTACK (DGROUP) | FAR_BSS
; and the program starts at CXINIT, offset 0 of the code.
;
;**

TAB	EQU	09H			; tab character
STKRSV	EQU	2046			; startup stack (bytes below the 2K mark)

	.8087

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	EXTRN	_MAIN:NEAR
	EXTRN	RBRK:NEAR
PROG	ENDS

DATA	SEGMENT	WORD PUBLIC 'DATA'
DATA	ENDS

UDATA	SEGMENT	PARA PUBLIC 'DATA'
UDATA	ENDS

XSTACK	SEGMENT	PARA STACK 'DATA'
SBASE	DB	1400H DUP (?)		; the stack
XSTACK	ENDS

FAR_BSS	SEGMENT	PARA PUBLIC 'FAR_BSS'
FAR_BSS	ENDS

DGROUP	GROUP	DATA,UDATA,XSTACK
	ASSUME	CS:PROG,DS:DGROUP

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	PUBLIC	CXINIT,XCABT
CXINIT	PROC	FAR
	MOV	AX,DGROUP		; set up DS and a temporary stack
	MOV	DS,AX
	MOV	SP,STKRSV
	CLI
	MOV	SS,AX
	ADD	SP,OFFSET DGROUP:SBASE
	STI
	MOV	WORD PTR _PSP+2,ES	; save PSP segment
	MOV	_CMD+2,ES
	CLD
;
; Turn the segment table into sizes
;
	MOV	AX,ES
	SUB	_TSIZE,AX
	SUB	_XSIZE,AX
	SUB	D_002C,DGROUP
	SUB	D_0032,UDATA
	SUB	D_0038,UDATA
	SUB	D_003E,XSTACK
	SUB	D_001A,DGROUP
	SUB	D_0020,DGROUP
	SUB	_ZSIZE,FAR_BSS
;
; Get DOS version number
;
	MOV	AH,30H
	INT	21H
	MOV	WORD PTR _DOS,AX
	DEC	AL
	JG	M0
	MOV	DX,OFFSET DGROUP:DOSERR	; abort if DOS 1
	JMP	XCABT
;
; Clear the uninitialized data and the far BSS
;
M0:	PUSH	ES
	XOR	AX,AX
	MOV	CX,D_0038
	SHL	CX,1
	SHL	CX,1
	SHL	CX,1
	MOV	DI,UDATA
	MOV	ES,DI
	XOR	DI,DI
	REP	STOSW
	MOV	BX,_ZSIZE
	MOV	DX,FAR_BSS
M01:	MOV	ES,DX
	XOR	DI,DI
	SUB	BX,1000H
	JB	M02
	MOV	CX,8000H
	REP	STOSW
	ADD	DX,1000H
	JMP	M01
M02:	AND	BX,0FFFH
	MOV	CX,BX
	SHL	CX,1
	SHL	CX,1
	SHL	CX,1
	REP	STOSW
	POP	ES
;
; Examine the environment string area
;
	MOV	AX,ES:[2CH]
	MOV	WORD PTR _ENV+2,AX	; set up pointer to environment
	OR	AX,AX
	JZ	M1			; branch if null
	PUSH	ES
	LES	DI,_ENV
	XOR	AX,AX
	MOV	CX,7FFFH
M0A:	REPNZ	SCASB
	JNZ	M0C
	INC	_ENVC
	SCASB
	JNZ	M0A
	CMP	_DOS,3			; include command name for DOS 3
	JL	M0B
	MOV	DX,ES:[DI]
	ADD	DI,2
	MOV	WORD PTR _ENV,DI
M0D:	DEC	DX
	JL	M0B
	REPNZ	SCASB
	JZ	M0D
M0C:	MOV	DX,OFFSET DGROUP:ENVERR	; abort if corrupted environment
	JMP	XCABT
M0B:	MOV	_ENVL,DI		; save environment size
	POP	ES
	ADD	DI,_ENVC		; room for the vector
	ADD	DI,_ENVC
	ADD	DI,15
	SHR	DI,1
	SHR	DI,1
	SHR	DI,1
	SHR	DI,1
	MOV	_ESIZE,DI
;
; Examine command line
;
M1:	MOV	SI,80H			; check command line
	MOV	CL,ES:[SI]
	XOR	CH,CH
	JCXZ	M12			; branch if null
M10:	INC	SI			; scan for first arg
	MOV	AL,ES:[SI]
	CMP	AL,'='
	JE	M4			; branch if stack size
	CMP	AL,' '
	JE	M11			; branch if white space
	CMP	AL,TAB
	JNE	M12			; branch if normal arg
M11:	LOOP	M10
M12:	JMP	M5			; branch if no args found
;
; Get stack and heap sizes from command line
;
M4:	CALL	GETNUM			; get stack size
	JC	M47 			; branch if error
	TEST	BX,BX
	JZ	M43 			; bypass if size is 0
	CMP	AL,'K'
	JE	M41
	CMP	AL,'k'
	JNE	M42			; branch if not kilobytes
M41:	TEST	BX,0FFC0H
	JNZ	M47			; error if size > 63
	XCHG	BH,BL 			; multiply by 1024
	SHL	BH,1
	SHL	BH,1
	DEC	CX			; advance to next character
	JZ	M42
	INC	SI
	MOV	AL,ES:[SI]
M42:	MOV	_STACK,BX		; save stack size
M43:	TEST	CX,CX
	JZ	M5			; branch if end of command line
	CMP	AL,' '
	JE	M11			; loop if blank
	CMP	AL,TAB
	JE	M11			; loop if tab
	CMP	AL,'/'
	JNE	M47			; branch if not slash
	CALL	GETNUM			; get heap size
	JC	M47 			; branch if error
	XOR	DX,DX			; long result is in DX,BX
	TEST	BX,BX
	JZ	M45			; branch if size is 0
	CMP	AL,'K'
	JE	M44
	CMP	AL,'k'
	JNE	M46			; branch if not kilobytes
M44:	XCHG	BH,BL			; multiply by 1024
	ROL	BX,1
	ROL	BX,1
	MOV	DX,BX
	AND	BX,0FC00H
	AND	DX,3FFH
	DEC	CX			; advance to next character
	JZ	M46
	INC	SI
	MOV	AL,ES:[SI]
M46:	MOV	WORD PTR _MNEED,BX	; save heap size
	MOV	WORD PTR _MNEED+2,DX
M45:	TEST	CX,CX
	JZ	M5			; branch if end of command line
	CMP	AL,' '
	JE	M11 			; loop if blank
	CMP	AL,TAB
	JE	M11			; loop if tab
M47:	MOV	DX,OFFSET DGROUP:STKERR	; abort if stack/heap size error
	JMP	XCABT
;
; Size the stack, the environment section and the heap
;
M5:	MOV	AX,_STACK		; stack size in paragraphs
	ADD	AX,15
	SHR	AX,1
	SHR	AX,1
	SHR	AX,1
	SHR	AX,1
	CMP	AX,80H			; at least 2K
	JAE	M51
	MOV	AX,80H
M51:	MOV	D_003E,AX
	ADD	AX,AX
	ADD	AX,AX
	ADD	AX,AX
	ADD	AX,AX
	ADD	_TOP,AX
	MOV	AX,_ENEED		; environment section in paragraphs
	ADD	AX,15
	SHR	AX,1
	SHR	AX,1
	SHR	AX,1
	SHR	AX,1
	CMP	AX,40H			; at least 1K
	JAE	M52
	MOV	AX,40H
M52:	CMP	AX,_ESIZE
	JBE	M53
	MOV	_ESIZE,AX
M53:	MOV	AX,WORD PTR _MNEED	; heap in paragraphs
	MOV	BX,WORD PTR _MNEED+2
	ADD	AX,15
	ADC	BX,0
	CMP	BX,15
	JA	M47
	SHR	AX,1
	SHR	AX,1
	SHR	AX,1
	SHR	AX,1
	ROR	BX,1
	ROR	BX,1
	ROR	BX,1
	ROR	BX,1
	OR	AX,BX
	CMP	AX,80H			; at least 2K
	JAE	M54
	MOV	AX,80H
M54:	MOV	_HSIZE,AX
	ROL	AX,1
	ROL	AX,1
	ROL	AX,1
	ROL	AX,1
	MOV	WORD PTR _MNEED,AX
	AND	WORD PTR _MNEED,0FFF0H
	AND	AX,15
	MOV	WORD PTR _MNEED+2,AX
;
; Check that everything fits
;
	MOV	AX,ES:[2]		; top of memory
	CMP	_ZSIZE,0
	JE	M55
	MOV	AX,FAR_BSS
M55:	MOV	BX,XSTACK
	SUB	AX,BX
	SUB	AX,D_003E
	JB	M56
	ADD	BX,D_003E
	MOV	WORD PTR _EBASE+2,BX
	SUB	AX,_ESIZE
	JB	M56
	ADD	BX,_ESIZE
	MOV	_HBASE,BX
	SUB	AX,_HSIZE
	JB	M56
	ADD	BX,_HSIZE
	SUB	BX,D_002A
	CMP	BX,1000H
	JBE	M57
M56:	MOV	DX,OFFSET DGROUP:MEMERR	; abort if insufficient memory
	JMP	XCABT
M57:	MOV	AX,FAR_BSS
	SUB	AX,WORD PTR _PSP+2
	MOV	_TSIZE,AX
	MOV	AX,WORD PTR _EBASE+2	; environment section relative to DGROUP
	SUB	AX,DGROUP
	SHL	AX,1
	SHL	AX,1
	SHL	AX,1
	SHL	AX,1
	MOV	WORD PTR _EBASE,AX
	MOV	WORD PTR _EBASE+2,DGROUP
	PUSH	CX
	PUSH	ES
	PUSH	SI
	CALL	RBRK			; reset memory pool
	POP	SI
	POP	ES
	POP	CX
	OR	AX,AX
	JNZ	M56
;
; Copy the command line to the stack, splitting it into arguments
;
	MOV	SP,_TOP
	SUB	SP,CX
	DEC	SP
	MOV	DI,SP
	PUSH	DS
	PUSH	ES
	POP	DS
	PUSH	SS
	POP	ES
	MOV	BX,1			; argument count
	JCXZ	M62
M60:	LODSB
	CMP	AL,' '
	JE	M61
	CMP	AL,TAB
	JNE	M63
M61:	LOOP	M60
M62:	MOV	BYTE PTR ES:[DI],0
	JMP	M69
M63:	INC	BX
	CMP	AL,'"'
	JE	M66
	JMP	M65
M64:	LODSB
	CMP	AL,' '
	JE	M68
	CMP	AL,TAB
	JE	M68
M65:	STOSB
	LOOP	M64
M68:	XOR	AX,AX
	STOSB
	OR	CX,CX
	JNZ	M61
	JMP	M62
M66:	LODSB				; quoted argument
	CMP	AL,'"'
	JE	M68
	CMP	AL,'\'
	JNE	M67
	DEC	CX
	JZ	M68
	LODSB
M67:	STOSB
	LOOP	M66
	JMP	M68
M69:	POP	DS
	MOV	_ARGC,BX
;
; Program name: from the environment under DOS 3, else an empty string
;
	CMP	BYTE PTR _DOS,2
	JG	M70
	XOR	AX,AX
	PUSH	AX
	INC	SP
	MOV	_ARG,SP
	JMP	M72
M70:	LES	DI,_ENV
	XOR	AX,AX
	MOV	CX,0FFFFH
	REPNZ	SCASB
	INC	CX
	ADD	SP,CX
	MOV	DI,SP
	MOV	_ARG,DI
	PUSH	DS
	LDS	SI,_ENV
	PUSH	SS
	POP	ES
M71:	LODSB
	STOSB
	OR	AL,AL
	JNZ	M71
	POP	DS
	MOV	WORD PTR _ENV,0
;
; Build the argument vector
;
M72:	MOV	AX,_ARGC
	INC	AX
	ADD	AX,AX
	AND	SP,0FFFEH
	SUB	SP,AX
	MOV	SI,SP
	PUSH	DS
	POP	ES
	MOV	DI,_ARG
	MOV	_ARGV,SI
	XOR	AX,AX
	MOV	BX,_ARGC
	MOV	CX,0FFFFH
M73:	MOV	SS:[SI],DI
	ADD	SI,2
	REPNZ	SCASB
	DEC	BX
	JG	M73
	MOV	SS:[SI],AX
;
; Copy the environment to its section and build its vector
;
	PUSH	DS
	LES	DI,_EBASE
	MOV	CX,_ENVL
	LDS	SI,_ENV
	REP	MOVSB
	XOR	AX,AX
	STOSB
	POP	DS
	MOV	environ,DI
	MOV	CX,0FFFFH
	MOV	BX,_ENVC
	LES	DI,_EBASE
	MOV	SI,environ
	XOR	AX,AX
M74:	MOV	[SI],DI
	ADD	SI,2
	REPNZ	SCASB
	DEC	BX
	JNZ	M74
	MOV	[SI],AX
;
; Check for an 8087
;
	FNINIT
	FNSTSW	_NDPSW
	MOV	AX,100			; delay
	MOV	DX,AX
	IMUL	DX
	TEST	_NDPSW,0B8BFH
	JNZ	M75
	INC	_NDP
;
; Call _main, and return to DOS with its value
;
M75:	PUSH	DS
	POP	ES
	CALL	_MAIN
	MOV	AX,4C00H
	INT	21H
CXINIT	ENDP

;
; Get a decimal number from the command line into BX. Carry set on overflow.
;
GETNUM	PROC	NEAR
	XOR	BX,BX
G1:	DEC	CX
	JZ	G3
	INC	SI
	MOV	AL,ES:[SI]
	CMP	AL,'0'
	JL	G3
	CMP	AL,'9'
	JG	G3
	SUB	AL,'0'
	ADD	BX,BX
	JC	G2
	MOV	DX,BX
	ADD	BX,BX
	JC	G2
	ADD	BX,BX
	JC	G2
	ADD	BX,DX
	JC	G2
	XOR	AH,AH
	ADD	BX,AX
	JNC	G1
G2:	RET
G3:	CLC
	RET
GETNUM	ENDP

;
; Abort with the message at DS:DX
;
XCABT	PROC	NEAR
	MOV	AH,9
	INT	21H
	MOV	ES,WORD PTR _PSP+2
	MOV	AX,4C01H
	INT	21H
XCABT	ENDP

PROG	ENDS

DATA	SEGMENT	WORD PUBLIC 'DATA'
	EXTRN	_STACK:WORD		; stack size requested by the program
	EXTRN	_ENEED:WORD		; environment size needed by the program
	EXTRN	_MNEED:DWORD		; heap size needed by the program

	PUBLIC	_MODEL,_DOS,_ENV,_ENVL,_PSP,_CMD
	PUBLIC	_TSIZE,_XSIZE,_ZSIZE
	PUBLIC	_TOP,_BASE,_EBASE,_ESIZE,_HBASE,_HSIZE,_MSIZE
	PUBLIC	_ARG,_ARGV,_ARGC,environ,_ENVC
	PUBLIC	_FPA,_NDP,_NDPSW,_NDPCW,_FPERR,_SIGFPE,_OSERR
	PUBLIC	STKERR,MEMERR,HEAPERR

_MODEL	DW	0			; 0000 S model indicator
_DOS	DB	0			; 0002 DOS major version number
	DB	0			;      DOS minor version number
_ENV	DD	0			; 0004 address of original environment
_ENVL	DW	0			; 0008 size of original environment (bytes)
_PSP	DD	0			; 000A address of program segment prefix
_CMD	DW	80H			; 000E address of original command line
	DW	0
;
; Segment table: for each section, its frame and the frame or offset of its end.
; The startup turns the frames into sizes in paragraphs.
;
_TSIZE	DW	FAR_BSS			; 0012 total size in paragraphs
_XSIZE	DW	DGROUP			; 0014 (frame of DGROUP, relative to PSP)
	DW	0
D_0018	DW	DGROUP			; 0018
D_001A	DW	DGROUP			; 001A
	DW	0
D_001E	DW	DGROUP			; 001E
D_0020	DW	DGROUP			; 0020
	DW	0
D_0024	DW	FAR_BSS			; 0024 far BSS section
_ZSIZE	DW	FAR_BSS			; 0026 far BSS size in paragraphs
	DW	0
D_002A	DW	DGROUP			; 002A static data section
D_002C	DW	UDATA			; 002C its size in paragraphs
	DW	OFFSET DGROUP:UDATA
D_0030	DW	DGROUP			; 0030 uninitialized data section
D_0032	DW	UDATA			; 0032
	DW	OFFSET DGROUP:UDATA
D_0036	DW	DGROUP			; 0036
D_0038	DW	XSTACK			; 0038 its size in paragraphs
	DW	OFFSET DGROUP:SBASE
D_003C	DW	DGROUP			; 003C stack section
D_003E	DW	FAR_BSS			; 003E its size in paragraphs
_TOP	DW	OFFSET DGROUP:SBASE	; 0040 top of stack (relative to SS)
_BASE	DW	OFFSET DGROUP:SBASE	; 0042 base of stack (relative to DS)
_EBASE	DD	0			; 0044 address of environment section
_ESIZE	DW	0			; 0048 environment section size (paragraphs)
	DW	0			; 004A
_HBASE	DW	0			; 004C heap base frame
_HSIZE	DW	0			; 004E heap size in paragraphs
_MSIZE	DW	0			; 0050
_ARG	DW	0			; 0052 command line copy on the stack
_ARGV	DW	0			; 0054 argument vector pointer
_ARGC	DW	1			; 0056 argument count
environ	DW	0			; 0058 environment vector pointer
	DW	DGROUP
_ENVC	DW	0			; 005C environment count
	DW	0			; 005E
	DW	0			; 0060
	DW	0			; 0062
	DW	0			; 0064
	DW	0			; 0066
_FPA	LABEL	BYTE
_NDP	DB	0			; 0068 non-zero if 8087 is installed
_NDPSW	DW	0FFFFH			; 0069 8087 status word
_NDPCW	DW	0			; 006B 8087 control word
_FPERR	DW	0			; 006D floating point error code
_SIGFPE	DW	0			; 006F floating point error signal
_OSERR	DW	0			; 0071 DOS error code
;
; Abort messages
;
STKOVF	DB	"Stack overflow during arg parsing",0DH,0AH,"$"
DOSERR	DB	"Incompatible DOS version",0DH,0AH,"$"
STKERR	DB	"Invalid stack size",0DH,0AH,"$"
MEMERR	DB	"Insufficient memory",0DH,0AH,"$"
ENVERR	DB	"Corrupted environment",0DH,0AH,"$"
HEAPERR	DB	"Insufficient memory for specified heap size",0DH,0AH,"$"
DATA	ENDS

	END	CXINIT
