;===========================================================================;
;                                                                           ;
;               SUBROUTINES FOR UEFI FILE OPERATIONS SUPPORT.               ;
;                                                                           ;
;===========================================================================;

;--- Locate Simple File System Protocol (FS) ----------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;          RCX = Protocol pointer if operation passed  ;
;------------------------------------------------------;
EFI_Locate_File_System:
mov rcx,GuidFileSystem  ; Pointer to Fs GUID
call Locate_Protocol
jrcxz Bad_Fs            ; Spec. case: status=OK but handle=0
Exit_Fs:
ret
Bad_Fs:
stc
ret

;--- Open Volume for selected protocol ----------------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;          RCX = Protocol pointer                      ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;          RCX = Protocol pointer if operation passed  ;
;------------------------------------------------------;
EFI_Open_Volume:
xor rax,rax     ; Scratch pad=0
push rdx rax    ; Include make scratch pad
mov rdx,rsp     ; Stack is work area, 1 qword
mov rax,8       ; RAX=FS.OpenVolume, RCX=Prt.
call EFI_API    ; Return: RAX, CF
pop rcx rdx     ; Get RCX from scratch pad
jrcxz Bad_Fs    ; Go if Pointer=0
ret

;--- Open or Create file for selected parent handle -----------;
; Note. Can create directory, if attributes means it.          ;
; INPUT:   R15 = Global variables pool base address            ;
;          Use 64-bit flat memory addressing                   ;
;          RCX = Parent handle (example: directory for file)   ;
;          RAX = Attributes:                                   ;
;                D0=Read Only, D1=Hidden, D2=System, D3=Rsvd.  ;
;                D4=Directory, D5=Archive, other bits reserved ;
;          R8  = Pointer to NULL-terminated UNICODE16 string:  ;
;                file name                                     ;
;          R9  = Open mode: D63=Create, D1=Write, D0=Read      ;
;                D1,D0 must be "1" if D63=1, other bits rsvd.  ;
;                                                              ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed               ;
;          RAX = UEFI Status                                   ;
;          RCX = Opened/Created file handle, if passed         ;
;--------------------------------------------------------------;
EFI_Open_Create_File:
test rsp,8
jz Align_1
call EFI_Open_Create_File_Aligned
ret
Align_1:	
push rbx
call EFI_Open_Create_File_Aligned
pop rbx
ret

EFI_Open_Create_File_Aligned:

;--- Save registers and allocate stack space ---
push rbx rdx rsi rdi rbp r8 r9 r10 r11 r12 r13 r14 r15
;--- Prepare output parameter storage ---
xor rdx,rdx         ; Pre-clear scratch pad
push rdx            ; Create scratch pad
mov rdx,rsp         ; RDX=Pointer to scratch pad
sub rsp,40          ; Prevent AMI shadow bug
;--- Prepare input parameters and call ---
; rax already set by input	; Parm#5 = Attributes
push rax            ; Push qword #1
; r9 already set by input	; Parm#4 = Open Mode
push r9             ; Push qword #2
; r8 already set by input	; Parm#3 = String Pointer
push r8             ; Push qword #3
; rdx already set		; Parm#2 = Pointer to handle
push rdx            ; Push qword #4
; rcx already set		; Parm#1 = Parent protocol
push rcx            ; Push qword #5
call qword [rcx+8]	; Call function: open file
;--- Prepare for exit ---
add rsp,40+40       ; Remove parms+reserved area
pop rcx             ; Restore pointer
test rax,rax        ; Check RAX = UEFI status
jnz Bad_Open        ; Go if UEFI error returned
jrcxz Bad_Open      ; Go if handle=0
;--- Exit points ---		
Exit_Open:
pop r15 r14 r13 r12 r11 r10 r9 r8 rbp rdi rsi rdx rbx
ret
Bad_Open:
stc
jmp	Exit_Open

;--- Close file for selected file handle --------------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;          RCX = File handle                           ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;------------------------------------------------------;
EFI_Close_File:
mov rax,16      ; RAX=File.Close, RCX=Prot.
EFI_API_Entry:
jmp	EFI_API     ; Return: RAX, CF. RET there

;--- Delete file for selected file handle -------------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;          RCX = File handle                           ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;------------------------------------------------------;
EFI_Delete_File:
mov rax,24          ; RAX=File.Delete, RCX=Prot.
jmp EFI_API_Entry   ; Return: RAX, CF. RET there

;--- Read file for selected file handle ---------------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;          RCX = File handle                           ;
;          RDX = Buffer size, allocated total          ;
;          R8  = Buffer base address                   ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;          RDX = Buffer size updated, really read size ;
;------------------------------------------------------;
EFI_Read_File:
mov rax,32        ; RAX=File.Read, RCX=Prot.
EFI_File_Entry:
push rdx
mov rdx,rsp
call EFI_API      ; Return: RAX, CF
pop rdx
ret

;--- Write file for selected file handle --------------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;          RCX = File handle                           ;
;          RDX = Buffer size, allocated total          ;
;          R8  = Buffer base address                   ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;          RDX = Buf. size updated, really write size  ;
;------------------------------------------------------;
EFI_Write_File:
mov rax,40           ; RAX=File.Write, RCX=Prot.
jmp EFI_File_Entry

;--- Built path for files in the program directory -------;
; Convert path and file name string from ASCII to UNICODE ;
; Set address for file I/O buffer                         ;
;                                                         ;
; INPUT:   R15 = Global variables pool base address       ;
;          Use 64-bit flat memory addressing              ;
;          RSI = Pointer to file name for add, ASCII      ;
;          Global variables                               ;
; OUTPUT:  R8  = Pointer to path\name string, UNICODE     ;
;          RDI = Pointer to free buffer for file I/O      ;
;---------------------------------------------------------;
EFI_File_Path_Built:
push rax rsi
; UNDER CONSTRUCTION
mov rdi,[_File_Mem_Base+r15]
call String_Copy_Converted
xor eax,eax
stosw
mov r8,[_File_Mem_Base+r15]
lea rdi,[r8+32]
; UNDER CONSTRUCTION
pop rsi rax
ret

