;===========================================================================;
;                                                                           ;
;        BUILT INFORMATION STRING HANDLER FOR: CPU ADVANCED FEATURES.       ;
;                                                                           ;
;===========================================================================;

;------------------------------------------
; Intel CPU features, selected for visual:
;------------------------------------------
; Intel EM64T = CPUID#80000001h, EDX.29
; MMX         = CPUID#1, EDX.23
; SSE         = CPUID#1, EDX.25
; SSE2        = CPUID#1, EDX.26
; SSE3        = CPUID#1, ECX.0
; SSSE3       = CPUID#1, ECX.9
; SSE4.1      = CPUID#1, ECX.19
; SSE4.2      = CPUID#1, ECX.20
; AVX         = CPUID#1, ECX.28
; AVX2        = CPUID#7, EBX.5
;-
; AVX512F     = CPUID#7, EBX.16    ; Foundation  
; AVX512DQ    = CPUID#7, EBX.17    ; Doublewords and Quadwords operations     
; AVX512IFMA  = CPUID#7, EBX.21    ; Integer Fused Multiply and Add 
; AVX512PF    = CPUID#7, EBX.26    ; Prefetch          
; AVX512ER    = CPUID#7, EBX.27    ; Exponential and Reciprocal  
; AVX512CD    = CPUID#7, EBX.28    ; Conflict Detection 
; AVX512BW    = CPUID#7, EBX.30    ; Bytes and Words Operations 
; AVX512VL    = CPUID#7, EBX.31    ; Vector Length Control 
; AVX512VBM   = CPUID#7, ECX.1     ; Vector Byte Manipulation   

;----------------------------------------
; AMD CPU features, selected for visual:
;----------------------------------------
; AMD64       = CPUID#80000001h, EDX.29
; MMX         = CPUID#1, EDX.23
; SSE         = CPUID#1, EDX.25
; SSE2        = CPUID#1, EDX.26
; SSE3        = CPUID#1, ECX.0
; SSSE3       = CPUID#1, ECX.9
; SSE4.1      = CPUID#1, ECX.19
; SSE4.2      = CPUID#1, ECX.20
; SSE4A       = CPUID#80000001h, ECX.6
; SSE5        = CPUID#80000001h, ECX.11
; 3DNow       = CPUID#80000001h, EDX.31
; 3DNow+      = CPUID#80000001h, EDX.30
; AVX         = CPUID#1, ECX.28
; AVX2        = CPUID#7, EBX.5
;-
; AVX512F     = CPUID#7, EBX.16    ; Foundation  
; AVX512DQ    = CPUID#7, EBX.17    ; Doublewords and Quadwords operations     
; AVX512IFMA  = CPUID#7, EBX.21    ; Integer Fused Multiply and Add 
; AVX512PF    = CPUID#7, EBX.26    ; Prefetch          
; AVX512ER    = CPUID#7, EBX.27    ; Exponential and Reciprocal  
; AVX512CD    = CPUID#7, EBX.28    ; Conflict Detection 
; AVX512BW    = CPUID#7, EBX.30    ; Bytes and Words Operations 
; AVX512VL    = CPUID#7, EBX.31    ; Vector Length Control 
; AVX512VBM   = CPUID#7, ECX.1     ; Vector Byte Manipulation   

;------------------------------------------
; Notes about same support for Intel, AMD:
;------------------------------------------
; Probably differences:
; SSE4A, 3DNow, 3DNow+ (3DNowExt)
;-----------------------------------------

;--- Bullt information string for CPU advanced features ------------;
; If control here, assume CPUID function 0 supported,               ;
; support CPUID functions 00000001h, 80000001h verified here        ;
;                                                                   ;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_CPU_Features:

;--- Detect CPU vendor, yet explicit support Intel and AMD ---
xor eax,eax                   ; EAX=0, CPUID function num.
cpuid                         ; CPUID#0 = Get Vendor String
mov rsi,Sign_CPU              ; RSI = Pointer to Vendor Strings sequence
mov r8,Selector_CPU_Features  ; R8 = Pointer to list of pointers to selectors
Inf_Cfeat_4:
cmp [rsi+00],ebx
jne Inf_Cfeat_2       ; Go if signature mismatch
cmp [rsi+04],edx
jne Inf_Cfeat_2       ; Go if signature mismatch
cmp [rsi+08],ecx
je Inf_Cfeat_3        ; Go if signature match
Inf_Cfeat_2:
add rsi,0Ch           ; Modify signature pointer, Vendor Signature = 12 bytes
add r8,8              ; Modify features selector, 64-bit pointer = 8 bytes
cmp rsi,Sign_CPU+12   ; This compared offset for 2 iterations 
jbe Inf_Cfeat_4       ; Cycle for vendors
;--- Exit point 1 ---
ret

;--- Select interpreted table for CPU vendor --- Modified from v1.07 ---
Inf_Cfeat_3:                    ; Start parsing CPU features by descriptors
mov rsi,[r8]                    ; RSI = Pointer to features descriptors list
call Inf_Cfeat_5                ; First features set, different for Intel/AMD
mov rsi,AVX512_CPU_Features_1   ; String 1 of 2 for AVX512 family list
push rdi
call Inf_Cfeat_5                ; AVX512 subset 1, note not pre-locked for AMD
pop rax
sub rax,rdi  ; RAX = Address Before - Address After
cmp rax,-1   ; RAX=-1 means stored one byte: terminator 00h
jne @f       ; Go skip remove if strings really added
dec rdi      ; Remove empty string if no data (terminator 00 only)
@@:
mov rsi,AVX512_CPU_Features_2   ; String 2 of 2 for AVX512 family list
push rdi
call Inf_Cfeat_5                ; AVX512 subset 2, note not pre-locked for AMD
pop rax
sub rax,rdi  ; RAX = Address Before - Address After
cmp rax,-1   ; RAX=-1 means stored one byte: terminator 00h
jne @f       ; Go skip remove if strings really added
dec rdi      ; Remove empty string if no data (terminator 00 only)
@@:
ret

