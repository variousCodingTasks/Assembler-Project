; contains all possible instructions and directives
; should contain some warnings but no errors
;all three output file should be produced



Arr: .data 1, 3 , -15 , 100
mov #-15, r2
.entry S3
.extern Ex4
cmp Ex4 , L3
add r5, S3.2
sub r1,   r3
.entry Arr
not S5.1
S3: .struct 8 , "testing..." 
clr Ex
INs: lea  Arr ,r5
inc Ex
.extern Y
L3: .string "aaaaa" 
mov #20, Z
dec S5.1
LOO: .entry S5
S5: .struct -15 , "bbb"
jmp r7
bne S1.2
TEST: .extern  Ex
;Test
TOD: red Arr
prn #234
jsr Y
S1: .struct -19 , "abcd"
rts
stop
BLOOP: .extern Z