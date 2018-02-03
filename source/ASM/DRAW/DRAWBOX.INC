;===========================================================================;
;                                                                           ;
;                DRAW BOX, OPERATION IN THE TRANSIT BUFFER.                 ;
;                                                                           ;
;===========================================================================;

;--- Draw box in the transit buffer ----------------------;
; This version for 32 bits per pixel only.                ;
; Results visualized on the screen after call subroutine  ;
; Screen_Copy.                                            ;
;                                                         ;
; INPUT:   R15 = Pointer to global variables pool         ;
;          EAX = True color value for video memory write  ;
;          BX = Base X coordinate                         ;
;          DX = Base Y coordinate                         ;
;          CX = X size                                    ;
;          SI = Y size                                    ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  None                                           ;
;          SSE register XMM0 corrupted                    ;
;---------------------------------------------------------;
Draw_Box_SSE:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare XMM0=True color pattern ---
call Local_Parms_2
;--- Mark modified lines for show ---
call Local_Mark
;--- Setup DF, RBP, ECX ---
;--- Calculate RBX=Address=F(coordinates) ---
call Local_Parms_3
;--- Save size at units=pixels=4 bytes ---
mov edx,ecx	
;--- Begin Y-cycle, prepare for X-cycle ---
Box_Y_SSE:
mov rdi,rbx         ; RDI=Base address
mov ecx,edx         ; ECX=Size, pixels=dwords
;--- First X-cycle: left misaligned part ---	; RCX[63-32] also cleared
Box_Left_SSE:
test edi,00001111b
jz Box_Align_SSE
stosd               ; Use RDI
loop Box_Left_SSE   ; This as LOOPQ, use RCX
;--- Second X-cycle: main aligned part ---
Box_Align_SSE:
push rcx
shr ecx,7-2       ; RCX=Size, 128-byte blocks
jz End_X_SSE
Box_X_SSE:
movaps [rdi+000h],xmm0
movaps [rdi+010h],xmm0
movaps [rdi+020h],xmm0
movaps [rdi+030h],xmm0
movaps [rdi+040h],xmm0
movaps [rdi+050h],xmm0
movaps [rdi+060h],xmm0
movaps [rdi+070h],xmm0
add rdi,128
loop Box_X_SSE		; This as LOOPQ, use RCX
End_X_SSE:
pop rcx
;--- Third X-cycle: right misaligned part ---
and ecx,00011111b
rep stosd            ; Use RDI
;--- End of X-cycles ---
add rbx,rbp
dec si
jnz Box_Y_SSE
;--- End Y-cycle, exit ---
jmp Exit_Draw_Char

;--- Verify box color integrity in the transit buffer ----;
; Used for enable or disable tetris figures movement and  ;
; for verify tetris line integrity and enable fall down.  ;
; This version for 32 bits per pixel only.                ;
;                                                         ;
; INPUT:   R15 = Pointer to global variables pool         ;
;          EAX = True color value for video mem. compare  ;
;          BX = Base X coordinate                         ;
;          DX = Base Y coordinate                         ;
;          CX = X size                                    ;
;          SI = Y size                                    ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  CF = Verify result flag:                       ;
;               0(NC) = All pixels color equal input EAX  ;
;               1(C)  = Color mismatches detected         ;
;          SSE registers XMM0, XMM1 corrupted             ;
;---------------------------------------------------------;
Verify_Box_SSE:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare XMM0=True color pattern ---
call Local_Parms_2
;--- Clear comparision scratch pad at XMM1, save pattern in XMM2 ---
xorps xmm1,xmm1
movaps xmm2,xmm0
;--- Setup DF, RBP, ECX ---
;--- Calculate RBX=Address=F(coordinates) ---
call Local_Parms_3
;--- Save size at units=pixels=4 bytes ---
mov edx,ecx
;--- Begin Y-cycle, prepare for X-cycle ---	; EAX and XMM0 = Patterns
VBox_Y_SSE:
mov rdi,rbx           ; RDI=Base address
mov ecx,edx           ; RCX=Size, pixels=dwords
;--- First X-cycle: left misaligned part ---	; RCX[63-32] also cleared
VBox_Left_SSE:
test edi,00001111b
jz VBox_Align_SSE
scasd                 ; Use RDI
jne V_Mismatch
loop VBox_Left_SSE    ; This as LOOPQ, use RCX
;--- Second X-cycle: main aligned part ---
; Unrolled cycle: non-optimal for size but optimal for speed
VBox_Align_SSE:
push rcx
shr ecx,7-2			; RCX=Size, 128-byte blocks
jz VEnd_X_SSE
VBox_X_SSE:
movaps xmm0,xmm2
xorps xmm0,[rdi+000h]
orps xmm1,xmm0
movaps xmm0,xmm2
xorps xmm0,[rdi+010h]
orps xmm1,xmm0
movaps xmm0,xmm2
xorps xmm0,[rdi+020h]
orps xmm1,xmm0
movaps xmm0,xmm2
xorps xmm0,[rdi+030h]
orps xmm1,xmm0
movaps xmm0,xmm2
xorps xmm0,[rdi+040h]
orps xmm1,xmm0
movaps xmm0,xmm2
xorps xmm0,[rdi+050h]
orps xmm1,xmm0
movaps xmm0,xmm2
xorps xmm0,[rdi+060h]
orps xmm1,xmm0
movaps xmm0,xmm2
xorps xmm0,[rdi+070h]
orps xmm1,xmm0
;--- Check scratch pad ---
push rax rax
movups [rsp],xmm1
mov rax,[rsp+00]
or rax,[rsp+08]
pop rax rax
jnz V_Mismatch_1
;--- Cycle ---
add rdi,128
loop VBox_X_SSE         ; This as LOOPQ, use RCX
VEnd_X_SSE:
pop rcx
;--- Third X-cycle: right misaligned part ---
and ecx,00011111b
rep scasd               ; Use RDI
jne V_Mismatch
;--- End of X-cycles ---
add rbx,rbp
dec si
jnz VBox_Y_SSE
;--- End Y-cycle, exit points ---
clc
Draw_Exit_1:
jmp	V_Exit_1
V_Mismatch_1:
pop	rcx
V_Mismatch:
stc
V_Exit_1:
jmp	Exit_Draw_Char

