;===========================================================================;
;                                                                           ;
;      SKIP CONTEXT HANDLER FOR: PRIMARY (NON-EXTREME) MTRR REMAPPING.      ;
;                                                                           ;
;===========================================================================;

;--- Clear MTRR settings for non-optimal video RAM region ---------------;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          MTRR configuration variables:                                 ;
;          MTRR_Used   (dword) = MSR address for used MTRR               ;
;          MTRR_S_Base (qword) = Save old MTRR state, Base(i) MSR        ;
;          MTRR_S_Mask (qword) = Save old MTRR state, Mask(i) MSR        ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=Operation failure      ;
;------------------------------------------------------------------------;
Ext_Skip_MTRR_Map:
;--- Check operation required or not ---
mov al,[_Ext_Requests+r15]
test al,00000001b
jz Exit_ESMM
test al,00000110b
jz Exit_ESMM
test byte [_Sys_Bitmap_H+r15],00000100b
jz Exit_ESMM
and byte [_Sys_Bitmap_H+r15],11111011b
;--- Restore MTRR, use normal context ---
call Restore_MTRR
;--- Exit point ---
Exit_ESMM:
ret
