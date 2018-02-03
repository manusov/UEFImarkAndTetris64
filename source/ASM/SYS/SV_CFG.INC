;===========================================================================;
;                                                                           ;
;                   HANDLER FOR SAVE CONFIGURATION FILE.                    ;
;                                                                           ;
;===========================================================================;

;--- Save configuration file ProgramName.CFG, with user dialogue ---;
; For UEFImark file name is UEFIMARK.CFG                            ;
; File created if not exist or updated if exist                     ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;          Variable Sys_Bitmap and context control variables        ;
;-------------------------------------------------------------------;
Save_Config:
;--- Dialogue with user, send message ---
call Local_Question            ; Question to user (Y/N)
jnc Skip_SvCfg                 ; Exit if user press "N"
mov rsi,Msg_Y_Save             ; Message "Saving..."
call String_Write
;--- Get program run path for .EFI file, built path for .CFG file ---
; Note yet used root directory.
;---
mov rsi,Cfg_File_Name
call EFI_File_Path_Built       ; R8=Name, RDI=Buffer
;--- Save and prepare parameters ---
mov r11,rdi                    ; R11=Buffer, save
mov r12,r8                     ; R12=Path, save
mov r13,EFI_NOT_FOUND          ; R13=File not found
;--- Load current configuration file ---
; Here actual after Path_Built:
; R8  = Pointer to file path\name string
; RDI = Pointer to free buffer for file I/O
;---
mov rcx,[_EFI_Dir_Volume+r15]  ; RCX=Parent handle
xor rax,rax                    ; RAX=Attributes
mov r9,0000000000000003h       ; R9=Open mode: Open
call EFI_Open_Create_File      ; Open file
jc New_SvCfg                   ; Go if error or not exist
mov r9,r8                      ; R9=Path address
mov r8,rdi                     ; R8=Buffer base address
mov rdx,61440+1                ; RDX=Size, maximum 60KB
mov rbx,rdx                    ; RBX=Size for compare
call EFI_Read_File             ; RCX=Handle
jc Bad_SvCfg                   ; Go if file read error
call EFI_Close_File
jc Bad_SvCfg                   ; Go if close error
cmp rdx,rbx                    ; RDX=Really read, RBX=Req.
jae Bad_SvCfg                  ; Go if file size too big
;--- Save old file size ---
mov r10,rdx                    ; R10=File size
;--- Modify file name to .BAK ---
Back_SvCfg:
mov rbx,Bak_File_Ext           ; RBX=Replace extension
mov rdx,r9                     ; RDX=Current path
call Local_Change_Ext
jc Bad_SvCfg                   ; Go if error, bad path string
;--- Write current configuration file backup ---
; Here actual after path built and file read:
; R8  = Pointer to buffer with current configuration file
; R9  = Pointer to file path\name string
; R10 = Current configuration file size
;--- Check previous BAK exist ---
mov rcx,[_EFI_Dir_Volume+r15]  ; RCX=Parent handle
xor rax,rax                    ; RAX=Attributes
mov r8,r9                      ; R8=Path pointer
mov r9,0000000000000003h       ; R9=Open mode: Open
call EFI_Open_Create_File	; This for check file exist
;--- Delete previous BAK ---
jnc Del_Bak_SvCfg              ; Go delete if exist
cmp rax,r13
je No_Bak_SvCfg                ; Skip delete if not exist
jmp Bad_SvCfg                  ; Otherwise error
Del_Bak_SvCfg:
call EFI_Delete_File
jc Bad_SvCfg                   ; Go if delete error
;--- End of delete section, start open-write-close ---
No_Bak_SvCfg:
mov rcx,[_EFI_Dir_Volume+r15]  ; RCX=Parent handle
xor rax,rax                    ; RAX=Attributes
mov r9,8000000000000003h       ; R9=Open mode: Create
call EFI_Open_Create_File      ; Create file *.BAK
jc Bad_SvCfg                   ; Go if create error
mov r8,r11                     ; R8=Base
mov rdx,r10                    ; RDX=Size
call EFI_Write_File            ; RCX=Handle
jc Bad_SvCfg                   ; Go if write error
call EFI_Close_File
jc Bad_SvCfg                   ; Go if close error
cmp rdx,r10
jne Bad_SvCfg                  ; Go if disk full
;--- Modify file name to .CFG (return) ---
mov rbx,Cfg_File_Ext           ; RBX=Replace extension
mov rdx,r12                    ; RDX=Current path
call Local_Change_Ext
jc Bad_SvCfg                   ; Go if error, bad path string
;--- Create data block for new configuration file ---
New_SvCfg:
mov r10,rdi
;--- Header ---
mov	ax,'; '
stosw
mov rsi,String_Version
call String_Copy_Simple
mov rsi,Program_Vendor_Data_1
call String_Copy_Simple
mov rsi,Report_Middle
call String_Copy_Simple
mov rsi,Cfg_File_Header
call String_Copy_Simple
;--- Parameters list ---		
mov rsi,Keywords_List_1
Parms_SvCfg:
mov ax,0A0Dh
stosw                          ; Write end of prev. string
lodsq                          ; RAX=Offset of string, RSI+8
test rax,rax
jz All_SvCfg                   ; Go if end of parms. list
push rsi
xchg rsi,rax
call Local_Name_Parm           ; Get and mask parameter
pop rsi
add rsi,10                     ; Skip loader part
lodsq                          ; RAX=Handler pointer, RSI+8
push rax
call Local_Get_Mask_Parm       ; Write parm. name, also SI+2
call Local_Value_Parm          ; Write parm. value, ASCII
pop rax
call rax                       ; Call handler for addends
jmp Parms_SvCfg                ; Cycle for all parameters
;--- Set parameters for save file: R10=Size ---
All_SvCfg:
xchg r10,rdi
sub r10,rdi
;--- Write new configuration file ---
; Here actual after path built and new file built:
; R10 = Current configuration file size
; R11 = Pointer to buffer with current configuration file
; R12 = Pointer to file path\name string
;--- Check previous CFG exist ---
mov rcx,[_EFI_Dir_Volume+r15]  ; RCX=Parent handle
xor rax,rax                    ; RAX=Attributes
mov r8,r12                     ; R8=Path pointer
mov r9,0000000000000003h       ; R9=Open mode: Open
call EFI_Open_Create_File	; This for check file exist
;--- Delete previous CFG ---
jnc Del_Cfg_SvCfg              ; Go delete if exist
cmp rax,r13
je No_Cfg_SvCfg                ; Skip delete if not exist
jmp Bad_SvCfg                  ; Otherwise error
Del_Cfg_SvCfg:
call EFI_Delete_File
jc Bad_SvCfg                   ; Go if delete error
;--- End of delete section, start open-write-close ---
No_Cfg_SvCfg:
mov rcx,[_EFI_Dir_Volume+r15]  ; RCX=Parent handle
xor rax,rax                    ; RAX=Attributes
mov r9,8000000000000003h       ; R9=Open mode: Create
call EFI_Open_Create_File      ; Create file *.BAK
jc Bad_SvCfg                   ; Go if create error
mov r8,r11                     ; R8=Base
mov rdx,r10                    ; RDX=Size
call EFI_Write_File            ; RCX=Handle
jc Bad_SvCfg                   ; Go if write error
call EFI_Close_File
jc Bad_SvCfg                   ; Go if close error
cmp rdx,r10
jne Bad_SvCfg                  ; Go if disk full

