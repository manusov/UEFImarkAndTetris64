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
xor rax,rax         ; RAX = Function entry number
cqo                 ; RDX = Extended Verification Flag
Input_Entry:
mov rcx,[_EFI_Table+r15]
mov rcx,[rcx+30h]   ; RCX = EFI_SYSTEM_TABLE.ConInput
call EFI_API        ; Here OUTPUT_STRING
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
xor rdx,rdx   ; Otherwise bad output RDX.63-32
push rcx rdx
mov rdx,rsp   ; Pointer to RDX image in the stack
mov rax,8     ; RAX = Function entry number
jmp Input_Entry

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
; bug: need check rax for details, exit if errors
test rax,rax
jnz Input_Wait_Key
ret

;--- Get mouse presense only -----------------------------;
; INPUT:   R15 = Global variables pool base address       ;
;          Global variables                               ;
; OUTPUT:  CF flag = presense status:                     ;
;                    0(NC)=Mouse present                  ;
;                    1(C)=Mouse absent                    ;
;---------------------------------------------------------;
Ask_Mouse:
cmp byte [_Mouse_Present+r15],1
ret

;--- Wait release mouse buttons --------------------------;
; INPUT:   R15 = Global variables pool base address       ;
;          Global variables                               ;
; OUTPUT:  None                                           ;
;---------------------------------------------------------;
Wait_Release_Mouse:
; Under construction
ret

;--- Unified routine for get user event: keyboard or mouse ----;
; Reserved functionality: get target devices events            ;
; INPUT:   R15 = Global variables pool base address            ;
;          Global variables                                    ;
; OUTPUT:  CF  = Event type:                                   ;
;                 0(NC)=No events or mouse move only           ;
;                 1(C)=Key press or mouse click                ;
;          RDX = Unified event code                            ;
;--------------------------------------------------------------;
;
; Keyboard Event Code in RDX, when RDX.63=0  (RDX.62-32=Reserved)
;  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
;  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 9 9 8 7 6 5 4 3 2 1 0
; -----------------------------------------------------------------
; |                        EFI Key Code                           |  K-EVENT
; -----------------------------------------------------------------
; 
; Mouse Event Code in EDX, when RDX.63=1  (RDX.62-32=Reserved)
;  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
;  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
; -----------------------------------------------------------------
; |1|0|R|L|  Mouse Y-Coordinate   |0|0|0|0|  Mouse X-Coordinate   |  M-EVENT
; -----------------------------------------------------------------
; 
; L,R - LEFT and RIGHT buttons state
; EDX = 00000000h if no events
; EDX = FFFFFFFFh if Target Device Event (reserved for time stamp)   D-EVENT
; 
Get_User_Event:
; Under construction
xor rdx,rdx
call Input_Check_Key
cmc
; Under construction
ret




