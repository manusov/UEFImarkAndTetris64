;===========================================================================;
;                                                                           ;
; SUBROUTINE FOR OPERATIONS WITH GAME FIELD AS REACTION OF MOVE AND ROTATE. ;
;           X-MOVE AND ROTATE BY USER EVENTS, Y-MOVE BY TIMER.              ;
;                  THIS MODULE IS PART OF WIN_02.INC.                       ;
;                                                                           ;
;===========================================================================;

;--- Operations with game field, f(timer events, user events) ----;
; INPUT:   R15 = Global variables pool base address, flat 64      ;
;          RSI = Pointer to window descriptor structure, flat 64  ;
;          R8L = Requests bitmap, see WIN_01.INC comments         ;
;          Global variables                                       ;
;                                                                 ;
; OUTPUT:  R8L = Reqs. bitmap modified, see WIN_01.INC comments   ;
;          Parameters for Fast_Update_SSE if R8L.4=1              ;
;          Global variables                                       ;
;-----------------------------------------------------------------;
Field_Engine:
;--- Check for deferred request type 1: New figure ---
; Note 1. Deferred request need because figure must be movable
; some time quants, for horizontal move it after fall done.
; Note 2. Use Delta_Y for new figure delay
cmp byte [_Deferred_New+r15],0
je W2E_30      ; Go if no figures required
;--- Check time for deferred figure ---
cmp word [_Delta_Y+r15], DY_Per_New_Figure
jbe W2E_30
;--- Make figure ---
call New_Figure_Entry
jmp W2E_36     ; Exit with mark visual

;--- Check for deferred request type 2: Line fall down ---
W2E_30:
cmp word [_Deferred_Line+r15],0
je W2E_19                           ; Skip if no deferred reqs.
;--- Check time for line deferred fall down ---
; Use Delta_Y for line fall blinking visual delay
cmp word [_Delta_Y+r15], DY_Per_Line_Fall
jbe W2E_00
xor dx,dx
mov [_Delta_Y+r15],dx
xchg dx,[_Deferred_Line+r15]
;--- Make fall down ---
push rax rbx rcx rdx rsi
xor bx,bx            ; BX=Xbase, at private win.
mov cx,[rsi+12]
mov ax,dx
sub ax,13            ; AX=Ybase for source
mov si,ax            ; SI=Ysize
sub si,4             ; -4, otherwise copy up border
call Shift_Box_Down_SSE
pop rsi rdx rcx rbx rax
or r8l,01000000b    ; R8L.6=Revisual all if line fall
jmp W2E_15

;--- Check changes required or not ---
W2E_19:
movzx ax,byte [_Delta_Rotate+r15]
or ax,[_Delta_X+r15]
or ax,[_Delta_Y+r15]
jz W2E_00           ; Go skip if no changes

;---------- This branch for handling simple timer event: move only ---------;
;--- Pass1=Clear old figure ---
mov bl,00000000b
call Figure_Engine

;--- Get and handling results of keys window, figure transformations ---
;--- Save old state for conditional restore ---
mov ch,0                      ; CH=Flag for Y-move
mov cl,[_Current_Rotate+r15]
mov dx,[_Current_X+r15]
mov bx,[_Current_Y+r15]
;--- Check for rotation changed ---
mov al,0
xchg al,[_Delta_Rotate+r15]
add [_Current_Rotate+r15],al
and byte [_Current_Rotate+r15],3  ; Rotation
test al,al                        ; This prevent X+Y at single pass
jnz W2E_33                        ; Go if rotation changed
;--- Check for X changed ---
xor ax,ax
xchg ax,[_Delta_X+r15]
add [_Current_X+r15],ax           ; X-coordinate = F(user)
test ax,ax                        ; This prevent X+Y at single pass
jnz W2E_33                        ; Go if X changed
;--- Check for Y changed ---
cmp byte [_Deferred_New+r15],0
jne W2E_33                        ; Skip Y handling if wait new
xor ax,ax
xchg ax,[_Delta_Y+r15]
test ax,ax
setnz ch                          ; CH=Flag for Y-move
add [_Current_Y+r15],ax           ; Y-coordinate = F(time)
W2E_33:
;--- Pass2=Verify field for new position ---
push rbx
mov bl,00000001b
call Figure_Engine
pop rbx

