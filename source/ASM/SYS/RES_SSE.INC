;===========================================================================;
;                                                                           ;
;                     RESTORE CONTEXT HANDLER FOR SSE.                      ;
;                                                                           ;
;===========================================================================;

;--- Restore CR4 and MXCSR registers after enable use SSE ----------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          Variable Sys_Bitmap and context control variables        ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;-------------------------------------------------------------------;
Restore_SSE:
ldmxcsr [_SSE_S_MXCSR+r15]    ; Load old MXCSR
mov rax,[_SSE_S_CR4+r15]      ; EAX=Old CR4
mov cr4,rax                   ; Load old CR4
clc
ret
