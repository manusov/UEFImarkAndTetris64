;===========================================================================;
;                                                                           ;
;            LIBRARY FOR ACPI DETECTION AND TABLES INTERPRETING.            ;
;                                                                           ;
;===========================================================================;

; Notes.
; ACPI tables loaded at boot time can be contained in memory of type
; EfiAcpiReclaimMemory (recommended) or EfiAcpiMemoryNvs.
; ACPI FACS must be contained in memory of type EfiAcpiMemoryNvs.
; Use EFI CONFIGURATION TABLE for fast detect ACPI method,
; simple direct memory scan by memory map is not optimal.

;--- Find ACPI support by ACPI1 and ACPI2 GUIDs -------;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;                                                      ;
; OUTPUT:  CF  = Flag: 0(NC)=Found, 1(C)=Not found     ;
;          AL  = ACPI version                          ;
;          RBX = Pointer to RSDP, valid if found       ;
;------------------------------------------------------;
FindAcpiSupport:
push rcx rdx rbp
;--- Prepare counter for 2 GUIDs ---
mov ebp,2             ; 2 passes for ACPI 2.0/1.0
;--- Prepare GUID ---
ScanAcpiGuid:
mov rax,GuidAcpi20
cmp ebp,2
je LoadAcpiGuid
mov rax,GuidAcpi10
LoadAcpiGuid:
mov rdx,[rax+008h]
mov rax,[rax+000h]
;--- Prepare for scan EFI Configuration Table ---
mov rbx,[_EFI_Table+r15]   ; RBX=EFI_SYSTEM_TABLE
mov rcx,[rbx+068h]         ; RCX=Number of entries
mov rbx,[rbx+070h]         ; RBX=EFI_CONFIGURATION_TABLE
;--- Scan EFI Configuration Table ---
ScanCfgTable:
cmp rax,[rbx+000h]    ; Compare GUID, low qword
jne NextCfgEntry
cmp rdx,[rbx+008h]    ; Compare GUID, high qword
je FoundCfgEntry
NextCfgEntry:
add rbx,24
loop ScanCfgTable     ; Here is LOOPQ, use RCX
;--- Cycle for scan ACPI 1.0 if ACPI 2.0+ not found ---
dec ebp
jnz ScanAcpiGuid
;--- Exit points ---
BadFindAcpi:
stc                   ; CF=1(C) means not found
ExitFindAcpi:
pop rbp rdx rcx
ret
;--- Entry detected, parse RSDP at entry address RBX ---
FoundCfgEntry:
mov rbx,[rbx+010h]    ; RBX=UEFI ACPI data pointer
mov rax,'RSD PTR '
cmp [rbx],rax         ; Check ACPI RSDP signature
jne BadFindAcpi       ; Go if bad signature
cmp byte [rbx+00Fh],2
jae ModeAcpi2         ; Go if ACPI 2.0+ detected
;--- Support ACPI 1.0 mode ---
ModeAcpi1:
mov rcx,20            ; ACPI 1.0 length=20 bytes
mov rdx,rbx           ; RDX=Base address
call ChecksumAcpi     ; RSDP 1.0 checksum
jnz BadFindAcpi       ; Go if checksum bad
mov al,1              ; AL=1 means ACPI 1.0
ExitFindAcpi_1:
jmp ExitFindAcpi      ; This branch for checksum OK
;--- Support ACPI 2.0+ mode ---
ModeAcpi2:
mov ecx,[rbx+014h]    ; ACPI 2.0 length=variable
mov rdx,rbx           ; RDX=Base address
call ChecksumAcpi     ; RSDP 2.0 checksum
jnz ModeAcpi1         ; Try ACPI1 if checksum bad
mov al,[rbx+00Fh]     ; AL=ACPI version byte
jmp ExitFindAcpi_1    ; This branch for checksum OK

;--- Find ACPI table by signature ------------------------;
; INPUT:   R15 = Global variables pool base address       ;
;          Use 64-bit flat memory addressing              ;
;          AL  = ACPI version, select scan mode:          ;
;                 0 = Not used value                      ;
;                 1 = Use ACPI 1.0 RSDT, 32-bit pointers  ;
;                 2 = Use ACPI 2.0 XSDT, 64-bit pointers  ;
;                >2 = Same as 2                           ;
;          RBX = Pointer to RSDP, valid if found          ;
;          EDX = Table signature, 4 ASCII chars           ;
;                                                         ;
; OUTPUT:  CF  = Flag: 0(NC)=Found, 1(C)=Not found        ;
;          RBX = Pointer to table, valid if found         ;
;          RCX = Table limit, valid if found              ;
;                limit is last byte address+1             ;
;---------------------------------------------------------;
FindAcpiTable:
push rax rdx rsi rbp
;--- Select mode: ACPI 1.0 or 2.0+ ---
cmp al,2
jae TableAcpi2

