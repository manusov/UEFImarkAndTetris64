;===========================================================================;
;                                                                           ;
;        SUBROUTINES FOR PCI CONFIGURATION SPACE OPERATIONS SUPPORT.        ;
;                                                                           ;
;===========================================================================;

;--- Scan PCI Bus for devices matched BaseClass/SubClass/PI -----------;
; INPUT:   R15 = Global variables pool base address                    ;
;          Use 64-bit flat memory addressing, without segments         ;
;          RDI = Pointer to destination buffer                         ;
;          RSI = Destination buffer size limit, exit when RDI=RSI      ;
;          EAX = CMP-mask for BaseClass/SubClass/PI                    ;
;          EDX = AND-mask for BaseClass/SubClass/PI                    ;
;                                                                      ;
; OUTPUT:  CF  = Status: 0(NC)=No errors, 1(C)=Error                   ;
;          AH  = PCI Status, if CF=1, not used yet, reserved           ;
;                  when CF=1 and AH=0 means register content error     ;
;          RDI = Modify by output data store                           ;
;          RCX  = Number of entries in the output buffer               ;
;          Memory at RDI contain 16-bit entries:                       ;
;          Bus 8-bit / Device 5-bit / Function 3-bit                   ;
;----------------------------------------------------------------------;
PCI_Scan_Class:
cld
push rbp rbx rdi rsi rdx rax
;--- Get detect PCI bus (MCFG) result and setup parameters ---
mov rsi,[_MCFG_Base+r15]
test rsi,rsi
jz Bad_Scan_PCI
xor rbx,rbx             ; RBX, but not RSI 4GB-align
;--- Start scan cycle ---
Scan_PCI:	
cmp	dword [rsi+rbx],0FFFFFFFFh  ; Reg=0, VID:DID
je Next_Dev                     ; Go if VID:DID=FFFFh:FFFFh
;--- Read Header type ---
mov dl,[rsi+rbx+0Eh]    ; Reg=0Eh, Header type
;--- Check single/multi function ---
Scan_Fnc:
mov ebp,ebx
shr ebp,12              ; Skip 12-bit reg. address
test dl,dl
js Multi_Func
test ebp,07h            ; Skip fnc. 1-7 if Single-Fn.
jnz Next_Dev
;--- Scan functions of multi-function ---
Multi_Func:	
cmp	dword [rsi+rbx],0FFFFFFFFh   ; Reg=0, VID:DID
je Next_Fnc             ; Bus:Dev:Fnc empty ?
;--- Detect entry ---
mov eax,[rsi+rbx+008h]  ; Reg=08h, Base/Sub/PI/Rev
shr eax,8
and eax,[rsp+008h]      ; EDX image = AND-mask
cmp eax,[rsp+000h]      ; EAX image = CMP-mask
jne Next_Fnc
;--- Save entry: Bus/Device/Function, format: 8/5/3 bits ---
mov eax,ebp
stosw
;--- Check output buffer size limit ---
cmp rdi,[rsp+010h]
jae Bad_Scan_PCI
;--- Continue scan cycle, next function ---
Next_Fnc:
and ebp,07h          ; Here bits [2-0] = Function Address
cmp ebp,07h
je Next_Dev          ; Go if function 7 already scanned
add ebx,00001000h    ; Function number+1
jmp Scan_Fnc
;--- Cycle for next device ---
Next_Dev:
add ebx,00008000h    ; Increment bits [17-15] = Device
and ebx,0FFF8000h    ; Set Function = 0
jz Exit_Scan_PCI     ; Bus/Device Address not overflow ?
cmp rbx,[_MCFG_Size+r15]
jb Scan_PCI
;--- End of scan cycle ---
Exit_Scan_PCI:
mov rcx,rdi
sub rcx,[rsp+018h]
shr rcx,1            ; RCX=Number of 2-byte entries
;--- Exit point, note RAX and RDI not restored ---
clc
Exit_Sc_PCI:
pop rdx rdx rsi rbx rbx rbp
ret
Bad_Scan_PCI:
stc
jmp Exit_Sc_PCI

