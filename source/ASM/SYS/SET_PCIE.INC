;===========================================================================;
;                                                                           ;
;            SET CONTEXT HANDLER FOR SET PCI EXPRESS PACKET SIZE.           ;
;                                                                           ;
;===========================================================================;

;--- Set maximum supported packet size for PCI Express -------------;
; This subroutine conditionally run by _Sys_Bitmap mechanism,       ;
; only if PCIe supported and change packet size is possible         ;
; Registers values, changed by this procedure, already saved        ;
; by Check_PCIe procedure                                           ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;          Variable Sys_Bitmap and context control variables        ;
;-------------------------------------------------------------------;
Setup_PCIe:
;--- Prepare MCFG MMIO base address ---
mov rdi,[_MCFG_Base+r15]   ; RDI = MCFG base
;--- Select maximum acceptable packet size ---
mov dl,[_PCIe_A_Caps+r15]  ; DL=Adapter
mov al,[_PCIe_H_Caps+r15]	
mov dh,al			; DH=Host
and dx,0707h
cmp dl,dh
jbe Min_Packet
mov dl,dh
Min_Packet:	
and dl,00000111b
shl dl,5                   ; DL=Packet length code
;--- Enter to critical section ---
cli
;--- Set packet size for adapter ---
mov bx,[_Adapter_Cfg+r15]  ; Adapter Bus:Device:Function
mov al,[_PCIe_A_Reg+r15]   ; Adapter Register Address
call Local_Setup_PCIe
;--- Set packet size for host ---
mov bx,[_Host_Cfg+r15]     ; Host Bus:Device:Function
mov al,[_PCIe_H_Reg+r15]   ; Host Register Address
call Local_Setup_PCIe
;--- Exit from critical section ---
sti
;--- Set Restore Requirement Flag, also clear CF=0(NC) ---
or byte [_Sys_Bitmap_H+r15],00000010b    ; Bit D9=PCIe
;--- Exit ---
ret

;---------- Local subroutines ----------------------------------------------;

;--- Set PCI Express packet length ---------------------------------;
; INPUT:   RDI = MCFG MMIO base address                             ;
;          BX  = PCI Bus/Device/Function                            ;
;          AL  = PCI Register address of PCIe capability structure  ;
;          DL  = Length code at bits [2-0], bits [7-3] must be 0    ;
; OUTPUT:  CF  = Reserved for error flag after PCI operations       ;
;          AH  = Reserved for status after PCI operations           ;
;-------------------------------------------------------------------;
Local_Setup_PCIe:
movzx rbx,bx
shl rbx,12
movzx rax,al
add rbx,rax
mov cx,[rdi+rbx+8]    ; Read
and cl,00011111b      ; Pre-blank
or cl,dl              ; Modify
mov [rdi+rbx+8],cx    ; Write
ret                   ; Here CF=0(NC) after OR
