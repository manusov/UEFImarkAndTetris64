;===========================================================================;
;                                                                           ;
;    SUBROUTINES FOR TIMINGS AND DELAYS BY CPU TSC (TIME STAMP COUNTER).    ;
;                                                                           ;
;===========================================================================;

;--- Get microseconds from TSC start, use TSC ------------;
; Assume TSC support already verifyed by caller           ;
; Assume TSC functional and readable for current process  ;
; Assume variable CPUCLK_Period initialized               ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          TSC value, incremented each CPU clock          ;
;          Global Variables                               ;
; OUTPUT:  CF  = Error flag:                              ;
;                0(NC)=Result valid, 1(C)=Error           ;
;          R13 = TSC converted to microseconds,           ;
;                valid if CF=0(NC)                        ;
;---------------------------------------------------------;
TSC_to_Microseconds:
push rax rbx rdx
mov rax,1000000000   ; RAX=fsec per uS
cqo                  ; D[127-64]=0
mov rbx,[_CPUCLK_Period+r15]
test rbx,rbx
stc
jz Error_uS
div rbx
xchg ebx,eax         ; RBX=Clocks per uS
rdtsc                ; EDX:EAX=TSC
shl rdx,32
or rax,rdx           ; RAX=TSC
xor edx,edx          ; This clear all RDX
div rbx              ; RAX=uS per current TSC
mov r13,rax
clc
Error_uS:
pop rdx rbx rax	; CF=Status
ret

;--- Delay ECX microseconds, use TSC ---------------------;
; Assume TSC support already verifyed by caller           ;
; Assume TSC functional and readable for current process  ;
; Assume variable CPUCLK_Period initialized               ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          ECX = Delay value, units = microseconds        ;
;          TSC value, incremented each CPU clock          ;
;          Global Variables                               ;
; OUTPUT:  CF = Error flag:                               ;
;               0(NC)=Executed OK, 1(C)=Error             ;
;---------------------------------------------------------;
Delay_ECX_uS:
push rax r13
xchg eax,ecx              ; Note, set RAX[63-32]=0
call TSC_to_Microseconds  ; Read for calculate limit
jc Delay_Exit             ; Go if error
add rax,r13               ; RAX=Target value
Delay_1:
call TSC_to_Microseconds  ; Read for compare with limit
jc Delay_Exit             ; Go if error
cmp r13,rax
jb Delay_1          ; Go by low dwords compare
Delay_Exit:
pop r13 rax         ; Otherwise return with CF=0(NC)
ret

;--- Start background count for delayed event, use TSC ---;
; Assume TSC support already verifyed by caller           ;
; Assume TSC functional and readable for current process  ;
; Assume variable CPUCLK_Period initialized               ;
; See complementar subroutine: Count_Check                ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          ECX = Delay value, units = microseconds        ;
;          TSC value, incremented each CPU clock          ;
;          Global Variables                               ;
; OUTPUT:  CF = Error flag:                               ;
;                    0(NC)=Executed OK, 1(C)=Error        ;
;---------------------------------------------------------;
Count_Start:
push rax r13
xchg eax,ecx                ; Note, set RAX[63-32]=0		
;--- Get timer current value, microseconds ---
call TSC_to_Microseconds    ; R13=Timer value, uS
jc Count_Bad
;--- Calculate timer target value, microseconds ---
add rax,r13
mov [_Count_Event+r15],rax
;--- Set count active flag ---
mov al,1
Count_Set:
mov [_Count_Active+r15],al
;--- Exit points ---
Count_Exit:
clc
Count_Bad:
jmp Delay_Exit

;--- Check background count for delayed event, use TSC ---;
; Assume TSC support already verifyed by caller           ;
; Assume TSC functional and readable for current process  ;
; Assume variable CPUCLK_Period initialized               ;
; See complementar subroutine: Count_Start                ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          TSC value, incremented each CPU clock          ;
;          Global Variables                               ;
; OUTPUT:  CF = Error flag:                               ;
;                    0(NC)=Executed OK, 1(C)=Error        ;
;          ZF = Event flag, valid only if CF=0(NC):       ;
;                    0(NZ)=No events, 1(Z)=Event detected ;
;          Count mode stopped if event detected           ;
;---------------------------------------------------------;
Count_Check:
push rax r13
;--- Check count mode active ---
cmp byte [_Count_Active+r15],1
jne Count_Exit     ; ZF=0(NZ), there CLC
;--- Get timer current value, microseconds ---
call TSC_to_Microseconds
jc Count_Bad       ; ZF not valid, CF=1(C)
;--- Check Count Event ---
cmp r13,[_Count_Event+r15]
jb Count_Exit      ; ZF=0(NZ), there CLC
;--- Clear count mode and exit ---
xor al,al          ; AL=0 and ZF=1(Z), CF=0(NC)
jmp Count_Set
