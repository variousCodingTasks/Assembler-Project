; demonstartes second pass errors.
; second pass will be executed only if
; no errors were detected during first
; pass.

.entry LOOP
.entry LENGTH
.extern L3
.extern W
MAIN: mov S1.1, W
add r2,STR
add r2,STRR
LOOP: jmp W1
LOOP1: jmp LOOP
prn #-5
MAIN1: mov r1.1, W
MAIN2: mov S1.1, mov.2
sub r1, r4
inc K
mov S1.2, r3
mov S1.2, MAIN1
bne L3
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
S1: .struct 8, "ab"