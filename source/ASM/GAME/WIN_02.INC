;===========================================================================;
;                                                                           ;
;             HANDLERS FOR GAME WINDOW 2, CENTRAL = MAIN FIELD.             ;
;                                                                           ;
;===========================================================================;

;------- Window 2 (game main field) handler for initialize ----;
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
W2_Init:
;--- Special support for Mode < 1024x768 ---
mov ax,539
cmp word [_Video_Ysize+r15],768
jae W2I_1
mov ax,539-40
W2I_1:
mov [rsi+14],ax
;--- X,Y centering ---
xor ebx,ebx         ; RBX=0 for X centering, BX=2 for Y
mov ax,[_Video_Xsize+r15]
W2I_0:
sub ax,[rsi+rbx+12]
shr ax,1
and al,11111100b
mov [rsi+rbx+16],ax
mov ax,[_Video_Ysize+r15]
inc ebx             ; Modify BX for Y centering
inc ebx
cmp bl,2
jbe W2I_0
;--- Allocate high memory ---
jmp Win_Local_00    ; Reserve for window sprite
; RET there

;------- Window 2 (game main field) handler for open ----------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W2_Open:
;--- All box ---
mov eax,[Runtime_Palette_256+17*4]
call Win_Local_01
;--- Top line ---
mov eax,[Runtime_Palette_256+18*4]
xor bx,bx          ; BX=Xbase
xor dx,dx          ; DX=Ybase
mov cx,[rsi+12]    ; CX=Xsize
push rsi
mov si,1           ; SI=Ysize
call Draw_Box_SSE
pop rsi
;--- Up line 2 ---
mov eax,[Runtime_Palette_256+19*4]
mov dx,1           ; DX=Ybase
push rsi
mov si,1           ; SI=Ysize
call Draw_Box_SSE
pop rsi
;--- Bottom line, double ---
mov eax,[Runtime_Palette_256+20*4]
sub cx,1           ; CX=Xsize
mov dx,[rsi+14]    ; Get Ysize
dec dx
dec dx             ; DX=Ybase
push rsi
mov si,2           ; SI=Ysize
call Draw_Box_SSE
pop rsi
;--- Vertical grid ---
mov bp,[rsi+14]  ; Get Ysize
sub bp,4
mov ax,[rsi+12]  ; Get Xsize
cwd
mov bx,13
div bx
xchg di,ax
dec di		
mov bx,12         ; Start Xoffset=12
W2O_01:
mov eax,[Runtime_Palette_256+21*4]
xor dx,dx         ; Start Yoffset=0
mov cx,2          ; Xsize=2
mov si,1          ; Ysize=1
call Draw_Box_SSE
mov eax,[Runtime_Palette_256+22*4]
inc dx
call Draw_Box_SSE
mov eax,[Runtime_Palette_256+23*4]
inc dx
mov si,bp
call Draw_Box_SSE
add bx,13
dec di
jnz W2O_01
;--- Mark visual mode and exit ---
or r8l,01000000b  ; R8L.6=Request copy by overlap
ret

;------- Window 2 (game main field) handler for close ---------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W2_Close:
; Reserved
W2C_00:
ret

;------- Window 2 (game main field) handler for work events ---;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;        Output parameters for Window(i)_Event routine         ;
;        if fast update request active, R8L.4=1 returned:      ;
;        BX  = Base X coordinate                               ;
;        DX  = Base Y coordinate                               ;
;        AX  = X size                                          ;
;        DI  = Y size                                          ;
;--------------------------------------------------------------;
W2_Event:
;--- Analyse current event for ENTER key, need re-blank game field ---
; Note this check before check Game_ON, because must work when game over
cmp rdx,EFI_KEY_ENTER       ; Include D63=0=keyboard
je W2_Open
;--- Check game flow active ---
cmp byte [_Game_ON+r15],0
je W2C_00                   ; Go skip if game not started
;--- Analyse current event for timer ---
cmp rdx,0FFFFFFFFFFFFFFFFh  ; Check Timer event
jne W2C_00                  ; Go skip if not a timer event
;--- Go to Field Engine ---
jmp Field_Engine

