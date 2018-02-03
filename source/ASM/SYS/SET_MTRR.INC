;===========================================================================;
;                                                                           ;
;           SET CONTEXT HANDLER FOR VIDEO MEMORY MTRR ATTRIBUTES.           ;
;                                                                           ;
;===========================================================================;

;--- Set video memory MTRR attributes to write combining (WC) ------;
; This subroutine conditionally run by _Sys_Bitmap mechanism,       ;
; only if MTRR supported and re-programmed                          ;
; Registers values, changed by this procedure, already saved        ;
; by Check_MTRR procedure                                           ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Flag: 0(NC)=Operation Passed, 1(C)=Failed           ;
;          Variable Sys_Bitmap and context control variables        ;
;-------------------------------------------------------------------;
Setup_MTRR:
;--- Select base,size parameters source: prefetchable BAR or GOP ---
;--- Get prefetchable BAR info ---
mov eax,[_PBAR_Base+r15]      ; EAX=Region base, low dword
mov edx,[_PBAR_Base_E+r15]    ; EDX=Region base, high dword
mov esi,[_PBAR_Size+r15]      ; ESI=Region size, low dword
mov edi,[_PBAR_Size_E+r15]    ; EDI=Region size, high dword
;--- Check PCI PnP parameters valid ---

		;@

cmp byte [_PBAR_Reg+r15],0
je Use_GOP_Range          ; Go if no PCI PnP PBAR

		;@

test edx,edx
jnz Use_GOP_Range         ; Go if base > 4GB
test edi,edi
jz Set_MTRR               ; Go if size < 4GB
;--- Get GOP BAR info ---
; Use two 32-bit registers instead one 64-bit register, 
; becasue IA32 RDMSR/WRMSR compatibility required
;---
Use_GOP_Range:
mov eax,[_Vmem_Base_L+r15]
mov edx,[_Vmem_Base_H+r15]
mov esi,[_Vmem_Size_L+r15]
mov edi,[_Vmem_Size_H+r15]
;--- Entry point for Extreme Mode ---
Setup_MTRR_Entry:
;--- Align address mask = EDI:ESI ---
Set_MTRR:	
mov ebx,01000000h         ; 16MB
cmp esi,ebx
jae Aligned_MTRR
mov esi,ebx
;--- Calculate address mask = EDI:ESI ---
Aligned_MTRR:	
sub esi,1                 ; Need set CF, cannot DEC
jnc Not_Above_4
dec edi
Not_Above_4:
not esi
not edi
and si,0F000h             ; Clear not used bits
or si,00800h              ; Set D11=Enable
mov bl,[_Address_Size+r15]
sub bl,32
mov cl,32
sub cl,bl
mov ebx,0FFFFFFFFh
shr ebx,cl
and edi,ebx
;--- Prepare control fields ---
and ax,0F000h             ; Clear not used bits
mov al,01h                ; 01h=Write Combining
;--- Setup MTRR ---
mov ecx,[_MTRR_Used+r15]  ; Address of MSR
cli
wrmsr                     ; Write Base(i) MTRR, EDX:EAX
inc ecx                   ; MSR address+1
xchg eax,esi
mov edx,edi
wrmsr                     ; Write Mask(i) MTRR, EDX:EAX
sti
;--- Set Restore Requirement Flag, bit D10=MTRR ---
or byte [_Sys_Bitmap_H+r15],00000100b  ; also CF=0
;--- Exit, here CF=0(NC) ---
ret
