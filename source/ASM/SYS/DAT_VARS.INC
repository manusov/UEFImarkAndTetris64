;===========================================================================;
;                                                                           ;
;                        CONSTANTS AND VARIABLES.                           ;
;                                                                           ;
;===========================================================================;

; Label GLOBAL_VARIABLES_POOL located at center of pool, not start of pool,
; it optimal for make offsets 8-bit, optimal offsets from -128 to +127.
; Optimal interval is [-128...+127], not [0...+255]
; because offsets is signed.
; Constants, take place in the EXE/EFI module located at start of block,
; for example " DB Value ",
; constants, don't take place in the EXE/EFI module located at end of block,
; for example " DB ? ",
; this for real file size minimization.

;---------- CPU constants --------------------------------------------------;

;--- MXCSR = Multimedia Control and Status Register ---
; "Flag" bits is errors indicators
; "Mask" bits allow ignore errors
; See IA-32 documentation for details
; MXCSR Bits Comments:
; Initial Value 00001F80h = 0000 0000 0000 0000 0001 1111 1000 0000 b
;
; D0     = 0 = Invalid Operation Flag
; D1     = 0 = Denormal Operand Flag
; D2     = 0 = Divide-by-Zero Flag
; D3     = 0 = Overflow Flag
; D4     = 0 = Underflow Flag
; D5     = 0 = Precision Flag
; D6     = 0 = Denormals are Zeroes Flag
; D7     = 1 = Invalid Operations Mask
; D8     = 1 = Denormal Operand Mask
; D9     = 1 = Divide-by-Zero Mask
; D10    = 1 = Overflow Mask
; D11    = 1 = Underflow Mask
; D12    = 1 = Precision Mask
; D14,13 = 00 = Rounding Control Field
; D15    = 0 = Flush to Zero mode
; D16-31 = 0 = Reserved

Load_MXCSR      DD  00001F80h
MXCSR_Rsvd_1    DD  0    ; Reserved for extensions and alignment 8

;---------- Video constants ------------------------------------------------;

;--- Hard restrictions ---
Limit_X		=  2048    ; Limit X, pixels
Limit_Y		=  2048    ; Limit Y, pixels
Limit_BPP	=  32      ; Limit bits per pixel

;--- Ranges for video mode selection ---
; Note. Minimum yet 800x600, required support 640x480.

Video_Xmin      DW  800   ;! 640 ; Minimum X, pixels
Video_Xmax      DW  1024  ; Maximum X, pixels, updated by load cfg
Video_Ymin      DW  600   ;! 480 ; Minimum Y, pixels
Video_Ymax      DW  768   ; Maximum Y, pixels, updated by load cfg
Video_BPPmin    DB  32    ; Minimum bits per pixel
Video_BPPmax    DB  32    ; Maximum bits per pixel, updated by load cfg
Video_Rsvd_2    DB  6 DUP (?)  ; Reserved for extensions and alignment 8

;---------- Time and game status visual control ----------------------------;

Old_Seconds     DB  ?   ; Save seconds for changes detect
Chg_Seconds     DB  ?   ; Time revisualization request
Chg_Status      DB  ?   ; Status revisualization request, D0=T, D1=S
Chg_Rsvd_1      DB  5 DUP (?)  ; Reserved for extensions and alignment 8
Start_Time      DQ  ?   ; Microseconds from game start

;---------- Delayed events control -----------------------------------------;

Count_Active    DB  ?          ; 0=No delayed events, 1=Wait for delayed ev.
Count_Rsvd_1    DB  7 DUP (?)  ; Reserved for extensions and alignment 8
Count_Event     DQ  ?          ; Time, microseconds, for delayed event

;---------- Game control ---------------------------------------------------;

