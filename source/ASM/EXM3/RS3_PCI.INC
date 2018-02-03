;===========================================================================;
;                                                                           ;
;       RESTORE CONTEXT HANDLER FOR: VRAM RANGE PREFETCHABLE STATUS.        ;
;                                                                           ;
;===========================================================================;

;--- Restore PCI bridge range status for video memory -------------------;
; Include PCI Express bridge Non-Prefetchable and Prefetchable ranges.   ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;                                                                        ;
;          PCI configuration addresses and offsets:                      ;
;          R14 = MCFG MMIO base address for PCIe video bridge registers  ;
;          R13 = MCFG MMIO base address for PCIe video adaptor registers ;
;          R12 = Offset, can use [r13+r12] for select video memory BAR   ;
;          PCI configuration variables:                                  ;
;          Ext_Common, bit D5 = PCI Bridge reprogrammed flag             ;
;          Ext_Bridge_L     = Prefetchable range limit:base, low 16      ;
;          Ext_Bridge_H_L   = Prefetchable range base, high 32           ;
;          Ext_Bridge_H_H   = Prefetchable range limit, high 32          ;
;          Ext_Bridge_NP    = Non-Prefetchable range limit:base, low 16  ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=PCI PnP Failure        ;
;------------------------------------------------------------------------;
Ext_Restore_PCI_Ranges:
; UNDER DEBUG AND CONSTRUCTION
; RANGE BELOW 4GB CAN'T BE PREFETCHABLE ON INTEL SOCKET 1150 PLATFORMS ?
; BY MODELING UNDER DOS.
ret
