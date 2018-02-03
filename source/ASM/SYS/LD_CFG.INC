;===========================================================================;
;                                                                           ;
;                   HANDLER FOR LOAD CONFIGURATION FILE.                    ;
;                                                                           ;
;===========================================================================;

;--- Load configuration file ProgramName.CFG -----------------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF    = Flag: 0(NC)=Operation Passed, 1(C)=Failed        ;
;          Variable Sys_Bitmap and context control variables        ;
;-------------------------------------------------------------------;
Load_Config:
;--- Initializing file access, also used later, for screen save ---
call EFI_Locate_File_System
jc Bad_LdCfg                        ; Go if UEFI error returned
call EFI_Open_Volume
jc Bad_LdCfg                        ; Go if UEFI error returned
mov [_EFI_Dir_Volume+r15],rcx

;--- Pre-clear parameters for prevent unexpected values ---
xor	eax,eax
mov	[_Video_XYsize+r15],eax         ; Access both X,Y
mov	[_Video_BPP+r15],al
;--- Get program run path for .EFI file, built path for .CFG file ---
mov rsi,Cfg_File_Name
call EFI_File_Path_Built
;--- Load configuration file ---
; Here actual after Path_Built
; R8  = Pointer to file path\name string
; RDI = Pointer to free buffer for file I/O
;----------
xor rax,rax                         ; RAX=Attributes
mov r9,0000000000000001h            ; R9=Open mode: Open for Read
call EFI_Open_Create_File           ; Open file
jc Exit_LdCfg_New                   ; Go if Open file error
mov r8,rdi                          ; R8=Base address
mov rdx,61440+1                     ; RDX=Maximum size=60KB
mov rbx,rdx                         ; This for compare later
call EFI_Read_File                  ; If 60KB+1 = too big size
jc Exit_LdCfg_New                   ; Go if Read file error
cmp rdx,rbx                         ; RDX=Really read, RBX=Req.
jae Exit_LdCfg_New                  ; Go if file size too big
call EFI_Close_File                 ; Close file
jc Exit_LdCfg_New                   ; Go if Close file error
;--- Interpreting configuration file ---
mov r8d,1                           ; R8D=Lines counter
add rdx,rdi                         ; RDI=Start, RDX=End file location
;--- Start interpreting cycle ---
L_Interp_Cycle:
cmp rdi,rdx                         ; Compare pointer with limit
jae Done_LdCfg                      ; Go if end of file detected
mov rsi,Keywords_List
;--- Start keywords comparision cycle ---
; RBX  = Pointer to keyword nul-terminated string
; R9   = Pointer to keyword handling subroutine
;---
L_Key_Cycle:
lodsq
xchg rbx,rax                        ; RBX=Data pointer: keyword
lodsq
xchg r9,rax                         ; R9=Code pointer: handler
add rsi,2+10                        ; Skip entire: CFGL+CFGS
test rbx,rbx                        ; Check end of keywords list
jz L_Key_Error                      ; Go if not recognized word
call Local_Cmp_String
;--- End of compare cycle, result=ZF flag ---
jne L_Key_Cycle                      ; Go if key mismatch
sub rsi,2+10                         ; RSI=Current structure
cmp rdi,rdx                          ; Check limit after keyword
jae Done_LdCfg                       ; Go if end of file detected
call r9                              ; Call handler for keyword
jc L_Key_Error		; Go if error
;--- Parameter update ---
call Local_Set_Mask_Parm             ; Here also LODSW, SI+2
;--- Cycle ---
add rsi,10                           ; Skip saver block 
jmp L_Interp_Cycle                   ; Continue if no errors
;--- End of keyword comparision cycle ---
; control here if keywords not recognized
L_Key_Error:
call Local_Error_String
jmp Exit_LdCfg_New                   ; Skip parameters setup