Game_ON         DB  ?   ; Game flow switch: 0=OFF, 1=ON
Sound_Switch    DB  ?   ; Sound enable: 0=Disabled, 1=Enabled
Color_Scheme    DB  ?   ; Color scheme: 0-7, 0 is gray
Game_Figures    DW  ?   ; Number of game figures done
Game_Lines      DW  ?   ; Number of game lines done
Game_Level      DB  ?   ; Current game level
Game_Score      DB  ?   ; Game score

Current_Figure  DB  ?   ; Current random selected figure
Current_X       DW  ?   ; Signed addend for figure current X, pixels
Current_Y       DW  ?   ; Signed addend for figure current Y, pixels
Current_Rotate  DB  ?   ; Signed addend for figure rotation, 90 deg.
Current_Quant   DD  ?   ; Time quant for figure fall down, microsec.
Target_Quant    DQ  ?   ; Timer value for make next fall by pixel

Delta_X         DW  ?   ; Signed addend request for current X, pixels
Delta_Y         DW  ?   ; Signed addend request for current Y, pixels
Delta_Rotate    DB  ?   ; Signed rotation request for figure
Deferred_New    DB  ?   ; Deferred request flag for new figure
Deferred_Line   DW  ?   ; Deferred line, must fall down after visual

Game_Rsvd_1     DB  4 DUP (?)  ; Reserved for extensions and alignment 8

Start_Pause     DB  ?   ; Pause flag for start messages view

;---------- Base point for addressing, minimize offsets width -128/+127 ----;

Global_Variables_Pool:

;---------- Save parameters, received from UEFI firmware or UEFI shell -----;

EFI_Handle      DQ  ?   ; This application handle
EFI_Table       DQ  ?   ; System table address

;---------- Memory allocation control --------------------------------------;

EFI_Mem_Total   DQ  ?   ; Total memory reported by firmware, bytes
Use_Mem_Base    DQ  ?   ; Base address of this application pool
Use_Mem_Size    DQ  ?   ; Size of of this application pool

;---------- Context save-modify-restore control variables ------------------;
; Feature available flags:  
; D0=AGP, D1=PCIE, D2=MTRR, D[3]=Reserved
; Feature unconditional reprogramming flags:
; D4=Memory allocated by UEFI firmware, release required
; D5=Video mode changed by UEFI GOP, restore and clear required
; UEFImark Extreme Edition support:
; D[7,6]=Context control, for Extreme Edition only
;         00b = Normal context, no Extreme Edition
;         01b = Normal context run under Extreme Edition
;         10b = Extreme Mode 2, change status, base address and size of VRAM
;         11b = Extreme Mode 3, change VRAM status, but base, size unchanged
; Feature conditional reprogramming flags:
; D8=AGP, D9=PCIE, D10=MTRR, D11=CR4+MXSCR(SSE), 
; D12=Reserved (4GB in DOS version)
; Additional flags:
; D13=Flag means Adapter_Cfg is address of 3D Controller, not VGA Controller,
;     this disable extreme mode
; D14=Enable pause for messages screen view during program start
; D15=Enable save configuration file, changes detected

Sys_Bitmap	    DW  ?   ; Sequence actions bitmap

;---------- Physical addressing control variables --------------------------;

Address_Size    DB  ?   ; CPU Physical address size, bits
Address_Rsvd_1  DB  5 DUP (?)  ; Reserved for extensions and alignment 8

;---------- CPU clock and precision time variables -------------------------;

CPUCLK_Period   DQ  ?   ; CPU Core Clock Period, femptoseconds
Microseconds    DQ  ?   ; Time stamp, microseconds

;---------- Video subsystem control ----------------------------------------;

Current_Vmode   DD  ?   ; Current mode, used by firmware or shell
Target_Vmode    DD  ?   ; Target mode for game

Video_Mode_R    DW  ?   ; Reserved
Video_Xsize     DW  ?   ; X, pixels for selected video mode
Video_Ysize     DW  ?   ; Y, pixels for selected video mode
Video_Xbytes    DW  ?   ; Possible SizeInMemory > SizeVisualized
Vmem_Base       DQ  ?   ; Linear 64-bit address for Video RAM access
Vmem_Size       DQ  ?   ; Flat frame buffer size, calculated by X,Y
Vmem_Total      DQ  ?   ; Total video memory by physical scan

