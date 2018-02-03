;===========================================================================;
;                                                                           ;
;               SET CONTEXT HANDLER FOR SETUP GAME PARAMETERS.              ;
;                                                                           ;
;===========================================================================;

;--- Set control variables for game process and graphics -----------;
; This settings is defaults.                                        ;
; If file Name.CFG found and loaded, this settings overwrite by     ;
; file. If file not loaded, this settings used for game start.      ;
;                                                                   ;
; INPUT:   R15 = Global variables pool base address                 ;
;          Use 64-bit flat memory addressing, without segments      ;
; OUTPUT:  CF  = Flag: 0(NC)=Operation Passed, 1(C)=Failed          ;
;          Variable Sys_Bitmap and context control variables        ;
;-------------------------------------------------------------------;
Setup_Game:
;--- Blank variables ---
xor eax,eax                  ; EAX=0, and CF=0(NC)
mov [_Game_ON_Plus+r15],ax   ; + Sound_Switch
mov [_Color_Scheme+r15],al
mov [_Figures_Plus+r15],eax  ; + Lines
mov [_Level_Plus+r15],ax     ; + Score
mov [_Count_Active+r15],al
;--- Blank score gamer name ---
mov rdi,Buffer_6             ; RDI = Destination
mov rcx,Buffer_6_Size        ; RCX = Buffer size, bytes
rep stosb                    ; Blank gamer name, AL=0
;--- Prepare seconds comparision value ---
mov byte [_Old_Seconds+r15],0FFh
;--- Set Pause=ON for view start messages ---
mov byte [_Start_Pause+r15],1
or byte [_Sys_Bitmap_H+r15],040h  ; D14=Pause flag
;--- Exit ---
ret     ; Here CF=0(NC), no errors

