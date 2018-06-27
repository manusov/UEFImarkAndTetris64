;===========================================================================;
;                                                                           ;
;         SUBROUTINES FOR SUPPORT UEFI SIMPLE TEXT OUTPUT PROTOCOL.         ;
;                                                                           ;
;===========================================================================;

;---------- Output UNICODE text string ------------------------;
; INPUT:   R15 = Global variables pool base address            ;
;                                                              ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI Status                                    ;
;--------------------------------------------------------------;
Output_Clear:
push rcx rdx
mov rcx,[_EFI_Table+r15]   ; RCX = EFI_SYSTEM_TABLE address
mov rcx,[rcx+40h]          ; RCX = EFI_SYSTEM_TABLE.ConOut
mov	edx,00000007h          ; RDX = Attribute=white, parm#2
mov eax,30h                ; RAX = Function entry number
call EFI_API               ; Here CLEAR_SCREEN
pop rdx rcx
ret

;---------- Output UNICODE text string ------------------------;
; INPUT:   RDX = String address, flat 64-bit                   ;
;          R15 = Global variables pool base address            ;
;                                                              ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI Status                                    ;
;--------------------------------------------------------------;
Output_String:
push rcx
mov rcx,[_EFI_Table+r15]   ; RCX = EFI_SYSTEM_TABLE address
mov rcx,[rcx+40h]          ; RCX = EFI_SYSTEM_TABLE.ConOut
mov eax,08h                ; RAX = Function entry number
call EFI_API               ; Here OUTPUT_STRING
pop rcx
ret

;---------- Set video attribute for next string writes --------;
; INPUT:   AL = Video attribute                                ;
;          R15 = Global variables pool base address            ;
;                                                              ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI_STATUS                                    ;
;--------------------------------------------------------------;
Output_Attribute:
push rcx rdx
mov rcx,[_EFI_Table+r15]   ; RCX = EFI_SYSTEM_TABLE address
mov rcx,[rcx+40h]          ; RCX = EFI_SYSTEM_TABLE.ConOut
movzx rdx,al               ; Convert input parameters
mov rax,28h                ; RAX = Function entry number
call EFI_API               ; Here SET ATTRIBUTE
pop rdx rcx
ret

;--- Output ASCII text string, with conversion to UNICODE -----;
; Use transit buffer: Buffer_UNICODE.                          ;
; Maximum string size = 160 chars, include NULL terminator,    ;
; limited by Buffer_UNICODE size.                              ;
;                                                              ;
; INPUT:   RSI = String address, flat 64-bit                   ;
;          R15 = Global variables pool base address            ;
;                                                              ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI Status                                    ;
;          RSI = Return modified, for easy access next data    ;
;--------------------------------------------------------------;
String_Write:
push rdx rdi
mov	rdi,Buffer_UNICODE
mov rdx,rdi
call String_Copy_Converted    ; [RSI] -> [RDI]
xor eax,eax
stosw
call Output_String            ; [RDX] -> [Screen]
pop rdi rdx
ret

;--- Write string with bold white color -----------------------;
; INPUT:   RSI = Pointer to string                             ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
String_Write_Bold:
push rax
mov al,0Fh                    ; 0Fh = Bold white video attribute
call Output_Attribute
call String_Write
mov al,07h                    ; 07h = Normal white video attribute
call Output_Attribute
pop rax
ret
