;===========================================================================;
;                                                                           ;
;     SUBROUTINES FOR GET DATE AND TIME, AS NUMBERS AND AS TEXT STRINGS.    ;
;                                                                           ;
;===========================================================================;

;--- EFI Time structures, stack frame used, not declared as variables ---;
; EFI_Time_Value:
; EFI_Time_Year		DW  ?	; +0
; EFI_Time_Month	DB  ?	; +2
; EFI_Time_Day		DB  ?	; +3
; EFI_Time_Hour		DB  ?	; +4
; EFI_Time_Minute	DB  ?	; +5
; EFI_Time_Second	DB  ?	; +6
; EFI_Time_Pad1		DB  ?	; +7
; EFI_Time_Nanosecond	DD  ?	; +8
; EFI_Time_TimeZone	DW  ?	; +12
; EFI_Time_DayLight	DB  ?	; +14
; EFI_Time_Pad2		DB  ?	; +15
;------------------------------------------------------------------------;
; EFI_Time_Capabilities:
; EFI_TCap_Resolution	DD  ?	; +0
; EFI_TCap_Accuracy	DD  ?	; +4
; EFI_TCap_SetsToZero	DD  ?	; +8
;------------------------------------------------------------------------;

;---------- Get Date and Time, use EFI Runtime Services ----------;
; INPUT:   R15 = Global variables pool base address               ;
;          Use 64-bit flat memory addressing, without segments    ;
;          Global variables                                       ;
;                                                                 ;
; OUTPUT:  CF  = Error flag: 0(NC)=No errors, 1(C)=Error          ;
;          RAX = EFI_STATUS                                       ;
;          Next parameters valid if no errors:                    ;
;          BL  = Seconds      ,   0-59                            ;
;          BH  = Minutes      ,   0-59                            ;
;          CL  = Hours        ,   0-23                            ;
;          CH  = Day of week  ,   0=Sunday, ... 6=Saturday        ;
;          DL  = Day of month ,   1-31                            ;
;          DH  = Month        ,   1-12                            ;
;          SI  = Year                                             ;
;-----------------------------------------------------------------;
EFI_Get_Time:
mov rbx,[_EFI_Table+r15]
mov rbx,[rbx+058h]   ; EFI_System_Table.RuntimeServ
xor rax,rax
push rax rax
mov rcx,rsp       ; RCX = EFI_Time_Value
mov rsi,rcx       ; RSI = Copy for addressing
push rax rax
mov rdx,rsp       ; RDX = EFI_TCap_Resolution
;--- Parameters for function call ---
; RCX = Pointer to structure EFI_Time_Value in the stack frame
; RDX = Pointer to structure EFI_Time_Capabilities in the stack frame
mov rax,018h      ; there call qword [rbx+rax]
call EFI_API_1
jc EFI_Time_Bad
;--- Extract output parameters ---
mov bl,[rsi+06]   ; [EFI_Time_Second]
mov bh,[rsi+05]   ; [EFI_Time_Minute]
mov cl,[rsi+04]   ; [EFI_Time_Hour]
mov dl,[rsi+03]   ; [EFI_Time_Day]
mov dh,[rsi+02]   ; [EFI_Time_Month]
mov si,[rsi+00]   ; [EFI_Time_Year]
;--- Calculate day of week ---
mov ch,0FFh        ; Pre-set value for errors		
push rdi rax rbx rcx rdx
xor eax,eax        ; SumOfDays=0
mov bx,1980        ; BX=First valid year, 1980
cmp si,bx
jb Wrong_Date      ; Go if Year<1980
cmp si,3000
ja Wrong_Date      ; Go if Year>3000
test dh,dh
jz Wrong_Date      ; Go if Month=0
cmp dh,12
ja Wrong_Date      ; Go if Month>12
test dl,dl
jz Wrong_Date      ; Go if DayOfMonth=0
cmp dl,31
ja Wrong_Date      ; Go if DayOfMonth>31
mov rdi,January_Days-1
Next_Year:
mov byte [rdi+2],28
test bl,03h
jnz Not_29_Days
inc byte [rdi+2]   ; Set 28/29 for February
Not_29_Days:
mov rcx,1          ; RCX=Months counter
Next_Month:
cmp bx,si
jb Not_My_Year     ; Go if Year not match
cmp cl,dh
jae My_Month       ; Go if Month and Year match
Not_My_Year:
add al,[rdi+rcx]   ; Add Days of Month
adc ah,0           ; Carry to high byte
inc rcx            ; Month+1
cmp cl,12
jbe Next_Month     ; Cycle if Month<13
inc ebx            ; Year+1
jmp Next_Year
My_Month:
add al,dl
adc ah,0
dec ax             ; -1 because day of month 1-based
xor edx,edx
mov cx,7
div cx             ; AH=Mod, Day of Week
xchg eax,edx       ; For code size optimization, AL=DL
add al,2           ; 2=Tuesday, 01.01.1980 is Tuesday
cmp al,7
jb Set_Week_Day
sub al,7
Set_Week_Day:
mov [rsp+9],al     ; Update CH image in the stack
Wrong_Date:
pop rdx rcx rbx rax rdi
;--- Remove stack data ---
EFI_Time_Bad:
add rsp,32
;--- Generating status flag CF=F(RAX) ---
test rax,rax       ; This also clear CF
jz EFI_Time_Exit   ; Skip if RAX=0, status=OK
stc                ; CF=1(C) means error
;--- Exit ---
EFI_Time_Exit:
ret

