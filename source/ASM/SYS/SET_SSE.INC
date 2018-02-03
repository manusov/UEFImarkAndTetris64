;===========================================================================;
;                                                                           ;
;              SET CONTEXT HANDLER FOR ENABLE SSE INSTRUCTIONS.             ;
;                                                                           ;
;===========================================================================;

;--- Set CR4 and MXCSR registers for enable use SSE instructions ---;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;          Variable Sys_Bitmap and context control variables        ;
;-------------------------------------------------------------------;
Setup_SSE:
;--- Support SSE already verified by previous operations ---
;--- Save and modify CR4 ---
mov rax,cr4
mov [_SSE_S_CR4+r15],rax    ; Save CR4 image
or ah,2                     ; Required OSFXSR, CR4.Bit9=1
mov cr4,rax
;--- Save and modify MXCSR ---
stmxcsr [_SSE_S_MXCSR+r15]
ldmxcsr [_Load_MXCSR+r15]
;--- Set flag for restore CR4 and MXCSR request ---
or byte [_Sys_Bitmap_H+r15],00001000b    ; Mark SSE
;--- Exit points, here CF=0(NC) ---
ret
