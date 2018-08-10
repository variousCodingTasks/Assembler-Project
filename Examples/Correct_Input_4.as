; same as "Correct_Input_1.as", except the lines are shuffled
; in a way that instructions/directrives keep their order,
; thus, the output files should be identical.

MAIN: mov S1.1, W
.entry LOOP
add r2,STR
STR: .string "abcdef"
LOOP: jmp W
.entry LENGTH
prn #-5
sub r1, r4
LENGTH: .data 6,-9,15
.extern L3
inc K
mov S1.2, r3
K: .data 22
bne L3
S1: .struct 8, "ab"
END: stop
.extern W