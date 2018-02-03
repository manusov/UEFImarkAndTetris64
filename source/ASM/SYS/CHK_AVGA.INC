;===========================================================================;
;                                                                           ;
;        VERIFY COMPATIBILITY HANDLER FOR FIND ACTIVE VIDEO ADAPTER.        ;
;                                                                           ;
;===========================================================================;

;--- Scan PCI configuration space (MCFG), detect active VGA/XGA ----;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Check Passed, 1(C)=Check Failed        ;
;          Variable: Adapter_Cfg = Active VGA bus/device/function   ;
;-------------------------------------------------------------------;
Check_Active_Video:
;--- Scan PCI bus ---
mov rdi,PCI_Matches_1         ; RDI=Base address
mov rsi,PCI_Matches_2         ; RSI=Limit
mov eax,0030000h
mov edx,0FFFE00h              ; VGA=0300xxh, XGA=0301xxh
push rdi
call PCI_Scan_Class
pop rsi
jc Bad_Act_VGA                ; Go if error
jrcxz Bad_Act_VGA             ; Go if no entries
;--- Detect Active VGA ---
; Here RSI=List of B/D/F-entries, CX=Number of entries
Cycle_Act_VGA:	lodsw
mov [_Adapter_Cfg+r15],ax     ; This for PCIe device
mov [_Act_Video_Cfg+r15],ax   ; This required if owervrite
xchg ebx,eax                  ; BX=Bus/Device/Function

;--- v1.05 corrections ---
; This prevents hardware scan for Oracle VMBox,
; exit cycle if one adapter or this adaptor is last for scan.
cmp ecx,1 
je Exit_Act_VGA
;---    
		
call Check_Active_PCI_Video
jnc Exit_Act_VGA
loop Cycle_Act_VGA		; Note here LOOPQ, use RCX
;--- Exit points ---
Bad_Act_VGA:
stc
Exit_Act_VGA:
ret