;---------- Scan for required PCI Capability Structure ----------------;
; INPUT:   R15 = Global variables pool base address                    ;
;          Use 64-bit flat memory addressing, without segments         ;
;          RBX = PCI Bus/Device/Function address in the MCFG MMIO      ;
;          AL  = Structure type byte, 8-bit for Standard Capabilities  ;
;                                                                      ;
; OUTPUT:  CF  = Error flag: 0(NC)=No Errors, 1(C)=Bad PCI status      ;
;          ZF  = Status flag: 0(NZ)=Structure not found, 1(Z)=Found    ;
;          AH  = If error, when CF=1(C):                               ;
;                Reserved for PCI status, if CF=1                      ;
;                when CF=1 and AH=0 means register content error       ;
;          AL  = If no errors, CF=0(NC) and structure found ZF=1(Z):   ;
;                PCI Register address for structure                    ;
;                8-bit for Standard Configuration Space                ;
;----------------------------------------------------------------------;
PCI_Scan_Capability:
push rbx rcx rdx rdi
xchg edx,eax        ; DL=Structure type byte
mov	cl,[rbx+0034h]  ; Register=0034h, CAP.PTR.
;--- Cycle for scan capabilities chain ---
Re_Scan_Cap:
test cl,cl
jz N_Fnd_PCI_Cap    ; Stop if CAP.PTR=0
test cl,00000011b
jnz Error_PCI_Cap   ; Must be align 4
movzx rdi,cl
mov cx,[rbx+rdi]    ; Read Cap. Structure
xchg cl,ch          ; CL=Next Cap. or zero=stop
cmp ch,dl           ; DL=Capability Code, by Input
jne Re_Scan_Cap
;--- Set output parameter AL=Register ---
xchg eax,edi
;--- End of cycle ---
Exit_PCI_Cap:
pop rdi rdx rcx rbx
ret
N_Fnd_PCI_Cap:
or al,0FFh          ; CF=0(NC), ZF=0(NZ)
jmp Exit_PCI_Cap
Error_PCI_Cap:
stc                 ; CF=1(C), ZF=Don't care
jmp Exit_PCI_Cap

;---------- Scan for required PCI Extended Capability Structure -------;
; INPUT:   R15 = Global variables pool base address                    ;
;          Use 64-bit flat memory addressing, without segments         ;
;          RBX = PCI Bus/Device/Function address in the MCFG MMIO      ;
;          AX  = Structure type byte, 16-bit for Extended Capabilities ;
;                                                                      ;
; OUTPUT:  CF  = Error flag: 0(NC)=No Errors, 1(C)=Bad PCI status      ;
;          ZF  = Status flag: 0(NZ)=Structure not found, 1(Z)=Found    ;
;          AH  = If error, when CF=1(C):                               ;
;                Reserved for PCI status, if CF=1                      ;
;                when CF=1 and AH=0 means register content error       ;
;          AX  = If no errors, CF=0(NC) and structure found ZF=1(Z):   ;
;                PCI Register address for structure                    ;
;                12-bit for Extended Configuration Space               ;
;----------------------------------------------------------------------;
PCI_Scan_Extended_Capability:
push rbx rcx rdx rdi
xchg edx,eax       ; DX=Structure type word
mov cx,0100h       ; Reg=0100h, Ext. Cap. Base
;--- Cycle for scan capabilities chain ---
Re_Scan_E_Cap:
test cx,cx
jz N_Fnd_PCI_Cap   ; Stop if CAP.PTR.=0
test cl,00000011b
jnz Error_PCI_Cap  ; Must be align 4
cmp cx,0100h
jb Error_PCI_Cap   ; Must be above 256 regs.
movzx rdi,cx
mov ecx,[rbx+rdi]  ; Read Cap. Structure
mov ax,cx          ; AX=Ext.Cap. ID
shr ecx,20         ; CX=Next Cap. Address
cmp ax,dx          ; DX=Capability Code, by Input
jne Re_Scan_E_Cap
;--- Set output parameter AL=Register ---
xchg eax,edi
;--- End of cycle ---
jmp Exit_PCI_Cap

