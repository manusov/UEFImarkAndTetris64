;===========================================================================;
;                                                                           ;
;                       DATA FOR GAME FLOW TIMINGS.                         ;
;                                                                           ;
;===========================================================================;

; Figure fall delay per pixel, microseconds, for cursor down
Fast_Fall_Timing:
DD  130

; New figure generating and Line fall delay, time of blinking visual
DY_Per_New_Figure  = 15   ; Time for move figure after fall possible
DY_Per_Line_Fall   = 09

; Figure fall delay per pixel, microseconds, F(Game Level)
Game_Levels_Supported  = 15
Game_Levels_Timings:
DD  16000  ; Level=0
DD  10000  ; Level=1
DD  09500  ; Level=2
DD  09000  ; Level=3
DD  08500  ; Level=4
DD  08000  ; Level=5
DD  07500  ; Level=6
DD  07000  ; Level=7
DD  06500  ; Level=8
DD  06000  ; Level=9
DD  05500  ; Level=10
DD  05000  ; Level=11
DD  04500  ; Level=12
DD  04000  ; Level=13
DD  03500  ; Level=14
DD  03000  ; Level=15
; continue under construction, now limited by CMP
