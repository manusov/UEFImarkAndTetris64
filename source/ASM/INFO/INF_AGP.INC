;===========================================================================;
;                                                                           ;
;   BUILT INFORMATION STRING HANDLER FOR: AGP VERSION AND TRANSFER MODE.    ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for AGP capabilities, include FW -----;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_AGP:
mov al,0
stosb      ; Write terminator byte (0) for empty string
ret        ; AGP function not supported at UEFI edition, see DOS debug version

