;===========================================================================;
;                                                                           ;
;             RESTORE CONTEXT HANDLER FOR: VRAM BAR RESIZING.               ;
;                                                                           ;
;===========================================================================;

;--- Restore original size of video memory BAR --------------------------;
; Include reprogramming video adapter and PCI Express bridge.            ;
; Use Resizable BAR Capability.                                          ;
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
Ext_Restore_VRAM_Size:
;--- Check operation required or not ---
test byte [_Ext_Requests+r15],00000100b
jz Exit_ERVS
;--- Disable Interrupts ---
cli
;--- Store Fence ---
; Reserved, under construction
;--- Disable video memory (adaptor), set Device Control.Memory Space=0 ---
and byte [r13+004h],11111101b
;--- Disable video memory (bridge), set Device Control.Memory Space=0 ---
and byte [r14+004h],11111101b
;--- Write resizable BAR capability control register, size field ---
movzx rbx,word [_Resizable_Reg+r15]
mov al,[r13+rbx+9]
and al,11100000b
or al,[_Resizable_Value+r15]
mov [r13+rbx+9],al
;--- Reprogramming BAR value, preserve control bits for PCI safe ---
mov al,[r13+r12]
and eax,00001111b
or eax,[_Ext_PBAR_Base+r15]     ; EAX=Base [31-00]
mov edx,[_Ext_PBAR_Base_E+r15]  ; EDX=Base [63-32]
mov [r13+r12],eax
mov [r13+r12+4],edx
;--- Enable video memory (bridge), set Device Control.Memory Space=1 ---
or byte [r14+004h],00000010b
;--- Enable video memory (adaptor), set Device Control.Memory Space=1 ---
or byte [r13+004h],00000010b
;--- Exit points, first enable interrupts ---
sti
Exit_ERVS:
ret
