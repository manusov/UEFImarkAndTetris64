;===========================================================================;
;                                                                           ;
;               SET CONTEXT HANDLER FOR: VRAM BAR RESIZING.                 ;
;                                                                           ;
;===========================================================================;

;--- Resize video memory BAR --------------------------------------------;
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
;          Ext_Resize_Flag (bit) = Resize required and supported         ;
;          Ext_PBAR_Size (qword) = New size for extreme context          ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=PCI PnP Failure        ;
;------------------------------------------------------------------------;
Ext_Set_VRAM_Size:
;--- Check operation required or not ---
test byte [_Ext_Requests+r15],00000100b
jz Exit_ESVS
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
;--- Read, save, modify resizable BAR capability control reg., size field ---
movzx rbx,word [_Resizable_Reg+r15]
mov al,[r13+rbx+9]
mov [_Resizable_Value+r15],al
mov rcx,[_Ext_PBAR_Size+r15]
bsr rcx,rcx
sub cl,19
and al,11100000b
or al,cl
mov [r13+rbx+9],al
;--- Read and save bridge range ---
; Not required, because already saved for base relocation
;--- Modify bridge range size (here base already relocated) ---
mov ax,[_Ext_PBAR_Size_H+r15]
mov edx,[_Ext_PBAR_Size_E+r15]
sub ax,1              ; Can't DEC, need CF
sbb edx,0
mov bx,[r14+024h]     ; Prefetchable Base [31-16]
mov ecx,[r14+028h]    ; Prefetchable Base [63-32]
and bl,11110000b
add bx,ax
adc ecx,edx
and bl,11110000b
or bl,00000001b
mov [r14+026h],bx     ; Prefetchable Limit [31-16]
mov [r14+02Ch],ecx    ; Prefetchable Limit [63-32]
;--- Enable video memory (bridge), set Device Control.Memory Space=1 ---
or byte [r14+004h],00000010b
;--- Enable video memory (adaptor), set Device Control.Memory Space=1 ---
or byte [r13+004h],00000010b
;--- Exit points, first enable interrupts ---
sti
Exit_ESVS:
ret
