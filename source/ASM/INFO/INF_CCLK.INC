;===========================================================================;
;                                                                           ;
;        BUILT INFORMATION STRING HANDLER FOR: CPU CLOCK MEASUREMENT.       ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for CPU clock, measured by TSC -------;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_CPU_Clock:
;--- Write left part of string ---
mov rsi,String_CCLK
call String_Copy_Simple
;--- Calculate value ---
mov rax,1000000000000000            ; Femptoseconds per Second
cqo                                 ; D[127-64] = 0
imul rbx,[_CPUCLK_Period+r15],1000  ; RBX=Psec/clk
test rbx,rbx
jz Inf_CCLK_2                       ; Go if divide by zero
div rbx                             ; RAX=Frequency, KHz
cqo
mov rbx,1000
div rbx                             ; EAX=Frequency, MHz
mov bl,0                            ; BL=Template mode
call Decimal_Print_32   ; Integer part
mov al,'.'
stosb
xchg rax,rdx
cqo
mov rcx,100
div rcx

;?! BUG WITH VISUAL FIRST DIGIT CHAR (FLOAT) INDEPENDENT OF IT WEIGHT ?!

call Decimal_Print_32    ; Float part
;--- Write "MHz ---
mov rsi,String_CCLK_1
call String_Copy_Simple
jmp Inf_CCLK_1
;--- Write "?" if wrong results ---
Inf_CCLK_2:
mov al,'?'
stosb
;--- Terminate string and exit ---
Inf_CCLK_1:	
jmp Inf_Terminate

