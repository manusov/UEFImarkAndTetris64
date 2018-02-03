;===========================================================================;
;                                                                           ;
;         DRAW CHARS AND STRINGS, OPERATION IN THE TRANSIT BUFFER.          ;
;                                                                           ;
;===========================================================================;

;--- Draw string of chars in the transit buffer ----------;
; This version for 32 bits per pixel only.                ;
; Results visualized on the screen after call subroutine  ;
; Screen_Copy.                                            ;
; Internal structure optimized to minimize time for       ;
; parameters load and conversion, when draw chars         ;
; subroutine used in the cycle, use address instead       ;
; coordinates, for one calculation per string, not per    ;
; each char.                                              ;
; Mode 1 = Disjunction = Front color = EAX, Back color    ;
; not accepted, use existed without overwrite.            ;
; Mode 2 = Overlap = Front Color = EAX, Back color = EBP. ;
; No alignment required for X and Y, string of chars can  ;
; start at any pixel.                                     ;
;                                                         ;
; INPUT:   R15 = Pointer to global variables pool         ;
;          RSI    = Pointer to nul-terminated string      ;
;                   use flat 64-bit memory addressing     ;
;          EAX    = True color value for front color      ;
;          EBP    = True color value for back color       ;
;          BX     = X base for first char up left point   ;
;          DX     = Y base for first char up left point   ;
;          CL     = Font X size                           ;
;          CH     = Font Y size                           ;
;          ECX.31 = Mode select:                          ;
;                    0(NC)=Mode 1, Disjunction            ;
;                    1(C)=Mode 2, Overlap                 ;
;          ECX bits [30-16] = Reserved                    ;
;                                                         ;
; OUTPUT:  RSI = Updated by source string read            ;
;          MM0-MM7 corrupted                              ;
;---------------------------------------------------------;

Draw_String:
push rax rbx rcx rdx rdi rbp
cld
; YET SUPPORTED ONLY 8X16, SPECIAL FAST
; UNIVERSAL ROUTINE UNDER CONSTRUCTION
; NEED OPTIMIZING, PROBABLY TABLE CALLS
;--- Setup parameters ---
test ecx,ecx
js Draw_Str_12
call Local_Parms_4     ; MM0=Pattern=EAX:EAX
jmp Draw_Str_13
Draw_Str_12:
call Local_Parms_5     ; MM0-MM3=Patterns
Draw_Str_13:
mov rdi,[_Use_Mem_Base+r15]
movzx rbp,word [_Video_Xsize+r15]
movzx rax,dx
mul rbp
movzx rbx,bx
add rax,rbx
lea rdi,[rdi+rax*4]
;--- Cycle for write char ---
Draw_Str_1:
lodsb           ; Use RSI
cmp al,0
je Draw_Str_2   ; Go if end of string
movzx rbx,al
shl rbx,4       ; Here 16 bytes per char
; USE 32-BIT OFFSETS WITH 64-BIT REGISTERS = ADDRESS RELOCATION BUG
;! add rbx,Font_8x16
mov rax,Font_8x16
add rbx,rax
;!
test ecx,ecx
js Draw_Str_10
call Local_Disjunct_Char_SSE_8x16
jmp Draw_Str_11
Draw_Str_10:
call Local_Overlap_Char_SSE_8x16
Draw_Str_11:
add rdi,8*4     ; Here chars is 8 pixels per X
jmp Draw_Str_1  ; Cycle for next char of string
;--- Clear MMX state ---
Draw_Str_2:
emms             ; Better one per string, not per char
;--- Mark lines in the bitmap ---
push rsi
mov dx,[rsp+24]  ; Restore input DX=Ybase
mov si,16        ; SI=Ysize
call Local_Mark
pop rsi
;--- Restore registers and exit ---
Exit_Draw_Str:
pop rbp rdi rdx rcx rbx rax
ret


;---------- Local subroutines ----------------------------------------------;


