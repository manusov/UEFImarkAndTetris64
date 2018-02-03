;===========================================================================;
;                                                                           ;
;                  EQUATIONS FOR ACCESS GLOBAL VARIABLES.                   ;
;          THIS NUMBERS IS OFFSETS FROM GLOBAL_VARIABLES_POOL BASE.         ;
;       SEE FILE " DAT_VARS.INC " FOR DETAIL COMMENTS ABOUT VARIABLES.      ;
;                                                                           ;
;===========================================================================;

;--- CPU multimedia context constants ---

_Load_MXCSR         = -060h   ; dword , -060h-05Dh
; Reserved = -05Ch = 4 bytes reserved, -05Ch-059h

;--- Video configuration constants ---

_Video_Xmin         = -058h   ; word  , -058h-057h
_Video_Xmax         = -056h   ; word  , -056h-055h
_Video_Ymin         = -054h   ; word  , -054h-053h
_Video_Ymax         = -052h   ; word  , -052h-051h
_Video_BPPmin       = -050h   ; byte  , -050h-050h
_Video_BPPmax       = -04Fh   ; byte  , -04Fh-04Fh
; Reserved = 6 bytes reserved, -04Eh-049h

;--- Time count and visual control ---

_Old_Seconds        = -048h   ; byte  , -048h-048h
_Chg_Seconds        = -047h   ; byte  , -047h-047h
_Chg_Status         = -046h   ; byte  , -046h-046h
; Reserved = 5 bytes reserved, -045h-041h
_Start_Time         = -040h		; qword , -040h-039h

_Count_Active       = -038h   ; byte  , -038h-038h
; Reserved = -037h = 7 bytes reserved, -037h-031h
_Count_Event        = -030h   ; qword , -030h-029h

;--- Game control, for Tetris ---

_Game_ON            = -028h   ; byte  , -028h-028h
_Sound_Switch       = -027h   ; byte  , -027h-027h
_Color_Scheme       = -026h   ; byte  , -026h-026h
_Game_Figures       = -025h   ; word  , -025h-024h
_Game_Lines         = -023h   ; word  , -023h-022h
_Game_Level         = -021h   ; byte  , -021h-021h
_Game_Score         = -020h   ; byte  , -020h-020h

_Game_ON_Plus       = -028h   ; word  , alias
_Figures_Plus       = -025h   ; dword , alias
_Level_Plus         = -021h   ; word  , alias

_Current_Figure     = -01Fh   ; byte  , -01Fh-01Fh
_Current_X          = -01Eh   ; word  , -01Eh-01Dh
_Current_Y          = -01Ch   ; word  , -01Ch-01Bh
_Current_X_Y        = -01Eh   ; dword , alias
_Current_Rotate     = -01Ah   ; byte  , -01Ah-01Ah
_Current_Quant      = -019h   ; dword , -019h-016h
_Target_Quant       = -015h   ; qword , -015h-00Eh

_Delta_X            = -00Dh   ; word  , -00Dh-00Ch
_Delta_Y            = -00Bh   ; word  , -00Bh-00Ah
_Delta_X_Y          = -00Dh   ; dword , alias
_Delta_Rotate       = -009h   ; byte  , -009h-009h
_Deferred_New       = -008h   ; byte  , -008h-008h
_Deferred_Line      = -007h   ; word  , -007h-006h
; Reserved = bytes reserved, -005h-002h

_Start_Pause        = -001h   ; byte  , -001h-001h

;--- Base BP here ---

;--- Save context at start of UEFI application ---

_EFI_Handle         = 000h    ; qword , 000h-007h
_EFI_Table          = 008h    ; qword , 008h-00Fh

;--- UEFI memory allocate control ---

_EFI_Mem_Total      = 010h    ; qword , 010h-017h
_Use_Mem_Base       = 018h    ; qword , 018h-01Fh
_Use_Mem_Size       = 020h    ; qword , 020h-027h

;--- Bitmap for initialization and de-initialization control ---

_Sys_Bitmap         = 028h    ; word  , 028h-029h
_Sys_Bitmap_L       = 028h    ; byte  , alias
_Sys_Bitmap_H       = 029h    ; byte  , alias
_Address_Size       = 02Ah    ; byte  , 02Ah-02Ah
; Reserved = 5 bytes reserved, 02Bh-02Fh

;--- Time measurement and TSC control ---

_CPUCLK_Period      = 030h    ; qword , 030h-037h
_Microseconds       = 038h    ; qword , 038h-03Fh

