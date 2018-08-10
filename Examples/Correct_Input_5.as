; another correct input which includes no errors
; or warnings, and contains all 4 types of operands
; addressings.

.entry X
.entry Y
.entry L1
.entry L2
.extern Ext1
.extern Ext2

L1: mov r1 , r2
	cmp #-13 , #+15 
	add S1.1 , X
	sub #+20 , r5
L2:	not PSW
	clr Ext1
	lea S2.2 , r3
	inc S2.1
	dec Ext2
L3:	jmp Y
	bne Y 
	red Z
	prn #15
	jsr Ext1
	rts
	stop

Y: .data 1 , 511 , -5 , -15
S1: .struct -512 , "aaaa"
S2: .struct +511 , "bbbb"
X: .data 1 , 2 , 3 , 4 , 5 , 6
Z: .string "abcde"
.string "no_label"