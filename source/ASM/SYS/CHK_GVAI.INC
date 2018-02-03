;===========================================================================;
;                                                                           ;
; VERIFY COMPATIBILITY HANDLER FOR GET VIDEO ADAPTER / VIDEO PROTOCOL INFO. ;
;                                                                           ;
;===========================================================================;

;--- Get UEFI Video Adapter / Video Protocol info ------------------;
; Result: verify video compatibility, get current UEFI video mode   ;
; for restore at RES_VMOD.INC                                       ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF    = Flag: 0(NC)=Check Passed, 1(C)=Check Failed      ;
;          Video control variables and buffers updated              ;
;-------------------------------------------------------------------;
Check_Video:
;--- Get GOP interface pointer ---
mov rcx,[_EFI_Gop+r15]  ; RCX=Interface pointer
;--- Analysing current mode info ---
mov rbx,[rcx+018h]      ; RBX=Mode info table pointer
mov eax,[rbx+004h]      ; EAX=Current mode number
cmp eax,[rbx+000h]      ; Compare with max. mode
ja Bad_Video            ; Go if mode number>max
;--- Save current mode number ---
mov [_Current_Vmode+r15],eax
;--- Exit points ---
Good_Video:
clc
ret
Bad_Video:
stc
ret
