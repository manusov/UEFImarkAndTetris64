;===========================================================================;
;                                                                           ;
;              LIBRARY FOR INTERFACE TO EFI SERVICES (API).                 ;
;                                                                           ;
;===========================================================================;

;---------- Gate for EFI API Call (simplified) ----------------;
; Save-restore registers, make parameters shadow in the stack, ;
; call target subroutine.                                      ;
; This procedure for simplified protocols, see also EFI_API_1. ;
;                                                              ;
; INPUT:   RCX = EFI table address                             ;
;          RAX = EFI function entry offset in the table        ;
;          Function input parameters as transit                ;
;                                                              ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI Status                                    ;
;          Function output parameters as transit               ;
;--------------------------------------------------------------;
EFI_API:
;--- Align stack ---
test rsp,8
jz EFI_API_A
call EFI_API_Aligned
ret
EFI_API_A:
push r15
call EFI_API_Aligned
pop r15
ret

EFI_API_Aligned:
;--- Save registers ---
push rbx rcx rdx rsi rdi rbp
push r8 r9 r10 r11 r12 r13 r14 r15
;--- Make parameters shadow, reserve 80 bytes, include AMI bug! ---
sub rsp,80
;--- Call function, RCX=Table base, RAX=Index ---
call qword [rcx+rax]
;--- Remove parameters shadow ---
EFI_API_Exit:
add rsp,80
;--- Analyse status ---
test rax,rax         ; RAX = UEFI STATUS
jz	Good_Status      ; Here CF=0(NC) after TEST instr.
stc                  ; Set CF=1(C) if EAX>0
;--- Restore registers and exit ---
Good_Status:
pop r15 r14 r13 r12 r11 r10 r9 r8
pop rbp rdi rsi rdx rcx rbx
ret

;---------- Gate for EFI API Call (normal) --------------------;
; Save-restore registers, make parameters shadow in the stack, ;
; call target subroutine.                                      ;
; This procedure for normal protocols, see also EFI_API.       ;
;                                                              ;
; INPUT:   RBX = EFI table address                             ;
;          RAX = EFI function entry offset in the table        ;
;          Function input parameters as transit                ;
;                                                              ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI Status                                    ;
;          Function output parameters as transit               ;
;--------------------------------------------------------------;
EFI_API_1:
;--- Align stack ---
test rsp,8
jz EFI_API_1_A
call EFI_API_1_Aligned
ret
EFI_API_1_A:
push r15
call EFI_API_1_Aligned
pop r15
ret

EFI_API_1_Aligned:
;--- Save registers ---
push rbx rcx rdx rsi rdi rbp
push r8 r9 r10 r11 r12 r13 r14 r15
;--- Make parameters shadow, reserve 80 bytes, include AMI bug! ---
sub rsp,80
;--- Call function, RBX=Table base, RAX=Index ---
call qword [rbx+rax]
;--- Go to remove parameters shadow and exit ---
jmp EFI_API_Exit

;--- Locate EFI protocol by GUID ----------------------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;          RCX = Pointer to Protocol 128-bit GUID      ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;          RCX = Protocol pointer if operation passed  ;
;------------------------------------------------------;
Locate_Protocol:
push rbx rdx r8
;--- Prepare stack scratch pad at [rsp] for work area ---
xor eax,eax
push rax
;--- Call function ---
mov rbx,[_EFI_Table+r15]   ; RBX = EFI_SYS_TABLE
mov rbx,[rbx+060h]         ; RBX = EFI_SYS_TABLE.BootServ
mov rax,018h+0128h         ; RAX = EFI_Locate_Protocol
xor edx,edx                ; Registration = 0
mov r8,rsp                 ; Stack is work area, 1 qword
call EFI_API_1
pop	rcx                    ; RCX=Pointer to Interface
;--- Check pointer value ---
jrcxz Bad_Protocol         ; Go if Pointer=0
;--- Exit ---
Exit_Protocol:
pop r8 rdx rbx
ret
Bad_Protocol:
stc
jmp	Exit_Protocol

