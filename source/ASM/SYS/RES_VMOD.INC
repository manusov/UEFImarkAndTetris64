;===========================================================================;
;                                                                           ;
;         RESTORE CONTEXT HANDLER FOR RETURN UEFI TEXT VIDEO MODE.          ;
;                                                                           ;
;===========================================================================;

;--- Restore video mode, used by UEFI firmware or shell ------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          Variable Sys_Bitmap and context control variables        ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;-------------------------------------------------------------------;
Restore_T_Mode:
;--- Restore original video mode ---
mov rax,8                      ; RAX=GOP.ModeSet
mov rcx,[_EFI_Gop+r15]         ; [RCX+RAX]=Call
mov rdx,[_Current_Vmode+r15]   ; RDX=Mode
call EFI_API                   ; Return: RAX, CF
jc Exit_RTM
;--- Clear screen in the original video mode ---
call Output_Clear
;--- Exit ---
Exit_RTM:
ret             ; Here CF=Error Flag


;--- THIS FOR DEBUG ---
;
;   call  Blank_Transit
;   call  Screen_Copy
;   clc
;   ret
;---

