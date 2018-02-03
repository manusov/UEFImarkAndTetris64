;===========================================================================;
;                                                                           ;
; BUILT INFORMATION STRING HANDLER FOR: PCI EXPRESS VERSION AND PARAMETERS. ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for PCI express capabilities ---------;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_PCIe:
;--- Prepare MCFG MMIO base address ---
mov rbp,[_MCFG_Base+r15]   ; RBP = MCFG base
;--- Check bridge presence ---
cmp byte [_PCIe_H_Reg+r15],0
je Inf_PCIe_8              ; Go if PCIe bridge absent
cmp byte [_PCIe_A_Reg+r15],0
jne Inf_PCIe_1             ; Go if PCIe video present
Inf_PCIe_8:
ret
;--- Write left part of string ---
Inf_PCIe_1:
mov rsi,String_PCIe_B
call String_Copy_Simple
;--- Write parameters for host ---
mov bx,[_Host_Cfg+r15]
mov al,[_PCIe_H_Reg+r15]
call Local_Inf_PCIe_1
;--- Write left part of string ---
mov rsi,String_PCIe_A
call String_Copy_Simple
;--- Write parameters for adapter ---
mov bx,[_Adapter_Cfg+r15]
mov al,[_PCIe_A_Reg+r15]
; subroutine executed next

;---------- Local subroutines ----------------------------------------------;

;--- Built PCIe device info string ---------------------------------;
; INPUT:   RBP = MCFG MMIO base address                             ;
;          BX  = PCI configuration address: bus/dev/fnc             ;
;          AL  = Register address for PCIe capability structure     ;
;          RDI = Pointer to destination string                      ;
;                                                                   ;
; OUTPUT:  RSI = Pointer updated by strings sequence read           ;
;          RDI = Pointer updated by string write                    ;
;-------------------------------------------------------------------;
Local_Inf_PCIe_1:
;--- Current width and speed ---
movzx rax,al
movzx rbx,bx
shl rbx,12
add rbx,rbp
mov cx,[rbx+rax+12h]      ; Link Status Register
mov rsi,String_PCIe_1
call Local_Inf_PCIe_2
;--- Current TLP ---
mov cx,[rbx+rax+08h]      ; Device Control Register
shr cl,5
call Local_Inf_PCIe_3
call String_Copy_Simple   ; Write ", max: x"
;--- Maximum supported with and speed ---
mov cx,[rbx+rax+0Ch]      ; Link Capabilities Register
mov rsi,String_PCIe_1
call Local_Inf_PCIe_2
;--- Maximum supported TLP ---
mov cx,[rbx+rax+04h]      ; Device Capabilities Register
and cl,00000111b
call Local_Inf_PCIe_3		
;--- Terminate string and exit ---
mov	al,0
stosb
ret

;--- Built part of PCIe device info string: width and speed --------;
; INPUT:   CX  = Bit field from PCIe capability structure           ;
;          RSI = Pointer to source text strings sequence            ;
;          RDI = Pointer to destination string                      ;
;                                                                   ;
; OUTPUT:  RSI   = Pointer updated by strings sequence read         ;
;          RDI   = Pointer updated by string write                  ;
;-------------------------------------------------------------------;
Local_Inf_PCIe_2:
push rax rbx
mov ax,cx
shr ax,4
and eax,00111111b
mov bl,0                 ; BL=Template mode
call Decimal_Print_32    ; Write link width
call String_Copy_Simple  ; Write ", "
xchg ax,cx
and al,00001111b
dec ax
cmp al,2
jbe Inf_PCIe_4
mov al,3
Inf_PCIe_4:
mov ah,3
push rsi
mov rsi,String_25GTS
call String_Select_Copy  ; Write link speed
pop rsi
call String_Copy_Simple  ; Write "GT/s, TLP="
Exit_PCIe_2:	
pop rbx rax
ret

;--- Built part of PCIe device info string: TLP maximum size -------;
; INPUT:   CX  = Bit field from PCIe capability structure           ;
;          RSI = Pointer to source text strings sequence            ;
;          RDI = Pointer to destination string                      ;
;                                                                   ;
; OUTPUT:  RSI = Pointer updated by strings sequence read           ;
;          RDI = Pointer updated by string write                    ;
;-------------------------------------------------------------------;
Local_Inf_PCIe_3:
push rax rbx
cmp cl,5
ja Inf_PCIe_5
mov eax,128            ; Multiplier for calculate
shl ax,cl              ; Calculate TLP size
mov bl,0               ; BL=Template mode
call Decimal_Print_32  ; Write link width
jmp Inf_PCIe_6
Inf_PCIe_5:
mov al,'?'
stosb
Inf_PCIe_6:
jmp Exit_PCIe_2

