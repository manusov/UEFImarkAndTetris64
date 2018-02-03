;===========================================================================;
;                                                                           ;
;   CHECK CONFIG. HANDLER FOR: VRAM BAR RELOCATION REQUIRED AND POSSIBLE.   ;
;                                                                           ;
;===========================================================================;

;--- Check Required/Possible for video RAM BAR relocation above 4GB -----;
; Required for prepare to redefine VRAM BAR Prefetchable and MTRR status ;
; on platforms with non-optimal initial settings.                        ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          PCI configuration addresses and offsets:                      ;
;          R14 = MCFG MMIO base address for PCIe video bridge registers  ;
;          R13 = MCFG MMIO base address for PCIe video adaptor registers ;
;          R12 = Offset, can use [r13+r12] for select video memory BAR   ;
;                                                                        ;
;          PBAR_Base (qword) = Base address for normal context           ;
;          PBAR_Size (qword) = Size for normal context                   ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=PCI PnP Failure        ;
;          PCI configuration variables:                                  ;
;          Ext_Relocate_Flag: 0=Relocation not requred or not supported  ;
;                             1=Relocation requred and supported         ;
;          Optional updated variables:                                   ;
;          Ext_PBAR_Base (qword) = New base address for extreme context  ;
;          Ext_PBAR_Size (qword) = New size for extreme context          ;
;------------------------------------------------------------------------;
Ext_Check_VRAM_Base:
;--- Get video memory range, check for 64-bit BAR ---
mov al,[r13+r12]    ; AL=Low byte of BAR
and al,00001111b    ; Mask analysed bits
cmp al,00001100b    ; PBAR=1b, Width=10b, M/IO=0b
jne Exit_ECVB       ; Exit if not PBAR,64,Mem

;--- Prepare base and size ---
mov rbx,[_PBAR_Base+r15]   ; RBX = PBAR base
mov rcx,[_PBAR_Size+r15]   ; RCX = PBAR size
;--- Temporarily set Extreme Base,Size = Current Base,Size ---
mov [_Ext_PBAR_Base+r15],rbx
mov [_Ext_PBAR_Size+r15],rcx

;--- Check special support for Integrated Video Adapter at Bus 0 --- @
cmp byte [_Act_Video_Bus+r15],0
je Integrated_ECVB

;--- Calculate limit ---
lea rcx,[rbx+rcx-1]    ; RCX = PBAR limit
;--- Check bridge prefetchable range, check for 64-bit range ---
mov al,[r14+0026h]     ; PmemLimit.Type
mov ah,al              ; Because x86-64 limitations
mov al,[r14+0024h]     ; PmemBase.Type
and ax,0F0Fh           ; Mask Type fields
cmp ax,0101h           ; Check both report 64-bit
jne Exit_ECVB          ; Exit if wrong types
;--- Set flag for BAR and Range both 64-bit ---
or byte [_Ext_Transit+r15],00000001b
;--- Get bridge prefetchable range ---
mov esi,[r14+0028h]    ; PmemBase.[63-32]
shl rsi,32
mov ax,[r14+0024h]     ; PmemBase.[31-20]
and rax,0FFF0h
shl eax,16
or rsi,rax             ; RSI = Pmem range base
mov edi,[r14+002Ch]    ; PmemLimit.[63-32]
shl rdi,32
mov ax,[r14+0026h]     ; PmemBase.[31-20]
and rax,0FFF0h
shl eax,16
lea rdi,[rax+0FFFFFh]  ; RDI = Pmem range limit
;--- Check video memory range out of bridge prefetchable range ---
; Video memory BAR:     RBX=Base, RCX=Limit
; Bridge memory range:  RSI=Base, RDI=Limit
;---
cmp rsi,rdi

; Note TYAN S5533, prefetchable, but not optimal, still required remap
;@ jb Exit_ECVB ; ! DEBUG
	
; YET SET FLAG ONLY IF BRIDGE PREFETCHABLE RANGE DISABLED
;!  jae Request_ECVB    ; Go if Pmem range disabled
;!  cmp rbx,rsi
;!  jb Request_ECVB     ; Go if PBAR out of Pmem
;!  cmp rcx,rdi
;!  jna Exit_ECVB       ; Go if PBAR in the Pmem
;--- Exit points, set requests if optimizing required and possible ---
; D0=Request memory space from firmware, D1=Relocate by PCI PnP
;---

Integrated_ECVB:
Request_ECVB:
or byte [_Ext_Requests+r15],00000011b
Exit_ECVB:
clc
ret
