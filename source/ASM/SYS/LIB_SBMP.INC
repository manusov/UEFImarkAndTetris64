;===========================================================================;
;                                                                           ;
;          SUBROUTINES FOR SAVE SCREEN IMAGE AS BMP GRAPHICS FILE.          ;
;                                                                           ;
;===========================================================================;

;---------- Save screen image to BMP file ----------------------------------;
; Direct read video memory, not transit buffer, for benchmarks result       ;
; figure make accessible.                                                   ;
; Yet fixed for true color mode, video mode=32BPP, file=24BPP.              ;
; INPUT:   R15 = Global variables pool base address                         ;
;          Use 64-bit flat memory addressing, without segments              ;
;          Global variables                                                 ;
; OUTPUT:  None                                                             ;
;---------------------------------------------------------------------------;
Save_Screen_to_BMP:
push rax rbx rcx rdx rsi rdi rbp r8 r9
mov byte [_EFI_Action+r15],2     ; 2=Save screen image

;--- Checkpoint 0F4h = Entry procedure save screen to BMP file ---
CHK 0F4h

;--- Adjust BMP header to current video mode parameters ---
mov rbx,Save_BMP_Header
movzx eax,word [_Video_Xsize+r15]
movzx edx,word [_Video_Ysize+r15]
mov [rbx+18],eax                 ; Set Xsize, dword
mov [rbx+22],edx                 ; Set Ysize, dword
mul edx
imul eax,eax,3
mov [rbx+34],eax                 ; Set image size w/o header
add eax,14+40                    ; Prefix=14, Header=40 bytes
mov [rbx+02],eax                 ; Set file total size, dword

;--- Check free memory ---
;  Reserved
;! jb Mem_Error_SBMP
;--- Setup file path and try generating unique file name ---
; Subroutine Path_Built return:
; R8  = Pointer to file path\name string, UNICODE
; RDI = Pointer to free buffer for file I/O
;----------
xor ebp,ebp                      ; EBP=File Number Value
Re_Create_SBMP:
mov rdi,Name_BMP+4               ; RDI=Name pointer
mov eax,ebp                      ; EAX = Number value xxxx
mov bl,4                         ; Template = 4 chars
call Decimal_Print_32            ; Generate 'PICTxxxx.BMP'
mov rsi,Name_BMP
call EFI_File_Path_Built         ; R8=Name, RDI=Buffer
mov rcx,[_EFI_Dir_Volume+r15]    ; RCX=Parent handle
xor rax,rax                      ; RAX=Attributes
mov r9,0000000000000003h         ; R9=Open mode: Open
call EFI_Open_Create_File        ; This for check file exist
jnc Next_Name_SBMP               ; Go if file exist
mov rbx,800000000000000Eh
cmp rax,rbx
je Create_Ok_SBMP                ; Go if file not exist, OK
jmp File_Open_Error_SBMP         ; Go if file open error
Next_Name_SBMP:
call EFI_Close_File
jc File_Close_Error_SBMP         ; Go if file close error
inc ebp                          ; Modify file number
cmp bp,9999
ja File_Num_Error_SBMP           ; Overflow ?
jmp Re_Create_SBMP               ; Go create with new number

;--- Create new file, here R8=Pointer to name ---
Create_Ok_SBMP:
mov rcx,[_EFI_Dir_Volume+r15]    ; RCX=Parent handle
xor rax,rax                      ; RAX=Attributes
mov r9,8000000000000003h         ; R9=Open mode: Create
call EFI_Open_Create_File        ; This for check file exist
jc File_Open_Error_SBMP          ; Go if file create error

;--- Save BMP header ---
; Can't use complex open-write-close subroutines, because some writes
mov r8,Save_BMP_Header           ; R9=Buffer
mov rdx,Prefix_and_Header_Size   ; RDX=Size
mov rbx,rdx                      ; RBX=Required size
call EFI_Write_File              ; RCX=Handle
jc File_Write_Error_SBMP         ; Go if file write error
cmp rbx,rdx
jne Disk_Full_Error_SBMP         ; Go if disk full

;--- Prepare cycle for save BMP pixels array ---
; Note must not corrupt RCX=Handle
cld
mov rsi,[_Vmem_Base+r15]           ; RSI=Source (video memory)
movzx rbx,word [_Video_Xbytes+r15]
movzx rax,word [_Video_Ysize+r15]
dec rax                            ; Y-1
mul rbx                            ; Last line = X*(Y-1)
add rsi,rax                        ; Positioning to last line
add rbx,rax                        ; RBX=Total video array size, bytes
shr rbx,2                          ; Convert bytes to pixels, for 32BPP