;------- Verify validity of loaded parameters, make warnings if required ---;
Done_LdCfg:
;--- Xsize ---
mov	ax,[_Video_Xsize+r15]            ; AX=From Name.CFG, 0=None
mov	bx,[_Video_Xmin+r15]             ; BX=Supported minimum
mov	cx,Limit_X                       ; CX=Supported maximum (hard)
;!	mov	cx,[_Video_Xmax+r15]
call	Local_Parms_Limit              ; Select parameter
mov	[_Video_Xmax+r15],ax             ; Save parameter
;--- Ysize ---
mov ax,[_Video_Ysize+r15]
mov bx,[_Video_Ymin+r15]
mov cx,Limit_Y
;!	mov	cx,[_Video_Ymax+r15]
call Local_Parms_Limit               ; Same action for Y
mov [_Video_Ymax+r15],ax
;--- BPP (bits per pixel) ---
movzx ax,byte [_Video_BPP+r15]
movzx bx,byte [_Video_BPPmin+r15]
mov cx,Limit_BPP
;!	mov	cx,[_Video_BPPmax+r15]
call Local_Parms_Limit               ; Same action for BPP
mov [_Video_BPPmax+r15],al
;--- Color scheme, valid range: 0-7 ---
and byte [_Color_Scheme+r15],00000111b
;--- Sound switch, valid range: 0-1 ---
and byte [_Sound_Switch+r15],00000001b
;--- Game lines done ---
; No corrections required, 1 word, values 0-65535
;--- Game level ---
; No corrections required, 1 byte, values 0-255
;--- Game score ---
; No corrections required, 1 byte, values 0-255
;--- Start pause, include update Sys_Bitmap ---
and byte [_Sys_Bitmap_H+r15],0BFh    ; D14=Pause flag
and byte [_Start_Pause+r15],00000001b
jz Exit_LdCfg
or byte [_Sys_Bitmap_H+r15],040h     ; D14=Pause flag
	
;--- Exit ---
; Configuration file absence is non fatal error, use defaults
; Invalid configuration file is non fatal error, use defaults
Exit_LdCfg:
clc
Bad_LdCfg:
ret
Exit_LdCfg_New:
or byte [_Sys_Bitmap_H+r15],80h    ; CF=0(NC) after OR
ret


;---------- Main local subroutine ------------------------------------------;


;---------- Handlers for config file interpreting ------------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          RDI = Pointer to analysed text file Name.CFG             ;
;          RDX = Text file limit                                    ;
;          R8D = Current text string number in the text file        ;
;          EAX = Parameter value for parameters strings             ;
;                                                                   ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Syntax error     ;
;          RDI = Modified by text scan                              ;
;          Variables updated by config file strings:                ;
;          Ysize, Xsize, BPP, Color, Sound, Lines, Level, Score     ;
;-------------------------------------------------------------------;
HL_Parms:
;--- Handling  LINE FEED ---
HL_LF:
inc r8d                  ; Lines counter+1
;--- Handling SPACE ---
;--- Handling  TAB ---
;--- Handling CARRIAGE RETURN ---
HL_Space:
HL_Tab:
HL_CR:
HL_C_1:
clc                      ; CF=0(NC) means no errors
ret
;--- Handling  ";" ---
HL_Comment:
cmp	rdi,rdx              ; Compare pointer with limit
jae	HL_C_1               ; Go if end of file detected
mov	al,[rdi]             ; Get char from string
cmp	al,10                ; 10=0Ah=Line Feed
je	HL_LF                ; Go if line feed
cmp	al,13                ; 13=0Dh=Carriage Return
je	HL_CR                ; Go if carriage return
inc	rdi                  ; Select next char
HL_Comment_2:
jmp HL_Comment           ; Cycle for skip this string

