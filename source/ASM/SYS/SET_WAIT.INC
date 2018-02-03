;===========================================================================;
;                                                                           ;
;              WAIT USER PRESS ANY KEY FOR START MESSAGES VIEW.             ;
;                                                                           ;
;===========================================================================;

;--- Wait user press any key for start messages view ---------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Flag: 0(NC)=Operation Passed, 1(C)=Failed           ;
;-------------------------------------------------------------------;
Setup_Wait:
;--- Wait for key press ---
call Input_Wait_Key    ; Wait for key press
;--- Analysing input ---
ror rdx,16             ; Move scan code to DL
or dl,00100000b        ; Set lower case, D5=1
cmp rdx,EFI_K_N
jne Exit_Swait
;--- Update variables if press "N"=No wait ---
mov byte [_Start_Pause+r15],0      ; Wait flag=0
or byte [_Sys_Bitmap_H+r15],80h    ; D15=Save CFG
;--- Exit ---
Exit_Swait:
clc    ; CF=0(NC) means no errors
ret