;--- Blank transit buffer --------------------------------;
; Draw black box at full screen                           ;
; INPUT:   R15 = Pointer to global variables pool         ;
;          Global variables                               ;
; OUTPUT:  None                                           ;
;---------------------------------------------------------;
Blank_Transit:
push rax rbx rcx rdx rdi rbp rsi
;--- Blank ---
mov eax,dword [Runtime_Palette_256+15*4]
xor ebx,ebx
xor edx,edx
mov cx,[_Video_Xsize+r15]
mov si,[_Video_Ysize+r15]
call Draw_Box_SSE
;--- Draw frame ---
mov eax,dword [Runtime_Palette_256+12*4]
mov cx,[_Video_Xsize+r15]
mov si,1               ; Y-size
call Draw_Box_SSE      ; Horizontal, Up
mov dx,[_Video_Ysize+r15]
dec edx
call Draw_Box_SSE      ; Horizontal, Down
xor edx,edx
mov cx,si
mov si,[_Video_Ysize+r15]
call Draw_Box_SSE      ; Vertical, Left
mov bx,[_Video_Xsize+r15]
dec ebx
call Draw_Box_SSE      ; Vertical, Right
;--- Exit ---
jmp Exit_Draw_Char


;---------- Local subroutines ----------------------------------------------;


;--- Prepare parameters ----------------------------------;
; INPUT:   R15 = Pointer to global variables pool         ;
;          BX = Base X coordinate                         ;
;          DX = Base Y coordinate                         ;
;          CX = X size                                    ;
;          Global variables                               ;
;                                                         ;
; OUTPUT:  DF  = 0, for strings auto-increment            ;
;          RBX = Object base addr. in the transit buffer  ;
;          RCX = Bits [63-16] cleared, bits [15-0] saved  ;
;          RBP = Raster string length, bytes              ;
;          RAX saved                                      ;
;---------------------------------------------------------;
Local_Parms_3:
push rax
;--- Calculate address=F(coordinates) ---
movzx rbx,bx                  ; EBX=Base X coordinate
shl ebx,2                     ; *4 because 4 bytes per pixel
movzx rax,word [_Video_Xsize+r15]
shl eax,2
movzx rdx,dx
mul rdx
add rbx,rax                   ; Add base Y coordinate
add rbx,[_Use_Mem_Base+r15]   ; Add transit buffer base
;--- Conver size from pixels=4 bytes to 128-byte blocks ---
movzx rcx,cx
;--- Setup DF=0 ---
cld
;--- Setup RBP for Y-cycle ---
movzx rbp,word [_Video_Xsize+r15]
shl ebp,2
;--- Exit ---
pop rax
ret

;--- Mark modified lines for show by Screen_Copy ---------;
; INPUT:   DX = Base Y coordinate                         ;
;          SI = Y size                                    ;
; OUTPUT:  Update bitmap: Lines_Show                      ;
;---------------------------------------------------------;
Local_Mark:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare destination address ---
mov rdi,Lines_Show   ; RDI=Bitmap
movzx rax,dx         ; RAX=First updated line
shr eax,3            ; Convert to byte address
and al,11111100b     ; Convert to dword address
add rdi,rax          ; RDI=First updated byte
;--- Left unaligned, prepare mask=f(size) ---
movzx rcx,si
cmp cx,32
jbe Loc_Mark_4
mov cx,32
;--- Calculate how ones hit to current dword ---
Loc_Mark_4:
mov bl,32
mov bh,dl
and bh,00011111b
sub bl,bh
cmp cl,bl
jbe Loc_Mark_3
mov cl,bl
;--- Save mask size, generate mask ---
Loc_Mark_3:
mov bp,cx             ; BP=Left unaligned size
mov eax,0FFFFFFFFh    ; EAX used here and later
xor ebx,ebx
shld ebx,eax,cl
;--- Left unaligned, prepare position=f(base) ---
mov cl,dl
and cl,00011111b
jz Loc_Mark_1        ; Exit if no left unaligned
shl ebx,cl
;--- Left unaligned, mark ---
or [rdi],ebx
;--- Subtract left unaligned ---
sub si,bp
jbe Loc_Mark_2       ; Exit if mark done
add rdi,4
;--- Central aligned ---
Loc_Mark_1:
movzx rcx,si
shr ecx,5
rep stosd            ; Here EAX=FFFFFFFFh, use RDI
;--- Right unaligned ---
mov cx,si
and cl,00011111b
jz Loc_Mark_2        ; Exit if no right unaligned
xor ebx,ebx
shld ebx,eax,cl
or [rdi],ebx
;--- Exit ---
Loc_Mark_2:
jmp Exit_Draw_Char

