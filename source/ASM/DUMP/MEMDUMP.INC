;===========================================================================;
;                                                                           ;
;            SUBROUTINE FOR WRITE MEMORY DUMP, FOR DEBUG PURPOSE.           ;
;                                                                           ;
;===========================================================================;

;--- Write debug dump: Memory 256 bytes block -----------------;
; INPUT:   RSI = Block base address, flat 64                   ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Debug_Dump_Memory:
pushf
push rax rdx rsi rdi rbx rcx
;--- Prefix with change video attribute ---
push rsi
mov al,1Fh
call Output_Attribute
mov rsi,Debug_Dump_Names_2
call String_Write
pop rsi
;--- Write memory dump ---
mov rcx,16
;--- Begin string ---
Mem_Dump_1:
mov al,1Fh
call Output_Attribute
mov rdx,Msg_CRLF_U
call Output_String
; status ignored here
;--- Hex ---
push rsi
mov rdi,Buffer_ASCII
mov al,' '
stosb
mov bl,16
Dump_Hex_1:
lodsb
call Hex_Print_8
mov al,' '
stosb
dec bl
jnz Dump_Hex_1
pop rsi
;--- ASCII ---
mov ax,'  '
stosw
stosb
mov bl,16
mov ah,0                     ; Clear high byte for UNICODE
Dump_Copy_1:
lodsb
cmp al,' '
jb Dump_Copy_2
cmp al,'}'
jbe Dump_Copy_3
Dump_Copy_2:
mov al,'.'
Dump_Copy_3:
stosb                        ; 16-bit for UNICODE
dec bl
jnz Dump_Copy_1
;--- End of string ---
mov al,0
stosb
push rsi
mov rsi,Buffer_ASCII
call String_Write
pop rsi
loop Mem_Dump_1
;--- Exit with set normal video attribute ---
pop rcx rbx
jmp Debug_Dump_Done

