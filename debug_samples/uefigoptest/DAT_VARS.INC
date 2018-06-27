;===========================================================================;
;                                                                           ;
;                        CONSTANTS AND VARIABLES.                           ;
;                                                                           ;
;===========================================================================;

;--- Base point for addressing ---
Global_Variables_Pool:

;--- Save parameters, received from UEFI firmware or UEFI shell ---
EFI_Handle      DQ  ?   ; This application handle
EFI_Table       DQ  ?   ; System table address
;--- Work variables ---
EFI_Gop         DQ  ?   ; Pointer to UEFI GOP protocol interface

;--- Buffers for build and convert ASCII (compact8) to UNICODE (big16) ---
Buffer_ASCII    DB  160 DUP (?)
Buffer_UNICODE  DU  160 DUP (?)