;--- Draw char in the transit buffer, disjunct mode ------;
; Mode 1 - Disjunct, front color write, back color save.  ;
; Special case for 8x16 font, use MMX/SSE for fast write. ;
; Note MASKMOVQ use MMX registers but supported as SSE,   ;
; processor must support SSE for enable use MASKMOVQ.     ;
; This version for 32 bits per pixel only.                ;
; Results visualized on the screen after call subroutine  ;
; Screen_Copy.                                            ;
; Input parameters optimized to minimize time for         ;
; parameters load and conversion, when subroutine used    ;
; in the cycle, use address instead coordinates, for one  ;
; calculation per string, not per each char.              ;
; Front color = MM0, Back color not accepted, use existed ;
; without overwrite.                                      ;
; No alignment required for X and Y, char can start at    ;
; any pixel.                                              ;
; Assume DF=0 when this subroutine call.                  ;
;                                                         ;
; INPUT:   RDI = Pointer to transit video buffer, flat 64 ;
;          RBX = Pointer to font table element, flat 64   ;
;          MM0 = True color value for front color,        ;
;                replicated as 2 32-bit dwords            ;
;          RBP = Y-addend for vertical draw               ;
;                                                         ;
; OUTPUT:  None                                           ;
;          MM1 corrupted                                  ;
;          Caller must execute EMMS, don't use EMMS per   ;
;          each char, for minimize execution time.        ;
;---------------------------------------------------------;
Local_Disjunct_Char_SSE_8x16:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare parameters ---
lea rbp,[rbp*4-24]
mov rdx,Font_Mask_Table  ; Local special table
mov rcx,16
;--- Start draw char ---
Draw_Char_10:
movzx rax,byte [rbx]
;--- Start draw line, unrolled cycle for fast execution ---
;--- Pixels 7,6 ---
mov esi,eax         ; Also clear [63-32]
and esi,00C0h       ; Select 2 pixels
shr esi,3           ; Positioning 2 pixels
movq mm1,[rdx+rsi]  ; Load mask from table
maskmovq mm0,mm1    ; MM0=Color, MM1=Mask
add rdi,8           ; Destination advance
;--- Pixels 5,4 ---
mov esi,eax
and esi,0030h
shr esi,1
movq mm1,[rdx+rsi]
maskmovq mm0,mm1
add rdi,8
;--- Pixels 3,2 ---
mov esi,eax
and esi,000Ch
shl esi,1
movq mm1,[rdx+rsi]
maskmovq mm0,mm1
add rdi,8
;--- Pixels 1,0 ---
xchg esi,eax
and esi,0003h
shl esi,3
movq mm1,[rdx+rsi]
maskmovq mm0,mm1
add rdi,rbp
;--- End of unrolled cycle ---
inc rbx		
loop Draw_Char_10
;--- Exit ---
Exit_Draw_Char:
pop rsi
jmp Exit_Draw_Str  ; Other POPs there, after JMP

