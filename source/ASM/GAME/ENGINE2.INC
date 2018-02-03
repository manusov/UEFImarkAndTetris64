;===========================================================================;
;                                                                           ;
;      SUBROUTINE FOR OPERATIONS WITH FIGURE AS SET OF SQUARE BOXES.        ;
;      SQUARE BOXES LOCATION BITMAP DECLARED BY FIGURES_MASKS DATA.         ;
;                  THIS MODULE IS PART OF WIN_02.INC.                       ;
;                                                                           ;
;===========================================================================;

;--- Clear old, Verify field, Draw new, Restore old operations ---;
; INPUT:   R15 = Global variables pool base address, flat 64      ;
;          RSI = Pointer to window descriptor structure, flat 64  ;
;          BL      = Mode flags:                                  ;
;          BL.1    = Draw mode flag: 0=Clear, 1=Draw              ;
;          BL.0    = Verify mode flag: 0=Clear, 1=Verify          ;
;          Global variables                                       ;
;                                                                 ;
; OUTPUT:  AL = Status, valid if verify mode, when input EBX.30=1 ;
;          Bit 0 = Cannot move left-right, but can continue fall  ;
;          Bit 1 = Cannot move down, stop figure falling          ;
;          Bits[7-2]=Reserved                                     ;
;-----------------------------------------------------------------;
Figure_Engine:
mov al,0       ; Blank status
push r8 r9 rax rbx rcx rdx rsi rdi rbp
mov r9,rsp
ror ebx,2      ; D0->D30, D1->D31
;--- Initializing figure type and rotation position ---
push rbx
mov al,[_Current_Figure+r15]
and rax,3
imul rax,rax,F_Masks_Size
mov bl,[_Current_Rotate+r15]
and rbx,3
imul rbx,rbx,F_Mask_Size_Y
add rax,rbx
pop rbx
;--- Initializing parameters for draw figure as some sprites ---
push rax
movzx rax,word [rsi+12]  ; Here need input RSI
mov rcx,13               ; RCX=Xsize, yet fixed=13
mov si,13                ; SI=Ysize, yet fixed=13
cqo
div rcx
shr eax,1                ; Centering
dec eax                  ; Adjust centering
mul cx
xchg bx,ax
add bx,[_Current_X+r15]
mov dx,4                 ; DX=Ybase
add dx,[_Current_Y+r15]
mov rbp,Figures_Masks
pop rax
add rbp,rax
;--- Cycle init for Y-draw figure as some sprites ---
mov ax,F_Mask_Size_Y    ; EAX.[31-16]=Vertical size
shl eax,16
;--- Cycle init for X-draw figure as some sprites ---
W2E_02:
mov ax,F_Mask_Size_X   ; AX=Horizontal size
mov r8l,[rbp]          ; R8L=Byte of figure bitmap
;--- Pass1=Clear old, Pass2=Verify, Pass3=Draw new or Restore old ---
push rbx
W2E_01:
push rax
shl r8l,1       ; Move bitmap bit to CF flag
jnc W2E_08
test ebx,ebx
js W2E_04       ; Go if EBX.31=1, draw new
mov eax,[Runtime_Palette_256+17*4]
bt ebx,30
jc W2E_05		; Go if EBX.30=1, verify new
;--- Pass 1 = clear old figure ---
push rax rbx rcx rdi rbp
mov di,cx
inc bx              ; Xbase+1
dec cx              ; Xsize-2, draw central field
dec cx
call Draw_Box_SSE
mov cx,1            ; Xsize=1
dec bx
jz W2E_09           ; Skip if left line, no border
mov eax,[Runtime_Palette_256+23*4]		
W2E_09:
call Draw_Box_SSE   ; Draw left line
mov eax,[Runtime_Palette_256+23*4]
mov rbp,[r9+16]     ; RBP=Pointer to win. desc.
mov bp,[rbp+12]     ; Win. Xsize
dec bp
lea ebx,[ebx+edi-1]
cmp bx,bp
jb W2E_26
mov eax,[Runtime_Palette_256+17*4]
W2E_26:
call Draw_Box_SSE   ; Draw right line
pop rbp rdi rcx rbx rax
W2E_08:
jmp W2E_03
;--- Pass 2 = verify new position for overlaps ---
W2E_05:
test bx,bx
js W2E_06         ; Go if left limit detected
push rbx rsi
add bx,cx         ; BX+CX = Xbase+Xsize
mov rsi,[r9+16]   ; RSI=Pointer to win. desc.
cmp bx,[rsi+12]   ; Figure Xsize
pop rsi rbx
ja W2E_06         ; Go if right limit detected
push rdx rsi
add dx,si         ; DX+SI = Ybase+Ysize
inc dx
inc dx
mov rsi,[r9+16]   ; RSI=Pointer to win. desc.
cmp dx,[rsi+14]
pop rsi rdx
ja W2E_07         ; Go if down limit detected
;--- Verify figure position ---
; EAX = True color value for video mem. compare
; BX = Base X coordinate
; DX = Base Y coordinate
; CX = X size
; SI = Y size
;--- Check for move down not possible ---
push rbx rcx rdx rsi
add dx,si       ; Ybase+Ysize
dec dx          ; Set to figure bottom line
mov si,1        ; Ysize=1, one bottom line
inc bx          ; Skip vertical line pixel
mov cx,si       ; Horizontal size=1
call Verify_Box_SSE
pop rsi rdx rcx rbx
jc W2E_07       ; Go if move down not possib.
;--- Check for other movements not possible ---
push rbx rcx
inc bx                       ; Skip vertical line pixel
mov cx,1                     ; Horizontal size=1
call Verify_Box_SSE
pop rcx rbx
jnc W2E_03                   ; Go if other move possible
;--- Handling results ---
W2E_06:
or byte [r9+48],00000001b    ; D0=Xflag
jmp W2E_34
W2E_07:
or byte [r9+48],00000010b    ; D1=Yflag
W2E_34:
jmp W2E_03
;--- Pass 3 = draw new position or restore old ---
W2E_04:
mov eax,[Runtime_Palette_256+34*4]
call Draw_Box_SSE
push rbx rcx rdx rsi
inc bx
inc bx
inc dx
inc dx
mov cx,9
mov si,cx
mov eax,[Runtime_Palette_256+35*4]
call Draw_Box_SSE
pop rsi rdx rcx rbx
;--- Passes 1 or 2 done ---
W2E_03:
pop	rax
;--- Cycle stop for X-draw figure as some sprites ---
add bx,13             ; Add X-size, go right
dec ax                ; Decrement X-counter
jnz W2E_01
pop rbx
;--- Cycle stop for Y-draw figure as some sprites ---
add dx,13             ; Add Y-size, go down
inc rbp               ; Select next byte of bitmap
sub eax,000010000h    ; Decrement Y-counter
test eax,0FFFF0000h
jnz W2E_02
;--- Restore registers and return ---
pop rbp rdi rsi rdx rcx rbx rax r9 r8
ret