;--- Handling XSIZE ---
HL_Xsize:	
;--- Handling YSIZE ---
HL_Ysize:
;--- Handling BPP (BITS PER PIXEL) ---
HL_BPP:
;--- Handling COLOR ---
HL_Color:
;--- Handling SOUND ---
HL_Sound:
;--- Handling NUMBER OF LINES DONE ---
HL_Lines:
;--- Handling GAME LEVEL ---
HL_Level:
;--- Handling START PAUSE ---
HL_Pause:
HL_X_1:
call Local_Skip_Space    ; Skip spaces before "="
jnc HL_X_7               ; Go if error: no parameter
cmp al,'='               ; Required ParmName=Value
jne HL_X_1               ; Go if not "="
inc rdi                  ; Skip "="
call Local_Skip_Space    ; Skip spaces after "="
xor ebx,ebx
HL_X_5:
mov al,[rdi]             ; Interpreting ASCII number
cmp al,'0'
jb HL_X_9                ; Go if error: wrong char
cmp al,'9'
jna HL_X_8               ; Go if error: wrong char
HL_X_9:
push rax
call Local_Skip_Space    ; Skip spaces after number
pop rax
jc HL_X_7                ; Go if error: extra chars
jmp HL_X_4               ; End of string or file
HL_X_8:
imul ebx,ebx,10          ; Set power of 10
and eax,000Fh            ; Acceptable: 0-9
add ebx,eax              ; Add parsed digit
inc rdi                  ; Advance string pointer
cmp rdi,rdx
jb HL_X_5                ; Inversion of JAE
HL_X_4:
xchg eax,ebx             ; EAX=Parameter value
ret                      ; Here CF=0(NC)
HL_X_7:
stc
ret

;--- Handling GAME SCORE ---
; Use feature: if HL_X_1 subroutine return error, CF=1(C),
; then output parameter returned at EBX, not EAX
HL_Score:	
call HL_X_1              ; Return EAX or EBX if CF=1
jnc HL_S_5               ; Exit if no gamer string
cmp byte [rdi],','
jne HL_X_7               ; Exit if gamer string wrong
inc rdi                  ; Skip "," char, data at EBX
mov rbp,Buffer_6
mov rcx,Buffer_6_Size
call Local_Skip_Space
jnc HL_S_4               ; Go to blank if no name
HL_S_3:
mov al,[rdi]
cmp al,10
je HL_S_4                ; Go if LINE FEED
cmp al,13
je HL_S_4                ; Go if CARRIAGE RETURN
mov [rbp],al
inc rdi
cmp rdi,rdx
jae HL_S_4               ; Go if end of file detected
inc rbp
loop HL_S_3
HL_S_4:
jrcxz HL_S_2
HL_S_1:
mov byte [rbp],0
inc rbp
loop HL_S_1
HL_S_2:
xchg eax,ebx             ; Restore data from EBX
HL_S_5:
clc                      ; CF=0(NC) means no errors
ret

;--- Handling load GAME FIELD as bitmap ---
; Reserved yet.


;---------- Local subroutines ----------------------------------------------;


;--- Compare parameters and make limits ----------;
; INPUT:   R15 = Global variables pool pointer    ;
;          AX  = Parameter value                  ;
;          BX  = Minimum limit                    ;
;          CX  = Maximum limit                    ;
; OUTPUT:  AX  = Corrected by limits,             ;
;                if limits violated               ;
;                or parameter not defined (AX=0)  ;
;-------------------------------------------------;
Local_Parms_Limit:
test ax,ax
jz LdCfg_Parm_3          ; Go if parameter not defined
cmp ax,bx
jae LdCfg_Parm_1         ; Go if parameter >= min, OK
mov ax,bx                ; Correction if P < Pmin
LdCfg_Parm_1:
cmp ax,cx
jbe LdCfg_Parm_2         ; Go if parameter <= max, OK
LdCfg_Parm_3:
mov ax,cx                ; Correction if P > Pmax
LdCfg_Parm_2:
ret

;--- Compare text strings, support upper/lower case ------;
; INPUT:   RBX = Pointer to nul-terminated pattern        ;
;          RDI = Pointer to analysed text                 ;
; OUTPUT:  ZF flag = compare result:                      ;
;                    0(NZ)=Mismatch, 1(Z)=Match           ;
;          RDI     = Modified by analysed text scan       ;
;          RBP     = Storage for start DI                 ;
;---------------------------------------------------------;
Local_Cmp_String:
;--- Initializing compare cycle ---
push rsi
mov rsi,rbx              ; RSI=Pointer to keyword
mov rbp,rdi              ; RBP=Buffer address storage
;--- Compare with keyword, can't use CMPSB because lower/upper case ---
Key_Cmp_4:
lodsb                    ; Get char from pattern, use RSI
cmp al,0
je Key_Cmp_3             ; Go if end of pattern
mov ah,[rdi]             ; Get char from cfg. file
inc rdi
cmp al,'a'
jb Key_Cmp_1
cmp al,'z'
ja Key_Cmp_1
and al,11011111b         ; Make upper case
Key_Cmp_1:
cmp ah,'a'
jb Key_Cmp_2
cmp ah,'z'
ja Key_Cmp_2
and ah,11011111b         ; Make upper case
Key_Cmp_2:
cmp al,ah
je Key_Cmp_4             ; Continue cycle if match
mov rdi,rbp		           ; Restore buffer address if mismatch
Key_Cmp_3:
pop rsi
;--- Exit ---
ret

