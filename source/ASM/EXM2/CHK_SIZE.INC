;===========================================================================;
;                                                                           ;
;    CHECK CONFIG. HANDLER FOR: VRAM BAR RESIZING REQUIRED AND POSSIBLE.    ;
;                                                                           ;
;===========================================================================;

;--- Check Required/Possible for video RAM BAR resizing -----------------;
; Required for prepare to redefine VRAM BAR size for linear addressing   ;
; all physically present video memory, use Resizable BAR Capability.     ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          PCI configuration addresses and offsets:                      ;
;          R14 = MCFG MMIO base address for PCIe video bridge registers  ;
;          R13 = MCFG MMIO base address for PCIe video adaptor registers ;
;          R12 = Offset, can use [r13+r12] for select video memory BAR   ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=PCI PnP Failure        ;
;          PCI configuration variables:                                  ;
;          Ext_Resize_Flag: 0=Resizing not requred or not supported      ;
;                           1=Resizing requred and supported             ;
;          Optional updated variables:                                   ;
;          Ext_PBAR_Base (qword) = New base address for extreme context  ;
;          Ext_PBAR_Size (qword) = New size for extreme context          ;
;------------------------------------------------------------------------;
Ext_Check_VRAM_Size:
;--- Check flag for BAR and Range both 64-bit ---
test byte [_Ext_Transit+r15],00000001b
jz Exit_ECVS         ; Go if not support 64-bit
;--- Check presence Resizable BAR Capability ---
mov rbx,r13          ; RBX=Video cfg. address
mov ax,0015h         ; AX=Capability ID
call PCI_Scan_Extended_Capability
jc Error_ECVS        ; Go if PCI PnP failure
jnz Exit_ECVS        ; Go if OK, but no capability
;--- Check presence valid fields for required BAR ---
add ax,4                      ; Skip capability header
movzx rax,ax
movzx rcx,byte [rbx+rax+4]    ; RsBAR control register
shr cl,5          ; RCX=Number of BARs
jz Error_ECVS     ; Go if error: N=0 BARs
cmp cl,6
ja Error_ECVS     ; Go if error: N>6 BARs
Scan_ECVS:
mov dx,[rbx+rax+4]
and dl,00000111b        ; DL=BAR index
shl dl,2                ; 4 bytes per BAR
add dl,10h              ; BAR0 at 10h ... BAR5 at 24h
cmp dl,[_PBAR_Reg+r15]  ; Compare PCI reg. address
je Found_ECVS     ; Go if BAR found
add ax,8
loop Scan_ECVS    ; Here LOOPQ (RCX)
jmp Exit_ECVS     ; Exit if not found
;--- Save capability structure register address ---
Found_ECVS:	mov	[_Resizable_Reg+r15],ax
;--- Get maximum size from Resizable BAR Capability structure ---
mov ecx,[rbx+rax]   ; RsBAR capability register
bsr ecx,ecx         ; ECX = Bit index
sub cl,4            ; CL=Max., 0 means 1MB
;--- Compare current and maximum supported BAR sizes ---
and dh,00011111b   ; DH=Current
cmp dh,cl
je Exit_ECVS       ; Go if Current=Max
ja Error_ECVS      ; Go error if Current>Max
;--- Exit points, set requests if optimizing required and possible ---
; D0=Request memory space from firmware, D2=Resize by Resizable BAR Cap.
;---
Request_ECVS:
or byte [_Ext_Requests+r15],00000101b
Exit_ECVS:
clc
ret
Error_ECVS:
stc
ret
