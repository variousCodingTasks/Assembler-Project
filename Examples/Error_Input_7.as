; demonstrates a "memory is full" error
; this should be detcted first during 
; first pass, though first pass will
; not stop if the memory becomes full,
; however, no output files will be created
; in such case.
; each line in this example contains 32 
; characters in addition to the terminating '\0'.

S1: .string "abcdefghijklmnopqrstuvwxyz012345"
S2: .string "abcdefghijklmnopqrstuvwxyz012345"
S3: .string "abcdefghijklmnopqrstuvwxyz012345"
S4: .string "abcdefghijklmnopqrstuvwxyz012345"
S5: .string "abcdefghijklmnopqrstuvwxyz012345"
S6: .string "abcdefghijklmnopqrstuvwxyz012345"
S7: .string "abcdefghijklmnopqrstuvwxyz012345"
S8: .string "abcdefghijklmnopqrstuvwxyz012345"