;--- Exit points ---
Ok_SvCfg:
clc
ret
Bad_SvCfg:
stc
ret
Skip_SvCfg:
mov rsi,Msg_N_Save    ; Message "Skipping..."
call String_Write
clc
ret


;---------- Local subroutines ----------------------------------------------;


;---------- Handlers for config file entries addends ---------------;
; Built special addends for parameters, required it                 ;
; " Parameter=Value "  - write at cycle, without this subroutine    ;
; " Parameter=Value, addend " - addend write by this subroutine,    ;
; Example: " Score = 200, Ivanov " - here " Ivanov " is addend      ;
;                                                                   ;
; INPUT:   RDI         = Pointer to creating text file *.CFG        ;
;          RDX         = Text file limit                            ;
;          Data buffer = Buffer_6                                   ;
;                                                                   ;
; OUTPUT:  RDI         = Modified by text scan                      ;
;-------------------------------------------------------------------;
HS_Score:	
mov rbp,Buffer_6      ; Buffer_6 contain Gamer Name
mov rcx,Buffer_6_Size
cmp byte [rbp],0
je HS_Exit            ; Go if Gamer Name empty
mov ax,', '
stosw
stosb
HS_S_1:
mov al,[rbp]
inc rbp
cmp al,0
je HS_Exit      ; Go if Gamer Name copied OK
stosb
loop HS_S_1     ; Here LOOPQ, use RCX
;--- Exit, some entries reserved  ---
HS_Space:
HS_Tab:
HS_LF:
HS_CR:
HS_Comment:
HS_Xsize:
HS_Ysize:
HS_BPP:
HS_Color:
HS_Sound:
HS_Lines:
HS_Level:
HS_Pause:
HS_Exit:	ret

