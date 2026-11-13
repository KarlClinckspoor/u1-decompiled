	NAME	SP3330
;
; F_3330 (alien ship turn in combat): the original bytes; the C version does not match.
;
	.8087
PROG	SEGMENT	BYTE PUBLIC 'PROG'
PROG	ENDS
DATA	SEGMENT	WORD PUBLIC 'DATA'
DATA	ENDS
UDATA	SEGMENT	WORD PUBLIC 'DATA'
UDATA	ENDS
DGROUP	GROUP	DATA,UDATA
	ASSUME	CS:PROG,DS:DGROUP

DATA	SEGMENT
	EXTRN	D_0126:BYTE
	EXTRN	D_012C:BYTE
	EXTRN	D_013A:BYTE
	EXTRN	D_013C:BYTE
	EXTRN	D_01EE:BYTE
	EXTRN	D_01F0:BYTE
	EXTRN	D_0C92:BYTE
	EXTRN	D_0C94:BYTE
	EXTRN	D_0C96:BYTE
	EXTRN	D_0C98:BYTE
	EXTRN	D_0C9A:BYTE
	EXTRN	D_0C9C:BYTE
	EXTRN	D_0C9E:BYTE
	EXTRN	D_0DA6:BYTE
	EXTRN	D_0DE0:BYTE
	EXTRN	D_0DE2:BYTE
	EXTRN	D_26C0:BYTE
DATA	ENDS
PROG	SEGMENT
	EXTRN	F_1BBF:NEAR
	EXTRN	F_1BE9:NEAR
	EXTRN	F_1C2A:NEAR
	EXTRN	F_254B:NEAR
	EXTRN	F_31D7:NEAR
	EXTRN	F_3CF1:NEAR
	EXTRN	F_4069:NEAR
	EXTRN	F_614E:NEAR
	PUBLIC	F_3330
F_3330:
	PUSH	BP                                 ; 3330 55
	SUB	SP, 0AH                             ; 3331 83 EC 0A
	MOV	BP, SP                              ; 3334 8B EC
	MOV	AX, WORD PTR D_012C                 ; 3336 A1 2C 01
	TEST	AX, AX                             ; 3339 85 C0
	JE	L_3341                               ; 333B 74 04
	DEC	WORD PTR D_012C                     ; 333D FF 0E 2C 01
L_3341:
	MOV	WORD PTR D_0DE0, 0                  ; 3341 C7 06 E0 0D 00 00
	CALL	F_1C2A                             ; 3347 E8 E0 E8
	MOV	AX, WORD PTR D_0126                 ; 334A A1 26 01
	SHL	AX, 1                               ; 334D D1 E0
	MOV	SI, AX                              ; 334F 8B F0
	PUSH	WORD PTR D_0C9E[SI]                ; 3351 FF B4 9E 0C
	CALL	F_3CF1                             ; 3355 E8 9F 29
	MOV	SP, BP                              ; 3358 8B E5
	MOV	AX, WORD PTR D_01EE                 ; 335A A1 EE 01
	MOV	BX, 196H                            ; 335D BB 96 01
	IMUL	BX                                 ; 3360 F7 EB
	MOV	SI, AX                              ; 3362 8B F0
	MOV	AX, WORD PTR D_01F0                 ; 3364 A1 F0 01
	MOV	BX, 3AH                             ; 3367 BB 3A 00
	IMUL	BX                                 ; 336A F7 EB
	ADD	SI, AX                              ; 336C 03 F0
	CMP	WORD PTR D_26C0[SI], 0              ; 336E 83 BC C0 26 00
	JNE	L_3378                              ; 3373 75 03
	DB	0E9H
	DW	L_34A3-$-2                  ; 3375 E9 2B 01
