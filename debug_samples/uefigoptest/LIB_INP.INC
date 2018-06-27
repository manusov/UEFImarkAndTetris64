;===========================================================================;
;                                                                           ;
;         SUBROUTINES FOR INPUT DEVICES SUPPORT: KEYBOARD AND MOUSE.        ;
;                                                                           ;
;===========================================================================;

;--- Reset keyboard input -------------------------------------;
; Use EFI Simple Text Input Protocol                           ;
; INPUT:   R15 = Global variables pool base address            ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI Status                                    ;
;--------------------------------------------------------------;
Input_Reset:
push rcx rdx
xor eax,eax                ; RAX = Function entry number
cqo                        ; RDX = Extended Verification Flag
Input_Entry:
mov rcx,[_EFI_Table+r15]
mov rcx,[rcx+30h]          ; RCX = EFI_SYSTEM_TABLE.ConInput
call EFI_API               ; Here OUTPUT_STRING
pop rdx rcx
ret

;---------- Check input status and get pressed key ------------;
; Use EFI Simple Text Input Protocol                           ;
; This procedure return also if no keys pressed, check only.   ;
; Key code cleared from buffer, unlike BIOS INT 16h function 1.;
; INPUT:   R15 = Global variables pool base address            ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI_STATUS                                    ;
;          EDX = EFI_KEY, valid if Status=No errors            ;
;          RDX.63-32 cleared                                   ;
;--------------------------------------------------------------;
Input_Check_Key:
xor	edx,edx                ; Otherwise bad output RDX.63-32
push rcx rdx
mov	rdx,rsp                ; Pointer to RDX image in the stack
mov	eax,8                  ; RAX = Function entry number
jmp	Input_Entry

;---------- Check input status and wait for pressed key -------;
; Use EFI Simple Text Input Protocol                           ;
; This procedure not return if no keys pressed, excl. errors.  ;
; INPUT:   R15 = Global variables pool base address            ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error       ;
;          RAX = EFI_STATUS                                    ;
;          RDX = EFI_KEY, valid if Status=No errors            ;
;          RDX.63-32 cleared                                   ;
;--------------------------------------------------------------;
Input_Wait_Key:
call Input_Check_Key
; bug: need check rax for status details, exit if errors
test rax,rax
jnz Input_Wait_Key
ret

