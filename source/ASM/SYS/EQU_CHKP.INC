;===========================================================================;
;                                                                           ;
;               EQUATIONS FOR PORT 80 CHECKPOINTS GENERATION.               ;
;                                                                           ;
;===========================================================================;

;---------- Port 80h checkpoints -------------------------------------------;

MACRO CHKOUT		; Note: must not corrupt flags
{
OUT 80H,AL
}

MACRO CHK x1		; Note: corrupt AL
{
MOV AL,x1
CHKOUT
}

;---------- CPU registers dump ---------------------------------------------;

MACRO REGDUMP		; Dump only: CPU registers RAX-R15
{
call Debug_Dump_Registers
}

MACRO REGDUMPW		; Dump and wait: CPU registers RAX-R15
{
call Debug_Dump_Registers
push rax rdx
call Input_Wait_Key
pop rdx rax
}

MACRO REGDUMPH		; Dump and halt: CPU registers RAX-R15
{
call Debug_Dump_Registers
L0:
jmp L0
}

;---------- Memory dump ----------------------------------------------------;

MACRO MEMDUMP x1	; Dump only: Memory from RSI=Address
{
push rsi
mov rsi,x1
call Debug_Dump_Memory
pop rsi
}

MACRO MEMDUMPW x1	; Dump and wait: Memory from RSI=Address
{
push rsi
mov rsi,x1
call Debug_Dump_Memory
pop rsi
push rax rdx
call Input_Wait_Key
pop rdx rax
}

MACRO MEMDUMPH x1	; Dump and halt: Memory from RSI=Address
{
mov rsi, x1
call Debug_Dump_Memory
L0:
jmp L0
}

