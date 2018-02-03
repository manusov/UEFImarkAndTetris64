;===========================================================================;
;                                                                           ;
;           DRAW AND GET SPRITE, OPERATION IN THE TRANSIT BUFFER.           ;
;                                                                           ;
;===========================================================================;

;--- Draw sprite in the transit buffer -------------------;
; This version for 32 bits per pixel only.                ;
; Mode 1 = Disjunct with existed pixels, overlap if       ;
;          color pattern not equal 0FFFFFFFFh.            ;
; Results visualized on the screen after call subroutine  ;
; Screen_Copy.                                            ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          RAX = Linear 64-bit address of graphics image, ;
;                must by aligned by 16                    ;
;                (SSE MOVAPS used in future, now not)     ;
;          BX  = Base X coordinate                        ;
;          DX  = Base Y coordinate                        ;
;          CX  = X size                                   ;
;          SI  = Y size                                   ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  None                                           ;
;          MMX registers MM0, MM1 corrupted               ;
;---------------------------------------------------------;
Draw_Sprite_SSE_Disjunct:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare MM6=AND Mask, MM7=CMP Pattern ---
mov rbp,0FFFFFFFFFFFFFFFFh
movq mm7,rbp
;--- Mark modified lines for show ---
call Local_Mark
;--- Setup DF, RBP, RCX ---
;--- Calculate EBX=Address=F(coordinates) ---
call Local_Parms_3
;--- Set AX=Ysize, RSI=Source base, linear 64 ---
xchg rax,rsi
;--- Save size at units=pixels=4 bytes ---
mov edx,ecx             ; Also clear bits [63-32]
;--- Begin Y-cycle, prepare for X-cycle ---
Spdj_Y_SSE:
mov rdi,rbx             ; RDI=Base address
movzx rcx,dx            ; RCX=Size, pixels=dwords
;--- Main X-cycle: long part ---
Spdj_Long_SSE:
shr ecx,2               ; RCX=Size, 16-byte blocks
jz Spdj_EndX_SSE
Spdj_X_SSE:
;--- Conditional copy CX 16-byte blocks ---
movq mm0,[rsi+00]
movq mm2,[rsi+08]
;--- Low Qword ---
movq mm1,mm0
pcmpeqd mm1,mm7
pxor mm1,mm7
;--- High Qword ---
movq mm3,mm2
pcmpeqd mm3,mm7
pxor mm3,mm7
;--- Update memory ---
maskmovq mm0,mm1
add rdi,8
maskmovq mm2,mm3
add rdi,8
add rdi,16
;--- Cycle ---
loop Spdj_X_SSE         ; This as LOOPQ (RCX)
;--- Conditional copy tail ---
Spdj_EndX_SSE:
mov ecx,edx             ; Also clear RCX[63-32]
and ecx,3
jz Spdj_EndX1_SSE
push rax
Spdj_Tail_SSE:
lodsd                   ; Use RSI
cmp eax,0FFFFFFFFh
je Spdj_Skp_SSE
stosd                   ; Use RDI
Spdj_Skp_SSE:
loop Spdj_Tail_SSE      ; This as LOOPQ (RCX)
pop rax
;--- End of X-cycles ---
Spdj_EndX1_SSE:
add rbx,rbp
dec ax
jnz Spdj_Y_SSE
;--- Exit ---
emms
jmp Exit_Draw_Char