;--- Interpreting table, start of cycle, this point also for call ---
Inf_Cfeat_5:
lodsw              ; AX = load 16-bit tag
cmp ax,0FFFFh
je Inf_Terminate   ; Go if end of list, exit point 2:  MOV AL,0 / STOSB / RET
test ax,0F000h     ; Test bits [15-12], yet valid 0000b, 0001b only
jz Inf_Cfeat_6     ; Go if unconditional string
and ah,0Fh
cmp ah,1
je Inf_Cfeat_8     ; Go if CODE=1, means extended CPUID, function 80000000h
cmp ah,2
je Inf_Cfeat_11    ; Go if CODE=2, means CPUID function 7
                   ; Otherwise standard CPUID, function 0
;--- Standard CPUID ---
xor eax,eax          ; CPUID function 0, for verify support function 1
cpuid
cmp eax,00000001h
jb Inf_Cfeat_7       ; Go if not supported
mov eax,00000001h    ; CPUID function 00000001h = Standard Features
jmp Inf_Cfeat_9

;--- Extended CPUID ---
Inf_Cfeat_8:
mov eax,80000000h    ; CPUID function 80000000h, for verify support 80000001h
cpuid
cmp eax,80000001h
jb Inf_Cfeat_7       ; Go if not supported
mov eax,80000001h    ; CPUID function 80000001h = Extended Features
jmp Inf_Cfeat_9

;--- Function 7 ---
Inf_Cfeat_11:
xor eax,eax          ; CPUID function 0, for verify support function 7
cpuid
cmp eax,00000007h
jb Inf_Cfeat_7       ; Go if not supported
mov eax,00000007h    ; CPUID function 00000007h = Structured Extended Features

;--- Execute CPUID ---
Inf_Cfeat_9:
xor ecx,ecx             ; Clear subfunction number, yet 0 only used
cpuid                   ; Result: EAX, EBX, ECX, EDX

;--- Check selected bit ---
dec rsi                 ; RSI-1
dec rsi                 ; RSI-1, total RSI-2, re-address 16-bit tag word
lodsw                   ; Re-load 16-bit tag dword

mov ah,al               ; AL must be saved, tested bit number
shr ah,5
cmp ah,1
je Inf_Cfeat_10         ; Go if 1 = EBX selected 
mov ebx,ecx             ; EBX = temporary target
cmp ah,2
je Inf_Cfeat_10         ; Go if 2 = ECX selected 
mov ebx,edx             ; Otherwise EDX selected, note initial EAX corrupted  
                        ; EAX selection not required / not supported yet
Inf_Cfeat_10:
and eax,0000001Fh       ; Select bit number field, bits EAX[4-0] 
bt ebx,eax              ; EBX = Bitmap , EAX = Selected bit number
jnc Inf_Cfeat_7         ; Go if selected bit=0

;--- Write string ---
Inf_Cfeat_6:
lodsq                   ; RAX=String address
push rsi
xchg rsi,rax            ; RSI=String address
call String_Copy_Simple
mov al,' '
stosb
pop rsi
jmp Inf_Cfeat_5         ; Continue cycle for features descriptors

;--- Skip string ---
Inf_Cfeat_7:
lodsq                   ; This LODSQ for addition RSI+8 only, RAX ignored
jmp Inf_Cfeat_5         ; Continue cycle for features descriptors


; This is old format entries interpreter used before v1.07
;-
;  ;--- Interpreting table, start of cycle ---
;  Inf_Cfeat_5:
;  lodsb
;  cmp al,0FFh
;  je Inf_Terminate  ; Go if end of list, exit point 2:  MOV AL,0 / STOSB / RET
;  test al,10000000b
;  jz Inf_Cfeat_6          ; Go if unconditional string
;  test al,01000000b
;  jnz Inf_Cfeat_8         ; Go if extended CPUID
;                          ; Otherwise standard CPUID
;  ;--- Standard CPUID ---
;  xor eax,eax
;  cpuid
;  cmp eax,00000001h
;  jb Inf_Cfeat_7          ; Go if not supported
;  mov eax,00000001h
;  jmp Inf_Cfeat_9
;
;  ;--- Extended CPUID ---
;  Inf_Cfeat_8:
;  mov eax,80000000h
;  cpuid
;  cmp eax,80000001h
;  jb Inf_Cfeat_7          ; Go if not supported
;  mov eax,80000001h
;
;  ;--- Execute CPUID ---
;  Inf_Cfeat_9:
;  cpuid
;
;  ;--- Check selected bit ---
;  dec rsi
;  lodsb
;  mov ebx,edx
;  test al,00100000b
;  jz Inf_Cfeat_10         ; Go if EDX selected
;  mov ebx,ecx
;  Inf_Cfeat_10:
;  and eax,0000001Fh
;  bt ebx,eax
;  jnc Inf_Cfeat_7         ; Go if selected bit=0
;
;  ;--- Write string ---
;  Inf_Cfeat_6:
;  lodsq                   ; RAX=String address
;  push rsi
;  xchg rsi,rax            ; RSI=String address
;  call String_Copy_Simple
;  mov al,' '
;  stosb
;  pop rsi
;  jmp Inf_Cfeat_5
;
;  ;--- Skip string ---
;  Inf_Cfeat_7:
;  lodsq                   ; This LODSQ for addition RSI+8 only, RAX ignored
;  jmp Inf_Cfeat_5
;-
