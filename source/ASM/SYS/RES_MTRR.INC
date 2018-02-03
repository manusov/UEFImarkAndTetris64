;===========================================================================;
;                                                                           ;
;                RESTORE CONTEXT HANDLER FOR MTRR SETTINGS.                 ;
;                                                                           ;
;===========================================================================;

;--- Restore MTRR settings after set video memory to WC ------------;
; This subroutine conditionally run by _Sys_Bitmap mechanism,       ;
; only if MTRR supported and re-programmed                          ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          Variable Sys_Bitmap and context control variables        ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;-------------------------------------------------------------------;
Restore_MTRR:
cli
mov ecx,[_MTRR_Used+r15]      ; ECX = MSR address
mov eax,[_MTRR_S_Base_L+r15]
mov edx,[_MTRR_S_Base_H+r15]
wrmsr                         ; Restore Base MTRR
inc ecx                       ; MSR address+1
mov eax,[_MTRR_S_Mask_L+r15]
mov edx,[_MTRR_S_Mask_H+r15]
wrmsr                         ; Restore Mask MTRR
sti
ret

