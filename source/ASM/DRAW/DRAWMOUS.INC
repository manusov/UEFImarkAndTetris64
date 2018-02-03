;===========================================================================;
;                                                                           ;
;            DRAW MOUSE CURSOR, OPERATION IN THE TRANSIT BUFFER.            ;
;                                                                           ;
;===========================================================================;

;--- Show mouse cursor -----------------------------------;
; Update and save video transit buffer.                   ;
; Need call Screen_Copy for video memory update.          ;
; INPUT:   R15 = Global variables pool base address       ;
;          Global variables                               ;
;          Buffer_3 = Unpacked image of mouse cursor      ;
; OUTPUT:  Buffer_4 = Video storage updated               ;
;---------------------------------------------------------;
Show_Mouse:
clc                    ; CF=0(NC) means Show_Mouse
Show_Mouse_1:
push rax rbx rcx rdx rsi rdi
;--- Setup parameters ---
mov bx,[_Mouse_X+r15]  ; BX=Mouse cursor X position
mov dx,[_Mouse_Y+r15]  ; DX=Mouse cursor Y position
mov rdi,Buffer_3
mov cx,[rdi+08]        ; CX=Mouse cursor X size
mov si,[rdi+10]        ; SI=Mouse cursor Y size
;--- Check subroutine type ---
mov rax,Buffer_4       ; Safe addressing 64 bit
mov rax,[rax]          ; RAX=Buffer_4 linear
jc Show_Mouse_2
;--- Save graphics image before draw ---
call Get_Sprite_SSE
;--- Modify graphics image: draw mouse cursor ---
mov rax,[rdi+00]       ; RAX=Buffer_3 linear address
Show_Mouse_2:
call Draw_Sprite_SSE_Disjunct
;--- Exit ---
pop rdi rsi rdx rcx rbx rax
ret

;--- Hide mouse cursor -----------------------------------;
; Restore video transit buffer                            ;
; Need call Screen_Copy for video memory update           ;
; INPUT:   R15 = Global variables pool base address       ;
;          Global variables                               ;
;          Buffer_4 = Video storage                       ;
; OUTPUT:  Buffer_4 = Transit buffer updated              ;
;---------------------------------------------------------;
Hide_Mouse:
stc               ; CF=1(C) means Hide_Mouse
jmp Show_Mouse_1

