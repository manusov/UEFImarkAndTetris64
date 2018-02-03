;===========================================================================;
;                                                                           ;
;  TABLES FOR SET OPTIMAL (EXTREME) SYSTEM CONTEXT PROCEDURES SEQUENCING.   ;
;     EXTREME MODE 2 = CHANGE VIDEO RAM STATUS, BASE ADDRESS AND SIZE.      ;
;                                                                           ;
;===========================================================================;

; ECNT=Extreme context settings sequence element
; X1=Checkpoint Code (Port 80h)
; X2=Subroutine
; Special X1 values: 00h=Stop

MACRO  ECNT  x1, x2
{
DB  x1
DQ  x2
}

MACRO  STOPECNT
{
DB  00h
}

Set_Extreme_Context_Seq_M2:
ECNT  0B0h , Ext_Check_VRAM_Base
ECNT  0B1h , Ext_Check_VRAM_Size
ECNT  0B2h , Ext_Set_EFI_MMIO_Map
ECNT  0B3h , Ext_Skip_MTRR_Map
ECNT  0B4h , Ext_Set_VRAM_Base
;ECNT  0B5h , Ext_Set_VRAM_Size
ECNT  0B6h , Ext_New_MTRR_Map
STOPECNT

Clear_Extreme_Context_Seq_M2:
ECNT  0C0h , Ext_Restore_MTRR_Map
;ECNT  0C1h , Ext_Restore_VRAM_Size
ECNT  0C2h , Ext_Restore_VRAM_Base
ECNT  0C3h , Ext_Restore_EFI_MMIO_Map
STOPECNT
