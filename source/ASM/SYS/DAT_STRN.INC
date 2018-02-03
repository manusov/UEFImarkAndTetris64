;===========================================================================;
;                                                                           ;
;               MESSAGES STRINGS FOR PROGRAM START AND EXIT.                ;
;                                                                           ;
;===========================================================================;

;--- Progress messages ---

Msg_C_CPUF   DB  'Check CPU features...',0
Msg_C_CLK    DB  'Measure CPU clock...',0
Msg_C_UEFI   DB  'Check UEFI firmware configuration...',0
Msg_C_Mem    DB  'Check free memory size...',0
Msg_C_ACPI   DB  'Detecting ACPI and MCFG...',0
Msg_L_Cfg    DB  'Load configuration file...',0
Msg_C_Video  DB  'Get graphics protocol info...',0
Msg_C_VMode  DB  'Find compatible video mode...',0
Msg_C_VFlt   DB  'Check flat frame buffer support...',0
Msg_C_PCI    DB  'Check PCI bus present...',0
Msg_C_VGA    DB  'Find active video adapter...',0
Msg_C_BAR    DB  'Find prefetchable BAR for flat frame buffer...',0
Msg_C_AGP    DB  'Check AGP present...',0
Msg_C_PCIe   DB  'Check PCI Express present...',0
Msg_C_MTRR   DB  'Check MTRR present...',0

Msg_S_AGP    DB  'Enable AGP Fast Write...',0
Msg_S_PCIe   DB  'Set PCI Express packet size...',0
Msg_S_MTRR   DB  'Set MTRR attributes...',0
Msg_S_SSE    DB  'Enable SSE context...',0
Msg_S_Mouse  DB  'Detect mouse...',0
Msg_S_Game   DB  'Setup program parameters...',0
Msg_S_Wait   DB  0Dh,0Ah,'Press any key to continue start. N=No wait...',0
Msg_S_Gmod   DB  'Next step is set graphics video mode.',0Dh,0Ah,00Dh,0Ah,0

Msg_R_Tmod   DB  'Restore video mode...',0
Msg_R_SSE    DB  'Restore SSE context...',0
Msg_R_MTRR   DB  'Restore MTRR attributes...',0
Msg_R_PCIe   DB  'Restore PCI Express settings...',0
Msg_R_AGP    DB  'Restore AGP settings...',0
Msg_S_Cfg    DB  'Check program configuration...',0
Msg_R_Mem    DB  'Release memory pages...',0

;--- Fatal errors messages (cause program exit) ---

Msg_E_CPUF   DB  'Too old CPU, required SSE and TSC support.',0
Msg_E_CLK    DB  'CPU clock measurement error.',0
Msg_E_UEFI   DB  'Uncompatible UEFI firmware configuration.',0
Msg_E_Mem    DB  'Insufficient memory, 32MB required.',0
Msg_E_ACPI   DB  'ACPI detection error.',0
Msg_E_Cfg    DB  'Load configuration error.',0
Msg_E_Video  DB  'Compatible graphics protocol not found.',0
Msg_E_VMode  DB  'No compatible video modes, 800x600x32bpp required.',0
Msg_E_VFlt   DB  'Flat frame buffer not supported by video adapter.',0
Msg_E_PCI    DB  'PCI bus detection error. MCFG not found or bad.',0
Msg_E_VGA    DB  'Active video adapter not found by hardware scan.',0
Msg_E_BAR    DB  'Scan PCI bus failure.',0
Msg_E_AGP    DB  'Scan AGP capabilities failure.',0
Msg_E_PCIe   DB  'Scan PCI Express capabilities failure.',0
Msg_E_MTRR   DB  'Invalid MTRR configuration.',0

Msg_SE_AGP   DB  'Set AGP Fast Write mode failure.',0
Msg_SE_PCIe  DB  'Set PCI Express packet size failure.',0
Msg_SE_MTRR  DB  'Set MTRR failure.',0
Msg_SE_SSE   DB  'Enable SSE context failure.',0
Msg_SE_Mouse DB  'Mouse detect error.',0
Msg_SE_Game  DB  'Cannot setup program parameters.',0
Msg_SE_Wait  DB  'Wait key failure.',0
Msg_SE_Gmod  DB  'Set graphics mode failure.',0

Msg_RE_Tmod  DB  'Restore text video mode failure.',0
Msg_RE_SSE   DB  'Restore SSE context failure.',0
Msg_RE_MTRR  DB  'Restore MTRR failure.',0
Msg_RE_PCIe  DB  'Restore PCI Express settings failure.',0
Msg_RE_AGP   DB  'Restore AGP settings failure.',0
Msg_SE_Cfg   DB  'Cannot save configuration.',0
Msg_RE_Mem   DB  'Cannot release memory.',0

;--- Non-fatal errors messages (not cause program exit) ---

Msg_E_Line   DB  'Error in the configuration file, line # '
Msg_E_Line_1 DB  5 DUP (20h),0
Msg_E_File   DB  0Dh,0Ah,'File operations error.',0Dh,0Ah,07h,0

;--- Dialogue questions messages ---

Msg_Q_Save   DB  0Dh, 0Ah, 0Ah, 'Save configuration (y/n) ? : ',0
Msg_Y_Save   DB  0Dh, 0Ah, 'Saving...', 0
Msg_N_Save   DB  0Dh, 0Ah, 'Skipping...', 0

;--- Special strings ---

Msg_Exit     DB  'Exit to UEFI...',0Dh,0Ah,0Ah,0
Msg_CRLF_2   DB  0Dh,0Ah
Msg_CRLF     DB  0Dh,0Ah,0
Msg_Error_1  DB  0Dh,0Ah,'ERROR: ',0
Msg_Error_2  DB  0Dh,0Ah,'Press any key...',0

Msg_OK       DB  'OK',0
Msg_FAILED   DB  'FAILED',0

Msg_CRLF_U   DU  0Dh,0Ah,0
