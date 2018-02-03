;===========================================================================;
;                                                                           ;
;    SET CONTEXT HANDLER FOR: DISABLE VRAM UC STATUS, IF SET BY FIRMWARE.   ;
;                                                                           ;
;===========================================================================;

;--- Remove video memory region from MTRR UC region, set by firmware ----;
; Optimal attribute for video memory is Write Combining (WC),            ;
; this reprogramming already set by non-extreme context procedures       ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          Configuration variables:                                      ;
;          PBAR_Base (qword) = Video memory base address                 ;
;          PBAR_Size (qword) = Video memory size                         ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=Operation failure      ;
;          MTRR control configuration variables:                         ;
;          Ext_Common, bit D4 = MTRR reprogrammed flag                   ;
;          FW_MTRR_Used   (dword) = MSR address for used MTRR            ;
;          FW_MTRR_S_Base (qword) = Save old MTRR state, Base(i) MSR     ;
;          FW_MTRR_S_Mask (qword) = Save old MTRR state, Mask(i) MSR     ;
;------------------------------------------------------------------------;
Ext_Change_Firmware_MTRR:
; UNDER DEBUG AND CONSTRUCTION
; RANGE BELOW 4GB CAN'T BE PREFETCHABLE ON INTEL SOCKET 1150 PLATFORMS ?
; BY MODELING UNDER DOS.
ret
