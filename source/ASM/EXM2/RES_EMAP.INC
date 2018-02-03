; ( ! )
; NEED TRY CHECK NEW METHOD: USE PAGING FOR TRANSLATE LINEAR ADDRESSES OF
; LOW WINDOW INTO HIGH WINDOW, EXAMPLE:
; LINEAR SPACE E0000000H-EFFFFFFFH -> 00000003F0000000H-00000003FFFFFFFFH,
; THIS CONTEXT CAN BE COMPATIBLE WITH UEFI FIRMWARE, CAN USE GOP.BLT
; AND RUN OTHER APPLICATIONS.
; ( ! )
; OR USE ALLOCATE POOL INSTEAD ALLOCATE PAGES
; ( ! )


;===========================================================================;
;                                                                           ;
;   RESTORE CONTEXT HANDLER FOR: MMIO REGION ALLOCATION BY UEFI FIRMWARE.   ;
;                                                                           ;
;===========================================================================;

;--- Restore UEFI memory map after request MMIO space -------------------;
; Use EFI_Free_Pages service.                                            ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          PCI configuration variables:                                  ;
;          Ext_Relocate_Flag (bit) = Relocation required and supported   ;
;          Ext_Resize_Flag (bit)   = Resize required and supported       ;
;          Ext_PBAR_Base (qword)   = New base addr. for extreme context  ;
;          Ext_PBAR_Size (qword)   = New size for extreme context        ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=UEFI Error             ;
;------------------------------------------------------------------------;
Ext_Restore_EFI_MMIO_Map:
;! THIS RESERVED
clc
ret
;! THIS RESERVED

;--- Check operation required or not ---
test byte [_Ext_Requests+r15],00000001b
jz Exit_EREMM
;--- Release address space, allocated for video memory ---
mov rcx,[_Ext_PBAR_Base+r15]
mov rdx,[_Ext_PBAR_Size+r15]
shr rdx,12            ; Bytes->Pages
call EFI_Free_Pages   ; Status=CF
;--- Exit point ---
Exit_EREMM:
ret
