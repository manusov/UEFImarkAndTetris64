;===========================================================================;
;                                                                           ;
;        COPY BLOCK IN THE WINDOW PRIVATE BUFFER FOR SHIFT BLOCK DOWN.      ;
;                   SPECIAL FOR FAST FIGURES LINE FALL DOWN.                ;
;                                                                           ;
;===========================================================================;

;--- Copy block in the private buffer --------------------;
; This version for 32 bits per pixel only.                ;
; Alignment req. for Base and Size: 16 bytes = 4 pixels   ;
; This version for block move down,                       ;
; modify Y from down to up Y coordinate (decrement mode). ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          Use 64-bit flat memory addr., without segments ;
;          Destination down/left point coordinates:       ;
;          BX  = Base X coordinate, pixels, alignment=4   ;
;          DX  = Base Y coordinate, pixels, no alignment  ;
;                This coordinates in the private buffer   ;
;          Source down/left point coordinates:            ;
;          AX  = Base Y coordinate, pixels, no alignment  ;
;          Source Base X = Destination Base X             ;
;          Sizes both for source and destination:         ;
;          CX  = X size, pixels, alignment=4              ;
;          SI  = Y size, pixels, no alignment             ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  None                                           ;
;          SSE registers XMM0-XMM7 corrupted              ;
;---------------------------------------------------------;
Shift_Box_Down_SSE:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare for later calculate source address ---
movzx rdi,bx
shl edi,2                          ; Pixels to Bytes (32BPP)
movzx rax,ax
;--- Calculate RBX=Destination address=F(coordinates), and other parms. ---
call Local_Parms_3
;--- Calculate RDX=Source address=F(Coordinates) ---
movzx rdx,word [_Video_Xsize+r15]  ; DX must zero-ext.
shl edx,2                          ; Convert Pixels to Bytes
mul rdx
lea rdx,[rdi+rax]
add rdx,[_Use_Mem_Base+r15]        ; Add transit memory base
;--- Prepare AX=Vertical size, lines counter ---
mov ax,si
;--- Copy cycle ---
Down_Y_SSE:
mov rsi,rdx          ; RSI=Source, flat 64
mov rdi,rbx          ; RDI=Destination, flat 64
;--- First X-cycle: main aligned part ---
Down_Align_SSE:
push rcx
shr ecx,7-2          ; RCX=Size, 128-byte blocks
jz Down_EndX_SSE
;--- Load 128 bytes ---
Down_X_SSE:
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
add rsi,128        ; 8 x MOVAPS = 128 bytes
add rdi,128
loop Down_X_SSE    ; This LOOPQ, use RCX
Down_EndX_SSE:
pop rcx
;--- Second X-cycle: right misaligned part ---
push rcx
and ecx,00011111b
rep movsd              ; Use RSI, RDI, RCX
pop rcx
;--- End of X-cycles ---
sub rbx,rbp
sub rdx,rbp
dec ax
jnz Down_Y_SSE
;--- Exit ---
jmp Exit_Draw_Char
