;===========================================================================;
;                                                                           ;
;         RESTORE CONTEXT HANDLER FOR: VRAM UC STATUS BY FIRMWARE.          ;
;                                                                           ;
;===========================================================================;

;--- Restore video memory region MTRR UC status, if set by firmware -----;
; Optimal attribute for video memory is Write Combining (WC),            ;
; this reprogramming already set by non-extreme context procedures       ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          Configuration variables:                                      ;
;          PBAR_Base (qword)   = New base addr. for extreme context      ;
;          PBAR_Size (qword)   = New size for extreme context            ;
;          MTRR control configuration variables:                         ;
;          Ext_Common, bit D4 = MTRR reprogrammed flag                   ;
;          FW_MTRR_Used   (dword) = MSR address for used MTRR            ;
;          FW_MTRR_S_Base (qword) = Save old MTRR state, Base(i) MSR     ;
;          FW_MTRR_S_Mask (qword) = Save old MTRR state, Mask(i) MSR     ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=Operation failure      ;
;------------------------------------------------------------------------;
Ext_Restore_Firmware_MTRR:
; UNDER DEBUG AND CONSTRUCTION
; RANGE BELOW 4GB CAN'T BE PREFETCHABLE ON INTEL SOCKET 1150 PLATFORMS ?
; BY MODELING UNDER DOS.
ret
