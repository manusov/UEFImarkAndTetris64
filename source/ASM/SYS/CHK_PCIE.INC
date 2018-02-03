;===========================================================================;
;                                                                           ;
;   VERIFY COMPATIBILITY HANDLER FOR CHECK PCI EXPRESS INTERFACE PRESENT.   ;
;                                                                           ;
;===========================================================================;

;-- Check PCIE present for use fast write methods of video memory --;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Flag: 0(NC)=Check Passed, 1(C)=Check Failed         ;
;          Variable Sys_Bitmap and PCIE control variables           ;
;-------------------------------------------------------------------;
Check_PCIe:
mov r8l,2    ; Repeat twice for find separate 3D
;--- Blank variables ---
Rescan_3D:
mov byte [_PCIe_H_Reg+r15],0
mov byte [_PCIe_A_Reg+r15],0
;--- Scan for PCI-PCIe bridge ---
mov rdi,PCI_Matches_1  ; RDI=Base address
mov rsi,PCI_Matches_2  ; RSI=Limit
mov eax,0060400h
mov edx,0FFFFFEh       ; 060400, 060401
push rdi
call PCI_Scan_Class
pop rsi
jc Error_PCIe_1     ; Go if error
jrcxz Exit_PCIe_1   ; Go if no entries
;--- Detect bridge, parent bus provider ---
; Here RSI=List of B/D/F-entries, ECX=Number of entries
Cycle_PCIe:
mov rbx,[_MCFG_Base+r15]    ; RBX = MCFG base
lodsw                       ; AX=Host bus/device/function
mov bp,ax
movzx rax,ax
shl rax,12                  ; Convert to MCFG offset
add rbx,rax
mov edx,[rbx+0018h]
mov ax,[_Adapter_Cfg+r15]   ; AH=Target bus
shr edx,8                   ; DH=Subordinate bus, DL=Secondary bus
cmp ah,dl
jb Next_PCIe                ; Go if target bus < secondary bus
cmp ah,dh
jna Found_PCIe              ; Go if target bus <= subordinate bus
Next_PCIe:
loop Cycle_PCIe
;--- Additionally find separate 3D-controller ---
mov rdi,PCI_Matches_1   ; RDI (ES:DI) = Base address
mov rsi,PCI_Matches_2   ; RSI (SI) = Limit
mov eax,0030200h        ; Class=3D Controller
mov edx,0FFFFFFh
push rdi
call PCI_Scan_Class
pop rsi
jc Exit_PCIe_1       ; Go if error, not fatal
jrcxz Exit_PCIe_1    ; Go if no entries, not fatal
lodsw
mov [_Adapter_Cfg+r15],ax
or byte [_Sys_Bitmap_H+r15],00100000b	; Skip extr.
dec r8l
jnz	Rescan_3D
;--- Exit if bridge not found, no errors ---
Exit_PCIe_1:
jmp Exit_PCIe
;--- Parent bridge found ---
Found_PCIe:	
;--- PCIe Host ---
mov al,10h                 ; AL=PCIe Capability Code
call PCI_Scan_Capability
Error_PCIe_1:
jc Error_PCIe              ; Skip if error
jnz Exit_PCIe              ; Skip if PCIe not found
mov [_PCIe_H_Reg+r15],al
call Parse_PCIe_Capability
jc Error_PCIe              ; Skip if error
jz Exit_PCIe               ; Skip if no capabilities
mov [_PCIe_H_Caps+r15],al
mov [_PCIe_H_Ctrl+r15],cx
;--- Update address ---
mov [_Host_Cfg+r15],bp
;--- PCIe Adapter ---
movzx rbx,word [_Adapter_Cfg+r15]
shl rbx,12
add rbx,[_MCFG_Base+r15]   ; Add MCFG base to RBX
mov al,10h                 ; AL=PCIe Capability Code
call PCI_Scan_Capability
jc Error_PCIe              ; Skip if error
jnz Exit_PCIe              ; Skip if PCIe not found
mov [_PCIe_A_Reg+r15],al
call Parse_PCIe_Capability
jc Error_PCIe              ; Skip if error
jz Exit_PCIe               ; Skip if no capabilities
mov [_PCIe_A_Caps+r15],al
mov [_PCIe_A_Ctrl+r15],cx
;--- Check PCI Express re-programming possibility ---
mov dl,[_PCIe_H_Caps+r15]	
mov ah,dl       ; AH=Host, AL=Adapter
and ax,0707h    ; D[2-0]=Packet length sup.
cmp al,ah
jbe Min_AL
mov al,ah       ; AL=Maximum of both supported
Min_AL:
mov dl,[_PCIe_H_Ctrl_L+r15]	
mov ch,dl       ; CH=Host, CL=Adapter
and cx,0E0E0h   ; D[7-5]=Packet length set
shr cx,5
cmp cl,ch
jne Error_PCIe  ; Err. if Host/Adap. mismatch
cmp cl,al
ja Error_PCIe   ; Error if set > supported
je Exit_PCIe    ; Skip if set = supported
;--- Mark bitmap if PCI Express re-programming possible ---
or byte [_Sys_Bitmap_L+r15],00000010b  ; Bit D1=PCIe
;--- Exit if no errors, set CF=0(NC), both for possible/not ---
Exit_PCIe:
clc
ret
;--- Exit if errors detected, set CF=1(C) ---
Error_PCIe:
stc
ret

