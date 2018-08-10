; demonstartes various types of errors
; with instructions, operands and labels

.entry LOOP
.entry LENGTH
.entry L@@
.entry X:D
.extern L3
.extern W
.extern W@
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa: .data 1
MAIN mov S1.1, W
Ma,in: sub r1, r4
mov #1 , #1
mov r1 r2
mov , r1 r2
mov r1 , r2 ;
mov r1
mov
mov S:A , r3
mov #-1.5 , r2
mov #-1; , r2
stop r1
add r2,STR
LOOP: jm W
@LOOP: jm W
2LOOP: jm W
prn #-5
jmp #15
add r1 , #100
lea #15 , r2
sub r1, r4
inc K
mov S1.2, r3
bne L3
END: stop #12
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
S1: .struct 8, "ab"