;===========================================================================;
;                                                                           ;
;                SUBROUTINES FOR BUILT TEXT STRINGS SUPPORT.                ;
;                                                                           ;
;===========================================================================;

;--- Select string from strings sequence and copy it to destination -----;
; ASCII edition                                                          ;
; INPUT:   AL  = Selector, number of strings to skip                     ;
;          AH  = Selector Limit, skip move if AL>AH                      ;
;          RSI = Source Buffer, pointer to nul-terminated strings pool   ;
;          RDI = Pointer to destination string                           ;
;          Memory pointers is flat 64-bit, segment base=0                ;
;                                                                        ;
; OUTPUT:  RSI = Modified by read source string                          ;
;          RDI = Modified by write destination string                    ;
;------------------------------------------------------------------------;
String_Select_Copy:
call String_Selector
; No RET, continue in the next subroutine

;--- Copy String from source to destination -----------------------;
; Edition with no converters, simple copy, stop if byte=0          ;
; INPUT:   RSI = Pointer to nul-terminated source string (flat)    ;
;          RDI = Pointer destination string (flat)                 ;
;          Source string is ASCII (1 byte per char)                ;
;          Destination string is ASCII (1 byte per char)           ;
; OUTPUT:  RSI = Modified by read source string                    ;
;          RDI = Modified by write destination string              ;
;------------------------------------------------------------------;
String_Copy_Simple:
push rax
cld
String_Copy_4:
lodsb
cmp al,0
je Exit_SC
stosb
jmp String_Copy_4
Exit_SC:
pop rax
ret

;--- Select string from strings sequence --------------------------------;
; INPUT:   AL  = Selector, number of strings to skip                     ;
;          AH  = Selector Limit, skip move if AL>AH                      ;
;          RSI = Source Buffer, pointer to nul-terminated strings pool   ;
;          Memory pointers is flat 64-bit, segment base=0                ;
;                                                                        ;
; OUTPUT:  RSI = Pointer to selected string                              ;
;------------------------------------------------------------------------;
String_Selector:
cld
cmp al,ah
ja Exit_SL_2
push rax rcx
movzx rcx,al
jrcxz Zero_SL
Skip_SL:
lodsb
cmp al,0
jne Skip_SL           ; CX-1 only if 0 and JNE not executed
loop Skip_SL          ; in the 64-bit mode it is LOOPQ (RCX)
Zero_SL:
pop rcx rax
Exit_SL_2:
ret

;--- Copy String from source to destination -----------------------;
; Edition with ASCII to UNICODE converter                          ;
; INPUT:   RSI = Pointer to nul-terminated source string (flat)    ;
;          RDI = Pointer destination string (flat)                 ;
;          Source string is ASCII (1 byte per char)                ;
;          Destination string is UNICODE (2 bytes per char)        ;
; OUTPUT:  RSI = Modified by read source string                    ;
;          RDI = Modified by write destination string              ;
;------------------------------------------------------------------;
String_Copy_Converted:
cld
mov	ah,0                ; Clear high byte for UNICODE
String_Copy_1:
lodsb                   ; Load 8-bit ASCII char
cmp al,0
je DP32_Ret
cmp al,0Ah
je String_Copy_3        ; Enable LINE FEED
cmp al,0Dh
je String_Copy_3        ; Enable CARRIAGE RETURN
cmp al,' '
jb String_Copy_2
cmp al,'}'
jbe String_Copy_3
String_Copy_2:
mov al,'.'
String_Copy_3:
stosw                   ; Store 16-bit UNICODE char
jmp String_Copy_1

;--- Print 32-bit Decimal Number with pre-blank field ---------;
; INPUT:   EAX = Number value                                  ;
;          BL  = Template size, chars. 0=No template           ;
;          RDI = Destination Pointer                           ;
;          CX  = Number of pre-blanked chars                   ;
; OUTPUT:  RDI  = Modify                                       ;
;--------------------------------------------------------------;
Blank_Dec_Print_32:
push rax rcx rdi
movzx rcx,cx
mov al,' '
rep stosb               ; Use 64-bit RDI, RCX, but 8-bit AL
pop rdi rcx rax
; No RET, continue in the next subroutine

;---------- Print 32-bit Decimal Number -----------------------;
; INPUT:   EAX = Number value                                  ;
;          BL  = Template size, chars. 0=No template           ;
;          RDI = Destination Pointer (flat)                    ;
; OUTPUT:  RDI = New Destination Pointer (flat)                ;
;                modified because string write                 ;
;--------------------------------------------------------------;
Decimal_Print_32:
cld
push rax rbx rcx rdx
mov bh,80h-10
add bh,bl
mov ecx,1000000000
DP32_00:
xor edx,edx
div	ecx                 ; Produce current digit
and al,0Fh
test bh,bh
js DP32_02
cmp ecx,1
je DP32_02
cmp al,0                ; Not actual left zero ?
jz DP32_03
DP32_02:
mov bh,80h              ; Flag = 1
or al,30h
stosb                   ; Store char
DP32_03:
push rdx
xor edx,edx
mov eax,ecx
mov ecx,10
div ecx
mov ecx,eax
pop rax
inc bh
test ecx,ecx
jnz DP32_00
pop rdx rcx rbx rax
DP32_Ret:
ret

;---------- Print 64-bit Hex Number ---------------------------;
; INPUT:  RAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
Hex_Print_64:
push rax
ror rax,32
call Hex_Print_32
pop rax
; no RET, continue at next subroutine
;---------- Print 32-bit Hex Number ---------------------------;
; INPUT:  EAX = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
Hex_Print_32:
push rax
ror eax,16
call Hex_Print_16
pop rax
; no RET, continue at next subroutine
;---------- Print 16-bit Hex Number ---------------------------;
; INPUT:  AX  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
Hex_Print_16:
push rax
xchg al,ah
call Hex_Print_8
pop rax
; no RET, continue at next subroutine
;---------- Print 8-bit Hex Number ----------------------------;
; INPUT:  AL  = Number                                         ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
Hex_Print_8:
push rax
ror al,4
call Hex_Print_4
pop rax
; no RET, continue at next subroutine
;---------- Print 4-bit Hex Number ----------------------------;
; INPUT:  AL  = Number (bits 0-3)                              ;
;         RDI = Destination Pointer                            ;
; OUTPUT: RDI = Modify                                         ;
;--------------------------------------------------------------;
Hex_Print_4:
cld
push rax
and al,0Fh
;---
; DAA instruction is not supported in the 64-bit mode
;	add	al,90h
;	daa
;	adc	al,40h
;	daa
;---
cmp al,9
ja HP4_AF
add al,'0'
jmp HP4_Store
HP4_AF:
add al,'A'-10
HP4_Store:
stosb
pop rax
ret
