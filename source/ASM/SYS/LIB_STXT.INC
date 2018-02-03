;===========================================================================;
;                                                                           ;
;                  SUBROUTINES FOR SAVE REPORT AS TXT FILE.                 ;
;                                                                           ;
;===========================================================================;

;---------- Save report to TXT file ----------------------------------------;
; INPUT:   R15 = Global variables pool base address                         ;
;          Use 64-bit flat memory addressing, without segments              ;
;          Global variables                                                 ;
; OUTPUT:  None                                                             ;
;---------------------------------------------------------------------------;
Save_Report_to_TXT:
push rax rbx rcx rdx rsi rdi rbp r8 r9
mov byte [_EFI_Action+r15],1     ; 1=Save text report

;--- Checkpoint 0F0h = Entry procedure save report to TXT file ---
CHK 0F0h

;--- Check free memory ---
;   Reserved
;!  jb Mem_Error_STXT

;--- Setup file path and try generating unique file name ---
; Subroutine Path_Built return:
; R8  = Pointer to file path\name string, UNICODE
; RDI = Pointer to free buffer for file I/O
;----------
xor ebp,ebp                      ; EBP=File Number Value
Re_Create_STXT:
mov rdi,Name_TXT+4               ; RDI=Name pointer
mov eax,ebp                      ; EAX = Number value xxxx
mov bl,4                         ; Template = 4 chars
call Decimal_Print_32            ; Generate 'TEXTxxxx.TXT'
mov rsi,Name_TXT
call EFI_File_Path_Built         ; R8=Name, RDI=Buffer
mov rcx,[_EFI_Dir_Volume+r15]    ; RCX=Parent handle
xor rax,rax                      ; RAX=Attributes
mov r9,0000000000000003h         ; R9=Open mode: Open
call EFI_Open_Create_File        ; This for check file exist
jnc Next_Name_STXT               ; Go if file exist
mov rbx,800000000000000Eh
cmp rax,rbx
je Create_Ok_STXT                ; Go if file not exist, OK
jmp File_Open_Error_STXT         ; Go if file open error
Next_Name_STXT:
call EFI_Close_File
jc File_Close_Error_STXT         ; Go if file close error
inc ebp                          ; Modify file number
cmp bp,9999
ja File_Num_Error_STXT           ; Overflow ?
jmp Re_Create_STXT               ; Go create with new number
;--- Create new file, here R8=Pointer to name ---
Create_Ok_STXT:
mov rcx,[_EFI_Dir_Volume+r15]    ; RCX=Parent handle
xor rax,rax                      ; RAX=Attributes
mov r9,8000000000000003h         ; R9=Open mode: Create
call EFI_Open_Create_File        ; This for check file exist
jc File_Open_Error_STXT          ; Go if file create error

;--- Initialize base address ---
mov rdi,[_File_Mem_Base+r15]
mov r8,rdi

;--- Built report header ---
mov rsi,String_Version
call String_Copy_Simple
mov rsi,Program_Vendor_Data_1
call String_Copy_Simple
mov rsi,Report_Middle
call String_Copy_Simple
;!	mov	rsi,Report_Title
;!	call	String_Copy_Simple

;--- Built date and time string ---
call Report_Date_Time_String
mov rsi,Buffer_5
call String_Copy_Simple
mov rsi,Report_Middle_2
call String_Copy_Simple

;--- Built report strings, copy from benchmark screen results ---
mov rsi,Buffer_2
Read_Text:
lodsb
cmp al,0FFh
je End_Text
cmp al,000h
jne Write_Text
mov ax,0A0Dh
stosw
jmp Read_Text
Write_Text:
stosb
jmp Read_Text

;--- Save report file, here valid RDI=generated text block end address ---
End_Text:
mov r8,[_File_Mem_Base+r15]    ; R8=Buffer
mov rdx,rdi
sub rdx,r8                     ; RDX=Size
mov rbx,rdx                    ; RBX=Required size
call EFI_Write_File            ; RCX=Handle
jc File_Write_Error_STXT       ; Go if file write error
cmp rbx,rdx
jne Disk_Full_Error_STXT       ; Go if disk full

;--- Close file, input RCX=Handle ---
Close_STXT:
call EFI_Close_File
jc File_Close_Error_STXT       ; Go if file close error

;--- Reserved for context restore ---
Restore_STXT:
;
;--- Checkpoint 0F3h = Exit procedure save screen to TXT file ---
CHK 0F3h
;--- Exit ---
pop r9 r8 rbp rdi rsi rdx rcx rbx rax
ret

;---------- Errors handling ------------------------------------------------;

Mem_Error_STXT:              ; Entry for Memory errors
; Reserved
jmp Restore_STXT
File_Open_Error_STXT:        ; Entry for File Open Error
mov dh,11
Error_STXT:
mov dl,3
mov [_EFI_Act_Op+r15],dx
mov [_EFI_Status+r15],rax
jmp Restore_STXT
File_Close_Error_STXT:       ; Entry for File Close Error
mov dh,12
jmp Error_STXT
File_Write_Error_STXT:       ; Entry for File Write Error
mov dh,14
jmp Error_STXT
File_Num_Error_STXT:         ; Entry for can't assign num.
mov rax,8000000000000009h    ; Error=OUT_OF_RESOURCES
jmp File_Write_Error_STXT
Disk_Full_Error_STXT:
mov rax,800000000000000Bh    ; Error=VOLUME_FULL
jmp File_Write_Error_STXT