;--- Draw char in the transit buffer, overlap mode -------;
; Mode 2 - Overlap, front and back colors write.          ;
; Special case for 8x16 font, use MMX/SSE for fast write. ;
; This version for 32 bits per pixel only.                ;
; Results visualized on the screen after call subroutine  ;
; Screen_Copy.                                            ;
; Input parameters optimized to minimize time for         ;
; parameters load and conversion, when subroutine used    ;
; in the cycle, use address instead coordinates, for one  ;
; calculation per string, not per each char.              ;
; No alignment required for X and Y, char can start at    ;
; any pixel.                                              ;
; Assume DF=0 when this subroutine call.                  ;
;                                                         ;
; INPUT:   RDI = Pointer to transit video buffer, flat 64 ;
;          RBX = Pointer to font table element, flat 64   ;
;          RBP = Y-addend for vertical draw               ;
;          MM0 = True color pattern back:back             ;
;          MM1 = True color pattern front:back            ;
;          MM2 = True color pattern back:front            ;
;          MM3 = True color pattern front:front           ;
;                                                         ;
; OUTPUT:  None                                           ;
;          Caller must execute EMMS, don't use EMMS per   ;
;          each char, for minimize execution time.        ;
;---------------------------------------------------------;
Local_Overlap_Char_SSE_8x16:
push rax rbx rcx rdx rdi rbp rsi
;--- Prepare parameters ---
shl rbp,2
mov rcx,16
mov rsi,rbx
;--- Start draw char, unrolled cycle for fast write ---
Draw_Char_11:
lodsb         ; Use RSI
;--- Bits 7,6 ---
Bits_76:
shl al,1
jc Bits_76_1x
shl al,1
jc Bits_76_01
Bits_76_00:
movq [rdi],mm0
jmp Bits_54
Bits_76_01:
movq [rdi],mm1
jmp Bits_54
Bits_76_1x:
shl al,1
jc Bits_76_11
Bits_76_10:
movq [rdi],mm2
jmp Bits_54
Bits_76_11:
movq [rdi],mm3
;--- Bits 5,4 ---
Bits_54:
shl al,1
jc Bits_54_1x
shl al,1
jc Bits_54_01
Bits_54_00:
movq [rdi+08],mm0
jmp Bits_32
Bits_54_01:
movq [rdi+08],mm1
jmp Bits_32
Bits_54_1x:
shl al,1
jc Bits_54_11
Bits_54_10:
movq [rdi+08],mm2
jmp	Bits_32
Bits_54_11:
movq [rdi+08],mm3
;--- Bits 3,2 ---
Bits_32:
shl al,1
jc Bits_32_1x
shl al,1
jc Bits_32_01
Bits_32_00:
movq [rdi+16],mm0
jmp Bits_10
Bits_32_01:
movq [rdi+16],mm1
jmp Bits_10
Bits_32_1x:
shl al,1
jc Bits_32_11
Bits_32_10:
movq [rdi+16],mm2
jmp Bits_10
Bits_32_11:
movq [rdi+16],mm3
;--- Bits 1,0 ---
Bits_10:
shl al,1
jc Bits_10_1x
shl al,1
jc Bits_10_01
Bits_10_00:
movq [rdi+24],mm0
jmp Bits_Done
Bits_10_01:
movq [rdi+24],mm1
jmp Bits_Done
Bits_10_1x:
shl al,1
jc Bits_10_11
Bits_10_10:
movq [rdi+24],mm2
jmp	Bits_Done
Bits_10_11:
movq [rdi+24],mm3
;--- End of unrolled cycle, make cycle for 16 lines ---
Bits_Done:
add rdi,rbp
dec ecx
jnz Draw_Char_11
;--- Exit ---
pop rsi
jmp Exit_Draw_Str   ; Other POPs there, after JMP

;--- Prepare parameters ---------------------------------------;
; INPUT:   EAX = True color value for video memory write       ;
; OUTPUT:  MM0 = EAX replicated 2 times for 2 32-bit fields    ;
;--------------------------------------------------------------;
Local_Parms_4:
push rax          ; This for make frame
mov [rsp+04],eax  ; EAX=True color pattern
movq mm0,[rsp]
pop rax           ; This for remove frame
ret

;--- Prepare parameters ---------------------------------------;
; INPUT:   EAX = Front=1 true color value for video mem. write ;
;          EBP = Back=0 true color value for video mem. write  ;
; OUTPUT:  MM0 = 2 x 32 bit fields: EBP:EBP, colors=0,0        ;
;          MM1 = 2 x 32 bit fields: EAX:EBP, colors=0,1        ;
;          MM2 = 2 x 32 bit fields: EBP:EAX, colors=1,0        ;
;          MM3 = 2 x 32 bit fields: EAX:EAX, colors=1,1        ;
;--------------------------------------------------------------;
Local_Parms_5:
push rbp rax rbp   ; This for make frame
mov [rsp+04],eax
mov [rsp+12],ebp
movq mm0,[rsp+12]
movq mm1,[rsp+00]
movq mm2,[rsp+08]
movq mm3,[rsp+04]
pop rbp rax	rbp    ; This for remove frame
ret
