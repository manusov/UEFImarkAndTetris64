;===========================================================================;
;                                                                           ;
;   BUILT INFORMATION STRING HANDLER FOR: MTRR SETTINGS FOR VIDEO MEMORY.   ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for MTRR settings for video memory ---;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_MTRR:
;--- Check MTRR detected and used ---
mov ecx,[_MTRR_Used+r15]
jecxz Inf_MTRR_1      ; Go if MTRR not supported
cmp ecx,1
je Inf_MTRR_3         ; Go if MTRR sup., but no vacant
;--- Write left part of string ---
mov rsi,String_MTRR
call String_Copy_Simple
;--- Get base address ---
rdmsr
push rax      ; (1)
mov rbx,00000000FFFFF000h
and rax,rbx
shl rdx,32
or rax,rdx
push rax      ; (2)
;--- Write base address ---
call Hex_Print_64
;--- Write middle string ---
call String_Copy_Simple
;--- Get end address ---
inc ecx
rdmsr
and rax,rbx
shl rdx,32
or rax,rdx
;--- Invert number of bits N=address size) ---
mov rbp,0FFFFFFFFFFFFFFFFh
mov cl,[_Address_Size+r15]
shl rbp,cl
not rbp
xor rax,rbp
;--- Add base address ---
pop rbx       ; (2)
or rax,rbx
;--- Write end address ---
call Hex_Print_64
;--- Write middle string ---
call String_Copy_Simple
;--- Write memory type ---
pop rax       ; (1)
mov ah,6
cmp al,6
jbe Inf_MTRR_2
mov al,2
Inf_MTRR_2:
call String_Select_Copy		
;--- Terminate string ---
Inf_MTRR_4:
mov al,0
stosb
;--- Exit ---
Inf_MTRR_1:
ret
;--- Support for MTRR detected, but no vacant registers ---
Inf_MTRR_3:
mov rsi,String_MTRR_N
call String_Copy_Simple
jmp Inf_MTRR_4

