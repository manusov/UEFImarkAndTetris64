;===========================================================================;
;                                                                           ;
;      VERIFY COMPATIBILITY HANDLER FOR CHECK MTRR REGISTERS PRESENT.       ;
;                                                                           ;
;===========================================================================;

;---------- Comments -------------------------------------------------------
;
; IA32_MTRRCAP = MSR#000000FEh, Read Only.
; D00-D07 = Number of Variable Range MTRRs
; D08     = Fixed MTRR presence, 0=absent, 1=present
; D09     = Reserved = 0
; D10     = Write Combining Memory Type support: 0=not supported, 1=supported
; D11-D63 = Reserved = 0
; Typically contain 0000000000000508h, means:
; WC supported, Fixed MTRR supported, Number of Variable MTRR = 8
;---------------------------------------------------------------------------
;
; IA32_MTRR_DEF_TYPE = MSR#000002FFh, Read/Write.
; D00-D07 = Default Memory Type, used for regions, not described by MTRR
; D08-D09 = Reserved = 0
; D10     = Fixed Range MTRR enable: 0=disabled, 1=enabled
; D11     = MTRR capability enable: 0=disabled, all memory=UC, 1=enabled
; D12-D63 = Reserved = 0
; Memory types encoding (8-bit Memory Type used for all MTRRs):
; 00 = UC = Uncacheable
; 01 = WC = Write Combining
; 02 = Reserved
; 03 = Reserved
; 04 = WT = Write Through
; 05 = WP = Write Protected
; 06 = WB = Write Back
; 07-FF = Reserved
;---------------------------------------------------------------------------
;
; Region Base Address and Size is variable
; Name                     MSR# (hex)
; -------------------------------------------
; IA32_MTRR_PHYSBASE0      00000200
; IA32_MTRR_PHYSMASK0      00000201
; IA32_MTRR_PHYSBASE1      00000202
; IA32_MTRR_PHYSMASK1      00000203
; IA32_MTRR_PHYSBASE2      00000204
; IA32_MTRR_PHYSMASK2      00000205
; IA32_MTRR_PHYSBASE3      00000206
; IA32_MTRR_PHYSMASK3      00000207
; IA32_MTRR_PHYSBASE4      00000208
; IA32_MTRR_PHYSMASK4      00000209
; IA32_MTRR_PHYSBASE5      0000020A
; IA32_MTRR_PHYSMASK5      0000020B
; IA32_MTRR_PHYSBASE6      0000020C
; IA32_MTRR_PHYSMASK6      0000020D
; IA32_MTRR_PHYSBASE7      0000020E
; IA32_MTRR_PHYSMASK7      0000020F
;
; Variable Range MTRR. PHYSBASE(i) Registers:
; -------------------------------------------
; D00-D07 = Memory type code
; D08-D11 = Reserved = 0
; D12-D35 = Memory Region Base Address, 0-64GB (36-bit), 4K Granularity
; D36-D63 = Reserved = 0 (future use for address maximum 64-bit)
;
; Variable Range MTRR. PHYSMASK(i) Registers:
; -------------------------------------------
; D00-D10 = Reserved = 0
; D11     = Validity, 0=this MTRR not used, 1=used
; D12-D35 = Memory Region Address Mask, 0-64GB (36-bit), 4K Granularity
; D36-D63 = Reserved = 0 (future use for address maximum 64-bit)
;
; Fixed Range MTRR not listed here because not used for this subroutine
; See Intel/AMD documents.
;---------------------------------------------------------------------------

;-- Check MTRR present for use fast write methods of video memory --;
; Result: variables for mtrr set later at SET_MTRR.INC              ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Flag: 0(NC)=Check Passed, 1(C)=Check Failed         ;
;          Video control variables and buffers updated              ;
;-------------------------------------------------------------------;
Check_MTRR:
;--- Pre-blank MSR address ---
; Address=0 means capability not detected or not used
mov dword [_MTRR_Used+r15],0
;--- Detect MTRR capability ---
; Support CPUID function 1 already verified by previous operations
mov eax,1
cpuid                ; Get ECX:EDX=CPU features list
test dh,00010000b
jz None_MTRR         ; Absence is not error here
;--- Check required features ---
mov ecx,000000FEh    ; IA32_MTRRCAP
rdmsr
test al,al
jz None_MTRR         ; Go if no Variable MTRR
test ah,00000100b
jz None_MTRR         ; Go if no WC Memory Mode
xchg eax,ebx         ; BL = Number of Variable MTRR
mov cx,02FFh         ; IA32_MTRR_DEF_TYPE, ECX.[31-16]=0
rdmsr
test ah,00001000b
jz Error_MTRR        ; MTRR Capability Disabled ?

;--- v1.05 corrections ---
  ; Some platforms (example = Intel Skylake)
  ; has non-UC attribute as Default MTRR
	;	test	al,al
	;	jnz	Error_MTRR	; Default must be UC (Uncacheable)
;---

;--- Scan Variable MTRR ---
mov	cx,0201h   ; IA32_MTRR_PHYSMASK0, ECX.[31-16]=0
Scan_Var_MTRR:
rdmsr
test ah,00001000b
jz Found_MTRR      ; This MTRR free ?
Next_Var_MTRR:
inc ecx            ; Try next MTRR
inc ecx
dec bl             ; BL = Number of Variable MTRR
jnz Scan_Var_MTRR  ; Cycle for find vacant MTRR
;--- Exit without errors if vacant MTRR not found, CF=0(NC) ---
mov dword [_MTRR_Used+r15],1
clc
ret
;--- Exit if errors, CF=1(C)---
Error_MTRR:
stc
ret

;--- Available MTRR found ---
Found_MTRR:
mov [_MTRR_S_Mask_L+r15],eax  ; Save Mask [31-00]
mov [_MTRR_S_Mask_H+r15],edx  ; Save Mask [63-32]
dec ecx                       ; Modify MSR address
rdmsr                         ; Read Base [63-00]
mov [_MTRR_S_Base_L+r15],eax  ; Save Base [31-00]
mov [_MTRR_S_Base_H+r15],edx  ; Save Base [63-32]
mov [_MTRR_Used+r15],ecx      ; Save MSR address
;--- Set Enable Flag, bit D2=MTRR ---
or byte [_Sys_Bitmap_L+r15],00000100b  ; also CF=0
;--- Exit if no errors, go here with CF=0(NC) ---
None_MTRR:
ret