;--- Handling results of verify ---
; Bit AL.0 = Cannot move left-right, but can continue fall
; Bit AL.1 = Cannot move down, stop figure falling
test al,al
jz W2E_10                         ; Go if all moves possible
mov [_Current_Rotate+r15],cl
mov [_Current_X+r15],dx
mov [_Current_Y+r15],bx
test al,00000010b
;--- Pass3=Draw new position or restore old position ---
W2E_10:
pushf
mov bl,00000010b
call Figure_Engine
popf
;--- Check figure fall stop ---
jz W2E_11                         ; Go if move down possible, continue
;--- Additional check to prevent stick after X-move ---
test ch,ch                        ; CH=1 if this is Y-move, oth. 00
jz W2E_11                         ; Go if move down possible, continue
		
;---------- Check and handling line fall, sequentally, for all lines -------;
W2E_15:
push rax rbx rcx rdx rsi rdi rbp
;--- Prepare parameters ---
mov rcx,13                        ; Figure Xsize, [63-16]=0
movzx rax,word [rsi+12]           ; Window Xsize
cqo
div rcx
xchg ebp,eax                      ; EBP=Figure elements per line
xor bx,bx                         ; Xbase=0
mov dx,[rsi+14]
sub dx,3                          ; Ybase=Ysize-3 (gray bottom)
mov eax,[Runtime_Palette_256+17*4]
;--- Detect line ready for fall down ---
W2E_18:
push rbx rcx rsi rbp
mov si,1                          ; Ysize=1
inc bx                            ; Xbase+1
mov cx,si                         ; Xsize=1
W2E_13:
call Verify_Box_SSE
jnc	W2E_17                        ; Go skip if line empty
add bx,13                         ; Xbase+Figure Xsize
dec ebp                           ; EBP=Figure elems. per line
jnz W2E_13                        ; Cycle for all X-elements
stc                               ; Mark line not empty
W2E_17:
pop rbp rsi rcx rbx
jnc W2E_16                        ; Go next line if line empty
;--- Set deferred fall down variable ---
mov [_Deferred_Line+r15],dx
jmp W2E_27
;--- Cycle for all lines ---
W2E_16:
sub dx,13
cmp dx,32                         ; Main field blank = 32 pix.
jae W2E_18                        ; If not go, CF=1(C)
jmp W2E_14                        ; Jump with CF=1(C)
;--- Re-draw figures line if it ready to fall, beep ---
W2E_27:
push rdx
mov rsi,[rsp+24]                  ; Get RSI image at W2E_15
xor bx,bx                         ; BX=Xbase
mov rcx,13                        ; RCX=Xsize=13, yet fixed
movzx rax,word [rsi+12]           ; RAX=Field Xsize
cqo
div rcx
xchg ebp,eax                      ; EBP=Number of line elements
mov si,13                         ; SI=Ysize=13, yet fixed
pop rdx
sub dx,si
inc dx                            ; DX=Ybase
W2E_28:		
mov eax,[Runtime_Palette_256+35*4]
call Draw_Box_SSE
push rax rbx rcx rdx rsi
inc ebx                            ; Xbase+2
inc ebx
inc edx                            ; Ybase+2
inc edx
mov cx,9                           ; Xsize=9
mov si,cx                          ; Ysize=9
mov eax,[Runtime_Palette_256+34*4]
call Draw_Box_SSE
pop rsi rdx rcx rbx rax
add bx,cx
dec ebp                            ; EBP=Number of line elements=boxes
jnz W2E_28
call Sound_Long_ON
;--- Modify Lines and Level, Level=2Lines ---
; Note if LINES modified, set save configuration request flag
or byte [_Sys_Bitmap_H+r15],80h    ; Save config request
inc word [_Game_Lines+r15]         ; Lines+1
test word [_Game_Lines+r15],1
jnz W2E_35
inc byte [_Game_Level+r15]         ; Level+1 if 2 lines done
;--- Set Line Fall mode ---
W2E_35:
clc                                ; CF=0(NC) means line fall
;--- Exit fall down procedure ---
W2E_14:
pop rbp rdi rsi rdx rcx rbx rax
;--- Skip other events if line fall ---
jnc W2E_29                         ; Go revisual all if line fall

