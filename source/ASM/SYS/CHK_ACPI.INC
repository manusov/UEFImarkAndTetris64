;===========================================================================;
;                                                                           ;
;       VERIFY COMPATIBILITY AND DETECTION HANDLER FOR ACPI AND MCFG.       ;
;                                                                           ;
;===========================================================================;

;--- Find ACPI Tables data, scan EFI Cfg. Table by GUIDs -----------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Check Passed, 1(C)=Check Failed        ;
;          Global variables                                         ;
;-------------------------------------------------------------------;
Check_ACPI:
;--- Pre-blank results ---
xor rax,rax
mov [_ACPI_Version+r15],al   ; 0 means ACPI absent
mov [_MCFG_Base+r15],rax     ; 0 means MCFG absent
;--- Scan for ACPI support by ACPI1 and ACPI2 GUIDs ---
call FindAcpiSupport
jc Exit_ACPI                 ; Go if ACPI not found
mov [_ACPI_Version+r15],al
mov [_ACPI_Base+r15],rbx
;--- Scan for ACPI table: MCFG, accept AL, RBX, EDX ---
mov edx,'MCFG'
call FindAcpiTable
jc Exit_ACPI                 ; Go if MCFG not found
;--- Scan for ACPI MCFG table ---
call ParseAcpiMcfg
jc Exit_ACPI                 ; Go if MCFG wrong
mov [_MCFG_Base+r15],rbx
mov [_MCFG_Size+r15],rcx
;--- Exit, no aborts, even if ACPI not detected ---
Exit_ACPI:
clc                          ; CF=0(NC) means no errors
ret

