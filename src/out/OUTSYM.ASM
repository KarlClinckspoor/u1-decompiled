	NAME	OUTSYM
;
; Names for tables inside the Origin text module (src/origin/OM4437.ASM, DGROUP 267C-39E8)
; that the game code uses but that have no label there: absolute DGROUP offsets.
;
	PUBLIC	placenam,landnam,princnam,enttmpl,dmgfact,hpfact,movechan,foodper,mntattr,blockmsg,persp1,persp2,dmonname
placenam	EQU	307CH
landnam	EQU	3174H
princnam	EQU	31CCH
enttmpl	EQU	31DCH
dmgfact	EQU	368CH
hpfact	EQU	3698H
movechan	EQU	36E8H
foodper	EQU	36F8H
mntattr	EQU	3708H
blockmsg	EQU	37AEH
persp1	EQU	3808H
persp2	EQU	3814H
dmonname	EQU	3930H
	END
