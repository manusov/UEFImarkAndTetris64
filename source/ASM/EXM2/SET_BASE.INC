;===========================================================================;
;                                                                           ;
;               SET CONTEXT HANDLER FOR: VRAM BAR RELOCATION.               ;
;                                                                           ;
;===========================================================================;

;--- Relocate base address for video memory BAR -------------------------;
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
;          Ext_Relocate_Flag (bit) = Relocation required and supported   ;
;          Ext_PBAR_Base (qword)   = New base addr. for extreme context  ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=PCI PnP Failure        ;
;------------------------------------------------------------------------;
Ext_Set_VRAM_Base:
;--- Check operation required or not ---
test byte [_Ext_Requests+r15],00000010b
jz Exit_ESVM
;--- Check special support for Integrated Video Adapter at Bus 0 ---
cmp byte [_Act_Video_Bus+r15],0
je Integrated_ESVM
;--- Disable Interrupts ---
cli
;--- Store Fence ---
; Reserved, under construction
;--- Disable video memory (adaptor), set Device Control.Memory Space=0 ---
and byte [r13+004h],11111101b
;--- Disable video memory (bridge), set Device Control.Memory Space=0 ---
and byte [r14+004h],11111101b
;--- Read and save adaptor BAR ---
; Not required, because already saved for normal context
;--- Read and save bridge ranges, use 2x32 (not 64) for safe PCI ---
mov eax,[r14+024h]
mov [_Ext_Bridge_L+r15],eax
mov eax,[r14+028h]
mov [_Ext_Bridge_H_L+r15],eax
mov eax,[r14+02Ch]
mov [_Ext_Bridge_H_H+r15],eax
;--- Set new base addr. in the adaptor BAR, use 2x32, probably unaligned ---
mov eax,[_Ext_PBAR_Base+r15]
mov edx,[_Ext_PBAR_Base_E+r15]
mov [r13+r12],eax
mov [r13+r12+4],edx
;--- Calculate new base address and limit in the bridge addr./limit regs. ---
mov ebx,[_Ext_PBAR_Size+r15]
mov ecx,[_Ext_PBAR_Size_E+r15]
sub ebx,1			; Can't DEC, need CF
sbb ecx,0
or ebx,eax
or ecx,edx
shr ebx,16
shrd eax,ebx,16       ; EAX=Base and Limit both
and eax,0FFF0FFF0h    ; Mask both ctrl. bits [3-0]
or eax,000010001h
;--- Set new base address and limit in the bridge address/limit regs. ---
mov [r14+024h],eax    ; Pref. Base/Limit Low 16/16
mov [r14+028h],edx    ; Prefetchable Base Upper 32
mov [r14+02Ch],ecx    ; Prefetchable Limit Upper 32
;--- Enable video memory (bridge), set Device Control.Memory Space=1 ---
or byte [r14+004h],00000010b
;--- Enable video memory (adaptor), set Device Control.Memory Space=1 ---
Enable_ESVM:
or byte [r13+004h],00000010b
;--- Save old and set new (relocated) video memory address ---
mov rax,[_Ext_PBAR_Base+r15]
xchg rax,[_Vmem_Base+r15]
mov [_Ext_Save_Vbase+r15],rax
;--- Exit points, first enable interrupts ---
sti
Exit_ESVM:
ret

;---------- Support for Integrated video adapter at Bus 0 ------------------;
Integrated_ESVM:
;--- Disable Interrupts ---
cli
;--- Store Fence ---
; Reserved, under construction
;--- Disable video memory (adaptor), set Device Control.Memory Space=0 ---
and byte [r13+004h],11111101b
;--- Set new base addr. in the adaptor BAR, use 2x32, probably unaligned ---
mov eax,[_Ext_PBAR_Base+r15]
mov edx,[_Ext_PBAR_Base_E+r15]
mov [r13+r12],eax
mov [r13+r12+4],edx
;--- Go to restore state and exit ---
jmp Enable_ESVM
