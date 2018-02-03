;===========================================================================;
;                                                                           ;
;          TABLES FOR CREATE AND INTERPRETING CONFIGURATION FILE.           ;
;                                                                           ;
;===========================================================================;

;--- Common block ---
; CFGL.X1=String or char (data pointer is flat 64)
;--- Loader block (used by LD_CFG.INC) ---
; CFGL.X2=Handler for loader (code pointer is flat 64)
; CFGL.X3=Offset of parameter value in the Global_Variables_Pool
; CFGL.X4=Parameter width, bits: 8=Byte, 16=Word, 32=Dword, 64=Qword,
;         0=Entry not used, other values reserved
;         Qword mode reserved, 64-bit parameters not supported yet
;         for configuration file
;         Reserved functionality for bytes X4, X5:
;         parameter offset can be 14-bit, parameter width select bits
;         can be at D[15-14], this expands offsets from
;         [-128...+127] to [-16384...+16383].
;--- Saver block (used by SV_CFG.INC) ---
; CFGS.X1=Handler for saver (code pointer is flat 64)
; CFGS.X2=Offset of parameter value, see comments for CFGL.X3
; CFGS.X3=Parameter width, see comments for CFGL.X4
;---

;--- Chars (size=1, char) ---
CfgChar_00  DB  ' '  , 0    ; Space
CfgChar_01  DB  9    , 0    ; Tab
CfgChar_02  DB  10   , 0    ; Line feed
CfgChar_03  DB  13   , 0    ; Carriage return
CfgChar_04  DB  ';'  , 0    ; Comments strings

;--- Keywords (size, chars) ---
; Used for key words comparing and write config. file
; Relocated to  ' ASM \ NAMES \ NAME_C.INC, NAME_T.INC, NAME_U.INC '

;--- Numbers 0-9 ---
; Reserved, handled by code, no data

;--- List for chars and keywords handling ---
; "HL" means handler for loader
; "HS" means handler for saver
; Start chars and keywords list, entire list
; Relocated to  ' ASM \ NAMES \ NAME_C.INC, NAME_T.INC, NAME_U.INC '

