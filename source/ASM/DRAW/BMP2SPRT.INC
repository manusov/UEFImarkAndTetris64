;===========================================================================;
;                                                                           ;
;                   CONVERT DATA FROM BMP FORMAT TO SPRITE.                 ;
;                                                                           ;
;===========================================================================;

;--- Convert data from BMP file format to graphics sprite ----------;
; Support 4-bit color (16 colors), 8-bit color (256 colors) and     ;
; 24-bit true color (16M colors).                                   ;
; Caller must calculate available size of destination buffer        ;
; Result graphics sprite adjusted for current used video adapter    ;
; Xsize must be align 4                                             ;
; 16-byte prefix used: Word:XSize, Word:Ysize, 12 bytes reserved    ;
; Skip this prefix (add 16 to address) when call drawing proceudres ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;                                                                   ;
;          CF = Mode flag: 0(NC)=Maximum color as normal color      ;
;                          1(C)=Maximum color as transparent color  ;
;          RSI = Source pointer (flat 64-bit address)               ;
;          RDI = Destination pointer (flat 64-bit address)          ;
;          Global variables, video palette control                  ;
;                                                                   ;
; OUTPUT:  CF = Status flag: 0(NC)=Conversion OK                    ;
;                            1(C)=Error, format not supported       ;
;          RSI = Updated by source addressing (reserved)            ;
;          RDI = Updated by destination addressing (reserved)       ;
;          (!) Note. Current revision save input RSI, RDI,          ;
;          no output modified addresses.                            ;
;-------------------------------------------------------------------;

Converter_BMP2Sprite:
push rax rbx rcx rdx rsi rdi rbp r8
setc r8l
cld
mov rbp,Video_R_Width
;--- Check format ---
cmp word [rsi+000h],'BM'  ; Swapped for TASM->FASM
jne Error_Conv            ; Go if signature bad
cmp word [rsi+01Ah],1
jne Error_Conv            ; Go if N planes bad
cmp dword [rsi+01Eh],0
jne Error_Conv            ; Go if compressed
;--- Store and setup Xsize, Ysize ---
mov eax,[rsi+012h]      ; EAX = BMP Xsize
test eax,eax
jz Error_Conv           ; Go if Xsize=0
cmp eax,2048
ja Error_Conv           ; Go if Xsize>2048
test al,3
jnz Error_Conv          ; If Xsize misalign
stosw                   ; Store word: Xsize, use RDI
mov ecx,eax             ; RCX=Xsize
mov eax,[rsi+016h]      ; EAX = BMP Ysize
test eax,eax
jz Error_Conv           ; Go if Ysize=0
cmp eax,2048
ja Error_Conv           ; Go if Ysize>2048
stosw                   ; Store word: Ysize, use RDI
mov edx,eax             ; EDX=Ysize
;--- Blank reserved/alignment array ---
push rcx
mov rcx,12/4
xor eax,eax
rep stosd               ; Use EAX, RDI, ECX
;--- Setup address and Bits per pixel parameters ---
mov ax,[rsi+01Ch]       ; AX=Bits Per Pixel
mov ebx,[rsi+00Eh]      ; RBX=Header size, ext. to 64
lea rbx,[rsi+rbx+00Eh]  ; RBX=Palette offset
mov ecx,[rsi+00Ah]      ; This extend to 64 bit
add rsi,rcx             ; RSI=Bitmap offset
pop rcx

;--- Select handler = F(BPP) ---
; Parameters passed to handler:
; DS:ESI=Source pointer (set to bitmap)
; ES:EDI=Destination pointer
; DS:EBX=Palette pointer
; AX=Bits per pixel (BPP), not used by handler
; ECX=Xsize, EDX=Ysize (pixels), high words=0 for easy multiply
; RBP=Pointer to palette variables pool
;-------------------------------
cmp ax,4
je BMP_16             ; BPP=4, Colors=16
cmp ax,8
je BMP_256            ; BPP=8, Colors=256
cmp ax,24
je BMP_16M            ; BPP=24, Colors=16M
;--- Exit points ---
Error_Conv:
stc                   ; CF=1, error
jmp Exit_Conv
Good_Conv:
clc                   ; CF=0, no errors
Exit_Conv:
pop r8 rbp rdi rsi rdx rcx rbx rax
ret


