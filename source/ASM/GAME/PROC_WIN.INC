;===========================================================================;
;                                                                           ;
;              SUBROUTINES, CALLED FROM GAME WINDOWS OBJECTS.               ;
;                                                                           ;
;===========================================================================;

;---------- Allocate memory for window graphics sprite ----------;
; Formula: Size = Xsize*Ysize*4Bytes                             ;
; INPUT:   RSI = Pointer to window descriptor, flat 64-bit       ;
;          WORD [RSI+12] = Xsize                                 ;
;          WORD [RSI+14] = Ysize                                 ;
;          RDI = Free memory base                                ;
; OUTPUT:  RDI = Updated value, add this window size             ;
;          Corrupted: RAX, RDX                                   ;
;----------------------------------------------------------------;
Win_Local_00:
movzx rax,word [rsi+12]  ; X-size
movzx rdx,word [rsi+14]  ; Y-size
mul rdx                  ; X*Y
lea rdi,[rdi+rax*4]      ; *4 because 4 bytes per pix.
ret

;---------- Fill window graphics sprite with color --------------;
; Formula: Size = Xsize*Ysize*4Bytes                             ;
; INPUT:   R15 = Global variables pool base address              ;
;          RSI = Pointer to window descriptor, flat 64-bit       ;
;          WORD [RSI+12] = Xsize                                 ;
;          WORD [RSI+14] = Ysize                                 ;
;          WORD [RSI+16] = Xbase                                 ;
;          WORD [RSI+18] = Ybase                                 ;
;          EAX   = Color for fill sprite                         ;
; OUTPUT:  None                                                  ;
;----------------------------------------------------------------;
Win_Local_01:
push	rbx rcx rdx rsi
xor	bx,bx         ; X-base inside current sprite
xor	dx,dx         ; Y-base inside current sprite
mov	cx,[rsi+12]   ; X-size of current sprite
mov	si,[rsi+14]   ; Y-size of current sprite
call	Draw_Box_SSE
WL01_Exit:
pop rsi rdx rcx rbx
ret

;--- Convert decimal num. to ASCII and print to private buf. ----;
; Use Buffer_1, this buffer also used during start benchmarks    ;
; INPUT:   R15 = Global variables pool base address              ;
;          EAX    = Number                                       ;
;          EDI    = True color value for front color             ;
;          EBP    = True color value for back color              ;
;          BX     = X base for first char up left point          ;
;          DX     = Y base for first char up left point          ;
;          CL     = Font X size                                  ;
;          CH     = Font Y size                                  ;
;          ECX.31 = Mode select:                                 ;
;                    0(NC)=Mode 1, Disjunction                   ;
;                    1(C)=Mode 2, Overlap                        ;
;          ECX bits [30-16] = Reserved                           ;
;          EBX bits [31-16] = Template size for visual blank     ;
;                                                                ;
; OUTPUT:  None                                                  ;
;----------------------------------------------------------------;
Win_Local_02:
push rbx rcx rdx rsi
;--- Write number with pre-clear buffer ---
push rbx rcx rdi
mov rdi,Buffer_1       ; RDI=Pointer
ror ebx,16
movzx rbx,bx
mov cx,bx              ; CX=5, 5 chars, max N=65535
mov byte [rdi+rbx],0   ; Write string terminator
mov bl,0               ; BL=0 means no template
call Blank_Dec_Print_32
;--- Write terminator byte ---
; IF USE THIS METHOD, OLD DIGITS NOT CLEARED
;!  mov al,0
;!  stosb
pop rdi rcx rbx
;--- Draw string in the private buffer ---
mov rsi,Buffer_1
xchg eax,edi      ; EAX=Color
call Draw_String  ; This write to private buffer
;--- Exit ---
jmp WL01_Exit