;---------- Detecting Selected Video Adapter Activity ----------------------;
; INPUT:  R15 = Global variables pool base address                          ;
;         Use 64-bit flat memory addressing, without segments               ;
;         BX = Bus:Device:Function at 8:5:3 format, adapter cfg. address    ;
; OUTPUT: CF = Status Flag, 0(NC)=Adapter active,                           ;
;                           1(C)=Adapter passive or error.                  ;
;---------------------------------------------------------------------------;
; Method: Try to disable selected VGA by clear I/O Enable bit.              ;
;         If VGA ports disabled by I/O Enable=0, this VGA is active         ;
;         If VGA ports not disabled by I/O Enable=0, this adapter passive,  ;
;         Bus:Device:Function contain I/O enable bit is address of other    ;
;         (passive) adapter.                                                ;
;---------------------------------------------------------------------------;
Check_Active_PCI_Video:
push rax rbx rdx
;--- Prepare configuration address, for MCFG MMIO ---
movzx rbx,bx
shl rbx,12
add rbx,[_MCFG_Base+r15]
;--- Access VGA I/O ports ---
mov dx,03C4h      ; One of VGA Ports
mov al,1
out dx,al         ; Write INDEX
inc edx
in al,dx          ; Read indexed DATA
dec edx
cmp al,0FFh       ; FFh means adapter disabled
je This_Error	; Go if no adapters before disable
;--- Access PCI registers ---
cli               ; (!)
mov cx,[rbx+4]    ; Read PCI COMMAND REGISTER
xor cl,01h
test cl,01h
jnz Scan_Act_Done
mov [rbx+4],cx    ; Write PCI COMMAND REGISTER (IO=0)
;--- Access VGA I/O ports ---
mov al,1
out dx,al
inc edx
in al,dx
cmp al,0FFh       ; FFh means adapter disabled
pushf
xor cl,01h
mov [rbx+4],cx    ; Write (Restore) PCI COMMAND REG.
popf
Scan_Act_Done:
jnz This_Passive  ; If ZF=0 (NZ), this adapter passive
;--- Exit points ---
clc               ; CF=0(NC) means no errors
This_Exit:
sti               ; (!)
pop rdx rbx rax
ret
This_Error:
This_Passive:
stc               ; CF=1(C) means error
jmp	This_Exit

;---------- Extract address from Memory BAR and verify BAR validity ---;
; INPUT:   RBX = PCI Bus/Device/Function address in the MCFG MMIO      ;
;          AL  = BAR address, legal values: 10h,14h,18h,1Ch,20h,24h    ;
;                not-header BARs also possible for device-specific     ;
;                                                                      ;
; OUTPUT:  CF  = Error flag: 0(NC)=No Errors, 1(C)=Error               ;
;          AH  = Reserved for status, if CF=1, not used yet            ;
;                when CF=1 and AH=0 means register content error       ;
;                (AH not used yet)                                     ;
;          RDX = Base MMIO Address, control bits masked                ;
;                if MMIO above 4GB not sup., ECX (address bits 63-32)  ;
;                must check caller, not this subroutine                ;
;                (x86 can sup. memory above 4GB, by PAE or x86-64)     ;
;          RCX = MMIO Range size                                       ;
;----------------------------------------------------------------------;
PCI_Detect_Memory_BAR:
push rsi rdi rax
xor rdx,rdx           ; Pre-blank
push rdx rdx          ; Reserve space for RCX, RDX
;--- Check Base Address Register (BAR(i)) ---
movzx rdi,al          ; 4 bytes of Registers (i)
mov ecx,[rbx+rdi]
mov eax,ecx
mov cl,00h            ; Clear bits D0-D7
mov [rsp+00],ecx      ; Update EDX image
and al,11110111b      ; Prefetchable bit don't care
jz PDMB_9             ; Good 00000000b or 00000100b
cmp al,00000100b
jne PDMB_5            ; Bad control or alignment ?
;--- Section for 64-bit BAR only (check BAR(i+1))---
cmp edi,24h           ; Cannot extend to 64-bit if last BAR
je PDMB_5             ; Exit if width not extensible
add edi,4             ; BAR(i+1) address
mov esi,edi           ; Also clear bits RSI.[63-32]
mov al,11111101b      ; Clear D1 = Memory Slave Bit
call PCI_BAR_Sizing   ; Sizing test for BAR extension 64
jc PDMB_1             ; Exit if error
;--- Update output parameter: range size, high dword=EDI ---
mov eax,esi
not eax
mov [rsp+12],eax    ; Update RCX.[63-32] image
;--- Analyse read-back ---
test esi,esi
jz PDMB_10          ; Good if 00000000h
inc esi
jnz PDMB_5          ; Error if not FFFFFFFFh
PDMB_10:
mov ecx,[rbx+rdi]
mov [rsp+04],ecx    ; Update RDX.[63-32] image
jmp PDMB_8
;--- Section for 32-bit BAR only ---
PDMB_9:
test eax,0FFF00000h
jz PDMB_5                ; Error if below 1MB
;--- BAR width-specific sections done ---
;--- Verify BAR(i) ---
PDMB_8:
movzx rsi,byte [rsp+16]  ; PCI BAR(i) address
mov al,11111101b         ; Clear D1 = Memory Slave Bit
call PCI_BAR_Sizing      ; Sizing test for BAR
;--- Update output parameter: range size, low dword=ESI ---
mov eax,esi
and al,11110000b
not eax
inc eax
mov [rsp+08],eax  ; Update ECX image
;--- Analyse read-back ---
xchg eax,esi      ; Good 00000000b or 00000100b
and al,11110011b  ; Prefetchable bit don't care
jnz PDMB_5        ; Bad control or alignment ?
call PCI_BAR_Check_Integrity
jc PDMB_5         ; Exit if 0/1 integrity error
;--- Exit points ---
PDMB_11:
pop rdx rcx rax rdi rsi
ret
PDMB_1:    ; Reserved for status update, AH image in the stack
PDMB_12:
stc
jmp PDMB_11
PDMB_5:    ; Reserved for status update, AH image in the stack
jmp PDMB_12