;---------------------------------------------------------------------------
; BMP file layout for 16 colors, 4 bits per pixel
;---------------------------------------------------------------------------
; Offset  Length        Value        Comment
;---------------------------------------------------------------------------
; 0000h   DB            'BM'         BMP file label
; 0002h   DD            ?            File size, bytes
; 0006h   DW            0,0          Reserved
; 000Ah   DD            00000076h    Offset of bitmap
;--- header start ---
; 000Eh   DD            00000028h    Length of header, bytes
; 0012h   DD            ?            Picture width, pixels
; 0016h   DD            ?            Picture height, pixels
; 001Ah   DW            1            Color planes
; 001Ch   DW            4            Bits per pixel=4
; 001Eh   DD            0            Compression type, 0=without
; 0022h   DD            ?            Image size, bytes
; 0026h   DD            2834         Horisontal resolution
; 002Ah   DD            2834         Vertical resolution
; 002Eh   DD            0            Total colors, not used here
; 0032h   DD            0            Used colors, not used here
;--- header stop, palette start ---
; 0036h   DD 16 DUP     ?            Palette entries: [ B, G, R, Reserved ]
; 0076h   DB X*Y/2 DUP  ?            Pixels logical colors, from left down
;                                    corner, 1 byte per 2 pixels
;---------------------------------------------------------------------------
BMP_16:
push rcx rdx
call Local_BMP_2
shr eax,1
add rsi,rax
mov eax,ecx
shr eax,1
pop rdx rcx
;--- Start Y-cycle ---
BMP_16_1:
push rax rcx rsi
shr cx,1
;--- Start X-cycle ---
BMP_16_2:
lodsb           ; Use RSI
push rax		
shr al,4
call Local_BMP_1
pop rax
and al,0Fh
call Local_BMP_1
loop BMP_16_2
;--- End X-cycle ---
BMP_16_3:
pop rsi rcx rax
sub rsi,rax
dec dx
jnz BMP_16_1
;--- End X-cycle, exit ---
Good_Conv_1:
jmp Good_Conv

;---------------------------------------------------------------------------
; BMP file layout for 256 colors, 8 bits per pixel
;---------------------------------------------------------------------------
; Offset  Length        Value        Comment
;---------------------------------------------------------------------------
; 0000h   DB            'BM'         BMP file label
; 0002h   DD            ?            File size, bytes
; 0006h   DW            0,0          Reserved
; 000Ah   DD            00000436h    Offset of bitmap
;--- header start ---
; 000Eh   DD            00000028h    Length of header, bytes
; 0012h   DD            ?            Picture width, pixels
; 0016h   DD            ?            Picture height, pixels
; 001Ah   DW            1            Color planes
; 001Ch   DW            8            Bits per pixel=8
; 001Eh   DD            0            Compression type, 0=without
; 0022h   DD            ?            Image size, bytes
; 0026h   DD            2835         Horisontal resolution
; 002Ah   DD            2835         Vertical resolution
; 002Eh   DD            256          Total colors
; 0032h   DD            256          Used colors
;--- header stop, palette start ---
; 0036h   DD 256 DUP    ?            Palette entries: [ B, G, R, Reserved ]
;--- image start ---
; 0436h   DB X*Y DUP    ?            Pixels logical colors, from left down
;                                    corner, 1 byte per pixel
;---------------------------------------------------------------------------

BMP_256:	
; BUG! 
; NEED ADAPTATION FOR XSIZE NOT ALIGN 16. 
; SEE ADAPTATION METHOD AT BMP_16 LABEL.
push rdx
lea rax,[rdx-1]
mul rcx
add rsi,rax
pop rdx
;--- Start Y-cycle ---
BMP_256_1:
push rcx
;--- Start X-cycle ---
BMP_256_2:
lodsb              ; Use RSI
call Local_BMP_1
loop BMP_256_2
;--- End X-cycle ---
pop rcx
sub rsi,rcx
sub rsi,rcx
dec dx
jnz BMP_256_1
;--- End X-cycle, exit ---
Good_Conv_2:
jmp Good_Conv_1

