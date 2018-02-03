;===========================================================================;
;                                                                           ;
;             HANDLERS FOR GAME WINDOW 3, UP/LEFT = COPYRIGHT.              ;
;                                                                           ;
;===========================================================================;

;---------- Window 3 (copyright) handler for initialize -------;
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
W3_Init:
;--- Initialize window ---
mov ax,[_Video_Xsize+r15]  ; Get screen size: Xs
shr ax,1                   ; Xs/2
add ax,16
mov [rsi+12],ax            ; Set private size: X=Xs/2
;--- Reserve high memory for window ---
call Win_Local_00          ; Reserve for window sprite
;--- Reserve high memory for unpack logo ---
add rdi,16384              ; Reserve +16KB for unpack logo
;--- Exit ---
ret

;---------- Window 3 (copyright) handler for open -------------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W3_Open:
;--- Blank window sprite ---
mov eax,[Runtime_Palette_256+24*4]
call Win_Local_01
;--- Prepare parameters for import BMP ---
call Win_Local_00    ; Calc. offset from start
mov rax,rdi          ; EAX used later
;--- Generating sprite from BMP ---
mov rsi,IC_Book_Logo_BMP
clc                  ; CF=0, normal color
call Converter_BMP2Sprite
;--- Write sprite ---
mov bx,8             ; BX=Xbase
mov dx,1             ; DX=Ybase
mov cx,[rax+0]       ; CX=Xsize
mov si,[rax+2]       ; SI=Ysize
add rax,16           ; Skip prefix, 16 bytes
call Draw_Sprite_SSE_Overlap
;--- Write version string ---
mov rsi,String_Version
mov bx,40            ; BX=String base X
mov dx,6             ; DX=String base Y
mov ecx,00001008h	; ECX.31=Mode, CH=Font Y, CL=Font X
mov eax,[Runtime_Palette_256+25*4]
call Draw_String     ; This write to private buffer
;---Mark visual mode and exit ---
or r8l,01000000b     ; R8L.6=Request copy by overlap
ret

;---------- Window 3 (copyright) handler for close ------------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W3_Close:
; Reserved
ret

;---------- Window 3 (copyright) handler for work events ------;
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
W3_Event:
; Reserved
ret

