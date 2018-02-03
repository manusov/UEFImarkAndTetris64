;===========================================================================;
;                                                                           ;
;  RESTORE UEFI MEMORY CONFIGURATION, DE-ALLOCATE REQUESTED MEMORY PAGES.   ;
;                                                                           ;
;===========================================================================;

;-- Restore UEFI memory configuration, de-allocate pages -----------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          Memory control variables                                 ;
; OUTPUT:  CF = Flag: 0(NC)=Check Passed, 1(C)=Check Failed         ;
;-------------------------------------------------------------------;
Release_Memory:
mov rcx,[_Use_Mem_Base+r15]
mov rdx,[_Use_Mem_Size+r15]
shr rdx,12                   ; Bytes->Pages
call EFI_Free_Pages          ; Status=CF
ret