;--- Mask parameter valid bits and set parameter -------------------;
; If parameter offset=0, then not update, this is not parameter     ;
; keywords, for example comments or empty string                    ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          RSI = Pointer to control data:                           ;
;                 BYTE at [RSI+0] = Parameter signed offset         ;
;                 BYTE at [RSI+1] = Parameter width, bits           ;
;          EAX = Parameter value, valid bits masked                 ;
;          Variables at Global Variables Pool                       ;
; OUTPUT:  RSI modified, return RSI+2                               ;
;          CL, RBX, RBP corrupted, RAX masked                       ;
;-------------------------------------------------------------------;
Local_Set_Mask_Parm:
push rax
lodsw                    ; AL=Parm offset (8-bit), AH=Width
cmp al,0
je LdCfg_Parm_5          ; Skip if this is no parm.
mov cl,32
sub cl,ah                ; CL=Mask shifts count
movsx rbp,al             ; RBP=Parameter offset, signed
mov ebx,0FFFFFFFFh
shr ebx,cl
pop rax
and eax,ebx              ; Mask valid bits: 8/16/32
not ebx
and [r15+rbp],ebx
or [r15+rbp],eax
ret                      ; Return after make
LdCfg_Parm_5:
pop rax                  ; This for restore SP
ret                      ; Return after skip

;--- Scan string for ASCII chars, skip spaces ------------;
; INPUT:   RDI = Pointer to string                        ;
;          RDX = Buffer limit for compare                 ;
; OUTPUT:  CF  = Flag:                                    ;
;                0(NC)=Char not found, end of string      ;
;                      or end of text file                ;
;                  1(C)=Char found                        ;
;          AL    = Last read char                         ;
;---------------------------------------------------------;
Local_Skip_Space:
LdCfg_Skip_3:
mov al,[rdi]             ; Get char from text file
cmp al,10
je LdCfg_Skip_1          ; Go if LINE FEED, end string
cmp al,13
je LdCfg_Skip_1          ; Go if CARRIAGE RETURN, end
cmp al,' '
je LdCfg_Skip_2          ; Go if SPACE, continue str.
cmp al,9
jne LdCfg_Skip_4         ; Go if not TAB, char detected
LdCfg_Skip_2:
inc rdi                  ; Address+1
cmp rdi,rdx		
jb LdCfg_Skip_3          ; Go if no end of file (n-JAE)
LdCfg_Skip_1:
clc                      ; CF=0 means char not found
ret                      ;      or end of file
LdCfg_Skip_4:
stc                      ; CF=1 means char found
ret

;--- Write error message about analysed string -----------;
; INPUT:   R15 = Global variables pool base address       ;
;          R8D = String number in the file                ;
; OUTPUT:  None                                           ;
;          Corrupted: RAX, RBX, RCX, RDX                  ;
;---------------------------------------------------------;
Local_Error_String:
mov rdi,Msg_E_Line_1     ; RDI = Destination string
mov bl,0                 ; BL=0 means no template
mov cx,5                 ; CX=Number of blanked chars
mov eax,r8d              ; EAX=Number, here max. 65535
call Blank_Dec_Print_32  ; Prepare line number
mov rsi,Msg_CRLF         ; Carriage Return, Line Feed
call String_Write
mov rsi,Msg_E_Line       ; Error msg. with line number
call String_Write
mov rsi,Msg_Error_2      ; "Press any key..."
call String_Write
call Input_Wait_Key      ; Wait for key press
mov rsi,Msg_CRLF         ; Next string
jmp String_Write         ; RET there