;--- Get parameter and mask it valid bits --------------------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          RSI = Pointer to control data:                           ;
;                 BYTE at [RSI+0] = Parameter signed offset         ;
;                 BYTE at [RSI+1] = Parameter width, bits           ;
;          Variables at Global Variables Pool                       ;
; OUTPUT:  EAX = Parameter value, valid bits masked                 ;
;          RSI modified, return RSI+2                               ;
;          CL, EBP corrupted                                        ;
;-------------------------------------------------------------------;
Local_Get_Mask_Parm:
lodsw              ; AL=Parm offset, AH=Width
mov cl,32
sub cl,ah          ; CL=Mask shifts count
movsx rbp,al       ; BP=Parameter offset, signed
mov eax,[rbp+r15]
mov ebp,0FFFFFFFFh
shr ebp,cl
and eax,ebp        ; Mask valid bits: 8/16/32
ret

;--- Write parameter name, tabulate, equality, space ---------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          RSI = Pointer to parameter name, nul-terminated string   ;
;          RDI = Pointer to destination buffer                      ;
; OUTPUT:  RSI updated by write                                     ;
;          RDI updated by write                                     ;
;          EAX corrupted                                            ;
;          CX corrupted                                             ;
;-------------------------------------------------------------------;
Local_Name_Parm:
mov rcx,rsi
call String_Copy_Simple  ; Copy parameter name
sub rsi,rcx    ; SI=End-Start=Parm. name length
mov rcx,9      ; Template for parm. name = 9
sub rcx,rsi    ; Subtract parm. name length
jc SvCfg_1     ; Overflow protect
mov al,' '
rep stosb      ; Write " ", align X-position of "="
SvCfg_1:
mov ax,'= '
stosw          ; Store "=" and " " chars
ret

;--- Write parameter value, CR, LF ---------------------------------;
; INPUT:   EAX = Parameter value, already masked                    ;
;          RDI = Pointer to destination buffer                      ;
; OUTPUT:  RDI updated by write                                     ;
;          AX corrupted                                             ;
;          BL corrupted                                             ;
;-------------------------------------------------------------------;
Local_Value_Parm:
mov bl,0              ; BL=Template mode, 0=No
jmp Decimal_Print_32  ; Write EAX=Parameter value

;--- Replace extension for path ------------------------------------;
; Notes. Need find last "." char, because directories namse also    ;
;        can have extensions.                                       ;
;        This procedure assume use 3-char extensions (.CFG/.BAK).   ;
;                                                                   ;
; INPUT:   RBX = Pointer to extension string, nul-terminated, ASCII ;
;          RDX = Pointer to path string, for update, UNICODE16      ;
; OUTPUT:  CF  = Error flag: 0(NC)=OK, 1(C)=Bad path string         ;
;-------------------------------------------------------------------;
Local_Change_Ext:
push rax rsi rdi
mov rsi,rdx       ; RSI=Path Pointer, RDX=Start
;--- Find end of string, word=00h ---
SvCfg_2:
lodsw
test ax,ax
jnz SvCfg_2       ; Scan for 00h, end of string
sub rsi,2
;--- Find last occured byte="." ---
std               ; Set strings decrement mode
SvCfg_3:
lodsw
cmp ax,0000h+'.'
je SvCfg_4        ; Go if "." found
cmp rsi,rdx
jb SvCfg_9        ; Go if error, "." not found
jmp SvCfg_3       ; Cycle for search "."
;--- Replace extension ---
SvCfg_4:
cld               ; Set strings increment mode
lea rdi,[rsi+4]   ; RDI=Path
mov rsi,rbx       ; RSI=Pattern const.
mov ah,0
SvCfg_11:
lodsb
cmp al,0
je SvCfg_12
stosw
loop SvCfg_11
;--- Check 3-char extensions requirements, by check last word ---
SvCfg_12:
cmp word [rdi],0
jne SvCfg_9
;--- Exit points ----
SvCfg_10:
pop rdi rsi rax  ; Here CF=0(NC), means no errors
ret
SvCfg_9:
cld
stc              ; CF=1(C) means error
jmp	SvCfg_10

;--- Write question message about save game and wait for keys ------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Mode flag:                                          ;
;                0(NC)=User press "N", means NO, skip operation     ;
;                1(C)=User press "Y", means YES, make operation     ;
;-------------------------------------------------------------------;
Local_Question:
push rax rdx rsi
;--- Message ---
mov rsi,Msg_Q_Save
call String_Write
;--- Wait for key press: acceptable "Y" or "N" ---
Quest_Wait:
call Input_Wait_Key   ; Wait for key press
;--- Analysing input ---
ror rdx,16            ; Move scan code to DL
or dl,00100000b       ; Set lower case, D5=1
cmp rdx,EFI_K_N
je Quest_Ok           ; CF=0(NC) if NO
cmp rdx,EFI_K_Y
jne Quest_Wait
stc                   ; CF=1(C) if YES
;--- Write char "Y" or "N" ---
Quest_Ok:
pushf
mov rsi,Buffer_ASCII
mov [rsi],dx          ; DH=00h, DL=Char
call String_Write
popf
;--- Exit ---
pop rsi rdx rax
ret



