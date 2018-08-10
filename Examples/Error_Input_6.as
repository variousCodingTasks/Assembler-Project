; this demonstrates a situtation
; where no ".ext" is created, due
; to unused declared extern labels,
; no errors should be produced in this
; case. an ".ob" is created since first
; and second passes don't find any errors.

.entry LOOP
.entry LENGTH
.extern L3
.extern W
add r2,STR
prn #-5
sub r1, r4
inc K
mov S1.2, r3
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
S1: .struct 8, "ab"