Video_BPP       DB  ?   ; Bits per pixel for selected video mode
Video_R_Width   DB  ?   ; Width of Red color field
Video_R_Lsb     DB  ?   ; Least significant bit of Red color field
Video_G_Width   DB  ?   ; Width of Green color field
Video_G_Lsb     DB  ?   ; Least significant bit of Green color field
Video_B_Width   DB  ?   ; Width of Blue color field
Video_B_Lsb     DB  ?   ; Least significant bit of Blue color field
Video_Rsvd_1    DB  ?   ; Reserved for extensions and alignment 8

;---------- Mouse control variables ----------------------------------------;

Mouse_Present   DB  ?   ; Mouse flag: 0=absent, 1=present
Mouse_Rsvd_1    DB  3 DUP (?)  ; Reserved for extensions and alignment 8
Mouse_X         DW  ?   ; Mouse current X coordinate
Mouse_Y         DW  ?   ; Mouse current Y coordinate
Mouse_Xmin      DW  ?   ; Mouse coordinate limit: Xmin
Mouse_Xmax      DW  ?   ; Mouse coordinate limit: Xmax
Mouse_Ymin      DW  ?   ; Mouse coordinate limit: Ymin
Mouse_Ymax      DW  ?   ; Mouse coordinate limit: Ymax

;---------- PCI configuration variables ------------------------------------;

Last_PCI_Bus    DB  ?   ; Number of PCI buses minus 1
PBAR_Reg        DB  ?   ; Video RAM BAR register address, 0=None
Act_Video_Cfg   DW  ?   ; Active video adapter bus/device/function
Host_Cfg        DW  ?   ; Host component bus/device/function
Adapter_Cfg     DW  ?   ; PCIe video adapt./3D controller bus/dev/fnc
PBAR_Base       DQ  ?   ; Video RAM prefetchable BAR base address
PBAR_Size       DQ  ?   ; Video RAM prefetchable BAR size

;---------- AGP variables --------------------------------------------------;

AGP_H_Reg       DB  ?   ; AGP Host Cap. Reg. Address (at Host_Cfg)
AGP_H_Stat      DB  ?   ; AGP H.Stat. D[7-0]=FW,AGP3,0,0,8x,4x,2x,1x
AGP_H_Cmd       DD  ?   ; Old state of Host AGP Command Register
AGP_A_Reg       DB  ?   ; AGP Adapter Cap. Reg. Addr. (at Adapt._Cfg)
AGP_A_Stat      DB  ?   ; AGP A.Stat. D[7-0]=FW,AGP3,0,0,8x,4x,2x,1x
AGP_A_Cmd       DD  ?   ; Old state of Adapter AGP Command Register    
AGP_Rsvd_1      DD  ?   ; Reserved for extensions and alignment 8

;---------- PCI Express variables ------------------------------------------;

PCIe_H_Reg      DB  ?   ; PCIe Host Cap. Reg. Address (at Host_Cfg)
PCIe_H_Caps     DB  ?   ; PCIe H. Device Cap. Register, bits [7-0]
PCIe_H_Ctrl     DW  ?   ; Old state of Host PCIe Device Control Reg.
PCIe_A_Reg      DB  ?   ; PCIe Adapter Cap. Reg. Addr. (at Host_Cfg)
PCIe_A_Caps     DB  ?   ; PCIe A. Device Cap. Register, bits [7-0]     
PCIe_A_Ctrl     DW  ?   ; Old state of A. PCIe Device Control Reg.
PCIe_Rsvd_1     DQ  ?   ; Reserved for extensions

;---------- MTRR variables -------------------------------------------------;