;---------------------------------------------------------------------------
; BMP file layout for 16M colors, 24 bits per pixel
;---------------------------------------------------------------------------
; Offset  Length        Value        Comment
;---------------------------------------------------------------------------
; 0000h   DB            'BM'         BMP file label
; 0002h   DD            ?            File size, bytes
; 0006h   DW            0,0          Reserved
; 000Ah   DD            00000036h    Offset of bitmap
;--- header start ---
; 000Eh   DD            00000028h    Length of header, bytes
; 0012h   DD            ?            Picture width, pixels
; 0016h   DD            ?            Picture height, pixels
; 001Ah   DW            1            Color planes
; 001Ch   DW            18h          Bits per pixel=24
; 001Eh   DD            0            Compression type, 0=without
; 0022h   DD            ?            Image size, bytes
; 0026h   DD            2834         Horisontal resolution
; 002Ah   DD            2834         Vertical resolution
; 002Eh   DD            0            Total colors, not used here
; 0032h   DD            0            Used colors, not used here
;--- header stop, image start (palette not used for true color) ---
; 0036h   DB X*Y*3 DUP  ?            Pixels logical colors, from left down
;                                    corner, 3 bytes per pixel [ B, G, R ]
;---------------------------------------------------------------------------

BMP_16M:	
; BUG! 
; NEED ADAPTATION FOR XSIZE NOT ALIGN 16. 
; SEE ADAPTATION METHOD AT BMP_16 LABEL.
push rdx
lea rax,[rdx-1]
mul rcx
imul rax,rax,3
lea rbx,[rsi+rax]
pop rdx
;--- Start Y-cycle ---
BMP_16M_1:
mov al,0	; Emulate logical color 0 for true color
push rcx
;--- Start X-cycle ---
BMP_16M_2:
call Local_BMP_1
add rbx,3
loop BMP_16M_2
;--- End X-cycle ---
pop rcx
imul rax,rcx,6
sub rbx,rax
dec dx
jnz BMP_16M_1
;--- End X-cycle, exit ---
jmp Good_Conv_2

;---------- Local subroutines ----------------------------------------------;

;--- Convert 24-bit color from BMP to SVGA controller-specific value ----;
; INPUT:   AL     = Logical color number                                 ;
;          RBX = Pointer to palette with 32-bit entries, flat 64-bit     ;
;          RDI = Destination pointer, flat 64-bit                        ;
;          RBP = Pointer to array of Width:Position  for R,G,B           ;
;          R8L = Mode flag: 0 = Maximum color as normal color            ;
;                           1 = Maximum color as transparent color       ;
; OUTPUT:  RDI    = Ponter modified                                      ;
;          Store color data at RDI                                       ;
;------------------------------------------------------------------------;
Local_BMP_1:
push rax rbx rdx rsi rbp
movzx rax,al
lea rbx,[rbx+rax*4]
;--- Generate EAX=True color ---
cqo                 ; RDX=0
mov rsi,2
Loc_BMP_1:
mov al,[rbx+rsi]
call Local_Palette_1
dec esi
jns Loc_BMP_1
mov eax,edx
;--- Check for transparency color: 00FFFFFFh -> FFFFFFFFh ---
test r8l,r8l
jz Loc_BMP_2        ; Go skip if normal mode
cmp eax,000FFFFFFh
jne Loc_BMP_2       ; Go skip if not my color
mov eax,0FFFFFFFFh
;--- Store true color ---
Loc_BMP_2:
stosd               ; Use RDI
;--- Exit ---
pop rbp rsi rdx rbx rax
ret

;--- Calculate start offset in the BMP file -----------------------------;
; Note. Special support for address alignment if Xsize not aligned by 4  ;
; INPUT:   RCX = Xsize, possible unaligned                               ;
;          RDX = Ysize                                                   ;
; OUTPUT:  RAX = Addend for offset first element                         ;
;          RCX = Addend for offset next elements in the sequence         ;
;------------------------------------------------------------------------;
Local_BMP_2:
lea rax,[rdx-1]
test cl,00001111b
jz Loc_BMP_3
and cl,11110000b
add rcx,16
Loc_BMP_3:
mul rcx
ret