;--- Prepare date and time string for report ---------;
; This for report text file, see also subroutine:     ;
; Built_Date_Time_String                              ;
;                                                     ;
; INPUT:   R15 = Global variables pool base address   ;
;          Variables:                                 ;
;          Old_Seconds, Chg_Seconds, Chg_Status       ;
; OUTPUT:  Buffer_5 contain string,                   ;
;          use variables:                             ;
;          Old_Seconds, Chg_Seconds, Chg_Status       ;
;-----------------------------------------------------;
Report_Date_Time_String:
push r8
mov r8l,1                    ; R8L=1 means Mode=Report
jmp Entry_Date_Time_String

;--- Built date and time string for game screen ------;
; This for game flow screen, see also subroutine:     ;
; Report_Date_Time_String                             ;
;                                                     ;
; INPUT:   R15 = Global variables pool base address   ;
;          Variables:                                 ;
;          Old_Seconds, Chg_Seconds, Chg_Status       ;
; OUTPUT:  Buffer_5 contain string,                   ;
;          use variables:                             ;
;          Old_Seconds, Chg_Seconds, Chg_Status       ;
;-----------------------------------------------------;
Built_Date_Time_String:
push r8
mov r8l,0                    ; R8L=0 means Mode=Screen
;--- Entry for report mode ---
Entry_Date_Time_String:
push rax rbx rcx rdx rsi rdi
;--- Setup pointer, clear buffer ---
cld
mov rdi,Buffer_5
push rdi
mov rcx,Buffer_5_Size
mov al,' '
rep stosb
pop rdi
;--- Get date and time by EFI API ---
call EFI_Get_Time
jc Time_Bad
;--- Write month, day, year ---
push rcx rsi
mov al,dh                    ; AL = Current month
dec eax
mov ah,11                    ; AH = Limit, 0-11 possible
mov rsi,String_Months
call String_Select_Copy      ; Month name
mov al,','
stosb
xchg eax,edx                 ; Need AL=DL, but compact
mov ah,31                    ; Limit=31
call Local_Compare_Write_8   ; Month day
inc rdi                      ; 2 spaces between
inc rdi                      ; day and year
pop rax                      ; Need AX=SI
mov dx,9999
call Local_Compare_Write_16  ; Year number
pop rcx
;--- Write week day name ---
mov al,ch
mov ah,6
mov rsi,String_Days
inc rdi                      ; 2 spaces between
inc rdi                      ; year and week day name
test r8l,r8l
jnz Week_Day                 ; Go it mode=report
mov rdi,Buffer_5+Buffer_5_1
Week_Day:
call String_Select_Copy      ; Week day name
;--- Write time Hours:Minutes:Seconds ---
inc rdi                      ; 2 spaces between
inc rdi                      ; week day name and hours
test r8l,r8l
jnz Time_HMS                 ; Go it mode=report
mov rdi,Buffer_5+Buffer_5_2
Time_HMS:
mov al,bl                    ; BL=Seconds
xchg al,[_Old_Seconds+r15]
cmp al,bl
setne al
or [_Chg_Seconds+r15],al     ; Need revisual if changed
or [_Chg_Status+r15],al      ; Need revisual if changed
mov al,cl                    ; AL=Hours
mov ah,23                    ; Limit=23
call Local_Compare_Write_8   ; Hours write
mov al,':'
stosb
mov al,bh                    ; AL=Minutes
mov ah,59
call Local_Compare_Write_8   ; Minutes write
mov al,':'
stosb
mov al,bl                    ; AL=Seconds
mov ah,59                    ; Limit=59
call Local_Compare_Write_8   ; Seconds write
;--- Terminate string ---
mov al,0
stosb
;--- Exit ---
Time_Bad:
pop rdi rsi rdx rcx rbx rax r8
ret


;---------- Local subroutines ----------------------------------------------;


;--- Write 8-bit value with validity check -----;
; INPUT:   RDI = Destination pointer            ;
;          AL  = Value                          ;
;          AH  = Maximum limit                  ;
; OUTPUT:  RDI = Modified by write              ;
;          Memory at RDI updated                ;
;-----------------------------------------------;
Local_Compare_Write_8:
push rbx
cmp al,ah
ja Bad_Cmp_8      ; Go if check fail
mov bl,2          ; Template 2 chars
movzx eax,al      ; EAX=Value
Good_Cmp_8:
call Decimal_Print_32
pop rbx
ret
Bad_Cmp_8:
mov ax,'??'
stosw
pop rbx
ret

;--- Write 16-bit value with validity check ----;
; INPUT:   RDI = Destination pointer            ;
;          AX  = Value                          ;
;          DX  = Maximum limit                  ;
; OUTPUT:  RDI = Modified by write              ;
;          Memory at RDI updated                ;
;-----------------------------------------------;
Local_Compare_Write_16:
push rbx
cmp ax,dx
ja Bad_Cmp_16      ; Go if check fail
mov bl,4           ; Template 4 chars
movzx eax,ax       ; EAX=Value
jmp Good_Cmp_8
Bad_Cmp_16:
mov ax,'??'
stosw
stosw
pop rbx
ret
