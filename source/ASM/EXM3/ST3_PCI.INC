;===========================================================================;
;                                                                           ;
;         SET CONTEXT HANDLER FOR: VRAM RANGE PREFETCHABLE STATUS.          ;
;                                                                           ;
;===========================================================================;

;--- Change PCI bridge range status for video memory --------------------;
; Include PCI Express bridge Non-Prefetchable and Prefetchable ranges.   ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;                                                                        ;
;          PCI configuration addresses and offsets:                      ;
;          R14 = MCFG MMIO base address for PCIe video bridge registers  ;
;          R13 = MCFG MMIO base address for PCIe video adaptor registers ;
;          R12 = Offset, can use [r13+r12] for select video memory BAR   ;
;          Configuration variables:                                      ;
;          PBAR_Base (qword) = Video memory base address                 ;
;          PBAR_Size (qword) = Video memory size                         ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=PCI PnP Failure        ;
;          PCI configuration variables:                                  ;
;          Ext_Common, bit D5 = PCI Bridge reprogrammed flag             ;
;          Ext_Bridge_L     = Prefetchable range limit:base, low 16      ;
;          Ext_Bridge_H_L   = Prefetchable range base, high 32           ;
;          Ext_Bridge_H_H   = Prefetchable range limit, high 32          ;
;          Ext_Bridge_NP    = Non-Prefetchable range limit:base, low 16  ;
;------------------------------------------------------------------------;
Ext_Change_PCI_Ranges:
; UNDER DEBUG AND CONSTRUCTION
; RANGE BELOW 4GB CAN'T BE PREFETCHABLE ON INTEL SOCKET 1150 PLATFORMS ?
; BY MODELING UNDER DOS.

;--- Check video memory prefetchable BAR exist ---

;--- Check PCIe bridge prefetchable range disabled ---

;--- Check PCIe bridge non-prefetchable range include video memory ---

;--- Calculate new PCI PnP values ---

;--- Store Fence ---
; Reserved, under construction

;--- Disable video memory (bridge), set Device Control.Memory Space=0 ---

;--- Enable video memory (bridge), set Device Control.Memory Space=1 ---

;--- Exit ---
ret

