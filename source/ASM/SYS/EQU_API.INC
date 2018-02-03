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

;--- Keyboard codes ---

EFI_KEY_RIGHT   =  00000003h
EFI_KEY_LEFT    =  00000004h
EFI_KEY_UP      =  00000001h
EFI_KEY_DOWN    =  00000002h
EFI_KEY_S       =  00730000h
EFI_KEY_P       =  00700000h
EFI_KEY_ENTER   =  000D0000h
EFI_KEY_ESC     =  00000017h

EFI_K_RIGHT     =  03h
EFI_K_LEFT      =  04h
EFI_K_UP        =  01h
EFI_K_DOWN      =  02h
EFI_K_S         =  73h
EFI_K_P         =  70h
EFI_K_N         =  6Eh
EFI_K_Y         =  79h
EFI_K_ENTER     =  0Dh
EFI_K_ESC       =  17h
