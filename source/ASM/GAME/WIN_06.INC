;===========================================================================;
;                                                                           ;
;           HANDLERS FOR GAME WINDOW 6, DOWN/ALL = KEYS BRIEF HELP.         ;
;           INCLUDE PRESSABLE BUTTONS (BUTTONS UNDER CONSTRUCTION).         ;
;                                                                           ;
;===========================================================================;

;---------- Window 6 (control keys) handler for initialize ----;
; INPUT:   R15 = Global variables pool base address            ;
;          R13 = Microseconds from PC start, TSC used          ;
;          RDX = User event code, 0=No user events             ;
;                FFFFFFFF.FFFFFFFFh = Timer event              ;
;          RCX = Windows reverse count, 1=Last window          ;
;          R8L = Requests bitmap, see output parameters        ;
;          RSI = Pointer to window descriptor structure        ;
;          RDI = Free system memory base, linear 32            ;
;                Used for prepare base linear address          ;
;                of window sprite in the system memory         ;
;                                                              ;
; OUTPUT:  Global variables can be updated                     ;
;          Video transit buffer can be updated                 ;
;          R8L.7 = Program exit request                        ;
;          R8L.6 = Request copy to transit buf. by overlap     ;
;          R8L.5 = Request copy to transit buf. by disjunct    ;
;          R8L.4 = Request for fast update transit buf + VRAM  ;
;                   R8L.4 only for Window(i)_Event routines    ;
;          R8L.[3-0] = Reserved                                ;
;          RDI = Updated by memory allocate for this window    ;
;                required memory size added to RDI             ;
;--------------------------------------------------------------;
W6_Init:
;--- Initialize window ---
mov ax,[_Video_Xsize+r15]   ; Get screen size: Xs
mov [rsi+12],ax             ; Set private size: X=Xs
mov ax,[_Video_Ysize+r15]   ; Get screen size: Ys
sub ax,[rsi+14]             ; Set private size: Y=Ys
mov [rsi+18],ax             ; Set base: Y=Ys-Ythis win.
;--- Reserve high memory for window and exit ---
jmp Win_Local_00            ; Reserve for window sprite

;---------- Window 6 (control keys) handler for open ----------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W6_Open:
;--- Draw box ---
mov eax,[Runtime_Palette_256+30*4]
call Win_Local_01
;--- Draw strings and keys rectangles, select table=f(mode) ---
mov ax,[rsi+12]
mov rsi,Game_Keys_800
cmp ax,1024
jb W6O_1
mov rsi,Game_Keys_1024
cmp ax,1600
jb W6O_1
mov rsi,Game_Keys_1600
;--- Strings write cycle ---
W6O_1:
lodsw         ; Use RSI
test ax,ax
jz W6O_2
xchg bx,ax    ; BX=String base X
lodsq
push rsi
xchg rsi,rax  ; RSI=String
mov dx,6      ; DX=String base Y
mov ecx,00001008h  ; ECX.31=Mode, CH=Font Y, CL=Font X
mov eax,[Runtime_Palette_256+31*4]
call Draw_String   ; This write to private buffer
pop rsi
jmp W6O_1
;--- Mark visual mode and exit ---
W6O_2:
or r8l,01000000b   ; R8L.6=Request copy by overlap
W6O_0:
ret

;---------- Window 6 (control keys) handler for close ---------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W6_Close:
; Reserved
ret

