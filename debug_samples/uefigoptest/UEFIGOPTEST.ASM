;===========================================================================;
;                                                                           ;
;               UEFI GOP SIMPLE TEST. READ CURRENT PARAMETERS.              ;
;                         (C)2013-2018 IC Book Labs.                        ;
;                                 FASM source.                              ; 
;      View and edit with FASM Editor 2.0 for correct text formatting.      ;
;                                                                           ;
;===========================================================================;

INCLUDE 'equ_api.inc'    ; Equations for UEFI API
INCLUDE 'equ_vars.inc'   ; Equations for variables access

format pe64 dll efi
entry main

section '.text' code executable readable
main:
;--- Entry point ---
push rbx rcx rdx rsi rdi rbp
push r8 r9 r10 r11 r12 r13 r14 r15
cld
mov	r15,Global_Variables_Pool    ; Can't LEA,[Address] because UEFI reloc. bug
mov [_EFI_Handle+r15],rcx
mov [_EFI_Table+r15],rdx
;--- Locate Graphics Output Protocol (GOP) ---
mov	rcx,GuidGop                  ; Pointer to GOP GUID
call Locate_Protocol
jc Error_1		; Go if UEFI error returned
mov [_EFI_Gop+r15],rcx           ; Save GOP interface pointer
;--- Analysing and print current mode info ---
mov	r14,[rcx+018h]               ; R14=Mode info table pointer
mov	eax,[r14+004h]               ; EAX=Current mode number
mov bl,0
mov rdi,EntryMode
call  Decimal_Print_32           ; Build ASCII number: mode number
mov r12,[r14+008h]
mov eax,[r12+004h]
mov rdi,EntryXsize
call  Decimal_Print_32           ; Build ASCII number: X-Size
mov eax,[r12+008h]
mov rdi,EntryYsize
call  Decimal_Print_32           ; Build ASCII number: Y-Size
;--- Exit points ---
mov	al,07h
call Output_Attribute
mov	rsi,Msg_OK
call String_Write
jmp Exit_1
Error_1:
mov	al,0Ch
call Output_Attribute
mov	rsi,Msg_BAD
call String_Write
Exit_1:
call Input_Wait_Key
mov	rsi,Msg_CRLF
call String_Write
;--- Restore registers ---
pop r15 r14 r13 r12 r11 r10 r9 r8
pop rbp rdi rsi rdx rcx rbx
xor eax,eax 	                 ; Clear entire RAX = EFI_STATUS = 0
retn			                     ; Simple form of termination

;--- Code library ---
INCLUDE 'lib_efi.inc'          ; UEFI API support library
INCLUDE 'lib_inp.inc'          ; UEFI input devices support library
INCLUDE 'lib_out.inc'          ; UEFI output devices support library
INCLUDE 'lib_text.inc'         ; Test strings build support library

;--- Data section ---
section '.data' data readable writeable
INCLUDE 'dat_strn.inc'         ; Text constants
INCLUDE 'dat_efi.inc'          ; UEFI constants
INCLUDE 'dat_vars.inc'         ; Constants and variables, last for (?) data

;--- Relocation section ---
section '.reloc' fixups data discardable
