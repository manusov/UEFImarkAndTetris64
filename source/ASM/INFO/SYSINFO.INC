;===========================================================================;
;                                                                           ;
;                 GET SYSTEM INFORMATION STRINGS SUBROUTINE.                ;
;                                                                           ;
;===========================================================================;

;---------- Get system information sequence execution -------------------;
; INPUT:   R15 = Pointer to global variables pool, flat 64-bit address   ;
;          RSI = Pointer to sequence, flat 64-bit address                ;
; OUTPUT:  None                                                          ;
;          Registers corrupted, exclude R15=Saved                        ;
;------------------------------------------------------------------------;
SysInfo_Sequencer:
cld
;--- Prepare string pointer ---
mov rdi,Buffer_2
;--- Cycle ---
SysInf_Cycle:
lodsb                        ; AL = Checkpoint code for Port 80h
test al,al
jz SysInf_Exit               ; Go if AL=0, means stop
CHKOUT                       ; Out to Diagnostics Port
lodsq                        ; RAX = Subroutine address, flat 64-bit
push rsi
call rax                     ; Call information subroutine
pop rsi
jmp SysInf_Cycle
;--- Exit ---
SysInf_Exit:
mov al,0FFh                  ; FFh = sequence terminator
stosb
ret

