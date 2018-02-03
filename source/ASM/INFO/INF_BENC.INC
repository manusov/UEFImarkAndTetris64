;===========================================================================;
;                                                                           ;
;   BUILT INFORMATION STRING HANDLER FOR: VIDEO MEMORY BENCHMARK RESULTS.   ;
;                   THIS INCLUDE BENCHMARKS EXECUTION.                      ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for video memory write benchmarks ----;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
; EAX = Base color, shifted, modified during cycle
; BX  = Base X coordinate, modified during cycle
; DX  = Base Y coordinate, modified during cycle
; CX  = Size X, constant
; SI  = Size Y, constant
; RDI = Color addend, integer, shifted, constant
; BP  = Color addend, mask for float, constant
; EBP.31-16 = Number of shifted drawings, constant

Info_Benchmarks:
;--- Terminate already existed strings ---
mov al,0FFh
stosb
dec rdi
;--- Blank transit buffer ---
call Blank_Transit
;--- Draw info strings in the transit buffer ---
mov al,[_Sys_Bitmap_L+r15]
; mov dx,[_Video_Ysize+r15]                                     ; v1.07 changes
; sub dx,192            ; old=176 (because add CSM), Overlap !
mov rcx,13*4            ; Color #13
test al,11000000b
jz Inf_Benc_40          ; Go if not extreme edition
; sub dx,16*2           ; This for extreme first+skip string
test al,10000000b
jz Inf_Benc_40          ; Go if extreme edition, but pass #1
mov rcx,07*4            ; Color #7
Inf_Benc_40:

; v1.07 changes
mov dx,[_Video_Ysize+r15]
sub dx,16+4
mov rsi,Buffer_2
@@:
lodsb
cmp al,0FFh
je @f
cmp al,0
jne @b
sub dx,16  ; Reserve 16-pixel line for this counted string
jmp @b
@@:
;-

mov rsi,Buffer_2

;--- Cycle for write info strings ---
Inf_Benc_20:
call Local_Inf_Benc_3
mov rcx,13*4            ; Color #13, for all other strings
add dx,16
cmp byte [rsi],0FFh
jne Inf_Benc_20
;--- Save registers ---
; RDX = Y position
; RSI = Work buffer for string write
; RDI = System information strings buffer
;---
push rdx rsi rdi        ; Notes 1,2,3
;--- Draw "Benchmarking..." string in the transit buffer ---
; Subroutine also update video memory
mov rsi,String_Info_1
mov rdi,13*4            ; RDI=Palette element
call Local_Inf_Benc_2
;--- Blank benchmarks scratch pad ---
mov rdi,Buffer_1
mov rcx,Buffer_1_Size/4
xor eax,eax
rep stosd
;--- Calculate and positioning base color addend ---
; Use RED color if not remapped context, BLUE color in the remapped context
;---
mov bx,word [_Video_R_Width+r15]   ; BH=Lsb, BL=Width
test byte [_Sys_Bitmap_L+r15],10000000b
jz Inf_Benc_42                      ; Go if not remapped
mov bx,word [_Video_B_Width+r15]    ; BH=Lsb, BL=Width
Inf_Benc_42:       ; Here color determined = f(remap)
mov ax,1
mov cl,bl          ; CL=BL=Selected color width
shl ax,cl          ; AX=Number of values
xor dx,dx
mov bp,256
div bp
movzx edi,ax       ; EDI=Integer addend
mov cl,bh          ; CL=BH=Selected color LSB
shl edi,cl         ; EDI=Integer addend, positioned
xchg ax,bp         ; AX=256
mov cx,dx          ; CX=Mod after divide by 256
xor dx,dx
test cx,cx
jz Inf_Benc_3
div cx
Inf_Benc_3:
dec ax             ; AX=Mask for comparision
movzx ebp,ax       ; BP=Mask for comparision
;--- Set base X, base Y, size X, size Y initial values ---
mov bx,192         ; BX=Xbase
mov dx,4           ; DX=Ybase
mov cx,256         ; CX=Xsize, required = add, see below
mov si,240         ; SI=Ysize, required = add, see below
;--- Set base color ---
xor eax,eax
;--- Set number of shifted drawings ---
or ebp,00F00000h                ; Cycle 240 times
;--- Clear key press flag ---
mov r8l,0

;----- Benchmarks method#1: CPU write to Video Memory, detected by GOP -----;

push rax rbx rdx rcx rsi rbp

