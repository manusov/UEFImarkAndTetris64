;===========================================================================;
;                                                                           ;
;        EXTREME CONTEXT SET/RESTORE CONTROL SEQUENCING SUBROUTINE.         ;
;                                                                           ;
;===========================================================================;

;---------- Extreme context Set and Clear sequence execution ------------;
; Note. Cannot output text strings by Simple Text Output Protocol,       ;
; because video memory region is remapped by procedures-handlers,        ;
; called from this procedure.                                            ;
; Errors detection (handler return CF=1) skip extreme context, but not   ;
; skip program execution (normal context used).                          ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, flat 64-bit address   ;
;          RSI = Pointer to sequence, flat 64-bit address                ;
;                                                                        ;
;          PCI configuration variables:                                  ;
;          MCFG_Base (qword)  = Base address of PCIe MCFG MMIO           ;
;          Host_Cfg (word)    = Bus/Dev/Fnc for PCIe Graphics Bridge     ;
;          Adapter_Cfg (word) = Bus/Dev/Fnc for PCIe Graphics Controller ;
;                                                                        ;
;          Variable and bits:                                            ;
;          Extreme_Control (dword) variable bits:                        ;
;          --- Common bits, Ext_Common ---                               ;
;          D00 = Extreme context selected by user                        ;
;          D01 = Extreme context required and supported                  ;
;          D02 = Extreme context physically set OK, restore required     ;
;          D[03] = Reserved                                              ;
;          D[04] = Used by Extreme Mode 3: MTRR reprogrammed flag        ;
;          D[05] = Used by Extreme Mode 3: PCI Bridge reprogrammed flag  ;
;          D[06-07] = Reserved                                           ;
;          --- Requests bits, Ext_Requests ---                           ;
;          D08 = Ext_Allocation_Flag (UEFI memory map resources)         ;
;          D09 = Ext_Relocate_Flag (BAR reprogramming)                   ;
;          D10 = Ext_Resize_Flag (BAR resizing)                          ;
;          D11 = Ext_MTRR_Flag, used from normal context routines        ;
;          D[12-15] = Reserved for new operations codes                  ;
;          --- Transit bits, Ext_Transit ---                             ;
;          D16 = Both video adaptor BAR and PCIe bridge support 64-bit   ;
;          D[17-23] = Reserved                                           ;
;          --- Error reporting bits, Ext_Errors ---                      ;
;          D24 = UEFI address space request failed                       ;
;          D25 = Video RAM BAR address relocation error                  ;
;          D26 = Video RAM BAR resizing error                            ;
;          D27 = Video RAM BAR programming MTRR error                    ;
;          D[28-31] = Reserved for new error codes                       ;
;          ---                                                           ;
;                                                                        ;
; OUTPUT:  None in registers                                             ;
;          Registers corrupted, exclude R15=Saved                        ;
;          Extreme_Control (dword) variable updated                      ;
;          Variables updated by handlers subroutines                     ;
;------------------------------------------------------------------------;
Extreme_Sequencer:

;--- Check system context ---
;!  test byte [_Sys_Bitmap_H+r15],00100000b ; D13
;!  jnz Extreme_Exit   ; Go if PCIe VGA not valid

;--- Prepare parameters ---
mov rax,[_MCFG_Base+r15]
movzx r12,byte [_PBAR_Reg+r15]  ; R12 = BAR cfg.
		
test r12l,r12l    ; R12L = BAR Cfg., 0=None
jz Extreme_Exit   ; Go if BAR number not valid

;! movzx r13,word [_Adapter_Cfg+r15]
movzx r13,word [_Act_Video_Cfg+r15]
	
shl r13,12
add r13,rax       ; R13 = Video adapter MCFG MMIO
movzx r14,word [_Host_Cfg+r15]
shl r14,12
add r14,rax       ; R14 = PCIe Host (video) MCFG MMIO
;--- Cycle for call target subroutines ---
Extreme_Cycle:
lodsb             ; AL = Checkpoint code for Port 80h
test al,al
jz Extreme_Exit   ; Go if AL=0, means stop
CHKOUT            ; Out to Diagnostics Port
lodsq             ; RAX=Subroutine address, flat 64-bit
push rsi
call rax          ; Call information subroutine
pop rsi
jnc Extreme_Cycle
;--- Exit ---
Extreme_Exit:
ret
