;===========================================================================;
;                                                                           ;
;              SUBROUTINES FOR PCI EXPRESS OPERATIONS SUPPORT.              ;
;                                                                           ;
;===========================================================================;

;---------- Detecting Selected Device PCI Express Capabilities -------------;
; Assume PCI MCFG mechanism detected if control here.                       ;
;                                                                           ;
; INPUT:  R15 = Global variables pool base address                          ;
;         Use 64-bit flat memory addressing, without segments               ;
;         RBX = PCI Bus/Device/Function address in the MCFG MMIO            ;
;               Host Bridge or Video Adapter address                        ;
;         AL = PCI Express Capability Structure register address            ;
;                                                                           ;
; OUTPUT: CF = Error flag: 0(NC)=No Errors, 1(C)=Bad PCI status             ;
;         ZF = Presence flag: 0(NZ)=Capabilities present, 1(Z)=None         ;
;         AL = Capabilities Register, bits [7-0], here actual bits [2-0],   ;
;              encoded maximum payload size:                                ;
;              000b = 128 bytes                                             ;
;              001b = 256 bytes                                             ;
;              010b = 512 bytes                                             ;
;              011b = 1024 bytes                                            ;
;              100b = 2048 bytes                                            ;
;              101b = 4096 bytes                                            ;
;              110b = Reserved                                              ;
;              111b = Reserved                                              ;
;                                                                           ;
;         CX = PCI Express Control Register, located at CAP_BASE+8          ;
;---------------------------------------------------------------------------;
Parse_PCIe_Capability:
push rbx
movzx rcx,al
;--- Read PCI Express Device Capabilities Register, bits [7-0] ---
mov al,[rbx+rcx+4]    ; Read PCIe DCAP Register
;--- Read PCIe Device Control Register ---
mov cx,[rbx+rcx+8]    ; Read PCIe D.Ctrl Register
;--- Analysing results ---
pop rbx
and al,00000111b
cmp al,00000110b    ; 110b=Wrong, 111b=Wrong
jae Er_PCIe
;--- Exit points ---
clc       ; CF=0(NC) means no errors
ret
Er_PCIe:
stc       ; CF=1(C) means error
ret		

