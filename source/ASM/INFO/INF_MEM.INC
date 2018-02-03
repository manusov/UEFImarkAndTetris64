;===========================================================================;
;                                                                           ;
;      BUILT INFORMATION STRING HANDLER FOR: ALL AND FREE MEMORY SIZE.      ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for base and extended memory size ----;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_System_Memory:
;--- Write left part of string ---
mov rsi,String_Mem
call String_Copy_Simple
;--- Get memory map, use File I/O buffer ---
mov rcx,[_File_Mem_Size+r15]
mov rdx,[_File_Mem_Base+r15]
push rdx
call EFI_Get_Memory_Map
pop rdx
jc Inf_Mem_1
;--- Calculate normal memory size ---
mov ebp,000000FEh         ; Mark types [1-7] = 1
call Local_Inf_Mem_1
;--- Write normal memory size, yet 32-bit EAX only, 4GB*1024=4TB max ---
push rbx                  ; RBX=Map entry size
mov bl,0                  ; BL=Template mode
call Decimal_Print_32
pop rbx
;--- Save normal memory size, used when extreme context built ---
shl rax,10                ; Kilobytes->Bytes
mov [_EFI_Mem_Total+r15],rax
;--- Write middle string ---
call String_Copy_Simple
;--- Calculate ACPI-reserved memory size ---
mov ebp,00000600h         ; Mark types [9-10] = 1
call Local_Inf_Mem_1
;--- Write ACPI-reserved memory size, yet 32-bit EAX only, 4TB max ---
mov bl,0                  ; BL=Template mode
call Decimal_Print_32
call String_Copy_Simple
;--- Check and write physical address width ---
movzx eax,byte [_Address_Size+r15]
cmp al,0
je Inf_Mem_1              ; Go if parameter not valid
call String_Copy_Simple   ; This not corrupt RAX
call Decimal_Print_32
call String_Copy_Simple
;--- Exit points ---
mov al,0
stosb
ret
Inf_Mem_1:
mov ax,'?'
stosw
ret

;---------- Local subroutines ----------------------------------------------;

;--- Calculate total size of selected type memory pages -------;
; INPUT:   EBP = Type selection bitmap                         ;
;          RDX = Base of memory map                            ;
;          RCX = Size of memory map, bytes                     ;
;          RBX = One descriptor (map element) size, bytes      ;
;                                                              ;
; OUTPUT:  RAX = Calculated size, kilobytes                    ;
;--------------------------------------------------------------;
Local_Inf_Mem_1:
push rdx rcx r8
xor r8,r8           ; Clear sum=size
Inf_Mem_10:
mov eax,[rdx+000h]  ; Get memory type
cmp eax,31
ja Inf_Mem_11       ; Go if too big type
bt ebp,eax
jnc Inf_Mem_11      ; Go if this type not add
add r8,[rdx+018h]   ; Add number of pages
Inf_Mem_11:
add rdx,rbx         ; Advance memory map pointer
sub rcx,rbx         ; Subtract from map size
ja Inf_Mem_10
mov rax,r8
shl rax,2           ; 4KB Pages -> Kilobytes
pop r8 rcx rdx
ret
