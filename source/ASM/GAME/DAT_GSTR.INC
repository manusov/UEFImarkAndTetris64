;===========================================================================;
;                                                                           ;
;             GAME-SPECIFIC TEXT STRINGS AND COPYRIGHT STRING.              ;
;      NOTE. COPYRIGHT STRING ALSO PRESENT IN THE SYS\DAT_CFG.INC FILE      ;
;                                                                           ;
;===========================================================================;

MACRO  KEYSTRING  x1, x2
{
DW  x1		; X-offset, pixels, must be ALIGN 4
DQ  x2		; Offset text string
}

; Text strings offsets also duplicated, possible text=f(size)
; This 8 strings must be sequental

Game_Keys_1024:
KEYSTRING  0020 , Key_String_00
KEYSTRING  0132 , Key_String_01
KEYSTRING  0260 , Key_String_02 
KEYSTRING  0360 , Key_String_03 
KEYSTRING  0464 , Key_String_04 
KEYSTRING  0552 , Key_String_05
KEYSTRING  0776 , Key_String_06
KEYSTRING  0916 , Key_String_07
DW  0

Game_Keys_800:
KEYSTRING  0016 , Key_String_00
KEYSTRING  0104 , Key_String_01
KEYSTRING  0192 , Key_String_02 
KEYSTRING  0272 , Key_String_03 
KEYSTRING  0352 , Key_String_04 
KEYSTRING  0432 , Key_String_05
KEYSTRING  0632 , Key_String_06
KEYSTRING  0712 , Key_String_07
DW  0

Game_Keys_1600:
KEYSTRING  0016 , Key_String_00
KEYSTRING  0112 , Key_String_01
KEYSTRING  0208 , Key_String_02 
KEYSTRING  0296 , Key_String_03 
KEYSTRING  0384 , Key_String_04 
KEYSTRING  0472 , Key_String_05
KEYSTRING  1408 , Key_String_06
KEYSTRING  1504 , Key_String_07
DW  0

Key_String_00  DB  18h,         ' rotate' , 0    ; Up
Key_String_01  DB  1Bh,20h,1Ah, ' move'   , 0    ; Left, Right
Key_String_02  DB  19h,         ' force'  , 0    ; Down
Key_String_03  DB  'S sound'     , 0
Key_String_04  DB  'P pause'     , 0
Key_String_05  DB  'ENTER start' , 0
Key_String_06  DB  'F10 BMP'     , 0
Key_String_07  DB  'ESC exit'    , 0


; This 4 boxes/strings must be sequental

MACRO  STATBOX  x1, x2, x3
{
DW  x1, x2    ; X-base, X-size, must be ALIGN 4
DB  x3, 0     ; Text string
}

Game_Status_Boxes:
STATBOX  000 , 076 , 'Run time'
STATBOX  152 , 052 , 'Level'
STATBOX  240 , 068 , 'Figures'
STATBOX  352 , 052 , 'Lines'
DW  0FFFFh

Game_String_Over  DB  'G A M E   O V E R',0
