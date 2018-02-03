;===========================================================================;
;                                                                           ;
;         VERIFY COMPATIBILITY HANDLER FOR CHECK PCI BUS PRESENT.           ;
;                                                                           ;
;===========================================================================;

;--- Verify PCI bus presence by PCI MCFG detection status ----------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Check Passed, 1(C)=Check Failed        ;
;          Variable: Last_PCI_Bus = Last PCI bus number             ;
;-------------------------------------------------------------------;
Check_PCI:
mov rax,[_MCFG_Base+r15]
test rax,rax
jz Bad_PCI      ; Go if base=0
mov rax,[_MCFG_Size+r15]
test rax,rax
jnz	Exit_PCI    ; Go if size>0
Bad_PCI:
stc             ; Set Error Flag if bad parm.
Exit_PCI:
ret