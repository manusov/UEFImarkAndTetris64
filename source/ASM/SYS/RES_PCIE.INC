;===========================================================================;
;                                                                           ;
;             RESTORE CONTEXT HANDLER FOR PCI EXPRESS SETTINGS.             ;
;                                                                           ;
;===========================================================================;

;--- Restore PCI Express settings after set maximum packet ---------;
; This subroutine conditionally run by _Sys_Bitmap mechanism,       ;
; only if PCIe supported and packet size is re-programmed           ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          Variable Sys_Bitmap and context control variables        ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;-------------------------------------------------------------------;
Restore_PCIe:
;--- Prepare MCFG MMIO base address ---
mov rdi,[_MCFG_Base+r15]           ; RDI = MCFG base
;--- Enter to critical section ---
cli
;--- Restore PCIe Control Register for adapter ---
movzx rbx,word [_Adapter_Cfg+r15]  ; Adapter B:D:F
shl rbx,12
movzx rax,byte [_PCIe_A_Reg+r15]   ; Adapter Reg.
add rbx,rax
mov cx,[_PCIe_A_Ctrl+r15]         ; Saved old value
mov [rdi+rbx+8],cx                ; Restore value
;--- Restore PCIe Control Register for host ---
movzx rbx,word [_Host_Cfg+r15]    ; Host B:D:F
shl rbx,12
movzx rax,byte [_PCIe_H_Reg+r15]  ; Host Reg.
add rbx,rax
mov cx,[_PCIe_H_Ctrl+r15]         ; Saved old value
mov [rdi+rbx+8],cx                ; Restore value
;--- Exit from critical section ---
sti
;--- Exit ---
ret

