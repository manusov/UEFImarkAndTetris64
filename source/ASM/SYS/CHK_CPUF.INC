;===========================================================================;
;                                                                           ;
;              VERIFY COMPATIBILITY HANDLER FOR CPU FEATURES.               ;
;                                                                           ;
;===========================================================================;

;--- Verify CPU support SSE and TSC --------------------------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Flag: 0(NC)=Check Passed, 1(C)=Check Failed         ;
;-------------------------------------------------------------------;
Check_CPU_Features:
;--- Verify CPU support CPUID instruction ---
; Check bit EFLAGS.21 toggleable, it is CPUID support indicator
; Note. Probably wrong result if trace this subroutine code
;--- Check for ID bit writeable for "1" ---
mov ebx,21
cli
pushf              ; In the 64-bit mode, push RFLAGS
pop rax
bts eax,ebx        ; Set EAX.21=1
push rax
popf
pushf
pop rax
sti
btr eax,ebx        ; Check EAX.21=1, Set EAX.21=0
jnc Bad_CPUF       ; Go if cannot set EFLAGS.21=1
;--- Check for ID bit writeable for "0" ---
cli
push rax
popf
pushf
pop rax
sti
btr eax,ebx        ; Check EAX.21=0
jc Bad_CPUF        ; Go if cannot set EFLAGS.21=0
;--- Verify CPU features ---
xor eax,eax
cpuid              ; Execute CPUID Function 0
test eax,eax
jz Bad_CPUF        ; Go if CPUID Fnc. 1 not supported
mov eax,1
cpuid              ; Execute CPUID Function 1
test dl,00010000b  ; EDX.4=TSC feature
jz Bad_CPUF        ; Go if TSC not supported
bt edx,25          ; EDX.25=SSE feature
jnc Bad_CPUF       ; Go if SSE not supported
;--- Detect physical memory address space size --
; Note, this function same for Intel and AMD
mov byte [_Address_Size+r15],36  ; Default=36
mov eax,80000000h
cpuid                      ; Execute CPUID Function 80000000h
cmp eax,80000008h
jb Skip_P_Addr             ; Skip if required fnc. not supported
mov eax,80000008h
cpuid                      ; Execute CPUID Function 80000008h
mov [_Address_Size+r15],al
Skip_P_Addr:
;--- Exit points ---
clc
ret
Bad_CPUF:
stc
ret

