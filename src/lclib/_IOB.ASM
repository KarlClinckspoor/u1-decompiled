	NAME	_IOB
;
; Lattice C library: the stdio FILE table (20 FILEs of 14 bytes), uninitialised.
; Its UDATA ends the program's BSS, so it must be the last module in the link.
;
PROG	SEGMENT	BYTE PUBLIC 'PROG'
PROG	ENDS
DATA	SEGMENT	WORD PUBLIC 'DATA'
DATA	ENDS
UDATA	SEGMENT	WORD PUBLIC 'DATA'
UDATA	ENDS
DGROUP	GROUP	DATA,UDATA
	ASSUME	CS:PROG,DS:DGROUP

	PUBLIC	_IOB

UDATA	SEGMENT
_IOB	DB	118H DUP (?)
UDATA	ENDS

	END
