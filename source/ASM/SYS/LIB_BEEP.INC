;===========================================================================;
;                                                                           ;
;              SUBROUTINES FOR PC SPEAKER AUDIO OUTPUT SUPPORT.             ;
;                                                                           ;
;===========================================================================;

;---------- Generating short sound by PC speaker --------------;
;  Use background mode, no wait during beep work               ;
;  This sound used for press any key                           ;
; INPUT:   R15 = Global variables pool base address            ;
;          Variable [Sound_Switch] = Sound enable flag         ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Sound_Short_ON:
push rbx rcx
mov bx,1400      ; BX  = Tone Divisor
mov ecx,4500     ; ECX = Time for sound on
Sound_Entry:
cmp byte [_Sound_Switch+r15],1
jne Sound_Exit   ; Go skip if sound disabled
call Enable_Speaker
call Count_Start
Sound_Exit:
pop rcx rbx
Sound_Ret:
ret

;---------- Generating long sound by PC speaker ---------------;
; Use background mode, no wait during beep work                ;
; This sound used for tetris line go down                      ;
; INPUT:   R15 = Global variables pool base address            ;
;          Variable [Sound_Switch] = Sound enable flag         ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Sound_Long_ON:
push rbx rcx
mov bx,2000      ; BX  = Tone Divisor
mov ecx,78000    ; ECX = Time for sound on
jmp Sound_Entry

;---------- Generating very long sound by PC speaker ----------;
; Use background mode, no wait during beep work                ;
; This sound used for tetris game over                         ;
; INPUT:   R15 = Global variables pool base address            ;
;          Variable [Sound_Switch] = Sound enable flag         ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Sound_Over_ON:
push rbx rcx
mov bx,2900      ; BX  = Tone Divisor
mov ecx,470000   ; ECX = Time for sound on
jmp Sound_Entry

;---------- Disable PC speaker sound if beep time over --------;
; INPUT:   R15 = Global variables pool base address            ;
;          Variable [Sound_Switch] = Sound enable flag         ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Sound_Check_OFF:
call Count_Check     ; Check timer
jc Sound_Ret         ; Go skip if timer error
jnz Sound_Ret        ; Go skip if not a time
jmp Disable_Speaker	 ; Go to disable sound

;---------- Disable PC speaker sound, time independent  -------;
; INPUT:   R15 = Global variables pool base address            ;
;          Variable [Sound_Switch] = Sound enable flag         ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Sound_OFF:
cmp byte [_Sound_Switch+r15],1
jne Sound_Ret   ; Go skip if mode not enabled
; No RET, go to disable sound

;---------- Disable PC speaker sound --------------------------;
; INPUT:   None                                                ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Disable_Speaker:
push rax
in al,61h          ; Read System Port B
and al,11111100b   ; Disable timer and audio data
out 61h,al         ; Write System Port B
pop rax
ret

;---------- Enable PC speaker sound, set tone frequency -------;
; INPUT:   BX  = Frequency divisor, Frequency = 1.193MHz/BX    ;
; OUTPUT:  None                                                ;
;--------------------------------------------------------------;
Enable_Speaker:
push rax rbx
;--- Set timer mode ---
mov al,10110110b
out 43h,al         ; Timer control, set channel=2, mode=3
;--- Set divisor value ---
xchg ax,bx
out 42h,al         ; Channel 2 data, set divisor, low byte
mov al,ah
out 42h,al         ; Channel 2 data, set divisor, high byte
;--- Enable Sound ---
in al,61h          ; Read System Port B
or al,00000011b    ; Enable timer and audio data
out 61h,al         ; Write System Port B
;--- Exit ---
pop rbx rax
ret

