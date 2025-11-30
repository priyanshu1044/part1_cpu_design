; Fibonacci Sequence
; Calculates first 10 numbers

MOVI R0, 0      ; F(n-2)
MOVI R1, 1      ; F(n-1)
MOVI R3, 10     ; Counter

; Print initial 0 and 1
OUT 0xFF01, R0  ; Print Number
MOVI R2, 32     ; Space
OUT 0xFF00, R2  ; Print Space
OUT 0xFF01, R1
OUT 0xFF00, R2

LOOP:
    MOV R2, R0      ; Temp = F(n-2)
    ADD R2, R1      ; Temp = F(n-2) + F(n-1)
    
    OUT 0xFF01, R2  ; Print Result
    MOVI R0, 32     ; Space
    OUT 0xFF00, R0  ; Print Space

    MOV R0, R1      ; F(n-2) = F(n-1)
    MOV R1, R2      ; F(n-1) = New Result

    MOVI R2, 1
    SUB R3, R2      ; Decrement Counter
    MOVI R2, 0
    CMP R3, R2      ; Compare with 0
    JNZ LOOP        ; If not zero, loop

HALT
