; Timer Example
; Simulates a simple countdown timer
; Shows Fetch/Compute/Store cycle clearly

MOVI R0, 5      ; Start countdown from 5

LOOP:
    OUT 0xFF01, R0  ; Store/Output current value
    MOVI R1, 10     ; Newline
    OUT 0xFF00, R1
    
    MOVI R1, 1
    SUB R0, R1      ; Compute: Decrement
    
    MOVI R1, 0
    CMP R0, R1      ; Check if 0
    JNZ LOOP        ; Fetch next instruction if not 0

HALT
