;===========================================================================;
;                                                                           ;
;           HANDLERS FOR GAME WINDOW 5, UNDER MAIN = GAME STATUS.           ;
;                                                                           ;
;===========================================================================;

;---------- Window 5 (game status) handler for initialize -----;
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
W5_Init:
;--- Start initialize window, X-positioning ---
		mov	ax,[_Video_Xsize+r15]	; Get screen size: Xs
		sub	ax,[rsi+12]		; Subtract this window size
		shr	ax,1			; X/2
		add	ax,24			; Align lines single digit
		and	al,0FCh			; Align 4
		mov	[rsi+16],ax		; Set base: X=(Xs-Xw/2)ALIGN4
;--- Y-positioning ---

	;!	mov	ax,_Video_Ysize		; Get screen size: Ys
	;!	sub	ax,109
	;!	mov	ds:[si+14],ax		; Set private base: Y=Ys-109
	
		mov	ax,539
		cmp	word [_Video_Ysize+r15],768
		jae	W5I_0
		mov	ax,539-40		; AX=Game field Ysize
W5I_0:		mov	bx,[_Video_Ysize+r15]
		sub	bx,ax
		shr	bx,1
		and	bl,11111100b		; BX=Game field Ybase
		add	ax,bx
		add	ax,6
		mov	[rsi+18],ax		; Set new Ybase
	
;--- Reserve high memory for window and exit ---
		jmp	Win_Local_00	; Reserve for window sprite
					; RET there

;---------- Window 5 (game status) handler for open -----------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W5_Open:
;--- Boxes and its text strings ---
mov eax,[Runtime_Palette_256+26*4]
call Win_Local_01
mov rsi,Game_Status_Boxes
W5O_01:
lodsw                ; Use RSI
cmp ax,0FFFFh
je W5O_02            ; Go if list done
;--- Box ---
xchg bx,ax           ; BX=Xbase
lodsw
xchg cx,ax           ; CX=Xsize
xor dx,dx            ; DX=Ybase=0, from private
push rsi
mov si,17            ; SI=Ysize=17
mov eax,[Runtime_Palette_256+27*4]
call Draw_Box_SSE
pop rsi
;--- String ---
add bx,8             ; BX=String base X
mov dx,1             ; DX=String base Y
mov ecx,00001008h    ; ECX.31=Mode, CH=Font Y, CL=Font X
mov eax,[Runtime_Palette_256+28*4]  ; Char color
call Draw_String     ; This write to private buffer
;--- Next Box/String ---
jmp W5O_01
W5O_02:
;--- Entry point for dynamical revisual ---
W5O_Entry:
;--- Check and clear status update ---
mov al,0
xchg al,[_Chg_Status+r15]
cmp al,0
je W5O_00
;--- Prepare Run time string for visual ---
mov rdi,Buffer_1
mov rax,r13
sub rax,[_Start_Time+r15]
cqo               ; RDX=Dividend D[127-64]=0
mov rbx,1000000   ; EBX=Microseconds per second
div rbx           ; RAX=Seconds from start
mov rcx,3600      ; RAX/3600=Hours
cmp rax,99*3600
ja W5O_03         ; Go skip if >99 hours
cqo               ; RDX=Dividend D[127-64]=0
div rcx
mov bl,2          ; Template=2 chars
call Decimal_Print_32  ; Store ASCII number, hours
mov al,':'
stosb
xchg ax,dx
mov cx,60         ; Here EBX.31-08=0
cwd
div cx
call Decimal_Print_32  ; Store ASCII number, minutes
mov al,':'
stosb
xchg ax,dx
call Decimal_Print_32  ; Store ASCII number, seconds
W5O_03:
mov al,0
stosb
;--- Visual Run time ---
mov bx,80           ; X-base inside current sprite
mov dx,1            ; Y-base inside current sprite
mov ecx,80001008h   ; ECX.31=Mode, CH=Font Y, CL=Font X
mov eax,[Runtime_Palette_256+29*4]  ; Char color
mov ebp,[Runtime_Palette_256+26*4]  ; Char color
mov rsi,Buffer_1
call Draw_String  ; This write to private buffer
;--- Visual Number of figures, Number of lines, Game level  ---
movzx eax,word [_Game_Lines+r15]
push rax                            ; Push qword #1
mov eax,00040000h+0408
push rax                            ; Push qword #2
movzx eax,word [_Game_Figures+r15]
push rax                            ; Push qword #3
mov eax,00050000h+0312
push rax                            ; Push qword #4
movzx eax,byte [_Game_Level+r15]
push rax                            ; Push qword #5
mov eax,00030000h+0208
push rax                            ; Push qword #6
;--- Pop and visual ---
mov si,3
W5O_04:
pop rbx rax           ; Here Pop 3 qword+qword in the cycle
mov ecx,80001008h     ; ECX.31=Mode, CH=Font Y, CL=Font X
mov edi,[Runtime_Palette_256+29*4]  ; Char color
mov ebp,[Runtime_Palette_256+26*4]  ; Backgnd. 1
call Win_Local_02
dec si
jnz W5O_04
;--- Mark visual mode and exit ---
or r8l,01000000b  ; R8L.6=Request copy by overlap
W5O_00:
ret

;---------- Window 5 (game status) handler for close ----------;
; Input/Output parameters same as for initialization handler   ;
; Except Input  RDI = Base linear address of win. sprite       ;
;        Output RDI = Not used                                 ;
;--------------------------------------------------------------;
W5_Close:
; Reserved
ret

;---------- Window 5 (game status) handler for work events ----;
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
W5_Event:
jmp W5O_Entry



