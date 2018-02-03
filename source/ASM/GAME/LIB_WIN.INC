;===========================================================================;
;                                                                           ;
;              SUBROUTINES FOR INTERPRETING WINDOWS OBJECTS.                ;
;                                                                           ;
;===========================================================================;

; Subroutines init,open,close,event replication is not optimal,
; but reserve easy method to add step-specific actions

;--- Windows descriptors interpreter, for initialization step ------;
; INIT handlers can change base, size and control data in the       ;
; windows descriptors, because this values used after INIT phase.   ;
; INIT handlers cannot draw, because base, size and control data    ;
; yet not set during INIT phase execution.                          ;
; For support memory allocation by window size and dynamical        ;
; resizing, descriptor must contain maximal window size, it used    ;
; for allocation, real size can be dynamically reduced, but cannot  ;
; dynamically set above start size, because memory allocated.       ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address, flat 64        ;
;          Game windows list data block                             ;
;          Global variables                                         ;
; OUTPUT:  Global variables updated                                 ;
;          Video memory and transit buffer updated                  ;
;-------------------------------------------------------------------;
Windows_Init:
push rax rbx rcx rdx rsi rdi rbp r13
;--- Setup cycle parameters ---
call Local_W_Setup_Parms_0
mov rdi,[_Use_Mem_Base+r15]  ; RDI=Transit buffer address

;! v0.99 patch start

;! add rdi,[_Vmem_Size+r15]  ; RDI=Free memory for windows

push rax rdx
movzx rax,word [_Video_Xbytes+r15]
movzx rdx,word [_Video_Ysize+r15]
mul rdx
add rdi,rax
pop rdx rax

;! v0.99 patch end

call TSC_to_Microseconds  ; R13=Microseconds
;--- Cycle for interpreting windows table ---
; Here RSI=Pointer to table, RCX=Number of entries
; RDI=Extended memory allocation
W_Init_Cycle:
mov	al,[rsi+00]     ; AL=Code for Port 80
CHKOUT              ; Output checkpoint
mov rax,[rsi+21]    ; RAX=Initialization handler offset
;--- Update window free memory base ---
mov [rsi+04],rdi    ; Update free memory base, qword
;--- Save registers, handler can corrupt it ---
push rcx rsi r13 r15
;--- Call handler ---
call rax            ; Call handler
;--- Restore registers ---
pop r15 r13 rsi rcx
;--- Modify pointer, cycle ---
add rsi,4+17+32     ; WINCHK=4, WINDESC=17, WINPROC=32
loop W_Init_Cycle   ; Cycle for all windows (LOOPQ)
;--- Exit ---
Win_Return:
pop r13 rbp rdi rsi rdx rcx rbx rax
ret

;--- Windows descriptors interpreter, for window open step ---------;
; OPEN handlers typically execute basic drawings.                   ;
; OPEN handlers can use common transit buffer as temporary memory   ;
; for drawings (example: convert BMP to Sprite), if transit buffer  ;
; overwrite by revisual background window. If transit buffer not    ;
; ovewrite and must be not corrupted, additional buffer required.   ;
; BUG: this available only for background routine, it write all     ;
; transit buffer.                                                   ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address, flat 64        ;
;          Game windows list data block                             ;
;          Global variables                                         ;
; OUTPUT:  Global variables updated                                 ;
;          Video memory and transit buffer updated                  ;
;-------------------------------------------------------------------;
Windows_Open:
push rax rbx rcx rdx rsi rdi rbp r13
;--- Setup cycle parameters ---
call Local_W_Setup_Parms_0
call TSC_to_Microseconds  ; R13=Microseconds
;--- Cycle for interpreting windows table ---
; Here RSI=Pointer to table, RCX=Number of entries
W_Open_Cycle:
mov al,[rsi+01]      ; AL=Code for Port 80
CHKOUT               ; Output checkpoint
mov rax,[rsi+29]     ; RAX=Open win. handler offset
call Local_W_Caller
xor r8l,r8l          ; Blank revisual flags
loop W_Open_Cycle
;--- Exit ---
jmp Win_Return

