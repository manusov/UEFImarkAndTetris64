;===========================================================================;
;                                                                           ;
;           SUBROUTINE FOR WRITE CPU REGISTERS, FOR DEBUG PURPOSE.          ;
;                                                                           ;
;===========================================================================;

;--- Write debug dump: CPU registers --------------------------;
; INPUT:   CPU registers: RAX,RBX,RCX,RDX,RSI,RDI,RBP,RSP      ;
;                         R8 ,R9 ,R10,R11,R12,R13,R14,R15      ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Debug_Dump_Registers:
pushf
push rax rdx rsi rdi
mov al,1Fh
call Output_Attribute
mov rsi,Msg_CRLF
call String_Write
mov rsi,Debug_Dump_Names_1
mov rax,[rsp+24]
call Debug_Dump_Reg
mov rax,rbx
call Debug_Dump_Reg
mov rax,rcx
call Debug_Dump_Reg
mov rax,rdx
call Debug_Dump_Reg
mov rax,[rsp+08]		           ; RSI image
call Debug_Dump_Reg
mov rax,rdi
call Debug_Dump_Reg
mov rax,rbp
call Debug_Dump_Reg
lea rax,[rsp+40+8]
call Debug_Dump_Reg
mov rax,r8
call Debug_Dump_Reg
mov rax,r9
call Debug_Dump_Reg
mov rax,r10
call Debug_Dump_Reg
mov rax,r11
call Debug_Dump_Reg
mov rax,r12
call Debug_Dump_Reg
mov rax,r13
call Debug_Dump_Reg
mov rax,r14
call Debug_Dump_Reg
mov rax,r15
call Debug_Dump_Reg
Debug_Dump_Done:
mov al,07h
call Output_Attribute
mov rsi,Msg_CRLF
call String_Write
jmp Debug_Dump_Exit

Debug_Dump_Reg:
pushf
push rax rdx rsi rdi
mov rdi,Buffer_ASCII
mov al,' '
stosb
call String_Copy_Simple      ; Write register name
mov [rsp+08],rsi
mov ax,' ='
stosw
stosb
mov rax,[rsp+24]             ; Get input RAX image
call Hex_Print_64            ; Write register value
mov al,'h'
stosb
mov eax,000A0D20h
stosd
mov rsi,Buffer_ASCII
mov rdi,Buffer_UNICODE
mov rdx,rdi
call String_Copy_Converted
xor ax,ax
stosw                        ; Terminator = 0000h
call Output_String
Debug_Dump_Exit:
pop rdi rsi rdx rax
popf
ret

