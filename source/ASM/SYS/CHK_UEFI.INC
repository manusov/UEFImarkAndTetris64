;===========================================================================;
;                                                                           ;
;        VERIFY COMPATIBILITY HANDLER FOR CHECK UEFI CONFIGURATION.         ;
;                                                                           ;
;===========================================================================;

;--- Get UEFI firmware configuration, locate protocols -------------;
; Result: locate all used UEFI protocols                            ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF    = Flag: 0(NC)=Check Passed, 1(C)=Check Failed      ;
;          Control variables and buffers updated                    ;
;-------------------------------------------------------------------;
Check_UEFI:
;--- Locate Legacy BIOS Protocol (check CSM) ---
mov rcx,GuidLegacyBios
call Locate_Protocol
setnc [_EFI_CSM_Found+r15]		

;--- Locate Graphics Output Protocol (GOP) ---
mov rcx,GuidGop          ; Pointer to GOP GUID
call Locate_Protocol
jc Bad_UEFI              ; Go if UEFI error returned
mov [_EFI_Gop+r15],rcx   ; Save GOP interface pointer

;--- Reserved ---
; Better use 1 protocol / 1 *.INC, for good error reporting
; Make subroutine for this operation

;--- Exit points ---
Good_UEFI:
clc
ret
Bad_UEFI:
stc
ret




