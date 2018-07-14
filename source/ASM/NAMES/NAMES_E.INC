;===========================================================================;
;                                                                           ;
;       COPYRIGHT AND NAMES: SPECIFIC FOR UEFIMARK EXTREME EDITION ONLY.    ;
;                                                                           ;
;===========================================================================;

;--- Common messages ---

; String_Version  DB  'UEFImark Extreme Edition v1.06. '
; String_Version  DB  'UEFImark Extreme Edition v1.07 DEBUG. '

;--- This 2 strings must be sequental, without separation by 0 ---
String_Version    DB  'UEFImark Extreme Edition v1.08. '
String_Copyright  DB  '(C) IC Book Labs.',0
;---

Program_Vendor_Data_1  DB  '  '
Program_Vendor_Data    DB  'http://uefi.composter.com.ua/' , 0

Msg_Starting           DB  0Dh,0Ah,0Ah,'Starting...',0Dh,0Ah,0

;--- Configuration file support ---

Cfg_File_Name  DB  'UEXTREME.'
Cfg_File_Ext   DB  'CFG',0
Bak_File_Ext   DB  'BAK',0

; Note need write prefix = String_Version.
Cfg_File_Header:
DB  '; Program configuration file, must be not larger 60KB.',13,10
DB  '; Comments started with ";" char.',13,10
DB  '; Xsize   = Video mode horizontal resolution, pixels, 800-2048.',13,10
DB  '; Ysize   = Video mode vertical resolution, pixels, 600-2048.',13,10
DB  '; Pause   = Pause during program start for messages view.',13,10
Cfg_File_Header_Stop:
DB  0

;--- Configuration options names ---

;--- Keywords (size, chars) ---
; Used for key words comparing and write config. file
CfgStr_00  DB  'Xsize'   , 0  ; Video mode X size, pixels
CfgStr_01  DB  'Ysize'   , 0  ; Video mode Y size, pixels
CfgStr_02  DB  '???'     , 0  ; Bits per pixel
CfgStr_03  DB  '?????'   , 0  ; Color scheme: 0-7, 0 is gray
CfgStr_04  DB  '?????'   , 0  ; Sound: 0=OFF or 1=ON
CfgStr_05  DB  '?????'   , 0  ; Game lines done for save
CfgStr_06  DB  '?????'   , 0  ; Game level for save
CfgStr_07  DB  '?????'   , 0  ; Game score for save
CfgStr_08  DB  'Pause'   , 0  ; Pause during program start

;--- Common and Loader blocks, 18 bytes ---
MACRO  CFGL  x1, x2, x3, x4
{
DQ  x1, x2
DB  x3, x4
}

;--- Saver block, 10 bytes ---
MACRO  CFGS  x1, x2, x3
{
DQ  x1
DB  x2, x3
}

;--- List Terminator ---
MACRO  CFGSTOP
{
DQ  0
}

;--- List for chars and keywords handling ---
; "HL" means handler for loader
; "HS" means handler for saver
; Start chars and keywords list, entire list

Keywords_List:
CFGL  CfgChar_00 , HL_Space   , 0 , 0
CFGS               HS_Space   , 0 , 0
CFGL  CfgChar_01 , HL_Tab     , 0 , 0
CFGS               HS_Tab     , 0 , 0
CFGL  CfgChar_02 , HL_LF      , 0 , 0
CFGS               HS_LF      , 0 , 0
CFGL  CfgChar_03 , HL_CR      , 0 , 0
CFGS               HS_CR      , 0 , 0
CFGL  CfgChar_04 , HL_Comment , 0 , 0
CFGS               HS_Comment , 0 , 0
;--- Start keywords list, used also for parameters update, part of list ---
Keywords_List_1:
CFGL  CfgStr_00  , HL_Xsize   ,  _Video_Xsize  , 16
CFGS               HS_Xsize   ,  _Video_Xmax   , 16
CFGL  CfgStr_01  , HL_Ysize   ,  _Video_Ysize  , 16
CFGS               HS_Ysize   ,  _Video_Ymax   , 16
;CFGL  CfgStr_02  , HL_BPP     ,  _Video_BPP    , 8
;CFGS               HS_BPP     ,  _Video_BPPmax , 8
;CFGL  CfgStr_03  , HL_Color   ,  _Color_Scheme , 8
;CFGS               HS_Color   ,  _Color_Scheme , 8
;CFGL  CfgStr_04  , HL_Sound   ,  _Sound_Switch , 8
;CFGS               HS_Sound   ,  _Sound_Switch , 8
;CFGL  CfgStr_05  , HL_Lines   ,  _Game_Lines   , 16
;CFGS               HS_Lines   ,  _Game_Lines   , 16
;CFGL  CfgStr_06  , HL_Level   ,  _Game_Level   , 8
;CFGS               HS_Level   ,  _Game_Level   , 8
;CFGL  CfgStr_07  , HL_Score   ,  _Game_Score   , 8
;CFGS               HS_Score   ,  _Game_Score   , 8
CFGL  CfgStr_08  , HL_Pause   ,  _Start_Pause  , 8
CFGS               HS_Pause   ,  _Start_Pause  , 8
CFGSTOP