;--- Draw sprite in the transit buffer -------------------;
; This version for 32 bits per pixel only.                ;
; Mode 2 = Overlap all pixels.                            ;
; Results visualized on the screen after call subroutine  ;
; Screen_Copy.                                            ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          RAX = Linear 64-bit address of graphics image, ;
;                must by aligned by 16 (SSE MOVAPS used)  ;
;          BX  = Base X coordinate                        ;
;          DX  = Base Y coordinate                        ;
;          CX  = X size                                   ;
;          SI  = Y size                                   ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  None                                           ;
;          SSE registers XMM0-XMM7 corrupted              ;
;---------------------------------------------------------;
Draw_Sprite_SSE_Overlap:
push rax rbx rcx rdx rdi rbp rsi
;--- Mark modified lines for show ---
call Local_Mark
;--- Setup DF, RBP, RCX ---
;--- Calculate EBX=Address=F(coordinates) ---
call Local_Parms_3
;--- Set AX=Ysize, RSI=Source base, linear 64 ---
xchg rax,rsi
;--- Save size at units=pixels=4 bytes ---
mov edx,ecx            ; Also clear bits [63-32]
;--- Begin Y-cycle, prepare for X-cycle ---
Sprt_Y_SSE:
mov rdi,rbx            ; EDI=Base address
movzx rcx,dx           ; RCX=Size, pixels=dwords
;--- First X-cycle: left misaligned part ---
Sprt_Left_SSE:
test edi,00001111b
jz Sprt_Align_SSE
movsd                  ; Use RSI, RDI
loop Sprt_Left_SSE     ; This as LOOPQ (RCX)
;--- Second X-cycle: main aligned part ---
Sprt_Align_SSE:
push rcx
shr ecx,7-2            ; RCX=Size, 128-byte blocks
jz Sprt_EndX_SSE
Sprt_X_SSE:
;--- Load 128 bytes ---
movaps xmm0,[rsi+000h]
movaps xmm1,[rsi+010h]
movaps xmm2,[rsi+020h]
movaps xmm3,[rsi+030h]
movaps xmm4,[rsi+040h]
movaps xmm5,[rsi+050h]
movaps xmm6,[rsi+060h]
movaps xmm7,[rsi+070h]
;--- Store 128 bytes ---
movaps [rdi+000h],xmm0
movaps [rdi+010h],xmm1
movaps [rdi+020h],xmm2
movaps [rdi+030h],xmm3
movaps [rdi+040h],xmm4
movaps [rdi+050h],xmm5
movaps [rdi+060h],xmm6
movaps [rdi+070h],xmm7
;--- Cycle ---
add rsi,128
add rdi,128
loop Sprt_X_SSE        ; This as LOOPQ (RCX)
Sprt_EndX_SSE:
pop	rcx
;--- Third X-cycle: right misaligned part ---
and ecx,00011111b
rep movsd              ; Use 64-bit RCX, RSI, RDI
;--- End of X-cycles ---
add rbx,rbp
dec ax
jnz Sprt_Y_SSE
;--- Exit ---
jmp Exit_Draw_Char

;--- Get sprite from the transit buffer ------------------;
; This version for 32 bits per pixel only.                ;
; Results visualized on the screen after call subroutine  ;
; Screen_Copy                                             ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          RAX = Linear 64-bit address of destination     ;
;                buffer, data copied from transit buffer  ;
;                to destination buffer                    ;
;                must by aligned by 16 (SSE MOVAPS used)  ;
;          BX  = Base X coordinate                        ;
;          DX  = Base Y coordinate                        ;
;          CX  = X size                                   ;
;          SI  = Y size                                   ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  None                                           ;
;          SSE registers XMM0-XMM7 corrupted              ;
;---------------------------------------------------------;
Get_Sprite_SSE:
push rax rbx rcx rdx rdi rbp rsi
;--- Setup DF, RBP, RCX ---
;--- Calculate EBX=Address=F(coordinates) ---
call Local_Parms_3
;--- Set AX=Ysize, RDI=Destination base, linear 64 ---
xchg rdi,rax
xchg ax,si
;--- Save size at units=pixels=4 bytes ---
mov edx,ecx           ; Also clear RDX[63-32]
;--- Begin Y-cycle, prepare for X-cycle ---
Gspr_Y_SSE:
mov rsi,rbx           ; RSI=Base address
mov ecx,edx           ; RCX=Size, pixels=dwords
;--- First X-cycle: left misaligned part ---
Gspr_Left_SSE:
test edi,00001111b
jz Gspr_Align_SSE
movsd                 ; Use 64-bit RSI, RDI
loop Gspr_Left_SSE    ; This as LOOPQ (RCX)
;--- Second X-cycle: main aligned part ---
Gspr_Align_SSE:	push	rcx
shr ecx,7-2           ; RCX=Size, 128-byte blocks
jz Gspr_EndX_SSE
Gspr_X_SSE:
;--- Load 128 bytes ---
movaps xmm0,[rsi+000h]
movaps xmm1,[rsi+010h]
movaps xmm2,[rsi+020h]
movaps xmm3,[rsi+030h]
movaps xmm4,[rsi+040h]
movaps xmm5,[rsi+050h]
movaps xmm6,[rsi+060h]
movaps xmm7,[rsi+070h]
;--- Store 128 bytes ---
movaps [rdi+000h],xmm0
movaps [rdi+010h],xmm1
movaps [rdi+020h],xmm2
movaps [rdi+030h],xmm3
movaps [rdi+040h],xmm4
movaps [rdi+050h],xmm5
movaps [rdi+060h],xmm6
movaps [rdi+070h],xmm7
;--- Cycle ---
add rsi,128
add rdi,128
loop Gspr_X_SSE         ; This as LOOPQ (RCX)
Gspr_EndX_SSE:
pop rcx
;--- Third X-cycle: right misaligned part ---
and ecx,00011111b
rep movsd               ; Use 64-bit RSI, RDI, RCX
;--- End of X-cycles ---
add rbx,rbp
dec ax
jnz Gspr_Y_SSE
;--- Exit ---
jmp Exit_Draw_Char