MTRR_Used       DD  ?   ; MSR address for used MTRR
                        ; Spec. values: 0=Not supported, 1=No vacant
MTRR_Rsvd_1     DD  ?   ; Reserved for extensions and alignment 8
MTRR_S_Base     DQ  ?   ; Save old MTRR state, Base(i) MSR
MTRR_S_Mask     DQ  ?   ; Save old MTRR state, Mask(i) MSR

;---------- SSE context variables ------------------------------------------;

SSE_S_CR4       DQ  ?   ; Save old CR4 state
SSE_S_MXCSR     DD  ?   ; Save old MXCSR state
SSE_Rsvd_1      DD  ?   ; Reserved for extensions and alignment 8

;---------- ACPI variables -------------------------------------------------;

ACPI_Version    DB ?    ; ACPI version byte from RSDP
ACPI_Rsvd_1     DB 7 DUP (?)  ; Reserved for extensions and alignment 8
ACPI_Base       DQ ?    ; Pointer to ACPI data, started from RSDP
MCFG_Base       DQ ?    ; Base address of PCIe MCFG MMIO (by ACPI)
MCFG_Size       DQ ?    ; Size of PCIe MCFG MMIO (by ACPI)

;---------- UEFI protocols pointers and UEFI-specific data -----------------;

EFI_Gop         DQ  ?   ; Pointer to UEFI GOP protocol interface
EFI_UgaDraw     DQ  ?   ; Pointer to UEFI UGADRAW protocol interface
EFI_Dir_Volume  DQ  ?   ; Pointer to File protocol for used volume

;---------- UEFI operations buffers and status reporting data --------------;

EFI_Action      DB  ?   ; Action, caused error, 0=None
EFI_Operation   DB  ?   ; Detalize operation: API call, caused error
EFI_Rsvd_1      DB  6 DUP (?)	; Reserved
EFI_Status      DQ  ?   ; Status, RAX after operation
File_Mem_Base   DQ  ?   ; Base address of buffer for file operations
File_Mem_Size   DQ  ?   ; Size of buffer for file operations
                        ; Buffer also used for generating memory map

;---------- CSM status and additional variables ----------------------------;

EFI_CSM_Found   DB  ?   ; CSM detect status: 0=found, 1=Not found
CSM_Rsvd_1      DB  7 DUP (?)  ; Reserved for extensions and alignment 8

;---------- UEFImark Extreme Edition support -------------------------------;

; Extreme_Control (dword) variable bits:
; --- Common bits, Ext_Common ---
; D00 = Extreme context selected by user
; D01 = Extreme context required and supported
; D02 = Extreme context physically set OK, restore required
; D03 = Reserved
; D04 = Used by Extreme Mode 3: MTRR reprogrammed flag
; D05 = Used by Extreme Mode 3: PCI Bridge reprogrammed flag
; D[06-07] = Reserved
; --- Requests bits, Ext_Requests ---
; D08 = Ext_Allocation_Flag (UEFI memory map resources)
; D09 = Ext_Relocate_Flag (BAR reprogramming)
; D10 = Ext_Resize_Flag (BAR resizing)
; D11 = Ext_MTRR_Flag, used from normal context routines
; D[12-15] = Reserved for new operations codes
; --- Transit bits, Ext_Transit ---
; D16 = Both video adaptor BAR and PCIe bridge support 64-bit
; D[17-23] = Reserved
; --- Error reporting bits, Ext_Errors ---
; D24 = UEFI address space request failed
; D25 = Video RAM BAR address relocation error
; D26 = Video RAM BAR resizing error
; D27 = Video RAM BAR programming MTRR error
; D[28-31] = Reserved for new error codes
; ---