L_3378:
	MOV	AX, WORD PTR D_0C92                 ; 3378 A1 92 0C
	MOV	WORD PTR D_0C96, AX                 ; 337B A3 96 0C
	MOV	BX, WORD PTR D_0C94                 ; 337E 8B 1E 94 0C
	MOV	WORD PTR D_0C98, BX                 ; 3382 89 1E 98 0C
	MOV	CX, WORD PTR D_013A                 ; 3386 8B 0E 3A 01
	SUB	CX, AX                              ; 338A 2B C8
	MOV	AX, WORD PTR D_013C                 ; 338C A1 3C 01
	SUB	AX, BX                              ; 338F 2B C3
	PUSH	CX                                 ; 3391 51
	MOV	WORD PTR [BP+8], AX                 ; 3392 89 46 08
	MOV	WORD PTR [BP+6], CX                 ; 3395 89 4E 06
	CALL	F_1BBF                             ; 3398 E8 24 E8
	MOV	SP, BP                              ; 339B 8B E5
	SUB	WORD PTR D_0C92, AX                 ; 339D 29 06 92 0C
	PUSH	WORD PTR [BP+8]                    ; 33A1 FF 76 08
	CALL	F_1BBF                             ; 33A4 E8 18 E8
	MOV	SP, BP                              ; 33A7 8B E5
	SUB	WORD PTR D_0C94, AX                 ; 33A9 29 06 94 0C
	MOV	AX, WORD PTR D_0C92                 ; 33AD A1 92 0C
	CMP	AX, 118H                            ; 33B0 3D 18 01
	JG	L_33C8                               ; 33B3 7F 13
	CMP	AX, 1EH                             ; 33B5 3D 1E 00
	JL	L_33C8                               ; 33B8 7C 0E
	CMP	WORD PTR D_0C94, 7DH                ; 33BA 83 3E 94 0C 7D
	JG	L_33C8                               ; 33BF 7F 07
	CMP	WORD PTR D_0C94, 18H                ; 33C1 83 3E 94 0C 18
	JGE	L_33E5                              ; 33C6 7D 1D
L_33C8:
	MOV	AX, WORD PTR D_0C96                 ; 33C8 A1 96 0C
	TEST	AX, AX                             ; 33CB 85 C0
	JE	L_33E0                               ; 33CD 74 11
	XOR	BX, BX                              ; 33CF 33 DB
	PUSH	BX                                 ; 33D1 53
	PUSH	WORD PTR D_0C9A                    ; 33D2 FF 36 9A 0C
	PUSH	WORD PTR D_0C98                    ; 33D6 FF 36 98 0C
	PUSH	AX                                 ; 33DA 50
	CALL	F_614E                             ; 33DB E8 70 2D
	MOV	SP, BP                              ; 33DE 8B E5
L_33E0:
	CALL	F_254B                             ; 33E0 E8 68 F1
	JMP	SHORT L_3458                        ; 33E3 EB 73
L_33E5:
	MOV	AX, WORD PTR D_0C96                 ; 33E5 A1 96 0C
	TEST	AX, AX                             ; 33E8 85 C0
	JE	L_33FD                               ; 33EA 74 11
	XOR	BX, BX                              ; 33EC 33 DB
	PUSH	BX                                 ; 33EE 53
	PUSH	WORD PTR D_0C9A                    ; 33EF FF 36 9A 0C
	PUSH	WORD PTR D_0C98                    ; 33F3 FF 36 98 0C
	PUSH	AX                                 ; 33F7 50
	CALL	F_614E                             ; 33F8 E8 53 2D
	MOV	SP, BP                              ; 33FB 8B E5
L_33FD:
	CMP	WORD PTR D_0C9C, 0                  ; 33FD 83 3E 9C 0C 00
	JE	L_3413                               ; 3402 74 0F
	MOV	AX, 8                               ; 3404 B8 08 00
	SUB	AX, WORD PTR D_0126                 ; 3407 2B 06 26 01
	MOV	BX, WORD PTR D_0DE2                 ; 340B 8B 1E E2 0D
	CMP	BX, AX                              ; 340F 3B D8
	JGE	L_341A                              ; 3411 7D 07
