;===========================================================================;
;                                                                           ;
;          BUILT INFORMATION STRING HANDLER FOR: CPU NAME STRING.           ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for CPUID CPU model string -----------;
; Assume CPUID instruct. support already verified, if control here  ;
;                                                                   ;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_CPU:
;--- Verify CPU brand string support ---
mov eax,80000000h
cpuid
cmp eax,80000004h
jb Inf_CPU_2
;--- Get CPU brand string ---
mov ebp,80000002h
Inf_CPU_3:
mov eax,ebp
cpuid
stosd
xchg eax,ebx      ; XCHG instead MOV, save size
stosd
xchg eax,ecx
stosd
xchg eax,edx
stosd
inc ebp
cmp bp,0004h
jbe Inf_CPU_3
;--- Format CPU brand string ---
mov rcx,48        ; String total length
sub rdi,rcx       ; RDI=Destination
mov rsi,rdi       ; RSI=Source, same yet
Inf_CPU_5:
lodsb
cmp al,0
je Inf_CPU_2      ; Go if unexpected end
cmp al,' '
loope Inf_CPU_5   ; Cycle if char=space
jrcxz Inf_CPU_2   ; Go if all chars=spaces
Inf_CPU_6:
stosb
lodsb
cmp al,0
loopne Inf_CPU_6  ; Cycle for copy string
jmp Inf_CPU_4
;--- If brand string not supported, get vendor string (old method) ---
Inf_CPU_2:
xor eax,eax
cpuid
xchg eax,ebx
stosd
xchg eax,edx
stosd
xchg eax,ecx
stosd
;--- Global entry point ---
Inf_Terminate:
;--- Terminate string ---
Inf_CPU_4:
mov al,0
stosb
;--- Exit ---
Inf_CPU_1:
ret

