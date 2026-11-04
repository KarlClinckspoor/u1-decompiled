	NAME	OR09DA
;
; Origin support library: a 16-colour palette plus border (EGA palette registers, the
; form INT 10h AX=1002h takes), data only. Its own module: in GEN and MONDAIN it follows
; OR4071's line table, SPACE has the table without it. Nothing in GEN refers to it.
;
PROG	SEGMENT	BYTE PUBLIC 'PROG'
PROG	ENDS
DATA	SEGMENT	WORD PUBLIC 'DATA'
DATA	ENDS
UDATA	SEGMENT	WORD PUBLIC 'DATA'
UDATA	ENDS
DGROUP	GROUP	DATA,UDATA
	ASSUME	CS:PROG,DS:DGROUP

	PUBLIC	D_09DA

DATA	SEGMENT
D_09DA	DB	0,1,2,3,4,5,6,7,38H,39H,3AH,3BH	; 09DA
	DB	3CH,3DH,3EH,3FH,0,0	; 09E6
DATA	ENDS

	END