L_3413:
	CMP	WORD PTR D_0DE2, 0FH                ; 3413 83 3E E2 0D 0F
	JLE	L_3454                              ; 3418 7E 3A
L_341A:
	MOV	AX, WORD PTR D_0126                 ; 341A A1 26 01
	ADD	AX, 3                               ; 341D 05 03 00
	MOV	BX, 0AH                             ; 3420 BB 0A 00
	IMUL	BX                                 ; 3423 F7 EB
	PUSH	AX                                 ; 3425 50
	MOV	BX, 1                               ; 3426 BB 01 00
	PUSH	BX                                 ; 3429 53
	CALL	F_4069                             ; 342A E8 D2 2B
	MOV	SP, BP                              ; 342D 8B E5
	CMP	AX, 14H                             ; 342F 3D 14 00
	MOV	WORD PTR [BP+4], AX                 ; 3432 89 46 04
	JG	L_343E                               ; 3435 7F 07
	CMP	WORD PTR D_0C9C, 0                  ; 3437 83 3E 9C 0C 00
	JE	L_3442                               ; 343C 74 04
L_343E:
	INC	WORD PTR D_0C9A                     ; 343E FF 06 9A 0C
L_3442:
	CMP	WORD PTR D_0C9A, 6                  ; 3442 83 3E 9A 0C 06
	JLE	L_344C                              ; 3447 7E 03
	CALL	F_254B                             ; 3449 E8 FF F0
L_344C:
	MOV	WORD PTR D_0DE2, 0                  ; 344C C7 06 E2 0D 00 00
	JMP	SHORT L_3458                        ; 3452 EB 04
L_3454:
	INC	WORD PTR D_0DE2                     ; 3454 FF 06 E2 0D
L_3458:
	MOV	AX, WORD PTR D_0C92                 ; 3458 A1 92 0C
	TEST	AX, AX                             ; 345B 85 C0
	JE	L_34A3                               ; 345D 74 44
	MOV	BX, 1                               ; 345F BB 01 00
	PUSH	BX                                 ; 3462 53
	PUSH	WORD PTR D_0C9A                    ; 3463 FF 36 9A 0C
	PUSH	WORD PTR D_0C94                    ; 3467 FF 36 94 0C
	PUSH	AX                                 ; 346B 50
	CALL	F_614E                             ; 346C E8 DF 2C
	MOV	SP, BP                              ; 346F 8B E5
	CALL	F_31D7                             ; 3471 E8 63 FD
	MOV	AX, WORD PTR [BP+6]                 ; 3474 8B 46 06
	TEST	AX, AX                             ; 3477 85 C0
	JNS	L_347F                              ; 3479 79 04
	NEG	AX                                  ; 347B F7 D8
	JMP	SHORT L_3482                        ; 347D EB 03
L_347F:
	MOV	AX, WORD PTR [BP+6]                 ; 347F 8B 46 06
L_3482:
	CMP	AX, 4                               ; 3482 3D 04 00
	JL	L_349A                               ; 3485 7C 13
	MOV	AX, WORD PTR [BP+8]                 ; 3487 8B 46 08
	TEST	AX, AX                             ; 348A 85 C0
	JNS	L_3492                              ; 348C 79 04
	NEG	AX                                  ; 348E F7 D8
	JMP	SHORT L_3495                        ; 3490 EB 03
L_3492:
	MOV	AX, WORD PTR [BP+8]                 ; 3492 8B 46 08
L_3495:
	CMP	AX, 4                               ; 3495 3D 04 00
	JGE	L_34A3                              ; 3498 7D 09
L_349A:
	PUSH	WORD PTR D_0DA6                    ; 349A FF 36 D4 5A
	CALL	F_1BE9                             ; 349E E8 48 E7
	MOV	SP, BP                              ; 34A1 8B E5
L_34A3:
	ADD	SP, 0AH                             ; 34A3 83 C4 0A
	POP	BP                                  ; 34A6 5D
	RET                                     ; 34A7 C3
PROG	ENDS
	END