;--- Video addressing and geometry control ---

_Current_Vmode      = 040h    ; dword , 040h-043h
_Target_Vmode       = 044h    ; dword , 044h-047h

_Video_Mode_R       = 048h    ; word  , 048h-049h
_Video_Xsize        = 04Ah    ; word  , 04Ah-04Bh
_Video_Ysize        = 04Ch    ; word  , 04Ch-04Dh
_Video_XYsize       = 04Ah    ; dword , alias
_Video_XYsize_Plus  = 04Ah    ; qword , alias
_Video_Xbytes       = 04Eh    ; word  , 04Eh-04Fh
_Vmem_Base          = 050h    ; qword , 050h-057h
_Vmem_Base_L        = 050h    ; dword , alias
_Vmem_Base_H        = 054h    ; dword , alias
_Vmem_Size          = 058h    ; qword , 058h-05Fh
_Vmem_Size_L        = 058h    ; dword , alias
_Vmem_Size_H        = 05Ch    ; dword , alias
_Vmem_Total         = 060h    ; qword , 060h-067h

;--- Video colors control ---

_Video_BPP          = 068h    ; byte  , 068h-068h
_Video_R_Width      = 069h    ; byte  , 069h-069h
_Video_R_Lsb        = 06Ah    ; byte  , 06Ah-06Ah
_Video_G_Width      = 06Bh    ; byte  , 06Bh-06Bh
_Video_G_Lsb        = 06Ch    ; byte  , 06Ch-06Ch
_Video_B_Width      = 06Dh    ; byte  , 06Dh-06Dh
_Video_B_Lsb        = 06Eh    ; byte  , 06Eh-06Eh
; Reserved = 1 byte reserved, 06Fh-06Fh
_Video_Colors       = 068h    ; qword , alias

;--- Mouse control ---

_Mouse_Present      = 070h    ; byte  , 070h-070h
; Reserved = 3 byte reserved, 071h-073h
_Mouse_X            = 074h    ; word  , 074h-075h
_Mouse_Y            = 076h    ; word  , 076h-077h
_Mouse_Xmin         = 078h    ; word  , 078h-079h
_Mouse_Xmax         = 07Ah    ; word  , 07Ah-07Bh
_Mouse_Ymin         = 07Ch    ; word  , 07Ch-07Dh
_Mouse_Ymax         = 07Eh    ; word  , 07Eh-07Fh

;--- PCI bus configuration control ---

_Last_PCI_Bus       = 080h    ; byte  , 080h-080h
_PBAR_Reg           = 081h    ; byte  , 081h-081h
_Act_Video_Cfg      = 082h    ; word  , 082h-083h
_Act_Video_Bus      = 083h    ; byte  , alias
_Host_Cfg           = 084h    ; word  , 084h-085h
_Adapter_Cfg        = 086h    ; word  , 086h-087h	
_PBAR_Base          = 088h    ; qword , 088h-08Fh
_PBAR_Base_E        = 08Ch    ; dword , alias
_PBAR_Size          = 090h    ; qword , 090h-097h
_PBAR_Size_E        = 094h    ; dword , alias

;--- AGP capabilities control ---

_AGP_H_Reg          = 098h    ; byte  , 098h-098h
_AGP_H_Stat         = 099h    ; byte  , 099h-099h
_AGP_H_Cmd          = 09Ah    ; dword , 09Ah-09Dh
_AGP_A_Reg          = 09Eh    ; byte  , 09Eh-09Eh
_AGP_A_Stat         = 09Fh    ; byte  , 09Fh-09Fh
_AGP_A_Cmd          = 0A0h    ; dword , 0A0h-0A3h
; Reserved = 4 bytes reserved, 0A4h-0A7h

;--- PCI Express capabilities control ---

_PCIe_H_Reg         = 0A8h    ; byte  , 0A8h-0A8h
_PCIe_H_Caps        = 0A9h    ; byte  , 0A9h-0A9h
_PCIe_H_Ctrl        = 0AAh    ; word  , 0AAh-0ABh
_PCIe_H_Ctrl_L      = 0AAh    ; byte  , alias
_PCIe_A_Reg         = 0ACh    ; byte  , 0ACh-0ACh
_PCIe_A_Caps        = 0ADh    ; byte  , 0ADh-0ADh
_PCIe_A_Ctrl        = 0AEh    ; word  , 0AEh-0AFh
_PCIe_A_Ctrl_L      = 0AEh    ; byte  , alias
; Reserved = 8 bytes reserved, 0B0h-0B7h

