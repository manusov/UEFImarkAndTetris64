;===========================================================================;
;                                                                           ;
;         HANDLERS FOR GAME WINDOW 1, ALL SCREEN = BACKGROUND FIELD.        ;
;                                                                           ;
;===========================================================================;

;---------- Window 1 (background) handler for initialize ------;
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
W1_Init:
;--- Initialize start time counter ---
mov [_Start_Time+r15],r13
;--- Initialize window ---
mov eax,[_Video_XYsize+r15]   ; Get scr. sizes: X,Y
mov [rsi+12],eax              ; Set bgn. sizes: X,Y
jmp Win_Local_00

;---------- Window 1 (background) handler for open ------------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W1_Open:
mov eax,[Runtime_Palette_256+16*4]
call Win_Local_01
;--- Mark visual mode and exit ---
or r8l,01000000b  ; R8L.6=Request copy by overlap
ret

;---------- Window 1 (background) handler for close -----------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W1_Close:
; Reserved
ret

;---------- Window 1 (background) handler for work events -----;
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
W1_Event:
; Reserved
ret
