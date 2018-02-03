;===========================================================================;
;                                                                           ;
;                          GAME WINDOWS DIRECTORY.                          ;
;                                                                           ;
;===========================================================================;

;--- Window descriptor layout, total size=4+17+32=53 bytes ---

; Checkpoints for Port 80h
MACRO  WINCHK  x1, x2, x3, x4			; Total 4 bytes
{
DB  x1      ; Checkpoint code for window initialization subrout.
DB  x2      ; Checkpoint code for window open subroutine
DB  x3      ; Checkpoint code for window close subroutine
DB  x4      ; Checkpoint code for window event subroutine
}

; Some parameters can be dynamically updated to adjust for current hardware,
; and for windows moving and resizing,  example: 
; X, Y sizes can be updated to adjust for video adapter and mode
MACRO  WINDESC  x1, x2, x3, x4, x5, x6		; Total 17 bytes
{
DQ  x1      ; Base linear address of window sprite at sys. memory
DW  x2, x3  ; X-size and Y-size for window sprite
DW  x4, x5  ; Current X-position and Y-position for window sprite
DB  x6      ; Window status: 0=Not initialized, 1=Off, 2=On
}

MACRO  WINPROC  x1, x2, x3, x4			; Total 32 bytes
{
DQ  x1      ; Near64 entry point for window initialization subr.
DQ  x2      ; Near64 entry point for window open subroutine
DQ  x3      ; Near64 entry point for window close subroutine
DQ  x4      ; Near64 entry point for window event subroutine
}

Game_Win_List:
Number_Of_Windows  DB  6
;--- Background ----
WINCHK   031h, 041h, 061h, 051h
WINDESC  0 , 0000 , 0000 , 0000 , 0000 , 0
WINPROC  W1_Init , W1_Open , W1_Close , W1_Event
;--- Game field ---
WINCHK   032h, 042h, 062h, 052h
WINDESC  0 , 0416 , 0539 , 0000 , 0000 , 0
WINPROC  W2_Init , W2_Open , W2_Close , W2_Event
;--- Copyright ---
WINCHK   033h, 043h, 063h, 053h
WINDESC  0 , 0000 , 0024 , 0000 , 0000 , 0
WINPROC  W3_Init , W3_Open , W3_Close , W3_Event
;--- Date and time ---
WINCHK   034h, 044h, 064h, 054h
WINDESC  0 , 0000 , 0024 , 0000 , 0000 , 0
WINPROC  W4_Init , W4_Open , W4_Close , W4_Event
;--- Game status ---
WINCHK   035h, 045h, 065h, 055h
WINDESC  0 , 0460 , 0017 , 0000 , 0000 , 0
WINPROC  W5_Init , W5_Open , W5_Close , W5_Event
;--- Keys help and press buttons ---
WINCHK   036h, 046h, 066h, 056h
WINDESC  0 , 0000 , 0024 , 0000 , 0000 , 0
WINPROC  W6_Init , W6_Open , W6_Close , W6_Event
;--- End of list ---
