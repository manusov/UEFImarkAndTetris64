;===========================================================================;
;                                                                           ;
;       VERIFY COMPATIBILITY HANDLER FOR CHECK COMPATIBLE VIDEO MODE.       ;
;                                                                           ;
;===========================================================================;

;----------------------------------------------------------------;
; EFI_GRAPHICS_OUTPUT_MODE_INFORMATION, returned by function
; EFI_GRAPHICS_OUTPUT_PROTOCOL.QueryMode()
; Offset(hex)  Size     Comments
;----------------------------------------------------------------;
; 00-03        dword    Structure version, yet used 0
; 04-07        dword    Horizontal resolution, pixels
; 08-0B        dword    Vertical resolution, pixels
; 0C-0F        dword    Bitmap EFI_GRAPHICS_PIXEL_FORMAT
; 10-1F     16 bytes    Color bitmaps EFI_PIXEL_BITMASK
; 20-23        dword    Pixels per scan line, logical length
;----------------------------------------------------------------;
; EFI_GRAPHICS_PIXEL_FORMAT bitmap:
; D00 = PixelRedGreenBlueReserved8BitPerColor, fixed mode 1
; D01 = PixelBlueGreenRedReserved8BitPerColor, fixed mode 2
; D02 = PixelBitMask, variable mode
; D03 = PixelBltOnly, "1" means direct VRAM access not supported
; D04 = PixelFormatMax
; D05-31=Limit
;----------------------------------------------------------------;
; EFI_PIXEL_BITMASK bitmaps: dwords Blue, Red, Green, Reserved
;----------------------------------------------------------------;
;
; EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE, accessible by GOP entry #4,
; information about current video mode
; Offset(hex)  Size     Comments
;----------------------------------------------------------------;
; 00-03        dword    MaxMode, maximum mode number
; 04-07        dword    Mode, current mode number
; 08-0F        qword    Addr. EFI_GRAPHICS_OUTPUT_MODE_INFORMATION
; 10-17        qword    Size EFI_GRAPHICS_OUTPUT_MODE_INFORMATION
; 18-1F        qword    FrameBufferBase, flat 64-bit address
; 20-27        qword    FrameBufferSize, flat 64-bit address


;--- Scan UEFI video modes list, find compatible video mode --------;
; Result: select UEFI GOP video mode for set later at SET_VMOD.INC  ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF    = Flag: 0(NC)=Check Passed, 1(C)=Check Failed      ;
;          Video control variables and buffers updated              ;
;-------------------------------------------------------------------;
Check_VMode:
;--- Prepare for scan video modes ---
mov rcx,[_EFI_Gop+r15]  ; RCX=GOP interface pointer
xor edx,edx             ; RDX=Video mode start number
xor r10d,r10d           ; R10D=Select mode Number
xor r11d,r11d           ; R11D=Xsize, for optimizing
;--- Get video mode info, use GOP.QueryMode() ---
Scan_GOP_Mode:
call Local_GOP_Query
jc Skip_GOP_Mode    ; Skip mode number if error
;--- Check info size ---
cmp rbp,0024h
jb Skip_GOP_Mode    ; Skip if bad info size
;--- Check Xsize ---
mov eax,[rbx+004h]
movzx esi,word [_Video_Xmin+r15]
movzx edi,word [_Video_Xmax+r15]
cmp eax,esi
jb Skip_GOP_Mode    ; Skip if Xsize < Xmin
cmp eax,edi
ja Skip_GOP_Mode    ; Skip if Xsize > Xmax
;--- Check Ysize ---
mov ebp,[rbx+008h]
movzx esi,word [_Video_Ymin+r15]
movzx edi,word [_Video_Ymax+r15]
cmp ebp,esi
jb Skip_GOP_Mode    ; Skip if Ysize < Ymin
cmp ebp,edi
ja Skip_GOP_Mode    ; Skip if Ysize > Ymax
;--- Check direct Video RAM access support ---
test byte [rbx+00Ch],00001000b
jnz Skip_GOP_Mode     ; Skip if PixelBltOnly=1
;--- Check Logical X length ---
cmp dword [rbx+020h],2048
ja Skip_GOP_Mode      ; Skip if Logical X > 2048
;--- Compare this mode and previous selected, replace if better ---
; EAX  = This mode Xsize
; R11D = Previous selected mode Xsize
; EDX  = This mode number
; R10D = Selected mode number for conditional replace
; EDI  = Optimal Xsize from configuration data (not used yet)
;---
cmp eax,r11d
jbe Skip_GOP_Mode
mov r11d,eax          ; Replace Xsize
mov r10d,edx          ; Replace Video Mode number
;--- Cycle for all supported video modes ---
Skip_GOP_Mode:
inc edx               ; EDX=Scanned mode number
mov rbx,[rcx+018h]    ; RBX=Mode info table pointer
cmp edx,[rbx+000h]    ; Compare Mode and Max.Mode
jbe Scan_GOP_Mode     ; Cycle for next mode scan
;--- Repeat read parameters for selected video mode ---
mov edx,r10d
mov [_Target_Vmode+r15],edx
call Local_GOP_Query
jc Exit_Vmode