;--- Begin cycle (SSE and 386 modes) ---
Inf_Benc_4:
push rax rdx
;--- Synchronization for benchmarks results stability ---
; Note need save EAX=True color pattern, corrupted by Check_Key subroutine
push rax rcx rdx
call Input_Check_Key
setnc r9l                       ; R9L=1 if key pressed
or r8l,r9l                      ; R8L=Summary
jnz Inf_Benc_23                 ; Skip if any key pressed
mov ecx,25000                   ; Approx. 25 ms
call Delay_ECX_uS
Inf_Benc_23:
pop rdx rcx rax
;--- Draw box ---
; Draw_Box_Benchmark_SSE procedures required:
; Xbase must be aligned by 4 pixels = 16 bytes
; Xsize must be aligned by 32 pixels = 128 bytes
test bl,00000011b
jnz Inf_Benc_8                  ; Go if not aligned
;--- This for aligned ---
call Draw_Box_Benchmark_SSE
mov r10,Buffer_1                ; Buffer_1+00 = For aligned
jmp Inf_Benc_9
;--- This for not aligned ---
Inf_Benc_8:	call	Draw_Box_Benchmark_386
mov r10,Buffer_1+16             ; Buffer_1+16 = For not align.
Inf_Benc_9:	add	[r10+00],eax    ; Update benchmarks results
adc [r10+04],edx                ; Use RDTSC parameters form
add qword [r10+08],256*240*4    ; Required = Input
;--- Restore registers ---
pop rdx rax
;--- Make cycle ---
inc ebx
inc edx
test bx,bp
jz Inf_Benc_6
add eax,edi
Inf_Benc_6:
sub ebp,00010000h
test ebp,00FF0000h
jnz Inf_Benc_4

pop rbp rsi rcx rdx rbx rax

;--- Skip GOP.BLT if remapped context and firmware VRAM addr. not valid ----;
test byte [_Sys_Bitmap_L+r15],10000000b
jnz Inf_Benc_41

;----- Benchmarks method#2: use GOP.BLT, optional accelerated --------------;
mov edi,00000100h               ; EDI=Color addend, BLT.Green

;--- Flush firmware state changes for first benchmarked pass validity ---
push rax rdx
call Draw_Box_Benchmark_BLT
pop rdx rax

;--- Begin cycle (BLT mode) ---
Inf_Benc_30:	
push rax rdx
;--- Synchronization for benchmarks results stability ---
; Note need save EAX=True color pattern, corrupted by Check_Key subroutine
push rax rcx rdx
call Input_Check_Key
setnc r9l                       ; R9L=1 if key pressed
or r8l,r9l                      ; R8L=Summary
jnz Inf_Benc_31                 ; Skip if any key pressed
mov ecx,25000                   ; Approx. 25 ms
call Delay_ECX_uS
Inf_Benc_31:	
pop rdx rcx rax
;--- Draw box, BLT use for aligned only, for comparision with SSE ---
test bl,00000011b
jz Inf_Benc_32                  ; Go if aligned
;--- This for not aligned, visual only, results skipped ---
call Draw_Box_Benchmark_386
jmp Inf_Benc_35
;--- This for aligned, results used ---
Inf_Benc_32:
call	Draw_Box_Benchmark_BLT
mov r10,Buffer_1+32             ; Buffer_1+32 = For BLT results
add [r10+00],eax                ; Update benchmarks results
adc [r10+04],edx                ; Use RDTSC parameters form
add qword [r10+08],256*240*4    ; Required = Input
;--- Restore registers ---
Inf_Benc_35:
pop rdx rax
;--- Make cycle ---
inc ebx
inc edx
add eax,edi
sub ebp,00010000h
test ebp,00FF0000h
jnz Inf_Benc_30
Inf_Benc_41:
;----- 2 cycles done (CPU and BLT), interpreting benchmarks results --------;

mov rbp,Buffer_1
mov rcx,3                    ; 3 passes: SSE, 386, BLT
Inf_Benc_10:
mov rbx,[rbp+08]             ; RBX=Total size, bytes
mov eax,0FFFFFFFFh           ; EAX="Not valid" flag
xchg rax,[rbp+00]            ; RAX=Total delta-TSC, clks
test r8l,r8l
jnz	Inf_Benc_11                 ; Go if benchmarks interrupted
mul qword [_CPUCLK_Period+r15]  ; RAX=Total Fsec
test rdx,rdx
jnz Inf_Benc_11              ; Go if overflow
test rbx,rbx
jz Inf_Benc_11               ; Go if overflow
div rbx                      ; RAX=Femptoseconds / 1 byte
xchg rbx,rax                 ; RBX=Femptoseconds / byte
mov rax,1000000000000000     ; RAX=Femptoseconds / 1 sec.
cqo
div rbx                      ; RAX=Bytes per second		
push rax
shr rax,20                   ; RAX / (1024*1024)
mov [rbp+00],eax             ; Store megabytes per second
pop rax
and eax,0FFFFFh
cdq
mov ebx,104858               ; integer(1024*1024/10)+1
div ebx
mov [rbp+04],eax             ; Store MB/10 per second
Inf_Benc_11:
add rbp,16
loop Inf_Benc_10