;--- MTRR context control ---

_MTRR_Used          = 0B8h    ; dword , 0B8h-0BBh
_MTRR_Rsvd_1        = 0BCh    ; dword , 0BCh-0BFh
_MTRR_S_Base        = 0C0h    ; qword , 0C0h-0C7h
_MTRR_S_Base_L      = 0C0h    ; dword , alias
_MTRR_S_Base_H      = 0C4h    ; dword , alias
_MTRR_S_Mask        = 0C8h    ; qword , 0C8h-0CFh
_MTRR_S_Mask_L      = 0C8h    ; dword , alias
_MTRR_S_Mask_H      = 0CCh    ; dword , alias

;--- SSE and multimedia context control ---

_SSE_S_CR4          = 0D0h    ; qword , 0D0h-0D7h
_SSE_S_MXCSR        = 0D8h    ; dword , 0D8h-0DBh
; Reserved = 4 bytes reserved, 0DCh-0DFh

;--- ACPI and memory-mapped PCI configuration support ---

_ACPI_Version       = 0E0h    ; byte  , 0E0h-0E0h
; Reserved = 7 bytes reserved, 0E1h-0E7h
_ACPI_Base          = 0E8h    ; qword , 0E8h-0EFh
_MCFG_Base          = 0F0h    ; qword , 0F0h-0F7h
_MCFG_Size          = 0F8h    ; qword , 0F8h-0FFh

;--- UEFI Protocols list and file system handles ---

_EFI_Gop            = 100h    ; qword , 100h-107h
_EFI_UgaDraw        = 108h    ; qword , 108h-10Fh
_EFI_Dir_Volume     = 110h    ; qword , 110h-117h

;--- UEFI operations buffers and status reporting data ---

_EFI_Action         = 118h    ; byte  , 118h-118h
_EFI_Operation      = 119h    ; byte  , 119h-119h
_EFI_Act_Op         = 118h    ; word  , alias
; Reserved = 6 bytes reserved, 11Ah-11Fh
_EFI_Status         = 120h    ; qword , 120h-127h
_File_Mem_Base      = 128h    ; qword , 128h-12Fh
_File_Mem_Size      = 130h    ; qword , 130h-137h

;--- CSM status and additional variables ---

_EFI_CSM_Found      = 138h    ; byte  , 138h-138h
; Reserved = 7 bytes reserved, 139h-13Fh

;--- Support UEFImark Extreme Edition ---

_Extreme_Control    = 140h    ; dword , 140h-140h
_Ext_Common         = 140h    ; byte  , alias
_Ext_Requests       = 141h    ; byte  , alias
_Ext_Transit        = 142h    ; byte  , alias
_Ext_Errors         = 143h    ; byte  , alias
_Resizable_Reg      = 144h    ; word  , 144h-145h
_Resizable_Value    = 146h    ; byte  , 146h-146h
; Reserved = byte reserved, 147h-147h
_Ext_PBAR_Base      = 148h    ; qword , 148h-14Fh
_Ext_PBAR_Base_H    = 14Ah    ; word  , alias
_Ext_PBAR_Base_E    = 14Ch    ; dword , alias
_Ext_PBAR_Size      = 150h    ; qword , 150h-157h
_Ext_PBAR_Size_H    = 152h    ; word , alias
_Ext_PBAR_Size_E    = 154h    ; dword , alias
_Ext_Bridge_L       = 158h    ; dword , 158h-15Bh
_Ext_Bridge_H_L     = 15Ch    ; dword , 15Ch-15Fh
_Ext_Bridge_H_H     = 160h    ; dword , 160h-163h
_Ext_Bridge_NP      = 164h    ; dword , 164h-167h
_Ext_Save_Vbase     = 168h    ; qword , 168h-16Fh

_FW_MTRR_Used       = 170h    ; dword , 0B8h-0BBh
_FW_MTRR_Rsvd_1     = 174h    ; dword , 0BCh-0BFh
_FW_MTRR_S_Base     = 178h    ; qword , 0C0h-0C7h
_FW_MTRR_S_Base_L   = 178h    ; dword , alias
_FW_MTRR_S_Base_H   = 17Ch    ; dword , alias
_FW_MTRR_S_Mask     = 180h    ; qword , 0C8h-0CFh
_FW_MTRR_S_Mask_L   = 180h    ; dword , alias
_FW_MTRR_S_Mask_H   = 184h    ; dword , alias

