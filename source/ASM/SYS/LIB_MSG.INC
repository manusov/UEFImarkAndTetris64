;===========================================================================;
;                                                                           ;
;       SUBROUTINES FOR WRITE ERRORS AND OPERATIONS COMPLETE MESSAGES.      ;
;                     THIS MESSAGES IS UEFI API RELEVANT.                   ;
;                                                                           ;
;===========================================================================;

;---------- Write success or error message -------------------------;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
;          Status variables                                         ;
; OUTPUT:  None                                                     ;
;-------------------------------------------------------------------;
Status_Msg_Write:
push rax rsi rdi
;--- Analysing status ---
mov dl,[_EFI_Action+r15]
test dl,dl
jz Skip_StMsg
;--- Skip one string ---
mov rsi,Msg_CRLF
call String_Write
;--- Built and Write action name string ---
mov rdi,Buffer_ASCII       ; RDI=Destination string
mov rsi,Save_File_Status   ; RSI=Source string
push rdi
mov al,dl   ; AL=Selector
mov ah,4    ; AH=Limit
call String_Select_Copy
mov al,dl			; 0=TXT OK, 1=BMP OK
mov rsi,Name_TXT
cmp al,1
je Name_StMsg
mov rsi,Name_BMP
cmp al,2
jne NoName_StMsg
Name_StMsg:
call String_Copy_Simple
mov al,0
stosb
NoName_StMsg:
pop rsi
call String_Write
mov rsi,Msg_CRLF
call String_Write
;--- Check errors detected ---
cmp dl,2
jbe Skip_StMsg         ; Go if no errors
;--- Built and Write UEFI API operation name string ---
mov rdi,Buffer_ASCII   ; RDI=Destination string
mov rsi,Str_Operation  ; RSI=Source string
push rdi
call String_Copy_Simple
mov rsi,Operations_Names
mov al,[_EFI_Operation+r15]
mov ah,14              ; Last code=14
call String_Select_Copy
mov al,0
stosb
pop rsi
call String_Write
mov rsi,Msg_CRLF
call String_Write
;--- Built and Write UEFI Error name string ---
mov rdi,Buffer_ASCII   ; RDI=Destination string
mov rsi,Str_Status     ; RSI=Source string
push rdi
call String_Copy_Simple
mov rax,[_EFI_Status+r15]
test rax,rax           ; RAX.63: 0=Warning, 1=Error
mov rsi,EFI_Warnings_Names
mov ah,4               ; Last warning=4
jns Warning_StMsg
mov rsi,EFI_Errors_Names
mov ah,32              ; Last error=32
Warning_StMsg:
call String_Select_Copy
mov al,0
stosb
pop rsi
call String_Write
mov rsi,Msg_CRLF
call String_Write
;--- Exit ---
Skip_StMsg:
pop rdi rsi rax
ret
