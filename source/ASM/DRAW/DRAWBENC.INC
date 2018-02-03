;===========================================================================;
;                                                                           ;
;         STORE DATA TO VIDEO MEMORY, DRAW BOX, USED FOR BENCHMARK.         ;
;                                                                           ;
;===========================================================================;

;--- Draw box for benchmark video memory write -----------;
; This version for 32 bits per pixel only.                ;
; This version for alignment restrictions only.           ;
; See also subroutine: Draw_Box_Benchmark_386             ;
; TSC used, assume support TSC already check by caller.   ;
; Lines_Show bitmap ignored here, write all strings.      ;
; Benchmark SSE-specific restrictions:                    ;
; Base address must be aligned by 16, this means          ;
; base X coordinate must be aligned by 4.                 ;
; Size nust be aligned by 128, this means                 ;
; X size must be aligned by 32.                           ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          Use 64-bit flat memory addressing, w/o segs.   ;
;          EAX = True color value for video memory write  ;
;          BX = Base X coordinate                         ;
;          DX = Base Y coordinate                         ;
;          CX = X size                                    ;
;          SI = Y size                                    ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  EDX:EAX = Delta TSC                            ;
;          SSE register XMM0 corrupted                    ;
;---------------------------------------------------------;
Draw_Box_Benchmark_SSE:
push rbx rcx rsi rdi rbp
;--- Prepare XMM0=True color pattern ---
call Local_Parms_2
;--- Setup DF, RBP, RCX ---
;--- Calculate EBX=Address=F(coordinates) ---
call Local_Parms_1
;--- Conver size from pixels=4 bytes to 128-byte blocks ---
shr rcx,7-2        ; RCX=Size, units=128-byte blocks
;--- Get start TSC ---
rdtsc
;--- Begin Y-cycle, prepare for X-cycle ---
Store_Y_SSE:
mov rdi,rbx
push rcx
;--- Begin X-cycle ---
Store_X_SSE:
movaps [rdi+000h],xmm0
movaps [rdi+010h],xmm0
movaps [rdi+020h],xmm0
movaps [rdi+030h],xmm0
movaps [rdi+040h],xmm0
movaps [rdi+050h],xmm0
movaps [rdi+060h],xmm0
movaps [rdi+070h],xmm0
add rdi,128
loop Store_X_SSE
;--- End X-cycle ---
pop rcx
add rbx,rbp
dec si
jnz Store_Y_SSE
;--- End Y-cycle, get end TSC, calculate delta-TSC ---
mov ecx,eax
mov ebx,edx
rdtsc
;--- Entry point ---
Benchmark_After_TSC:
sub eax,ecx
sbb edx,ebx
;--- Exit ---
pop rbp rdi rsi rcx rbx
ret

;--- Draw box for benchmark video memory write -----------;
; This version for 32 bits per pixel only.                ;
; This version without alignment restrictions.            ;
; See also subroutine: Draw_Box_Benchmark_SSE             ;
; TSC used, assume support TSC already check by caller.   ;
; Lines_Show bitmap ignored here, write all strings.      ;
;                                                         ;
; INPUT:   EAX = True color value for video memory write  ;
;          BX = Base X coordinate                         ;
;          DX = Base Y coordinate                         ;
;          CX = X size                                    ;
;          SI = Y size                                    ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  EDX:EAX = Delta TSC                            ;
;---------------------------------------------------------;
Draw_Box_Benchmark_386:
push rbx rcx rsi rdi rbp
;--- Setup DF, RBP, RCX ---
;--- Calculate EBX=Address=F(coordinates) ---
call Local_Parms_1
;--- Get start TSC ---
mov edi,eax
rdtsc
push rax
mov eax,edi
;--- Begin Y-cycle, prepare for X-cycle ---
Store_Y_386:
mov rdi,rbx
;--- Hardware X-cycle ---
push rcx
rep stosd        ; Use RDI
pop rcx
;--- End X-cycle ---
add rbx,rbp
dec si
jnz Store_Y_386
;--- End Y-cycle, get end TSC, calculate delta-TSC ---
pop rcx
mov rbx,rdx
rdtsc            ; EDX:EAX=Current TSC, EBX:ECX=Before draw
;--- Exit with calculate delta-TSC ---
jmp Benchmark_After_TSC

;---------- Local subroutines ----------------------------------------------;

;--- Prepare parameters ----------------------------------;
; INPUT:   R15 = Global variables pool base address       ;
;          Use 64-bit flat memory addressing, w/o segs.   ;
;          BX = Base X coordinate                         ;
;          DX = Base Y coordinate                         ;
;          CX = X size                                    ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  DF  = 0, for strings auto-increment            ;
;          RBX = Object base address in the video memory  ;
;          RCX = Bits [63-16] cleared, bits [15-0] saved  ;
;          RBP = Logical string length, can be > geom.    ;
;          RAX saved                                      ;
;---------------------------------------------------------;
Local_Parms_1:
push rax
;--- Calculate address=F(coordinates) ---
movzx rbx,bx               ; EBX=Base X coordinate
shl rbx,2                  ; *4 because 4 bytes per pixel
movzx eax,word [_Video_Xbytes+r15]
movzx rdx,dx
mul rdx
add rbx,rax                ; Add base Y coordinate
add rbx,[_Vmem_Base+r15]   ; Add video memory base
;--- Conver size from pixels=4 bytes to 128-byte blocks ---
movzx rcx,cx
;--- Setup DF=0 ---
cld
;--- Setup EBP for Y-cycle ---
movzx rbp,word [_Video_Xbytes+r15]
;--- Exit ---
pop rax
ret

;--- Prepare parameters ---------------------------------------;
; INPUT:   EAX  = True color value for video memory write      ;
; OUTPUT:  XMM0 = EAX replicated 4 times for 4 32-bit fields   ;
;--------------------------------------------------------------;
Local_Parms_2:
push rax rax
mov [rsp+04],eax
mov [rsp+12],eax
movups xmm0,[rsp]
pop rax rax
ret


