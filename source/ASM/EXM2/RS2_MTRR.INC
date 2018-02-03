;===========================================================================;
;                                                                           ;
;     RESTORE CONTEXT HANDLER FOR: SECONDARY (EXTREME) MTRR REMAPPING.      ;
;                                                                           ;
;===========================================================================;

;--- Restore MTRR attribute for relocated and resized video RAM BAR -----;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          MTRR control configuration variables:                         ;
;          Ext_Relocate_Flag (bit) = Relocation required and supported   ;
;          Ext_Resize_Flag (bit)   = Resize required and supported       ;
;          MTRR_Used   (dword)     = MSR address for used MTRR           ;
;          MTRR_S_Base (qword)     = Save old MTRR state, Base(i) MSR    ;
;          MTRR_S_Mask (qword)     = Save old MTRR state, Mask(i) MSR    ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=Operation failure      ;
;------------------------------------------------------------------------;
Ext_Restore_MTRR_Map:
;--- Check operation required or not ---
test byte [_Ext_Requests+r15],00001000b
jz Exit_ERMM
;--- Restore MTRR, use normal context ---
call Restore_MTRR
;--- Exit point ---
Exit_ERMM:
ret