;--- Setup all parameters for selected video mode ---
; Note this parameters already verifyed in the previous cycle
mov eax,[rbx+006h]       ; EAX.[31-15] = word [RBX+8]
mov ax,[rbx+004h]        ; AX = word [RBX+4]
mov [_Video_XYsize+r15],eax
mov eax,[rbx+020h]
shl eax,2
mov [_Video_Xbytes+r15],ax
;--- Set colors bitmap ---
mov al,[rbx+00Ch]
mov rbx,0000080808100800h
test al,00000001b
jnz Set_Colors
mov rbx,0010080808000800h
test al,00000010b
jnz Set_Colors
		;
		; UNDER CONSTRUCTION: NEED ANY TYPE BITMAP HANDLING
		;
Set_Colors:
mov bl,[_Video_BPPmax+r15]
mov [_Video_Colors+r15],rbx

;--- This fragment set defaults if no graphics protocols available ---
	;! DEBUG PURPOSE ONLY
	;!	mov	ax,[_Video_Ymax+r15]
	;!	shl	eax,16
	;!	mov	ax,[_Video_Xmax+r15]
	;!	mov	[_Video_XYsize+r15],eax
	;!	shl	eax,2
	;!	mov	[_Video_Xbytes+r15],ax
	;!	mov	rax,0000080808100800h
	;!	mov	al,[_Video_BPPmax+r15]
	;!	mov	[_Video_Colors+r15],rax
	;!	mov	rax,00000000E0000000h
	;!	mov	qword [_Vmem_Base+r15],rax
	;!	mov	qword [_Vmem_Size+r15],1024*768*4
	;! DEBUG PURPOSE ONLY


;--- Built palette, recalculate by R,G,B widths and shifts ---
mov rsi,Primary_Palette
mov rdi,Runtime_Palette_256
mov bx,Primary_Palette_Size
;--- Cycle for R,G,B ---
Built_Pal_1:
mov rbp,Video_R_Width
xor edx,edx
Built_Pal_2:
lodsb
call Local_Palette_1
	
	; USE 32-BIT OFFSETS WITH 64-BIT REGISTERS = ADDRESS RELOCATION BUG	
	;!	cmp	rbp,Video_B_Width
mov rax,Video_B_Width
cmp rbp,rax
	;!
jbe Built_Pal_2
;--- End cycle for R,G,B ---
xchg eax,edx
stosd
dec bx
jnz Built_Pal_1
;--- Clear visualization enable bitmap ---
mov rdi,Lines_Show
mov rcx,L_Show_Size/4
xor eax,eax            ; This also set CF=0(NC)
rep stosd
;--- Exit ---
Exit_Vmode:
ret

;---------- Local subroutines ----------------------------------------------;

;--- Inject color to palette entry -------------;
; INPUT:   AL  = Physical color component       ;
;          EDX = Physical color accumulator     ;
;          RBP = Pointer to Width:Position      ;
; OUTPUT:  EDX = Updated by inject color        ;
;          RBP  = Updated by +2                 ;
;-----------------------------------------------;
Local_Palette_1:
push rcx
mov ah,al
and eax,0000FF00h
mov cl,16
sub cl,[rbp+00]
shr ax,cl           ; Here lose low bits
mov cl,[rbp+01]
shl eax,cl
or edx,eax
add rbp,2
pop rcx
ret

;--- Query video mode -----------------------------;
; INPUT:   RCX = GOP interface pointer             ;
;          RDX = Video mode number, by GOP         ;
; OUTPUT:  CF  = Error flag: 0(NC)=OK, 1(C)=Error  ;
;          RAX = EFI Status                        ;
;          RBX = Mode info base address            ;
;          RBP = Mode info size                    ;
;--------------------------------------------------;
Local_GOP_Query:
xor rax,rax       ; [RCX+RAX]=Call address
push rax rax      ; Create scratch pad
mov r8,rsp        ; QWORD[R8]=Info Size
lea r9,[rsp+08]   ; QWORD[R9]=Info Base
call EFI_API
pop rbp rbx       ; RBP=Size, RBX=Base
ret