;---------- Window 6 (control keys) handler for work events ---;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;        Output parameters for Window(i)_Event routine         ;
;        if fast update request active, R8L.4=1 returned:     ;
;        BX  = Base X coordinate                               ;
;        DX  = Base Y coordinate                               ;
;        AX  = X size                                          ;
;        DI  = Y size                                          ;
;--------------------------------------------------------------;
W6_Event:
;--- Check for background sound off, for all events ---
call Sound_Check_OFF
;--- Detect event type ---
cmp rdx,0FFFFFFFFFFFFFFFFh
je W6E_40       ; Go if timer event
test rdx,rdx
js W6E_20       ; Go if mouse event
xchg eax,edx
;--- Keyboard handling ---
call Sound_Short_ON
;--- Esc, exit game ---
cmp eax,EFI_KEY_ESC   ; ESC=Exit
jne W6E_00
or r8l,10000000b      ; R8L.7=1, Request for exit
jmp Sound_OFF         ; RET there
;--- Enter, start game ---
W6E_00:
cmp eax,EFI_KEY_ENTER      ; ENTER, start game flow
jne W6E_01
mov byte [_Game_ON+r15],1  ; Game_ON=1=Game started
mov word [_Game_Figures+r15],-1
	;
	;! Disabled, because loaded from config file.
	;!      mov     _Game_Lines,0
	;!      mov     _Game_Level,0
	;
;--- Entry point for new figure draw after previous figure fall ---
New_Figure_Entry:
inc word [_Game_Figures+r15]
mov byte [_Chg_Status+r15],1
rdtsc
shr al,2
and al,3                          ; 4 types of figures: 0-3
mov [_Current_Figure+r15],al
;--- Set time quant=f(level) ---
movzx rbx,byte [_Game_Level+r15]
cmp bl,Game_Levels_Supported
jbe W6E_10
mov bl,Game_Levels_Supported
W6E_10:
shl ebx,2                         ; [rbx]=Dword
mov rax,Game_Levels_Timings
mov eax,[rax+rbx]
mov [_Current_Quant+r15],eax
;--- Blank other variables ---
xor eax,eax                       ; RAX.63-32 also cleared
mov [_Current_X_Y+r15],eax
mov [_Current_Rotate+r15],al
mov [_Delta_X_Y+r15],eax
mov [_Delta_Rotate+r15],al
mov [_Target_Quant+r15],eax
mov [_Deferred_New+r15],al
mov [_Deferred_Line+r15],ax
ret
;--- Cursor Up, rotation ---
W6E_01:
cmp eax,EFI_KEY_UP
jne W6E_02
mov byte [_Delta_Rotate+r15],1
ret
;--- Cursor Left ---
W6E_02:
cmp eax,EFI_KEY_LEFT
jne W6E_03
mov word [_Delta_X+r15],-13   ; Figure Xsize=13
ret
;--- Cursor Right ---
W6E_03:
cmp eax,EFI_KEY_RIGHT
jne W6E_04
mov word [_Delta_X+r15],13    ; Figure Xsize=13
ret
;--- Cursor Down, figure fall ---
W6E_04:
cmp eax,EFI_KEY_DOWN
jne W6E_05
mov rax,Fast_Fall_Timing
mov eax,[rax]
mov [_Current_Quant+r15],eax
ret
;--- "S" key, sound ON/OFF ---
W6E_05:
cmp eax,EFI_KEY_S
jne W6E_06
xor byte [_Sound_Switch+r15],1
jnz Sound_Short_ON			; RET there
jmp Sound_OFF			; RET there
;--- "P" key, pause ---
W6E_06:
cmp eax,EFI_KEY_P
jne W6E_07
Wait_2:
call Sound_Check_OFF
call Get_User_Event  ; Get event code
jnc Wait_2           ; Still wait if not my event
jmp Sound_Short_ON   ; RET there

;--- 
W6E_07:
; Reserved for extension
ret

;--- Mouse handling ---
W6E_20:
; Reserved, under construction
ret

;--- Handling timer tick as changing figure Y coordinate ---
W6E_40: 	
mov rax,r13                    ; R13=Save, RAX=Work
mov edx,[_Current_Quant+r15]   ; Also clear [63-32]
sub rax,[_Target_Quant+r15]
cmp rax,rdx
jb W6E_42
mov [_Target_Quant+r15],r13
inc word [_Delta_Y+r15]
W6E_42:
ret

