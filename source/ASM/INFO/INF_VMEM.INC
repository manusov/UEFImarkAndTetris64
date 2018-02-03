;===========================================================================;
;                                                                           ;
;         BUILT INFORMATION STRING HANDLER FOR: VIDEO MEMORY SIZE.          ;
;                                                                           ;
;===========================================================================;

;--- Bullt information string for video memory size ----------------;
; INPUT:   R15 = Pointer to global variables pool                   ;
;          RDI = Pointer for built system information text strings  ;
;          Use flat 64-bit addressing                               ;
; OUTPUT:  RDI = Modified if string built, otherwise preserved      ;
;          System information text strings updated                  ;
;-------------------------------------------------------------------;
Info_Video_Memory:
;--- Write left part of string ---
mov rsi,String_Vmem
call String_Copy_Simple
;--- Write VBE reported video memory size ---
mov bl,0                   ; BL=Template mode
mov rax,[_Vmem_Size+r15]   ; This info from GOP
shr rax,10                 ; Bytes -> Kilobytes
call Decimal_Print_32      ; This subr. use EAX only
call String_Copy_Simple
;--- Select video memory base address, check compatibility ---
	;!	xor	eax,eax
mov rbx,[_PBAR_Base+r15]         ; RBX = Base Address
mov rdx,[_PBAR_Size+r15]         ; RDX = Size Limit
test byte [_Sys_Bitmap_L+r15],10000000b
jz Inf_Vmem_5                    ; Go if not remapped
mov rbx,[_Ext_PBAR_Base+r15]     ; RBX = Base Address
mov rdx,[_Ext_PBAR_Size+r15]     ; RDX = Size Limit
Inf_Vmem_5:
test rbx,rbx
jz Inf_Vmem_4
test rdx,rdx
Inf_Vmem_4:
jz Inf_Vmem_1
;--- Write middle string ---
call String_Copy_Simple
;---------- Detect video memory by physical scan ---------------------------;
push rsi rdi
mov rcx,00080000h              ; RCX = Size Approximator, 512KB
;--- Prepare for cycle 1 ---	 ; RBX = Base,  RDX = Maximum size
Check_Exclus:	
push qword [rbx]
push qword [rbx+rcx]
mov eax,055555555h
mov edi,0AAAAAAAAh
;--- Cycle 2, required for WC memory fence, buffers flush ---
mov si,256
Fence_Cyc:
mov [rbx],eax
mov [rbx+rcx],edi
wbinvd            ; Writeback and Invalidate cache
dec si
jnz Fence_Cyc
;--- Check results ---
cmp [rbx+rcx],edi
jne Mem_Failure   ; Go if video memory failure
cmp [rbx],eax
stc               ; CF=1(C) means continue
je End_Pass       ; Go if try this size OK
cmp [rbx],edi
je End_Pass       ; Go if not failure, but stop, CF=0(NC)

Mem_Failure:	
	;!	xor	al,al		; AL=0, also CF=0(NC)

End_Pass:	
pop qword [rbx+rcx]
pop qword [rbx]
jnc End_Scan      ; Go if size detected or error
shl rcx,1
cmp rcx,rdx       ; RCX = Size
jb Check_Exclus
;--- End of cycle 1 ---
End_Scan:
pop rdi rsi
cmp al,055h
jne Inf_Vmem_2

;---------- Update and Write physical detected video memory size -----------;
mov bl,0                   ; BL=Template mode
xchg eax,ecx               ; Units=Bytes
mov [_Vmem_Total+r15],rax
shr eax,10                 ; Units=Kilobytes
call Decimal_Print_32
call String_Copy_Simple

;---------- Write GOP version ----------------------------------------------;
; After previous fragment, here valid parameters:
; BL=0, means no template
; RSI=Pointer to String_GOP
	; RESERVED
	;!	call	String_Copy_Simple
	;!	movzx	eax,[_GOP_Version_H+r15]
	;!	call	Decimal_Print_32
	;!	mov	al,'.'
	;!	stosb
	;!	mov	al,[_GOP_Version_L+r15]		; Here EAX.31-08=0
	;!	call	Decimal_Print_32

;---------- Terminate string and exit --------------------------------------;
Inf_Vmem_1:	
jmp Inf_Terminate
Inf_Vmem_2:	
mov al,'?'
stosb
jmp Inf_Vmem_1