;---------- Sizing PCI Address BAR ------------------------------------;
; INPUT:   RBX = PCI Bus/Device/Function address in the MCFG MMIO      ;
;          RSI = PCI Register address                                  ;
;          AL  = Bitmask for AND with PCI Command register             ;
;                                                                      ;
; OUTPUT:  CF  = Error flag: 0(NC)=No Errors, 1(C)=Bad PCI status      ;
;          AH  = Reserved for status, if CF=1                          ;
;          ESI = Bitmap after PCI BAR Read Back                        ;
;----------------------------------------------------------------------;
PCI_BAR_Sizing:
push rcx rdx
cli                ; (!) Start of critical operation
;--- Save PCI Command Register ---
mov	cx,[rbx+04]    ; Reg. 04h = PCI Command
push rcx           ; Save PCI Command Register
;--- Modify PCI Command Register ---
and cl,al          ; Clear Memory or IO access enable
mov [rbx+04],cx
;--- Save BAR(i) ---
mov eax,[rbx+rsi]  ; EAX = Storage
;--- Write pattern and read back ---
mov dword [rbx+rsi],0FFFFFFFFh  ; Write pattern
mov ecx,[rbx+rsi]  ; Read back
;--- Restore BAR(i), set output=ESI ---
mov [rbx+rsi],eax  ; Restore
mov esi,ecx
;--- Restore PCI Command Register ---
pop rcx            ; Restore PCI Command Register
mov [rbx+04],cx    ; Reg. 04h = PCI Command
;--- Exit ---
sti                ; (!) End of critical operation
clc
pop rdx rcx
ret

;---------- Check validity of PCI BAR Read Back Bitmap ----------------;
; INPUT:   EAX = Bitmap                                                ;
; OUTPUT:  CF  = Error flag: 0(NC)=No Errors, 1(C)=Bad Bitmap          ;
;----------------------------------------------------------------------;
PCI_BAR_Check_Integrity:
push rax rcx
mov rcx,32
Cycle1_CBI:
shl eax,1
jnc Cycle2_CBI
loop Cycle1_CBI    ; Here LOOPQ, use RCX
clc                ; All bits "1", BAR portion writeable
jmp Exit_CBI
Cycle2_CBI:
shl eax,1          ; Transfer here after first "0" detect
jc Exit_CBI
loop Cycle2_CBI    ; Here LOOPQ, use RCX
Exit_CBI:
pop rcx rax
ret

