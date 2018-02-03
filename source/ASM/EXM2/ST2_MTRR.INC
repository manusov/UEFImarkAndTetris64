;===========================================================================;
;                                                                           ;
;       SET CONTEXT HANDLER FOR: SECONDARY (EXTREME) MTRR REMAPPING.        ;
;                                                                           ;
;===========================================================================;

;--- Set MTRR attribute for relocated and resized video RAM BAR ---------;
; Optimal attribute for video memory is Write Combining (WC)             ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          MTRR control configuration variables:                         ;
;          Ext_Relocate_Flag (bit) = Relocation required and supported   ;
;          Ext_Resize_Flag (bit)   = Resize required and supported       ;
;          Ext_PBAR_Base (qword)   = New base addr. for extreme context  ;
;          Ext_PBAR_Size (qword)   = New size for extreme context        ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=Operation failure      ;
;          MTRR control configuration variables:                         ;
;          MTRR_Used   (dword) = MSR address for used MTRR               ;
;          MTRR_S_Base (qword) = Save old MTRR state, Base(i) MSR        ;
;          MTRR_S_Mask (qword) = Save old MTRR state, Mask(i) MSR        ;
;------------------------------------------------------------------------;
Ext_New_MTRR_Map:
;--- Check operation required or not ---
test byte [_Sys_Bitmap_L+r15],00000100b
jz Exit_ENMM
;--- Get prefetchable BAR info ---
mov eax,[_Ext_PBAR_Base+r15]    ; EAX=Base, low dword
mov edx,[_Ext_PBAR_Base_E+r15]  ; EDX=Base, high dword
mov esi,[_Ext_PBAR_Size+r15]    ; ESI=Size, low dword
mov edi,[_Ext_PBAR_Size_E+r15]  ; EDI=Size, high dword
;--- Call with entry point for extreme mode ---
call Setup_MTRR_Entry
jc Exit_ENMM
;--- Mark context for MTRR restore ---
or byte [_Ext_Requests+r15],00001000b
;--- Exit point ---
Exit_ENMM:
ret