;---------- Check and handling game over -----------------------------------;
W2E_12:		
;--- Check game over ---
push rax rbx rcx rdx rsi rdi rbp
movzx rax,word [rsi+12]    ; Xsize=FullXsize
mov rcx,13                 ; Figure Xsize
cqo
div rcx                    ; N = FullX / FigureX
xchg edi,eax               ; EDI = Elements per X
mov bx,1                   ; Xbase=1
mov dx,4+36                ; Ybase=4+36
mov eax,[Runtime_Palette_256+17*4]
mov cx,1                   ; Xsize=1
W2E_31:
push rsi
mov si,cx                  ; Ysize=1
call Verify_Box_SSE
pop rsi
jc W2E_32                  ; Go if game over
add bx,13                  ; Next figure line
dec edi                    ; EDI = Elements per X
jnz W2E_31
clc
jmp W2E_24                 ; Go skip if game not over
;--- Handling game over ---
W2E_32:
mov byte [_Game_ON+r15],0
xor bx,bx            ; BX=Xbase=0
xor dx,dx            ; DX=Ybase=0
mov cx,[rsi+12]      ; CX=Xsize, all field
mov si,22            ; SI=Ysize
mov eax,[Runtime_Palette_256+32*4]
call Draw_Box_SSE
mov rsi,Game_String_Over
mov bx,140           ; BX=String base X
mov dx,4             ; DX=String base Y
mov ecx,00001008h    ; ECX.31=Mode, CH=Font Y, CL=Font X
mov eax,[Runtime_Palette_256+33*4]
call Draw_String     ; This write to private buffer
call Sound_Over_ON
stc                  ; CF=1 here means GAME OVER
W2E_24:		
pop rbp rdi rsi rdx rcx rbx rax
jnc W2E_25           ; Go to next figure if game not over
;--- Return if Game Over ---
W2E_29:              ; Reserved place (here POP ECX at Tetris 16)
W2E_36:
or r8l,01000000b     ; R8L.6=Revisual all for Game Over
ret

;---------- Prepare for generating new figure ------------------------------;
;--- Generating new figure request ---
; Note deferred request instead just built figure need because figure must
; be movable some time quants, for horizontal move it after fall done.
W2E_25:
mov byte [_Deferred_New+r15],1

;--- Mark visual mode, prepare parameters for fast update and exit ---
W2E_11:               ; Reserved place (here POP ECX at Tetris 16)
test r8l,01000000b    ; R8L.6=Request for win. update
jnz W2E_00            ; Go if R8L.6=1
;--- This speed-optimized mode redraw only X,Y limited region ---
or r8l,00010000b      ; R8L.4=Request for fast update
mov bx,[rsi+12]       ; BX=This window Xsize
xor dx,dx
mov ax,bx
mov di,[rsi+14]          ; DI=This window Ysize
sub ax,128               ; AX=Limit Xmax
sub di,128               ; DI=Limit Ymax
shr bx,1                 ; X position = Xsize/2
add bx,[_Current_X+r15]  ; Add figure Xoffset
add dx,[_Current_Y+r15]  ; Add figure Yoffset
sub bx,128/2             ; Visual area Xbase = middle-1/2
jns W2E_20
xor bx,bx                ; Clear if limit Xmin
W2E_20:
sub dx,128/2             ; Visual area Xbase = middle-1/2
jns W2E_21
xor dx,dx                ; Clear if limit Ymin
W2E_21:
cmp bx,ax
jbe W2E_22
mov bx,ax                ; Set Xmax if limit Xmax
W2E_22:
cmp dx,di
jbe W2E_23
mov dx,di                ; Set Ymax if limit Ymax
W2E_23:
and bl,11111100b         ; Align X
mov ax,128               ; Xsize=fixed=128
mov di,ax                ; Ysize=fixed=128
;--- Exit ---
W2E_00:
ret

