;===========================================================================;
;                                                                           ;
;         VERIFY COMPATIBILITY HANDLER FOR MEASURE CPU CORE CLOCK.          ;
;                                                                           ;
;===========================================================================;

;--- Measure CPU core clock frequency by Time Stamp Counter (TSC) --;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Check Passed, 1(C)=Check Failed        ;
;          Variable CPUCLK_Period                                   ;
;-------------------------------------------------------------------;
Check_CPU_CLK:
;--- Synchronization with current seconds change ---
call Read_RTC_Seconds
mov bl,al
Wait_RTC_2:
call Read_RTC_Seconds
cmp bl,al
je Wait_RTC_2
;--- Measurement, wait 1 full second ---
mov bl,al
rdtsc
mov esi,eax
mov edi,edx
Wait_RTC_3:
call Read_RTC_Seconds
cmp bl,al
je Wait_RTC_3
rdtsc
sub eax,esi
sbb edx,edi
;--- Calculation with measurement result ---		
shl	rdx,32
lea	rsi,[rax+rdx]         ; Here RAX [127-64]=0
mov	rax,1000000000000000  ; Femptoseconds per Second
cqo                       ; D[127-64] = 0
test	rsi,rsi
jz	Bad_CPU_CLK           ; Go error if divisor = 0
div	rsi
mov	[_CPUCLK_Period+r15],rax  ; Femptosec. per TSC
;--- Exit if no errors, CF=0(NC) ---
clc
ret
;--- Exit if error, CF=1(C) ---
Bad_CPU_CLK:
stc
ret

;---------- Local subroutines --------------------------------------;

;--- Read seconds from RTC, wait for UIP=0 -----;
; UIP=Update In Progress flag,                  ;
; it must be 0 for valid read time.             ;
; Procedure optimized for speed (precision),    ;
; instead minimize code size.                   ;
; INPUT:   None                                 ;
; OUTPUT:  AL = RTC seconds counter             ;
;-----------------------------------------------;
Read_RTC_Seconds:
cli
;--- Wait for UIP=0 ---
Wait_RTC_1:
mov al,0Ah       ; Index=0Ah, control/status reg. 0Ah
out 70h,al
in al,71h        ; Read register 0Ah
test al,80h      ; Index=0Ah, Bit=7, UIP bit
jnz Wait_RTC_1
;--- Read seconds ---
mov al,00h       ; Index=00h, seconds register
out 70h,al
in al,71h        ; Read seconds register
;--- Exit ---
sti
ret

