; demonstartes errors that have
; to do with labels re-decleration,
; including trying to use an instruction's
; symbol as a new label
; these errors occur dring first pass

.entry LOOP
.entry LENGTH
.extern L3
.extern W
.extern L3
.entry LENGTH
.extern LOOP
.entry W
MAIN: mov S1.1, W
add r2,STR
LOOP: jmp W
prn #-5
sub r1, r4
inc K
mov S1.2, r3
bne L3
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
S1: .struct 8, "ab"
LOOP: .data -1 , 2 , 3
mov: .data 2
r1: .string "abcd"
W:  .string "abcd"
STR: .string "abcdef"