;===========================================================================;
;                                                                           ;
;                    TABLES FOR SEQUENCES EXECUTION.                        ;
;                                                                           ;
;===========================================================================;

; S=Sequence element
; X1=Checkpoint Code (Port 80h), X2=Check bit
; X3=Subroutine, X4=Message string, X5=Error string
; CheckBit special value: 0FFh=Stop
MACRO  S  x1, x2, x3, x4, x5
{
DB  x1, x2
DQ  x4, x3, x5
}

; SG=Sequence special element, last executed during graphics mode, 
; text out not available before execution, available after execution
; This element used for restore text mode when exit to OS.
; CheckBit.D7=1 means this special mode
; X1=Checkpoint Code (Port 80h), X2=Check bit
; X3=Subroutine, X4=Message string, X5=Error string
; CheckBit special value: 0FFh=Stop
MACRO  SG  x1, x2, x3, x4, x5
{
DB  x1, x2+80h		; X2.D7=1
DQ  x4, x3, x5
}

; CheckBit.D6=0 means bits [0-15] of _Sys_Bitmap used as run enable,
; bit selected by CheckBit.D[3-0]
; CheckBit.D6=1 means unconditional run, NC means No Conditions
NC = 040h		; X2.D6=1

; SS=Sequence special element, executed for switch to (new) graphics mode,
; text out not available after execution, available before execution
; This element used for set (new) graphics mode when run program,
; need skip text write OK/FAILED for this element.
; CheckBit.D5=1 means this special mode
; X1=Checkpoint Code (Port 80h), X2=Check bit
; X3=Subroutine, X4=Message string, X5=Error string
; CheckBit special value: 0FFh=Stop
MACRO  SS  x1, x2, x3, x4, x5
{
DB  x1, x2+20h		; X2.D5=1
DQ  x4, x3, x5
}

; Checkpoint Code=00h means stop
MACRO  STOPS
{
DB  00h
}

Start_Program_Sequence:
S   003h , NC , Check_CPU_Features , Msg_C_CPUF  , Msg_E_CPUF
S   004h , NC , Check_UEFI         , Msg_C_UEFI  , Msg_E_UEFI
S   008h , NC , Check_Memory       , Msg_C_Mem   , Msg_E_Mem
S   009h , NC , Check_ACPI         , Msg_C_ACPI  , Msg_E_ACPI
S   00Ah , NC , Setup_Game         , Msg_S_Game  , Msg_SE_Game
S   00Bh , NC , Load_Config        , Msg_L_Cfg   , Msg_E_Cfg
S   00Ch , NC , Check_Video        , Msg_C_Video , Msg_E_Video
S   00Dh , NC , Check_VMode        , Msg_C_VMode , Msg_E_VMode
;S   00Eh , NC , Check_VFlt         , Msg_C_VFlt  , Msg_E_VFlt
S   00Fh , NC , Check_PCI          , Msg_C_PCI   , Msg_E_PCI
S   010h , NC , Check_Active_Video , Msg_C_VGA   , Msg_E_VGA
S   011h , NC , Check_BAR          , Msg_C_BAR   , Msg_E_BAR
;S   012h , NC , Check_AGP          , Msg_C_AGP   , Msg_E_AGP
S   013h , NC , Check_PCIe         , Msg_C_PCIe  , Msg_E_PCIe
S   014h , NC , Check_MTRR         , Msg_C_MTRR  , Msg_E_MTRR
;S   015h , 00 , Setup_AGP          , Msg_S_AGP   , Msg_SE_AGP
S   016h , 01 , Setup_PCIe         , Msg_S_PCIe  , Msg_SE_PCIe
; Setup MTRR moved to SET_VMOD.INC, execute after GOP parameters valid
;S   017h , 02 , Setup_MTRR         , Msg_S_MTRR  , Msg_SE_MTRR
S   018h , NC , Setup_SSE          , Msg_S_SSE   , Msg_SE_SSE
;S   01Ch , NC , Setup_Mouse        , Msg_S_Mouse , Msg_SE_Mouse
S   01Dh , NC , Check_CPU_CLK      , Msg_C_CLK   , Msg_E_CLK	; Last is long
S   01Eh , 14 , Setup_Wait         , Msg_S_Wait  , Msg_SE_Wait
SS  01Fh , NC , Setup_G_Mode       , Msg_S_Gmod  , Msg_SE_Gmod
STOPS

Stop_Program_Sequence:
SG  070h , 05 , Restore_T_Mode     , Msg_R_Tmod  , Msg_RE_Tmod
S   072h , 11 , Restore_SSE        , Msg_R_SSE   , Msg_RE_SSE
S   073h , 10 , Restore_MTRR       , Msg_R_MTRR  , Msg_RE_MTRR
S   074h , 09 , Restore_PCIe       , Msg_R_PCIe  , Msg_RE_PCIe
S   075h , 08 , Restore_AGP        , Msg_R_AGP   , Msg_RE_AGP
S   076h , 15 , Save_Config        , Msg_S_Cfg   , Msg_SE_Cfg
S   077h , 04 , Release_Memory     , Msg_R_Mem   , Msg_RE_Mem
STOPS
