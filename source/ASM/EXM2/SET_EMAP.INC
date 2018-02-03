; ( ! )
; NEED TRY CHECK NEW METHOD: USE PAGING FOR TRANSLATE LINEAR ADDRESSES OF
; LOW WINDOW INTO HIGH WINDOW, EXAMPLE:
; LINEAR SPACE E0000000H-EFFFFFFFH -> 00000003F0000000H-00000003FFFFFFFFH,
; THIS CONTEXT CAN BE COMPATIBLE WITH UEFI FIRMWARE, CAN USE GOP.BLT
; AND RUN OTHER APPLICATIONS.
; ( ! )
; OR USE ALLOCATE POOL INSTEAD ALLOCATE PAGES
; ( ! )


;===========================================================================;
;                                                                           ;
;     SET CONTEXT HANDLER FOR: MMIO REGION ALLOCATION BY UEFI FIRMWARE.     ;
;                                                                           ;
;===========================================================================;

;--- Request MMIO space for video RAM BAR relocation and resizing -------;
; Use EFI_Allocate_Pages service.                                        ;
;                                                                        ;
; INPUT:   R15 = Pointer to global variables pool, use flat 64-bit addr. ;
;          PCI configuration variables:                                  ;
;          Ext_Relocate_Flag (bit) = Relocation required and supported   ;
;          Ext_Resize_Flag (bit)   = Resize required and supported       ;
;          Ext_PBAR_Base (qword)   = New base addr. for extreme context  ;
;          Ext_PBAR_Size (qword)   = New size for extreme context        ;
;                                                                        ;
; OUTPUT:  CF = Error flag: 0(NC)=No errors, 1(C)=UEFI Error             ;
;          PCI configuration variables:                                  ;
;          Ext_Allocation_Flag: 0=Cannot allocate MMIO pages, skip remap ;
;                               1=Allocate MMIO pages successfully       ;
;          Optional updated variables (by free pages window location):   ;
;          Ext_PBAR_Base (qword) = New base address for extreme context  ;
;          Ext_PBAR_Size (qword) = New size for extreme context          ;
;------------------------------------------------------------------------;
Ext_Set_EFI_MMIO_Map:
;--- Check operation required or not ---
test byte [_Ext_Requests+r15],00000001b
jz Exit_ESEMM
;--- Set block maximum address = CPU physical address limit ---
mov cl,[_Address_Size+r15]
mov r9,1
shl r9,cl
dec r9         ; R9 = Limit, CPU physical addr.

;--- Set block size = New prefetchable BAR size ---
;! mov r8,[_Ext_PBAR_Size+r15]
;! shr r8,12       ; R8 = Num. of contiguous 4KB pages
;--- Set memory type = Memory Mapped I/O ---
;! mov rdx,0Bh     ; RDX = EFI Memory Type = MMIO
;--- Set allocation type = By maximum address ---
;! mov rcx,1       ; RCX = EFI Mem. Allocate Type = Max.
;--- Try memory allocate, call function ---
;! call EFI_Allocate_Pages
;! jc Exit_ESEMM   ; Go if memory allocation error
;!

; DEBUG METHOD
mov rax,[_EFI_Mem_Total+r15]
bsr rcx,rax
cmp cl,31
jae Correct_ESEMM
mov cl,31
Correct_ESEMM:
mov rdx,4       ; Already shifted by 2
shl rdx,cl
cmp r9,rdx      ; Check phys. addressability, set CF
jc Exit_ESEMM   ; Go if physical address > limit
sub rdx,[_Ext_PBAR_Size+r15]

;--- Relocate base address to region, returned by UEFI firmware ---
mov [_Ext_PBAR_Base+r15],rdx
;--- Exit point, here valid CF=Error flag ---
Exit_ESEMM:
ret
