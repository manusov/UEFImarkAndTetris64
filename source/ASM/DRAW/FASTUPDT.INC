;===========================================================================;
;                                                                           ;
; COPY BLOCK FROM WINDOW PRIVATE BUFFER TO TRANSIT BUFFER AND VIDEO MEMORY. ;
;     SPECIAL FOR FAST UPDATE X,Y-LIMITED REGION, NOT UPDATE ALL WINDOW.    ;
;                     SEE NORMAL MODE: SCRCOPY.INC.                         ;
;                                                                           ;
;===========================================================================;

;--- Copy block from private buffer to transit and VRAM --;
; This version for 32 bits per pixel only.                ;
; Block limited by X and Y for minimize number of writes, ;
; special alignment for X-base and X-size required.       ;
; this subroutine used instead Screen_Copy if speed is    ;
; critical, results visualized just after write, without  ;
; Screen_Copy subroutine.                                 ;
;                                                         ;
; INPUT:   R15 = Pointer to global variables pool         ;
;          RAX = Linear 64-bit address of source buffer,  ;
;                data copied from this buffer to transit  ;
;                buffer and VRAM, alignment required:     ;
;                base alignment = 16 bytes  = 4 pixels    ;
;                size alignment = 128 bytes = 32 pixels   ;
;                (8 instructions SSE MOVAPS used)         ;
;                                                         ;
;          BX  = Base X coordinate, pixels, alignment=4   ;
;          DX  = Base Y coordinate, pixels, no alignment  ;
;                This coordinates in the private buffer   ;
;          CX  = X size, alignment=32                     ;
;          SI  = Y size, no alignment                     ;
;                This size of visualized part of p.buf.   ;
;                                                         ;
;          EBX.[31-16] = Private buffer window X-offset   ;
;          EDX.[31-16] = Private buffer window Y-offset   ;
;          ECX.[31-16] = Private buffer window X-size     ;
;          ESI.[31-16] = Private buffer window Y-size     ;
;                                                         ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  None                                           ;
;          SSE registers XMM0-XMM7 corrupted              ;
;---------------------------------------------------------;
Fast_Update_Video_SSE:
push rax rbx rcx rdx rdi rbp rsi
;--- Make local stack frame ---
; qword [rsp+00] = Source base address (private buffer)
; qword [rsp+08] = Length in 128-byte blocks
; qword [rsp+16] = Source addend for next string select
; qword [rsp+24] = Destination#1 base address (transit buffer)
; qword [rsp+32] = Destination#1 Y-addend for next string select
; qword [rsp+40] = Destination#2 base address (video memory)
; qword [rsp+48] = Destination#2 Y-addend for next string select
;--- Start pushes frame with input parameters ---
; qword [rsp+56+48] = Input RAX = Linear address of source buffer
; word  [rsp+56+40] = Input BX = Base X coordinate, pixels, alignment=4
; word  [rsp+56+24] = Input DX = Base Y coordinate, pixels, no alignment
; word  [rsp+56+32] = Input CX = X size, alignment=32
; word  [rsp+56+00] = Input SI = Y size, no alignment
; word  [rsp+56+42] = Input EBX.[31-16] = Private buffer window X-offset
; word  [rsp+56+26] = Input EDX.[31-16] = Private buffer window Y-offset
; word  [rsp+56+34] = Input ECX.[31-16] = Private buffer window X-size
; word  [rsp+56+02] = Input ESI.[31-16] = Private buffer window Y-size
;----------
sub rsp,56
;--- Set common parameters and parameters for Source = Private buffer ---
mov ax,[rsp+56+32]           ; Get input: draw Xsize
shr eax,5                    ; Units = 128-byte blocks
and eax,000007FCh            ; Convert to 64 and align
mov [rsp+08],rax             ; Set length
mov ax,[rsp+56+34]           ; Get input: p.buffer Xsize
shl eax,2                    ; Units = Bytes
and eax,0000FFFCh            ; Convert to 64 and align
mov [rsp+16],rax             ; Set source Y-addend
movzx rsi,word [rsp+56+24]   ; Get input: Ybase
mul rsi
mov di,[rsp+56+40]     ; Get input: Xbase
shl edi,2              ; Units = Bytes
and edi,0000FFFCh      ; Convert to 64 and align
add rax,rdi
add rax,[rsp+56+48]    ; Add source address
mov [rsp+00],rax       ; Set position at priv. buf.
;--- Set parameters for Destination#1 = Transit buffer ---
; Also initializing DF, RBX=Address=F(coordinates)
; Here used input:  BX=BaseX, DX=BaseY, RCX=Xsize, Global variables
; Here used output: DF=0, RBX=Base, RBP=Raster length
mov dx,[rsp+56+24]         ; Get base Y coordinate
add bx,[rsp+56+42]         ; Private + Transit Xoffs.
add dx,[rsp+56+26]         ; Private + Transit Yoffs.
movzx edi,bx
shl edi,2                  ; Also clear RDI[63-32]
movzx rsi,dx
call Local_Parms_3
xchg rax,rbp
mov [rsp+24],rbx           ; Set transit buffer address
mov [rsp+32],rax           ; Set transit buf. Y-addend
;--- Set parameters for Destination#2 = Video memory ---
movzx rax,word [_Video_Xbytes+r15]
mov rcx,[_Vmem_Base+r15]
mov [rsp+48],rax           ; Set Video RAM Y-addend
mul rsi                    ; Calculate Y-offset
add rdi,rax                ; Add X-offset
add rdi,rcx
mov [rsp+40],rdi           ; Set Video RAM address
;--- Init video data copy cycle, EAX=Addend, DX=Ycounter, RSI=Source base ---
mov rax,128
mov dx,[rsp+56+00]
mov rsi,[rsp+00]
;--- Start Y-cycle, RCX=Xcounter ---
FastUpd_00:
mov rcx,[rsp+08]
;--- Start X-cycle, Read 128 bytes from Private Buffer, pointer=RSI ---
FastUpd_01:
movaps xmm0,[rsi+000h]
movaps xmm1,[rsi+010h]
movaps xmm2,[rsi+020h]
movaps xmm3,[rsi+030h]
movaps xmm4,[rsi+040h]
movaps xmm5,[rsi+050h]
movaps xmm6,[rsi+060h]
movaps xmm7,[rsi+070h]
;--- Write 128 bytes to Transit Buffer, pointer=RBX ---
movaps [rbx+000h],xmm0
movaps [rbx+010h],xmm1
movaps [rbx+020h],xmm2
movaps [rbx+030h],xmm3
movaps [rbx+040h],xmm4
movaps [rbx+050h],xmm5
movaps [rbx+060h],xmm6
movaps [rbx+070h],xmm7
;--- Write 128 bytes to Video RAM, pointer=RDI ---
movaps [rdi+000h],xmm0
movaps [rdi+010h],xmm1
movaps [rdi+020h],xmm2
movaps [rdi+030h],xmm3
movaps [rdi+040h],xmm4
movaps [rdi+050h],xmm5
movaps [rdi+060h],xmm6
movaps [rdi+070h],xmm7
;--- Modify address ---
add rsi,rax
add rbx,rax
add rdi,rax
;--- X-Cycle ---     ; Cycle for 128-byte blocks
loop FastUpd_01      ; This as LOOPQ (RCX)
;--- Make Y-additions for draw next line ---
mov rsi,[rsp+00]
add rsi,[rsp+16]
mov [rsp+00],rsi    ; RSI=Source address
mov rbx,[rsp+24]
add rbx,[rsp+32]
mov [rsp+24],rbx    ; RBX=Destination#1 address
mov rdi,[rsp+40]
add rdi,[rsp+48]
mov [rsp+40],rdi    ; RDI=Destination#2 address
;--- Y-Cycle ---
dec dx
jnz FastUpd_00
;--- Remove frame, restore registers, exit ---
add rsp,56
jmp Exit_Draw_Char


