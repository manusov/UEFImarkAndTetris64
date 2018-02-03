;===========================================================================;
;                                                                           ;
;              SET CONTEXT HANDLER FOR SET GRAPHICS VIDEO MODE.             ;
;                                                                           ;
;===========================================================================;

;--- Set graphics video mode ---------------------------------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF = Flag: 0(NC)=Operation Passed, 1(C)=Failed           ;
;          Variable Sys_Bitmap and context control variables        ;
;-------------------------------------------------------------------;
Setup_G_Mode:
;--- Set mode ---
mov rax,8                         ; RAX=GOP.ModeSet
mov rcx,[_EFI_Gop+r15]            ; [RCX+RAX]=Call
mov rdx,[_Target_Vmode+r15]       ; RDX=Mode
call EFI_API                      ; Return: RAX, CF
jc Exit_Smode                     ; Exit if error
;--- Set video memory base and size ---	
mov rbx,[rcx+018h]                ; RBX=Mode info table pointer
mov rax,[rbx+018h]                ; RAX=FrameBufferBase
mov [_Vmem_Base+r15],rax          ; Save base
mov rax,[rbx+020h]                ; RAX=FrameBufferSize
    
; This changes for UEFImark x64 1.06
; for correct value under Oracle VirtualBox
test rax,rax
jns @f
and rax,07FFFFFFFh
inc eax
@@:
mov [_Vmem_Size+r15],rax                 ; Save size
;--- Set flag for video mode restore and clear display required ---
or byte [_Sys_Bitmap_L+r15],00100000b    ; D5=V.Flag
;--- Here setup MTRR, because VRAM Base/Size data must be ready from GOP ---
test byte [_Sys_Bitmap_L+r15],00000100b
jz Exit_Smode
call Setup_MTRR
;--- Exit ---
Exit_Smode:
ret