;--- Start cycle for save BMP pixels array, write file ---
;--- Copy block from transit video buffer to file I/O buffer ---
; Note. Palette convert during copy, for minimize delays
; Note. Block size limited to 64KB, after porting from DOS, 
;       re-optimizing required
; Note must not corrupt RCX=Handle
;--- Prepare Xsize ---
movzx rbp,word [_Video_Xsize+r15]  ; RBP = Xsize, pixels
;--- Cycle for all blocks ---
Block_SBMP:
mov rdi,[_File_Mem_Base+r15]    ; RDI=Destination
mov rdx,21844                   ; 21844 pixels = 65532 bytes, 24BPP
cmp rbx,rdx
jbe Last_SBMP
sub rbx,rdx                     ; This for not last pass of copy
jmp Copy_SBMP
Last_SBMP:
mov rdx,rbx                     ; This for last pass of copy
xor rbx,rbx

;--- Cycle for one block, cycle unrolled for fast ---
Copy_SBMP:
push rdx
;--- First check end of line ---		
Cyc_Copy_SBMP:
sub rbp,1            ; DEC don't modify CF
jnc Pixel_SBMP       ; Not go if RBP=-1
movzx rbp,word [_Video_Xsize+r15]  ; RBP=Xsize, pixels
imul rax,rbp,4
sub rsi,rax
movzx rax,word [_Video_Xbytes+r15]
sub rsi,rax
dec rbp              ; Correction for FFFF/0000

;--- Copy with conversion ---
; Note because read video memory is slow and not cacheable,
; use PUSH/POP RAX instead re-read
Pixel_SBMP:
push rcx
lodsd           ; Get pixel dword, RSI+4
push rax
mov cl,[_Video_B_Lsb+r15]
shr eax,cl
mov cx,0FF08h
sub cl,[_Video_B_Width+r15]
shr ch,cl
and al,ch
stosb           ; Store BLUE byte, RDI+1
pop rax
push rax
mov cl,[_Video_G_Lsb+r15]
shr eax,cl
mov cx,0FF08h
sub cl,[_Video_G_Width+r15]
shr ch,cl
and al,ch
stosb           ; Store GREEN byte, RDI+1
pop rax
mov cl,[_Video_R_Lsb+r15]
shr eax,cl
mov cx,0FF08h
sub cl,[_Video_R_Width+r15]
shr ch,cl
and al,ch
stosb           ; Store RED byte, RDI+1
pop rcx

;--- End of one RGB conversion, cycle for low buffer ---
dec edx
jnz Cyc_Copy_SBMP            ; Cycle for 65532 bytes block

;--- Save buffer to file ---
pop rdx                      ; RDX=Length, pixels
imul rdx,rdx,3               ; RDX=Length, bytes
mov r8,[_File_Mem_Base+r15]  ; R8=Buffer address
mov rdi,rdx                  ; RDI=Required size
call EFI_Write_File          ; RCX=Handle
jc File_Write_Error_SBMP     ; Go if file write error
cmp rdi,rdx
jne Disk_Full_Error_SBMP     ; Go if disk full

;--- Cycle for entire video array ---
test ebx,ebx                 ; Also clear CF
jnz Block_SBMP               ; Cycle for all array

;--- Close file, input RCX=Handle ---
Close_SBMP:
call EFI_Close_File
jc File_Close_Error_SBMP     ; Go if file close error

;--- Reserved for context restore ---
Restore_SBMP:
;

;--- Checkpoint 0F7h = Exit procedure save screen to BMP file ---
CHK 0F7h

;--- Exit ---
pop r9 r8 rbp rdi rsi rdx rcx rbx rax
ret

;---------- Errors handling ------------------------------------------------;

Mem_Error_SBMP:              ; Entry for Memory errors
; Reserved
jmp Restore_SBMP
File_Open_Error_SBMP:        ; Entry for File Open Error
mov dh,11
Error_SBMP:
mov dl,4
mov [_EFI_Act_Op+r15],dx
mov [_EFI_Status+r15],rax
jmp Restore_SBMP
File_Close_Error_SBMP:       ; Entry for File Close Error
mov dh,12
jmp Error_SBMP
File_Write_Error_SBMP:       ; Entry for File Write Error
mov dh,14
jmp Error_SBMP
File_Num_Error_SBMP:         ; Entry for can't assign num.
mov rax,8000000000000009h    ; Error=OUT_OF_RESOURCES
jmp File_Write_Error_SBMP
Disk_Full_Error_SBMP:
mov rax,800000000000000Bh    ; Error=VOLUME_FULL
jmp File_Write_Error_SBMP