Extreme_Control DD  ?   ; Extreme system context control
Resizable_Reg   DW  ?   ; Resizable BAR cap. addr., 16-bit/4KB-space
Resizable_Value DB  ?   ; Save resizable BAR current size
Ext_Rsvd_1      DB  ?   ; Reserved for alignment
Ext_PBAR_Base   DQ  ?   ; VRAM Prefetchable BAR new base for extreme
Ext_PBAR_Size   DQ  ?   ; VRAM Prefetchable BAR new size for extreme
Ext_Bridge_L    DD  ?   ; Save PCI Express bridge start/end base low
Ext_Bridge_H_L  DD  ?   ; Save PCI Express bridge start base high
Ext_Bridge_H_H  DD  ?   ; Save PCI Express bridge end base high
Ext_Bridge_NP   DD  ?   ; Save PCIe bridge non prefetchable range
Ext_Save_Vbase  DQ  ?   ; Save video memory base address

; FW means Firmware Allocated UC region
FW_MTRR_Used    DD  ?   ; MSR address for used MTRR
                        ; Spec. values: 0=Not supported, 1=No vacant
FW_MTRR_Rsvd_1  DD  ?   ; Reserved for extensions and alignment 8
FW_MTRR_S_Base  DQ  ?   ; Save old MTRR state, Base(i) MSR
FW_MTRR_S_Mask  DQ  ?   ; Save old MTRR state, Mask(i) MSR

;--- Buffers for built and convert ASCII (compact8) to UNICODE (big16) -----;

Buffer_ASCII    DB  160 DUP (?)
Buffer_UNICODE  DU  160 DUP (?)

;---------- Multifunctional buffers (not part of pool) ---------------------;
; Usage 1 = Buffer for graphics functions return info
; Usage 2 = Video palette unpack from R,G,B to hardware-specific values

Runtime_Palette_256:				; Alias, start
V_Adapter_Info  DB  512 DUP (?)
V_Mode_Info     DB  256 DUP (?)
Addend_Palette  DB  256 DUP (?)

; Bitmap for enable video strings update, 1 bit per string
; 256 bytes = 2048 bits, maximum 2048 strings

L_Show_Size	= 256
Lines_Show      DB  L_Show_Size DUP (?)

; Usage 1 = Benchmarks results: 3*16 bytes (SSE, 386, BLT)
; Usage 2 = Numbers convert to ASCII, game status window

Buffer_1_Size	= 48
Buffer_1        DB  Buffer_1_Size DUP (?)

; Usage 1 = System information strings
; Usage 2 = Scan PCI configuration space
; Usage 3 = Scan UEFI handles and protocols data base

Buffer_2_Size	= 1024
PCI_Matches_1:  ; Alias, start
Buffer_2        DB  Buffer_2_Size DUP (?)
PCI_Matches_2:  ; Alias, stop

; Buffer_3 used for unpack mouse cursor from BMP to sprite,
; maximum size = 32 pixels * 32 pixels * 4 bytes = 4096KB
; +8 bytes for save aligned linear address
; +4 bytes for save Xsize, Ysize
; +16 bytes for header
; +16 bytes for dynamical alignment
; total 4140 bytes

Buffer_3_Size	= 4140
Buffer_3        DB  Buffer_3_Size DUP (?)

; Buffer_4 used for save video memory image before draw mouse cursor
; maximum size = 32 pixels * 32 pixels * 4 bytes = 4096KB
; +8 bytes for save aligned linear address
; +16 bytes for dynamical alignment
; total 4120 bytes

Buffer_4_Size	= 4120
Buffer_4        DB  Buffer_4_Size DUP (?)
Buffer_4_Rsvd   DQ  ?    ; Reserved for extensions and alignment 8

; Buffer_5 used for built date and time string
; Example: "September, 26  2012 , Wednesday , 20:44:55"

Buffer_5_Size	= 48
Buffer_5_1	= 20
Buffer_5_2	= 32
Buffer_5        DB  Buffer_5_Size DUP (?)

; Buffer_6 used for save score gamer name

Buffer_6_Size	= 40
Buffer_6        DB  Buffer_6_Size DUP (?)

; Big buffers dynamically allocated by EFI services
