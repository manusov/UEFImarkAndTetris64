;===========================================================================;
;                                                                           ;
;      VERIFY COMPATIBILITY HANDLER FOR CHECK ALL AND FREE MEMORY SIZE.     ;
;                                                                           ;
;===========================================================================;

;-- Check UEFI memory configuration and request pages --------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Flag: 0(NC)=Check Passed, 1(C)=Check Failed         ;
;          Memory control variables updated                         ;
;-------------------------------------------------------------------;
Check_Memory:
;--- Try memory allocate ---
xor rcx,rcx   ; RCX = EFI Memory Allocate Type

mov rdx,4     ; RDX = EFI Memory Type = BS DATA
mov r8,8192   ; R8  = Num. of contiguous 4KB pages

	;!	mov	rdx,2		; RDX = EFI Memory Type = LOADER DATA
	;!	mov	r8,8192*2	; R8  = Num. of contiguous 4KB pages

xor r9,r9        ; R9  = Limit, here not used
call EFI_Allocate_Pages
jc Exit_Memory    ; Go if error
;--- Calculate results ---
shl r8,12         ; Pages->Bytes
test dl,00001111b
jz Align_Memory		; Go if aligned 16
and dl,11110000b  ; Otherwise make alignment
add rdx,10h       ; Also set CF=0 (NC)
;--- Save results: Base and Size, include for file operations ---
; Note file buffer also used for generating memory map
Align_Memory:
mov [_Use_Mem_Base+r15],rdx
mov [_Use_Mem_Size+r15],r8
lea rax,[rdx+r8-131072]                ; match
mov [_File_Mem_Base+r15],rax
mov qword [_File_Mem_Size+r15],131072  ; match
;--- Set flag for memory release required ---
or byte [_Sys_Bitmap_L+r15],00010000b  ; D4=M.Flag

;--- Exit ---
clc
Exit_Memory:
ret

