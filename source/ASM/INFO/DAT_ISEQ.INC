;===========================================================================;
;                                                                           ;
;         TABLES FOR GET SYSTEM INFORMATION PROCEDURES SEQUENCING.          ;
;                                                                           ;
;===========================================================================;

; SINF=System information sequence element
; X1=Checkpoint Code (Port 80h)
; X2=Subroutine
; Special X1 values: 00h=Stop

MACRO  SINF  x1, x2
{
DB  x1
DQ  x2
}

MACRO  STOPSINF
{
DB  00h
}

Get_Sys_Info_Sequence:
SINF  020h , Info_CSM
SINF  021h , Info_CPU
SINF  022h , Info_CPU_Clock
SINF  023h , Info_CPU_Features
SINF  024h , Info_Video_Mode
;SINF  025h , Info_AGP                 ; AGP not supported by UEFI edition
SINF  026h , Info_PCIe
SINF  027h , Info_System_Memory
SINF  028h , Info_Video_Memory
SINF  029h , Info_MTRR
SINF  02Ah , Info_Benchmarks
;SINF  02Bh , Info_Show
STOPSINF
