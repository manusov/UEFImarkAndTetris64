;===========================================================================;
;                                                                           ;
;    SUBROUTINES FOR UEFI MEMORY INFO/ALLOCATE/FREE OPERATIONS SUPPORT.     ;
;                                                                           ;
;===========================================================================;

;------------------------------------------------------------;
; EFI Memory Descriptor format, note alignment 8 for x64 UEFI
; Address, hex    Size       Comments
;------------------------------------------------------------;
; 00-03           Dword      Memory Type, see definition
; 04-07           Dword      Reserved for alignment
; 08-0F           Qword      EFI Physical Address
; 10-17           Qword      EFI Virtual Address
; 18-1F           Qword      Number of pages
; 20-27           Qword      Memory Attribute, see definition
;------------------------------------------------------------;
;
; Memory Type definition
; Num, hex    Type
;------------------------------------------------------------;
; 00          EfiReservedMemoryType 
; 01          EfiLoaderCode
; 02          EfiLoaderData
; 03          EfiBootServicesCode
; 04          EfiBootServicesData
; 05          EfiRuntimeServicesCode
; 06          EfiRuntimeServicesData
; 07          EfiConventionalMemory
; 08          EfiUnusableMemory
; 09          EfiAcpiReclaimMemory
; 0A          EfiAcpiMemoryNvs
; 0B          EfiMemoryMappedIo
; 0C          EfiMemoryMappedIoPortSpace
; 0D          EfiPalCode
; 0E          EfiMaxMemoryType (limit value only)
;
; Memory Allocate Type definition
; Num, hex    Type
;------------------------------------------------------------;
; 00          AllocateAnyPages
; 01          AllocateMaxAddress
; 02          AllocateAddress
; 03          MaxAllocateType (limit value only)
;
; Memory Attribute definition, not listed bits reserved
; Bit   Attribute
;------------------------------------------------------------;
; 00    UC = Uncacheable
; 01    WC = Write Combining
; 02    WT = Write Through (for cache)
; 03    WB = Write Back (for cache)
; 04    UCE = Uncacheable exported (for atomic ops. support)
; 12    WP = Write Protected (by hardware)
; 13    RP = Read Protected (by hardware)
; 14    XP = Execution Protected (for code fetch, by hardw.)
; 63    Runtime, means allocatable for paging
;------------------------------------------------------------;


;--- Get UEFI firmware memory map ---------------------;
; INPUT:   R15 = Global variables pool base address    ;
;          RCX = Memory map size limit                 ;
;          RDX = Base address for return memory map    ;
;          Use 64-bit flat memory addressing           ;
;                                                      ;
; OUTPUT:  CF  = Flag: 0(NC)=Passed, 1(C)=Failed       ;
;          RAX = UEFI Status                           ;
;          RCX = Size updated, can be >Input or <Input ;
;                if Size>Input, biggest buffer req.    ;
;          RDX = Map key                               ;
;          RBX = One descriptor size                   ;
;          RBP = Descriptor version                    ;
;          Memory map returned if status OK            ;
;------------------------------------------------------;
EFI_Get_Memory_Map:
push rsi rdi r8 r9 r10 r11 r12 r13 r14 r15
;--- Prepare call pointer ---
mov rbx,[_EFI_Table+r15]  ; RBX=EFI_SYS_TABLE
mov rbx,[rbx+060h]        ; RBX=EFI_SYS_TABLE.BootServ
;--- Built caller stack frame ---
push rcx
mov rcx,rsp         ; Parm#1=Pointer to map size
; rdx is direct     ; Parm#2=Pointer to map base
xor rax,rax         ; Pre-clear scratch pad
push rax
mov r8,rsp          ; Parm#3=Pointer to map key
push rax
mov r9,rsp          ; Parm#4=Pointer to dsc. size
push rax
mov rax,rsp         ; Parm#5=Pointer to dsc. ver.
;--- Built safe stack frame ---
sub rsp,40          ; Prevent AMI shadow bug
;--- Built callee stack frame ---
; rax already set   ; Parm#5 = Qword at stack
push rax            ; Push qword #1
; r9 already set    ; Parm#4 = R9
push r9             ; Push qword #2
; r8 already set    ; Parm#3 = R8
push r8             ; Push qword #3
; rdx already set   ; Parm#2 = RDX
push rdx            ; Push qword #4
; rcx already set   ; Parm#1 = RCX
push rcx            ; Push qword #5
call qword [rbx+018h+020h]	; Call function: get mem. map
add rsp,40+40		; Remove parms+reserved area
;--- Get output parameters from stack ---
pop rbp         ; RBP=Descriptor version
pop rbx         ; RBX=Descriptor size
pop rdx         ; RDX=Map key
pop rcx         ; RCX=Size
;--- Analysing status and exit ---
test rax,rax    ; Check RAX = UEFI status
jz Exit_Map     ; Go if UEFI no errors status
stc             ; CF=1 means error
Exit_Map:
pop r15 r14 r13 r12 r11 r10 r9 r8 rdi rsi
ret

;--- Allocate memory pages from the UEFI firmware -----;
; INPUT:   R15 = Global variables pool base address    ;
;          RCX = EFI Memory Allocate Type              ;
;          RDX = EFI Memory Type                       ;
;          R8  = Number of contiguous 4KB pages        ;
;          R9  = Address Value or Limit, used only for ;
;                AllocateAddress, AllocateMaxAddress   ;
;          Use 64-bit flat memory addressing           ;
;                                                      ;
; OUTPUT:  CF  = Flag: 0(NC)=OK, 1(C)=Failed           ;
;          RAX = UEFI Status                           ;
;          RDX = Base address, if allocate OK          ;
;------------------------------------------------------;
EFI_Allocate_Pages:
push rbx
;--- Prepare stack scratch pad at [rsp] for output ---
push r9
;--- Call function ---
mov rbx,[_EFI_Table+r15]  ; RBX=EFI_SYS_TABLE
mov rbx,[rbx+060h]        ; RBX=EFI_SYS_TABLE.BootServ
mov rax,018h+010h         ; RAX=EFI_Allocate_Pages
mov r9,rsp                ; Stack is work area, 1 qword
call EFI_API_1
pop rdx                   ; Get updated base address
;--- Exit ---
pop rbx
ret

;--- Free memory pages, get from the UEFI firmware ----;
; INPUT:   R15 = Global variables pool base address    ;
;          RCX = Physical Address                      ;
;          RDX = Number of contiguous 4KB pages        ;
;          Use 64-bit flat memory addressing           ;
;                                                      ;
; OUTPUT:  CF  = Flag: 0(NC)=OK, 1(C)=Failed           ;
;          RAX = UEFI Status                           ;
;------------------------------------------------------;
EFI_Free_Pages:
push rbx
;--- Call function ---
mov rbx,[_EFI_Table+r15]  ; RBX=EFI_SYS_TABLE
mov rbx,[rbx+060h]        ; RBX=EFI_SYS_TABLE.BootServ
mov rax,018h+018h         ; RAX=EFI_Free_Pages
call EFI_API_1
;--- Exit ---
pop rbx
ret
