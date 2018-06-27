;===========================================================================;
;                                                                           ;
;                  EQUATIONS FOR ACCESS GLOBAL VARIABLES.                   ;
;          THIS NUMBERS IS OFFSETS FROM GLOBAL_VARIABLES_POOL BASE.         ;
;       SEE FILE " DAT_VARS.INC " FOR DETAIL COMMENTS ABOUT VARIABLES.      ;
;                                                                           ;
;===========================================================================;

;--- Save context at start of UEFI application ---
_EFI_Handle  = 000h     ; qword , 000h-007h
_EFI_Table   = 008h     ; qword , 008h-00Fh
;--- UEFI Protocols list and file system handles ---
_EFI_Gop     = 100h     ; qword , 10h-17h
