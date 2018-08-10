;demonstrates errors that might occur
;with .ext/.ent files creation.
; in this example ".ob" and ".ext"
; are created, the ".ent" file, however,
; is not, since it contains undeclared
;variables


.entry LOOP
.entry LENGTH
.entry X
.entry mov
.entry r1
.extern L3
.extern W
;unused extern label will not be written to .ext file
.extern Ext
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