;---------- Support ACPI 1.0 mode, RSDT with 32-bit pointers ---------------;
TableAcpi1:
mov esi,[rbx+010h]     ; ACPI 1.0 RSDT pointer
test esi,esi
jz BadFindTable_1      ; Go if pointer=0
;--- Verify RSDT signature and checksum ---
cmp dword [rsi],'RSDT' ; Check RSDT signature
jne BadFindTable_1     ; Go if RSDT mismatch
mov ecx,[rsi+004h]     ; RCX=Table length, bytes
mov rdx,rsi
call ChecksumAcpi     ; RSDT checksum
jnz BadFindTable_1    ; Go if checksum bad
;--- Scan RSDT (32-bit pointers) for detect required table ---
mov eax,[rsp+16]      ; EAX=Signature, input EDX
add rcx,rsi           ; RCX=Scan pointers limit
add rsi,024h          ; RSI=Scan pointers base
ScanRsdt:
mov edx,[rsi]         ; RDX=Scanned table
cmp [rdx],eax
je TargetFound        ; Go if table found
add rsi,4             ; 4 byte pointers at RSDT
cmp rsi,rcx
jb ScanRsdt
BadFindTable_1:
jmp	BadFindTable_2    ; Go if table not found

;---------- Support ACPI 2.0+ mode, XSDT with 64-bit pointers --------------;
TableAcpi2:
mov rsi,[rbx+018h]    ; ACPI 2.0 XSDT pointer
test rsi,rsi
jz BadFindTable_2     ; Go if pointer=0
;--- Verify XSDT signature and checksum ---
cmp dword [rsi],'XSDT'
jne BadFindTable_2    ; Go if XSDT mismatch
mov ecx,[rsi+004h]    ; RCX=Table length, bytes
mov rdx,rsi
call ChecksumAcpi     ; XSDT checksum
jnz BadFindTable_2    ; Go if checksum bad
;--- Scan XSDT (64-bit pointers) for detect required table ---
mov eax,[rsp+16]      ; EAX=Signature, input EDX
add rcx,rsi           ; RCX=Scan pointers limit
add rsi,024h          ; RSI=Scan pointers base
ScanXsdt:
mov rdx,[rsi]         ; RDX=Scanned table
cmp [rdx],eax
je TargetFound        ; Go if table found
add rsi,8             ; 8 byte pointers at XSDT
cmp rsi,rcx
jb ScanXsdt
BadFindTable_2:
jmp BadFindTable      ; Go if table not found

;---------- Table found, verify it checksum --------------------------------;
TargetFound:
mov ecx,[rdx+004h]    ; RDX=Base, RCX=Length
call ChecksumAcpi     ; Checksum target table
jnz BadFindTable      ; Go if checksum bad
lea rcx,[rdx+rcx+1]   ; RCX=Limit
mov rbx,rdx           ; RDX=Base

;--- Exit points ---
clc                ; CF=0(NC) means found
ExitFindTable:
pop rbp rsi rdx rax
ret
BadFindTable:
stc                ; CF=1(C) means not found
jmp ExitFindTable

;--- Parse ACPI MCFG table and extract parameters -----;
; Note 1. Assume checksum already verified by caller.  ;
; Note 2. This version assume only one PCI bus MCFG    ;
; region, single-root machine                          ;
;                                                      ;
; INPUT:   R15 = Global variables pool base address    ;
;          Use 64-bit flat memory addressing           ;
;          RBX = Pointer to ACPI MCFG table            ;
;          RCX = Table limit (not used yet)            ;
;                                                      ;
; OUTPUT:  CF  = Flag: 0(NC)=Complete OK, 1(C)=Error   ;
;          RBX = Pointer to MCFG MMIO, valid if OK     ;
;          RCX = Size of  MCFG MMIO, valid if OK       ;
;------------------------------------------------------;
ParseAcpiMcfg:
cmp dword [rbx],'MCFG'
jne BadMcfg                  ; Go if signature mismatch
;--- Get parameters ---
movzx ecx,byte [rbx+037h]    ; CL=Last present bus

;@@ 0.93 patch #1
;inc	cl ; CL=First absent bus
inc ecx                ; RCX=First absent bus
		
shl	ecx,20             ; RCX=Size (1MB per bus)
mov	rbx,[rbx+02Ch]     ; RBX=Base (flat 64-bit)
;--- Exit points ---
clc
ret
BadMcfg:
stc
ret

;---------- Local subroutines ----------------------------------------------;

;--- Calculate byte array checksum -------------;
; INPUT:   RDX = Array base address             ;
;          RCX = Array length, bytes            ;
; OUTPUT:  ZF  = Status flag:                   ;
;                0(NZ)=Checksum bad, 1(Z)=Good  ;
;          AL  = Checksum                       ;
;-----------------------------------------------;
ChecksumAcpi:
push rcx rdx
mov al,0
SumAcpi:
add al,[rdx]
inc rdx
loop SumAcpi
test al,al           ; AL=Checksum
pop rdx rcx
ret