;--- Windows descriptors interpreter, for window close step --------;
; CLOSE handlers make window invisible. It can be before program    ;
; termination or re-open possible.                                  ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address, flat 64        ;
;          Game windows list data block                             ;
;          Global variables                                         ;
; OUTPUT:  Global variables updated                                 ;
;          Video memory and transit buffer updated                  ;
;-------------------------------------------------------------------;
Windows_Close:
push rax rbx rcx rdx rsi rdi rbp r13
;--- Setup cycle parameters ---
call Local_W_Setup_Parms_0
call TSC_to_Microseconds    ; R13=Microseconds
;--- Cycle for interpreting windows table ---
; Here RSI=Pointer to table, RCX=Number of entries
W_Close_Cycle:
mov al,[rsi+02]     ; AL=Code for Port 80
CHKOUT              ; Output checkpoint
mov rax,[rsi+37]    ; AX=Close win. handler offset
call Local_W_Caller
xor r8l,r8l         ; Blank revisual flags
loop W_Close_Cycle
;--- Exit ---
jmp Win_Return

;--- Windows descriptors interpreter, for window event step --------;
; EVENT handlers used for make and re-visual windows dynamical      ;
; changes by user and timer events.                                 ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address, flat 64        ;
;          Game windows list data block                             ;
;          Global variables                                         ;
;          Specific for Windows_Event subroutine:                   ;
;          R13 = Microseconds from PC start, TSC used               ;
;          RDX = User or timer event code, 0=No events              ;
;                                                                   ;
; OUTPUT:  Global variables updated                                 ;
;          Video memory and transit buffer updated                  ;
;          Specific for Windows_Event subroutine:                   ;
;          R8.7 = Program exit request                              ;
;          R8.6 = Request copy to transit buffer by overlap         ;
;          R8.5 = Request copy to transit buffer by disjunction     ;
;          R8.4 = Request for fast update                           ;
;          R8.[3-0] = Reserved                                      ;
;-------------------------------------------------------------------;
Windows_Event:
push rax rbx rcx rdx rsi rdi rbp r13 r9
xor r8l,r8l
xor r9l,r9l
;--- Setup cycle parameters ---
call Local_W_Setup_Parms_1
;--- Cycle for interpreting windows table ---
; Here RSI=Pointer to table, CX=Number of entries
W_Event_Cycle:
mov al,[rsi+03]      ; AL=Code for Port 80
CHKOUT               ; Output checkpoint
mov rax,[rsi+45]     ; RAX=Event win. handler offset
call Local_W_Caller
;--- Update returned requests bitmap (R9), blank curent (R8) ---
or r9l,r8l
xor r8l,r8l
;--- Cycle ---
loop W_Event_Cycle  ; Here as LOOPQ (RCX)
;--- Exit with set returned requests bitmap, restore R9 ---
mov r8l,r9l
pop r9
jmp Win_Return

;---------- Local subroutines ----------------------------------------------;

;--- Setup parameters for windows interpreter ------------;
; Used before cycle start                                 ;
; INPUT:   R15 = Global variables pool base address       ;
; OUTPUT:  Flag DF = 0                                    ;
;          RSI = Pointer to game windows list + 1         ;
;          RCX = Number of entries in the list            ;
;          RDX = 0, clear keyboard and mouse events       ;
;---------------------------------------------------------;
Local_W_Setup_Parms_0:
xor rdx,rdx
Local_W_Setup_Parms_1:   ; Except RDX
push rax
cld
mov rsi,Game_Win_List
lodsb                    ; Use RSI
movzx rcx,al
pop rax
ret

