;===========================================================================;
;                                                                           ;
;     VERIFY COMPATIBILITY HANDLER FOR FIND VIDEO RAM PREFETCHABLE BAR.     ;
;                                                                           ;
;===========================================================================;

;--- Find PCI PnP prefetchable BAR for video memory access ---------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF    = Flag: 0(NC)=Check Passed, 1(C)=Check Failed      ;
;          Variables: PBAR_Base, PBAR_Size                          ;
;-------------------------------------------------------------------;
Check_BAR:
;--- Pre-blank variables ---
xor rax,rax
mov [_PBAR_Base+r15],rax
mov [_PBAR_Size+r15],rax
mov [_PBAR_Reg+r15],al
;--- Detect video adapter BARs, get BX=adapter bus/device/function ---
movzx rbx,word [_Adapter_Cfg+r15]
shl rbx,12
add rbx,[_MCFG_Base+r15]		
mov rdi,0010h       ; First BAR = 0010h
Cycle_BAR:
mov ecx,[rbx+rdi]
mov al,cl
and cl,00001011b
cmp cl,00001000b    ; Prefetchable, 32/64-bit
je Found_BAR
and al,00000111b
cmp al,00000100b
jne Next_BAR        ; Go if not 64-bit memory BAR
add edi,4           ; Skip extension of 64-bit BAR
Next_BAR:
add edi,4
cmp edi,0024h       ; Last BAR = 0024h
jbe Cycle_BAR
;--- Exit if no errors but BAR not found, go here with CF=0(NC) ---
;--- Exit if errors, go here with CF=1(C) ---
Bad_BAR:
ret
;--- BAR found, detect parameters ---
Found_BAR:
xchg eax,edi
mov [_PBAR_Reg+r15],al      ; Used by extreme, AL=BAR Reg.
call PCI_Detect_Memory_BAR
jc Bad_BAR                  ; Go if error
;--- Store parameters ---
mov [_PBAR_Base+r15],rdx
mov [_PBAR_Size+r15],rcx
;--- Exit if no errors and BAR found, go here with CF=0(NC) ---
ret


