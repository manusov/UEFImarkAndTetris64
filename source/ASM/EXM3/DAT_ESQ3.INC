;===========================================================================;
;                                                                           ;
;  TABLES FOR SET OPTIMAL (EXTREME) SYSTEM CONTEXT PROCEDURES SEQUENCING.   ;
;  EXTREME MODE 3 = CHANGE VIDEO RAM STATUS ONLY, BASE AND SIZE UNCHANGED.  ;
;                                                                           ;
;===========================================================================;

; ECNT=Extreme context settings sequence element
; X1=Checkpoint Code (Port 80h)
; X2=Subroutine
; Special X1 values: 00h=Stop

Set_Extreme_Context_Seq_M3:
ECNT  0B8h , Ext_Change_PCI_Ranges
ECNT  0B9h , Ext_Change_Firmware_MTRR
STOPECNT

Clear_Extreme_Context_Seq_M3:
ECNT  0C8h , Ext_Restore_Firmware_MTRR
ECNT  0C9h , Ext_Restore_PCI_Ranges
STOPECNT