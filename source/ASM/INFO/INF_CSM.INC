;===========================================================================;
;                                                                           ;
;         BUILT INFORMATION STRING HANDLER FOR: UEFI CSM PRESENCE.          ;
;      NOTE. CSM = COMPATIBILITY SUPPORT MODULE FOR LEGACY ADAPTOR ROMS.    ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for UEFI CSM presence status ---------;
; Also built strings for UEFImark Extreme Edition                   ;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_CSM:
;--- Skip or write one of two strings for Extreme Edition modes ---
mov al,[_Sys_Bitmap_L+r15]
test al,11000000b           ; D7=Remapped, D6=By firmware
jz Inf_CSM_2                ; Go if non extreme
mov rsi,String_Not_Ext      ; This for firmware context
test al,10000000b
jz Inf_CSM_3                ; Go if not remapped
mov rsi,String_Ext          ; This for remapped context
Inf_CSM_3:
call String_Copy_Simple
xor eax,eax
stosw                       ; Terminate 2 strings (00,00)
Inf_CSM_2:
;--- Write one of two strings for CSM ---
cmp byte [_EFI_CSM_Found+r15],1
mov rsi,String_CSM_On       ; This for CSM found
je Inf_CSM_1
mov rsi,String_CSM_Off      ; This for CSM not found
Inf_CSM_1:
call String_Copy_Simple
;--- Terminate string and exit ---
jmp Inf_Terminate

