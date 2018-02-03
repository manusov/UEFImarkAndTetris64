;===========================================================================;
;                                                                           ;
;       COPY BLOCK FROM SYSTEM MEMORY TRANSIT BUFFER TO VIDEO MEMORY.       ;
;                      SEE FAST MODE: FASTUPDT.INC.                         ;
;                                                                           ;
;===========================================================================;

;--- Copy from transit buffer to video memory ------------;
; For time save, updated only strings, marked in the      ;
; bitmaps Lines_Show.                                     ;
;                                                         ;
; INPUT:   R15 = Pointer to global variables pool         ;
;          Global variables                               ;
; OUTPUT:  None                                           ;
;          SSE registers XMM0-XMM7 corrupted              ;
;---------------------------------------------------------;
Screen_Copy:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare parameters ---
cld
mov rbx,[_Use_Mem_Base+r15]           ; EBX=Source=Transit
mov rdx,[_Vmem_Base+r15]              ; EDX=Destination=VRAM
movzx rcx,word [_Video_Ysize+r15]     ; CX=Number of strings
;--- Vertical cycle (for strings), check and reset bitmap ---
Copy_V:
movzx rsi,word [_Video_Ysize+r15]
sub rsi,rcx       ; SI = Byte Offset . Bit number
mov eax,esi
shr rsi,3         ; SI=Byte Offset
and eax,7         ; AX=Bit number
mov rdi,Lines_Show
btr dword [rdi+rsi],eax
jnc Skip_H        ; Skip if this line not updated
;--- Prepare for horizontal cycle ---
mov rsi,rbx
mov rdi,rdx
movzx eax,word [_Video_Xsize+r15]
shr eax,7-2         ; -2 because Nbytes=Npixels*4
;--- Horizontal cycle (for pixels), here optimal sequence is important ---
Copy_H:
;--- Copy main part of string ---
;--- Load from system memory ---
movaps xmm0,[rsi+000h]
movaps xmm1,[rsi+010h]
movaps xmm2,[rsi+020h]
movaps xmm3,[rsi+030h]
movaps xmm4,[rsi+040h]
movaps xmm5,[rsi+050h]
movaps xmm6,[rsi+060h]
movaps xmm7,[rsi+070h]
;--- Store to video memory ---
movaps [rdi+000h],xmm0
movaps [rdi+010h],xmm1
movaps [rdi+020h],xmm2
movaps [rdi+030h],xmm3
movaps [rdi+040h],xmm4
movaps [rdi+050h],xmm5
movaps [rdi+060h],xmm6
movaps [rdi+070h],xmm7
;--- Modify address ---
add rsi,128
add rdi,128
;--- Cycle ---
dec eax         ; Cycle for 128-byte blocks
jnz Copy_H
;--- Copy string tail ---
push rcx
movzx rcx,word [_Video_Xsize+r15]
and cx,001Fh
rep movsd       ; Use RDI, RSI
pop rcx
;--- Make cycle for strings ---
Skip_H:
movzx rax,word [_Video_Xsize+r15]
shl eax,2       ; Pixels to DWords
add rbx,rax     ; Modify source address
movzx eax,word [_Video_Xbytes+r15]
add rdx,rax     ; Modify destination address
dec ecx
jnz Copy_V		
;--- Exit ---
jmp Exit_Draw_Char