;--- Restore registers, part 1 ---
pop	rdi                      ; Note 3
;--- Prepare for built text string ---
mov rsi,String_Benc
call String_Copy_Simple
;--- Write number, SSE MOVAPS method ---		
mov rbp,Buffer_1+00          ; RBP = SSE results location
cmp dword [rbp],0FFFFFFFFh
je Int_Bench		; Go if interrupted
call Local_Inf_Benc_1
;--- Write number, 386 REP STOSD method ---
mov rbp,Buffer_1+16          ; RBP = 386 results location
cmp dword [rbp],0FFFFFFFFh
je Int_Bench                 ; Go if interrupted
call Local_Inf_Benc_1
;--- Write number, BLT method ---
mov rbp,Buffer_1+32          ; RBP = BLT results location
call Local_Inf_Benc_1
jmp End_Bench
;--- Write string if interrupted by user press key ---
Int_Bench:
mov rsi,String_Benc_U
call String_Copy_Simple
;--- Terminate benchmark results string + all strings ---
End_Bench:
mov ax,0FF00h
stosw
;--- Restore registers, part 2 ---
pop rsi rdx                  ; Notes 2,1
;--- Write benchmark results string ---
mov rcx,13*4                 ; Color #13
call Local_Inf_Benc_3
;--- Draw version string in the transit buffer ---
; Subroutine also update video memory
mov bx,8                     ; BX=Xbase, pixels
mov dx,8                     ; DX=Ybase, pixels
mov rsi,String_Version       ; RSI=String pointer
mov rdi,14*4                 ; RDI=Palette element
call Local_Inf_Benc_4
;--- Draw vendor data string in the transit buffer ---
; Subroutine also update video memory
add dx,16
mov rsi,Program_Vendor_Data  ; RSI=String pointer
mov rdi,13*4                 ; RDI=Palette element
call Local_Inf_Benc_4
;--- Draw "Press any key..." string in the transit buffer and exit ---
mov rsi,String_Info_2
mov rdi,14*4                 ; RDI=Palette element
; No RET, continue in the next subroutine

;---------- Local subroutines ----------------------------------------------;

;--- Write one string and update video memory ------------;
; Write at fixed position: up, right corner               ;
; INPUT:   R15=Pointer to global variables pool           ;
;          RSI = String source pointer                    ;
;          RDI = Palette element pointer                  ;
;          Global variables                               ;
; OUTPUT:  None                                           ;
;          Transit buffer and video memory updated        ;
;---------------------------------------------------------;
Local_Inf_Benc_2:
;--- Color, front and back ---
mov rbx,Runtime_Palette_256
mov eax,[rbx+rdi]
mov ebp,[rbx+00*4]
;--- Geometry ---
mov bx,[_Video_Xsize+r15]
sub bx,35*8          ; BX=String base X, string 33+2 chars
mov dx,8             ; DX=String base Y
mov ecx,80001008h    ; ECX.31=Mode, CH=Font Y, CL=Font X
;--- Draw ---
call Draw_String     ; Update transit buffer
jmp Screen_Copy      ; Update video memory

;--- Write number Integer.Float and string ---------------;
; INPUT:   RSI = String source pointer                    ;
;          RDI = String destination pointer               ;
;          RBP = Work buffer pointer                      ;
;                  dword [rbp+00] = integer part          ;
;                  dword [rbp+04] = float part, 0-9       ;
;                  if integer part = FFFFFFFFh write "?"  ;
;                  (for measurement errors)               ;
;                                                         ;
; OUTPUT:  RSI = Pointer updated                          ;
;          RDI = Pointer updated                          ;
;---------------------------------------------------------;
Local_Inf_Benc_1:
mov bl,0
mov eax,[rbp+00]
cmp eax,0FFFFFFFFh
je Inf_Benc_21
call Decimal_Print_32
mov al,'.'
stosb
mov eax,[rbp+04]
call Decimal_Print_32
jmp Inf_Benc_22
Inf_Benc_21:	mov	al,'?'
stosb
Inf_Benc_22:
jmp String_Copy_Simple

;--- Write one string of system info strings -------------;
;    Color and X-coordinate const                         ;
; INPUT:   DX  = Base Y coordinate                        ;
;          RSI = Pointer to string                        ;
;          RCX = Color index * 4                          ;
;          Global variables                               ;
; OUTPUT:  None                                           ;
;---------------------------------------------------------;
Local_Inf_Benc_3:
mov rax,Runtime_Palette_256   ; Palette base
mov eax,[rax+rcx]             ; Color value
mov bx,16                     ; BX=Xcoordinate
mov ecx,00001008h             ; ECX.31=Mode, CH=Font Y, CL=Font X
jmp Draw_String

;--- Write one string of system info strings -------------;
;    Color and X-coordinate variable                      ;
; INPUT:   BX  = Base X coordinate                        ;
;          DX  = Base Y coordinate                        ;
;          RSI = Pointer to string                        ;
;          RDI = Palette element pointer                  ;
;          Global variables                               ;
; OUTPUT:  None                                           ;
;---------------------------------------------------------;
Local_Inf_Benc_4:
mov ecx,00001008h             ; ECX.31=Mode, CH=Font Y, CL=Font X
mov rax,Runtime_Palette_256   ; Palette base
mov eax,[rax+rdi]             ; Color value
jmp Draw_String

