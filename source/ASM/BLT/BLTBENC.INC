;===========================================================================;
;                                                                           ;
;         STORE DATA TO VIDEO MEMORY, DRAW BOX, USED FOR BENCHMARK.         ;
;   SUBROUTINE USE BLOCK TRANSFER (BLT) OPERATION, SEE ALSO DRAWBENC.INC.   ;
;                                                                           ;
;===========================================================================;

;---------------------------------------------------------------------------;
; BLT pixel color entry format:
; Offset  Width   Comments
;---------------------------------------------------------------------------;
; 00      BYTE    Blue color
; 01      BYTE    Green color
; 02      BYTE    Red color
; 03      BYTE    Reserved
;---------------------------------------------------------------------------;
; BLT Operations list:
; Code    Comments
;---------------------------------------------------------------------------;
; 00      EfiBltVideoFill
; 01      EfiBltVideoToBltBuffer
; 02      EfiBltBufferToVideo
; 03      EfiBltVideoToVideo
;---------------------------------------------------------------------------;
; Input parameters description for BLT function:
; Parm   Location         Comments
;---------------------------------------------------------------------------;
; #01    [rsp+00] = rcx   This, GOP instance pointer
; #02    [rsp+08] = rdx   BltBuffer, pointer to buffer for data transfer
; #03    [rsp+16] = r8    BltOperation, BLT operation code
; #04    [rsp+24] = r9    SourceX, source X coordinate, pixels
; #05    [rsp+32]         SourceY, source Y coordinate, pixels
; #06    [rsp+40]         DestinationX, destination X coordinate, pixels
; #07    [rsp+48]         DestinationY, destination Y coordinate, pixels
; #08    [rsp+56]         Width, width of rectangle
; #09    [rsp+64]         Height, height of rectangle
; #10    [rsp+72]         Delta, bytes per row for subrectangle mode, oth. 0
;---------------------------------------------------------------------------;

;--- Draw box for benchmark video memory write, use BLT --;
; This version for 32 bits per pixel only.                ;
; See also subroutines:                                   ;
; Draw_Box_Benchmark_386, Draw_Box_Benchmark_SSE          ;
; TSC used, assume support TSC already check by caller.   ;
; Lines_Show bitmap ignored here, write all strings.      ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          Use 64-bit flat memory addressing, w/o segs.   ;
;          EAX = True color value for video memory write  ;
;                untranslated fixed [B,G,R,Reserved]      ;
;          BX = Base X coordinate                         ;
;          DX = Base Y coordinate                         ;
;          CX = X size                                    ;
;          SI = Y size                                    ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  CF  = Error flag: 0(NC)=OK, 1(C)=Error         ;
;          R13 = Reserved for UEFI status,                ;
;                status return not used yet               ;
;          EDX:EAX = Delta TSC, use RDTSC output format   ;
;---------------------------------------------------------;
Draw_Box_Benchmark_BLT:
;--- Align stack ---
test rsp,8
jnz Draw_Box_Benchmark_BLT_A
call Draw_Box_Benchmark_BLT_Aligned
ret
Draw_Box_Benchmark_BLT_A:
push r15
call Draw_Box_Benchmark_BLT_Aligned
pop r15
ret

Draw_Box_Benchmark_BLT_Aligned:
push rbx rcx rsi rdi rbp
push r8 r9 r10 r11 r12 r13 r14 r15
sub rsp,40			; Prevent AMI shadow bug
;--- Prepare input parameters ---
push rax                ; Color dword, here=buffer
xor rdi,rdi             ; Parm #10 = Delta = 0
push rdi                ; Push qword #1
movzx rsi,si            ; Parm #9 = Height
push rsi                ; Push qword #2
movzx rcx,cx            ; Parm #8 = Width
push rcx                ; Push qword #3
movzx rdx,dx            ; Parm #7 = Destination Y
push rdx                ; Push qword #4
movzx rbx,bx            ; Parm #6 = Destination X
push rbx                ; Push qword #5
push rdi                ; P#5, Q#6 = Source Y = 0
xor r9,r9               ; Parm #4 = Source X = 0
push r9                 ; Push qword #7
xor r8,r8               ; Parm #3 = Operation = 0
push r8                 ; Push qword #8
lea rdx,[rsp+64]        ; Parm #2 = Color pattern
push rdx                ; Push qword #9
mov rcx,[_EFI_Gop+r15]  ; Parm #1 = GOP instance
push rcx                ; Push qword #10

;---------- Start of benchmarked section -----------------------------------;

;--- Get and store current TSC ---
; Note 1. RDX used both for RDTSC output and for UEFI protocol input,
; cannot eliminate this conflict, use fast save-restore by MOV.
; Note 2. RSI, RDI is nonvolatile by UEFI calling conventions,
; don't push/pop it for minimize losing TSC clocks and benchmarks precision.
; Note 3. Here main criteria: minimize clocks between RDTSC and CALL GOP.BLT.
mov rbp,rdx
rdtsc           ; EAX=TSC.31-00, EDX=TSC.63-32
mov esi,eax     ; ESI=TSC.31-00 (start time)
mov edi,edx     ; EDI=TSC.63-32 (start time)
mov rdx,rbp
;--- Call GOP.BLT routine, check UEFI status for detect errors ---
call qword [rcx+16]   ; Call function: GOP.BLT
test rax,rax          ; RAX = UEFI status
jnz Exit_Blt_2        ; Go if error
;--- Get current TSC, subtract for calculate delta-TSC ---
; Note 1. RAX used both for RDTSC output and for UEFI protocol output,
; cannot eliminate this conflict, need check RAX before RDTSC
rdtsc                 ; EAX=TSC.31-00, EDX=TSC.63-32
sub eax,esi           ; EAX=Delta-TSC, bits 31-00
sbb edx,edi           ; EDX=Delta-TSC, bits 63-32

;---------- End of benchmarked section -------------------------------------;

;--- Prepare for exit ---
pop rcx               ; Remove color qword
add rsp,80+40         ; Remove parms+reserved area
clc                   ; CF=0(NC) means no errors
;--- Exit points ---
Exit_Blt_1: 	
pop r15 r14 r13 r12 r11 r10 r9 r8
pop rbp rdi rsi rcx rbx
ret
;--- Error handling ---
; Note 1.
; This non-optimal as code size, but remove STC from RDTSC-measured branch.
Exit_Blt_2:
pop rcx              ; Remove color qword
add rsp,80+40        ; Remove parms+reserved area
stc                  ; CF=1(C) means errors
jmp Exit_Blt_1

