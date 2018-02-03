;===========================================================================;
;                                                                           ;
;           HANDLERS FOR GAME WINDOW 4, UP/RIGHT = DATE AND TIME.           ;
;                                                                           ;
;===========================================================================;

;------- Window 4 (date and time) handler for initialize ------;
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
W4_Init:
;--- Initialize window ---
mov ax,[_Video_Xsize+r15]  ; Get screen size: Xs
shr ax,1          ; Xs/2
sub ax,16
mov [rsi+12],ax   ; Set private size: X=Xs/2
add ax,32
mov [rsi+16],ax   ; Set private base: X=Xs/2
;--- Reserve high memory for window and exit ---
jmp Win_Local_00    ; Reserve for window sprite
; RET there

;------- Window 4 (date and time) handler for open ------------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W4_Open:
mov eax,[Runtime_Palette_256+24*4]
call Win_Local_01
;--- Entry point for dynamical revisual ---
W4O_Entry:
;--- Built date and time string at Buffer_5 ---
call Built_Date_Time_String
;--- Check and clear seconds update ---
mov al,0
xchg al,[_Chg_Seconds+r15]
cmp al,0
je W4O_0
;--- Write date and time string ---
;! mov bx,184 ; BX=String base X, from private
mov bx,[rsi+12]
sub bx,41*8
mov rsi,Buffer_5
mov dx,6                ; DX=String base Y, from private
mov ecx,80001008h       ; ECX.31=Mode, CH=Font Y, CL=Font X
mov eax,[Runtime_Palette_256+25*4]  ; Char color
mov ebp,[Runtime_Palette_256+24*4]  ; Background
call Draw_String        ; This write to private buffer
;--- Mark visual mode and exit ---
or r8l,01000000b        ; R8L.6=Request copy by overlap
W4O_0:
ret

;------- Window 4 (date and time) handler for close -----------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W4_Close:
; Reserved
ret

;------- Window 4 (date and time) handler for work events -----;
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
W4_Event:
jmp W4O_Entry