;--- Caller for windows handlers -------------------------;
; INPUT:   RAX = Handler near address, 64-bit flat        ;
;          RDX = Event code or 0 if no user events        ;
;          RSI = Pointer to entry of windows list         ;
; OUTPUT:  RSI = Modified                                 ;
;          RCX = Saved                                    ;
;          RDI = Saved                                    ;
;          R8L = Exit and VRAM update requests flags      ;
;          Other regs. corrupted, need P/P ALL at caller  ;
;---------------------------------------------------------;
Local_W_Caller:
;--- Save registers ---
push rcx rdx rsi rdi
;--- Save graphics parameters ---
push qword [_Video_XYsize_Plus+r15]
push qword [_Use_Mem_Base+r15]
;--- Setup procedure input parameters ---
; RDX=0, here no user events
; RDI=Base Linear
; RCX=Windows reverse count
; R15=Pointer to variables pool
; RSI=Pointer to entry
mov rdi,[rsi+04]   ; RDI = Sprite base linear address
;--- Setup graphics parameters for adjust to current window ---
; Yet replaced only _Hmem_Base, _Video_Xsize, _Video_Ysize because
; draw routines use this 3 parameters only
mov [_Use_Mem_Base+r15],rdi
mov ecx,[rsi+12]
mov [_Video_XYsize+r15],ecx
;--- Call handler ---
call rax   ; Call handler, can return coord.
;--- Restore graphics parameters ---
pop qword [_Use_Mem_Base+r15]
pop qword [_Video_XYsize_Plus+r15]
;--- Conditionally update transit buffer by this window draw ---
call Local_W_Draw   ; Can accept fast coordinates
;--- Restore registers, modify pointer, exit ---
pop rdi rsi rdx rcx
add rsi,4+17+32     ; WINCHK=4, WINDESC=17, WINPROC=32
ret

;--- Draw window in the transit buffer -------------------;
; INPUT:   R15 = Global variables pool base address       ;
;          RSP = Pointer to stack frame                   ;
;          R8L = Exit and VRAM update requests flags      ;
; OUTPUT:  None                                           ;
;          Regs. corrupted, need PUSH/POP ALL at caller   ;
;---------------------------------------------------------;
Local_W_Draw:
;--- Prepare input parameters ---
mov rsi,[rsp+16]     ; Get RSI image
;--- Conditionally hide mouse ---
call Ask_Mouse
jc No_Mouse_4
call Hide_Mouse
;--- Detect fast update mode ---
No_Mouse_4:
test r8l,00010000b   ; R8L.4=Fast update request
jnz W_Fast_Draw
;--- Prepare input parameters ---
mov rax,[rsi+04]   ; RAX = System memory address
mov bx,[rsi+16]    ; BX = X-base
mov dx,[rsi+18]    ; DX = Y-base
mov cx,[rsi+12]    ; CX = X-size
mov si,[rsi+14]    ; SI = Y-size
;--- Drawings in the transit buffer ---
test r8l,01000000b    ; R8L.6=Overlap request
jz W_No_Draw_1        ; Go if Overlap request=0
call Draw_Sprite_SSE_Overlap
W_No_Draw_1:
test r8l,00100000b    ; R8L.5=Disjunction request
jz W_After_Draw       ; Go if Disjunction request=0
call Draw_Sprite_SSE_Disjunct
jmp W_After_Draw
;--- Fast update mode, X,Y limited replication, minimal write block size ---
W_Fast_Draw:	
ror ebx,16
ror edx,16
ror eax,16
ror edi,16
mov bx,[rsi+16]    ; EBX.[31-16]=P.Buf. X-offset
mov dx,[rsi+18]    ; EDX.[31-16]=P.Buf. Y-offset
mov ax,[rsi+12]    ; EAX.[31-16]=P.Buf. X-size
mov di,[rsi+14]    ; EDI.[31-16]=P.Buf. Y-size
ror ebx,16         ; BX=Visualized X-offset
ror edx,16         ; DX=Visualized Y-offset
ror eax,16         ; AX=Visualized X-size
ror edi,16         ; DI=Visualized Y-size
mov ecx,eax        ; ECX=EAX, parameters reload
mov rax,[rsi+04]   ; RAX = System memory address
mov esi,edi        ; ESI=EDI, parameters reload
call Fast_Update_Video_SSE
;--- Conditionally show mouse ---
W_After_Draw:
call Ask_Mouse
jc No_Mouse_5
call Show_Mouse
;--- Exit ---
No_Mouse_5:
ret

