;===========================================================================;
;                                                                           ;
;                      EQUATIONS FOR CALL UEFI API.                         ;
;                                                                           ;
;===========================================================================;

;--- GUID template ---
MACRO GUID x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11
{
DD x1
DW x2, x3
DB x4, x5, x6, x7, x8, x9, x10, x11
}

