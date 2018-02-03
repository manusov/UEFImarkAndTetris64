;===========================================================================;
;                                                                           ;
;                SUBROUTINE FOR CONTROL SEQUENCE EXECUTION.                 ;
;                                                                           ;
;===========================================================================;

;---------- Sequence execution for program start and stop ---------------;
; INPUT:   RSI = Pointer to sequence, flat 64-bit                        ;
;          R15 = Global variables pool base address                      ;
; OUTPUT:  CF  = Error flag: 0(NC)=Executed OK, 1(C)=Exit with error     ;
;          RSI = Current pointer, restart after errors possible          ;
;          Registers corrupted                                           ;
;------------------------------------------------------------------------;
Start_Stop_Sequencer:
cld
;--- Start cycle, send checkpoint ---
Seq_Cycle:
lodsb               ; AL = Checkpoint code for Port 80h
cmp al,00h          ; 00h means stop sequence
je Seq_Exit         ; Here CF=0(NC) after CMP
CHKOUT              ; Out to Diagnostics Port
;--- Get control byte ---
lodsb               ; AL=Control Byte, Check Bit
;--- Analyse Enable Bitmap ---
test al,040h
jnz Seq_Make        ; Go if unconditional mode, D6=1
mov cl,al
and cl,00001111b
mov bx,1
rol bx,cl
test [_Sys_Bitmap+r15],bx
jnz Seq_Make
add rsi,26-2        ; Skip entry = 26 bytes, 2 already
jmp Seq_Cycle
;--- Check for graphics mode, diagnostics checkpoint ---
; Macro CHKOUT must not corrupt flags
Seq_Make:
test al,al          ; Result: SF flag = D7 bit
js Seq_Grap         ; Go if graphics mode, first byte.D7=1
test al,00100000b   ; Check D5 bit (new video mode)
jnz Seq_Switch
;--- Start normal mode ---
lodsq               ; RAX=Message string offset
;--- Execute ---
push rsi
xchg rsi,rax        ; RSI=Message string address, flat 64
call String_Write
pop rsi
lodsq               ; RAX=Subroutine address, flat 64
push rsi
call rax            ; Return CF=Flag
;--- Write "OK" or "FAILED" and make next string ---
Seq_Cycle_1:
pushf
mov rsi,Msg_OK      ; Use this string if CF=0 (no errors)
jnc Ok_or_Failed    ; Go if no errors
mov rsi,Msg_FAILED  ; Use this string if CF=1 (error)
Ok_or_Failed:
call String_Write
mov rsi,Msg_CRLF
call String_Write
popf
;--- Continue, but first write error message if CF=1 ---
Seq_Cycle_2:
pop rsi
lodsq            ; RAX=Err. msg. string addr., flat 64
jnc Seq_Cycle
;--- Exit if error ---
push rsi rax
mov rsi,Msg_Error_1
call String_Write
pop rsi
call String_Write
pop rsi
stc
;--- Exit if end of sequence ---
Seq_Exit:
ret
;--- Graphics mode support ---
Seq_Grap:
lodsq
xchg rbx,rax     ; RBX=String address, flat 64
lodsq            ; RAX=Subroutine address, flat 64
push rsi rbx
call rax         ; Return CF=Flag
pop rsi
pushf
call String_Write
popf
jmp Seq_Cycle_1
;--- Switch to graphics mode support ---
Seq_Switch:
lodsq            ; RAX=String address, flat 64
push rsi
xchg rsi,rax
call String_Write
pop rsi
lodsq            ; RAX=Subroutine address, flat 64
push rsi
call rax         ; Return CF=Flag
jmp Seq_Cycle_2

