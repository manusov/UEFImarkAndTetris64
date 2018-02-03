;===========================================================================;
;                                                                           ;
;   BUILT INFORMATION STRING HANDLER FOR: SELECTED VIDEO MODE PARAMETERS.   ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for parameters of used video mode ----;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_Video_Mode:
;--- Write left part of string ---
mov rsi,String_Vmode
call String_Copy_Simple
;--- Write parameters ---
mov eax,[_Target_Vmode+r15]
call Local_Inf_Vmode_2
call String_Copy_Simple
mov ax,[_Video_Xsize+r15]
call Local_Inf_Vmode
mov ax,[_Video_Ysize+r15]
call Local_Inf_Vmode
movzx ax,byte [_Video_BPP+r15]
call Local_Inf_Vmode_1
;--- Terminate string and exit ---
jmp Inf_Terminate

;---------- Local subroutines ----------------------------------------------;

;--- Write integer number and string ----------------;
; INPUT:   RSI = String source pointer               ;
;          RDI = String destination pointer          ;
;          AX   = Number for write                   ;
;                                                    ;
; OUTPUT:  RSI    = Pointer updated                  ;
;          RDI    = Pointer updated                  ;
;----------------------------------------------------;
Local_Inf_Vmode:
call Local_Inf_Vmode_1
jmp String_Copy_Simple

;--- Write integer number ---------------------------;
; INPUT:   RDI = String destination pointer          ;
;          AX  = Number for write                    ;
;                                                    ;
; OUTPUT:  RDI    = Pointer updated                  ;
;----------------------------------------------------;
Local_Inf_Vmode_1:
movzx eax,ax
Local_Inf_Vmode_2:
mov bl,0               ; BL=Template mode
jmp Decimal_Print_32

