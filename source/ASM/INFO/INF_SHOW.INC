;===========================================================================;
;                                                                           ;
;  SHOW INFORMATION STRINGS FOR SYSTEM INFORMATION AND BENCHMARKS RESULTS.  ;
;                                                                           ;
;===========================================================================;

;--- Show system information strings -------------------------------;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_Show:
mov al,0
stosb
ret       ; This step reserved