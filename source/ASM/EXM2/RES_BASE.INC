;===========================================================================;
;                                                                           ;
;             RESTORE CONTEXT HANDLER FOR: VRAM BAR RELOCATION.             ;
;                                                                           ;
;===========================================================================;

;--- Restore original base address of video memory BAR ------------------;
; Include reprogramming video adapter and PCI Express bridge.            ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;                                                                        ;
;          PCI configuration addresses and offsets:                      ;
;          R14 = MCFG MMIO base address for PCIe video bridge registers  ;
;          R13 = MCFG MMIO base address for PCIe video adaptor registers ;
;          R12 = Offset, can use [r13+r12] for select video memory BAR   ;
;                                                                        ;
;          PCI configuration variables:                                  ;
;          PBAR_Base (qword)  = Old base address for extreme context     ;
;          PBAR_Size (qword)  = Old size for extreme context             ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=PCI PnP Failure        ;
;------------------------------------------------------------------------;
Ext_Restore_VRAM_Base:
;--- Check operation required or not ---
test byte [_Ext_Requests+r15],00000010b
jz Exit_ERVB
;--- Check special support for Integrated Video Adapter at Bus 0 ---
cmp byte [_Act_Video_Bus+r15],0
je Integrated_ERVB
;--- Disable Interrupts ---
cli
;--- Store Fence ---
; Reserved, under construction
;--- Disable video memory (adaptor), set Device Control.Memory Space=0 ---
and byte [r13+004h],11111101b
;--- Disable video memory (bridge), set Device Control.Memory Space=0 ---
and byte [r14+004h],11111101b
;--- Reprogr. bridge ranges, restore old, use 2x32 (not 64) for safe PCI ---
mov eax,[_Ext_Bridge_L+r15]
mov [r14+024h],eax
mov eax,[_Ext_Bridge_H_L+r15]
mov [r14+028h],eax
mov eax,[_Ext_Bridge_H_H+r15]
mov [r14+02Ch],eax
;--- Reprogramming BAR value, preserve control bits for PCI safe ---
mov al,[r13+r12]
and eax,00001111b
or eax,[_PBAR_Base+r15]      ; EAX=Base [31-00]
mov edx,[_PBAR_Base_E+r15]   ; EDX=Base [63-32]
mov [r13+r12],eax
mov [r13+r12+4],edx
;--- Enable video memory (bridge), set Device Control.Memory Space=1 ---
or byte [r14+004h],00000010b
;--- Enable video memory (adaptor), set Device Control.Memory Space=1 ---
Enable_ERVB:
or byte [r13+004h],00000010b
;--- Restore video memory address ---
mov rax,[_Ext_Save_Vbase+r15]
mov [_Vmem_Base+r15],rax
;--- Exit points, first enable interrupts ---
sti
Exit_ERVB:
ret

;---------- Support for Integrated video adapter at Bus 0 ------------------;
Integrated_ERVB:
;--- Disable Interrupts ---
cli
;--- Store Fence ---
; Reserved, under construction
;--- Disable video memory (adaptor), set Device Control.Memory Space=0 ---
and byte [r13+004h],11111101b
;--- Reprogramming BAR value, preserve control bits for PCI safe ---
mov al,[r13+r12]
and eax,00001111b
or eax,[_PBAR_Base+r15]      ; EAX=Base [31-00]
mov edx,[_PBAR_Base_E+r15]   ; EDX=Base [63-32]
mov [r13+r12],eax
mov [r13+r12+4],edx
;--- Go to restore state and exit ---
jmp Enable_ERVB
