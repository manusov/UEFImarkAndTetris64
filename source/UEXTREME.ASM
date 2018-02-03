;===========================================================================;
;                                                                           ;
;            GRAPHICAL BENCHMARKS. UEFI X64 EDITION. MAIN MODULE.           ;
;          SEPARATE AND EXTREME EDITION = UEFIMARK WITHOUT TETRIS.          ;
;                        (C)2013-2017 IC Book Labs.                         ;
;                                                                           ;
;    PLEASE USE FASM EDITOR 2.0 FOR THIS SOURCES, FOR CORRECT TABULATIONS.  ;
;      Text format optimization for FASM Editor 2.0 started from v1.07.     ;            
;                                                                           ;
;===========================================================================;

;---------------------------------------------------------------------------;
;                         OPTIONS AND EQUATIONS.                            ;
;---------------------------------------------------------------------------;
INCLUDE 'ASM\SYS\equ_api.inc'   ; Equations for UEFI API
INCLUDE 'ASM\SYS\equ_errs.inc'  ; Equations for UEFI errors messages
INCLUDE 'ASM\SYS\equ_chkp.inc'  ; Equations for Port 80 use
INCLUDE 'ASM\SYS\equ_vars.inc'  ; Equations for variables access

format pe64 dll efi
entry main

;---------------------------------------------------------------------------;
;                        CODE SECTION, HEADBLOCK.                           ;
;---------------------------------------------------------------------------;
section '.text' code executable readable
main:

;---------- EFI application must save registers ----------------------------;
push rbx rcx rdx rsi rdi rbp
push r8 r9 r10 r11 r12 r13 r14 r15

;---------- Send first checkpoint code to Port 80h -------------------------;
CHK	001h

;---------- Initializing parameters ----------------------------------------;
cld
mov r15,Global_Variables_Pool   ; Don't use LEA because UEFI relocation bug
xor eax,eax
mov [_Sys_Bitmap+r15],ax
mov [_Extreme_Control+r15],eax

;---------- Save context received from firmware or UEFI shell --------------;
mov [_EFI_Handle+r15],rcx
mov [_EFI_Table+r15],rdx
		
;---------- Send first message to screen -----------------------------------;
mov al,07h
call Output_Attribute
mov rsi,Msg_CRLF_2
call String_Write			            ; Make new string
mov rsi,String_Version
call String_Write			            ; "Name+Copyright"
mov rsi,Program_Vendor_Data_1
call String_Write_Bold		        ; "http"
mov rsi,Msg_Starting
call String_Write			            ; "Starting..."

;---------- Start Program Sequence execution -------------------------------;
mov rsi,Start_Program_Sequence
call Start_Stop_Sequencer         ; Checkpoints 002h-01Bh
jc Bad_Config

;---------- System Info and Benchmarks with visualize parameters -----------;
; Checkpoints 020h-02Ah, normal context
		or	byte [_Sys_Bitmap_L+r15],01000000b
		mov	rsi,Get_Sys_Info_Sequence
		call	SysInfo_Sequencer

;---------- Wait key press or mouse click ----------------------------------;
; Under construction, required support Keyboard+Mouse, yet Keyboard only.
CHK 02Fh                          ; Checkpoint 02Fh = Wait user events
call Input_Wait_Key

;---------- Conditionally save sysinfo-benchmarks report to TXT file -------;
; Note check all RDX because need differentiate keyboard/mouse by RDX.63 bit
cmp rdx,00000013h		              ; UEFI key code = F9
jne Next_Sequen_1		              ; Go skip if not F9 key
call Save_Report_to_TXT

;---------- Conditionally save sysinfo-benchmarks screen to BMP file -------;
; Note check all RDX because need differentiate keyboard/mouse by RDX.63 bit
Next_Sequen_1:
cmp rdx,00000014h                 ; UEFI key code = F10
jne Skip_Screen_1                 ; Go skip if not F10 key
call Save_Screen_to_BMP
Skip_Screen_1:
call Input_Reset		              ; Otherwise F9 bug

;---------- Initializing extreme context -----------------------------------;
xor byte [_Sys_Bitmap_L+r15],11000000b
mov rsi,Set_Extreme_Context_Seq_M2
call Extreme_Sequencer            ; Checkpoints 0B0h-0BFh

;---------- System Info and Benchmarks with visualize parameters -----------;
mov rsi,Get_Sys_Info_Sequence
call SysInfo_Sequencer            ; Checkpoints 020h-02Ah, extreme context

;---------- Restore normal context, disable extreme context ----------------;
mov rsi,Clear_Extreme_Context_Seq_M2
call Extreme_Sequencer            ; Checkpoints 0C0h-0CFh

;---------- Wait key press or mouse click ----------------------------------;
; Under construction, required support Keyboard+Mouse, yet Keyboard only.
CHK 02Fh                          ; Checkpoint 02Fh = Wait user events
call Input_Wait_Key

;---------- Conditionally save sysinfo-benchmarks report to TXT file -------;
; Note check all RDX because need differentiate keyboard/mouse by RDX.63 bit
cmp rdx,00000013h                 ; UEFI key code = F9
jne Next_Sequen_2                 ; Go skip if not F9 key
call Save_Report_to_TXT

;---------- Conditionally save sysinfo-benchmarks screen to BMP file -------;
; Note check all RDX because need differentiate keyboard/mouse by RDX.63 bit
Next_Sequen_2:
cmp rdx,00000014h                 ; UEFI key code = F10
jne Skip_Screen_2                 ; Go skip if not F10 key
call Save_Screen_to_BMP
Skip_Screen_2:

;---------- Exit Program Sequence execution --------------------------------;
mov rsi,Stop_Program_Sequence
ESeq_Cycle:
call Start_Stop_Sequencer         ; Checkpoints 070h-076h
jnc ESeq_Stop
push rsi
mov rsi,Msg_Error_2
call String_Write
call Input_Wait_Key               ; Wait for key press
mov rsi,Msg_CRLF                  ; Next string
call String_Write		
pop rsi
jmp	ESeq_Cycle
ESeq_Stop:

;---------- Send last message to screen, before program vendor data --------;
call Status_Msg_Write             ; Results of F9=TXT, F10=BMP
mov rsi,Msg_Exit
call String_Write

;---------- Message with program vendor data and/or requirements -----------;
mov rsi,Program_Vendor_Data
call String_Write_Bold
mov rsi,Msg_CRLF
call String_Write                 ; Carriage Return, Line Feed

;---------- Send last checkpoint code to Port 80h, for normal termination --;
CHK 07Ch

;---------- Exit, EFI application must restore registers -------------------;
Exit_1:
pop r15 r14 r13 r12 r11 r10 r9 r8
pop rbp rdi rsi rdx rcx rbx
xor eax,eax 	                    ; RAX = EFI_STATUS = 0
retn                              ; Simple form of termination

;---------- Exit if errors detected during check configuration -------------;
Bad_Config:                 ; Here RBP=Offset of error message central string
CHK 07Dh                    ; Chk. 07Dh = Termination by errors, begin message
mov rsi,Msg_Error_2         ; "Press any key"
call String_Write
call Input_Wait_Key
mov rsi,Msg_CRLF
call String_Write           ; Carriage Return, Line Feed

;---------- Restore state for error branch ---------------------------------;
mov rsi,Stop_Program_Sequence
ESeq_Cycle_1:
call	Start_Stop_Sequencer   ; Checkpoints 070h-075h (skip 76h)
jnc ESeq_Stop_1
push rsi
mov rsi,Msg_CRLF
call String_Write            ; Carriage Return, Line Feed
pop rsi
jmp ESeq_Cycle_1
ESeq_Stop_1:

;---------- Message with program vendor data and/or requirements -----------;
mov rsi,Msg_CRLF             ; Next string
call String_Write		
mov rsi,Program_Vendor_Data
call String_Write_Bold
mov rsi,Msg_CRLF
call String_Write            ; Carriage Return, Line Feed

;---------- Checkpoint 07Fh = Termination program by errors, exit to OS ----;
CHK 07Fh

;---------- Go to exit point -----------------------------------------------;
jmp Exit_1

;---------------------------------------------------------------------------;
;                CODE SECTION, EXTERNAL MODULES CONNECT.                    ;
;---------------------------------------------------------------------------;
INCLUDE 'ASM\SYS\sequence.inc'  ; Subroutine for control sequences execution

INCLUDE 'ASM\SYS\chk_cpuf.inc'  ; Config verify handler: CPU features
INCLUDE 'ASM\SYS\chk_clk.inc'   ; Config verify handler: Measure CPU Clock
INCLUDE 'ASM\SYS\chk_uefi.inc'  ; Config verify handler: UEFI firmware cfg.
INCLUDE 'ASM\SYS\chk_mem.inc'   ; Config verify handler: Free memory size
INCLUDE 'ASM\SYS\chk_acpi.inc'  ; Config verify handler: ACPI detect
INCLUDE 'ASM\SYS\ld_cfg.inc'    ; Loader handler: Load configuration file
INCLUDE 'ASM\SYS\chk_gvai.inc'  ; Config verify handler: Get Video adp. info
INCLUDE 'ASM\SYS\chk_gvmi.inc'  ; Config verify handler: Get Video mode info
INCLUDE 'ASM\SYS\chk_vflt.inc'  ; Config verify handler: Check VRAM flat mode
INCLUDE 'ASM\SYS\chk_pci.inc'   ; Config verify handler: Check PCI bus present
INCLUDE 'ASM\SYS\chk_avga.inc'  ; Config verify handler: Check active Video
INCLUDE 'ASM\SYS\chk_bar.inc'   ; Config verify handler: Check Video RAM BAR
INCLUDE 'ASM\SYS\chk_agp.inc'   ; Config verify handler: Check AGP present
INCLUDE 'ASM\SYS\chk_pcie.inc'  ; Config verify handler: Check PCIe present
INCLUDE 'ASM\SYS\chk_mtrr.inc'  ; Config verify handler: Check MTRR present

INCLUDE 'ASM\SYS\set_agp.inc'   ; Set context handler: AGP Fast Write
INCLUDE 'ASM\SYS\set_pcie.inc'  ; Set context handler: PCIe Packet Size
INCLUDE 'ASM\SYS\set_mtrr.inc'  ; Set context handler: MTRR for video memory
INCLUDE 'ASM\SYS\set_sse.inc'   ; Set context handler: SSE enable
INCLUDE 'ASM\SYS\set_mous.inc'  ; Set context handler: Detect mouse
INCLUDE 'ASM\SYS\set_game.inc'  ; Set context handler: Game parameters
INCLUDE 'ASM\SYS\set_wait.inc'  ; Wait user press any key for messages view
INCLUDE 'ASM\SYS\set_vmod.inc'  ; Set context handler: graphics video mode

INCLUDE 'ASM\SYS\res_vmod.inc'  ; Restore context handler: Text video mode
INCLUDE 'ASM\SYS\res_sse.inc'   ; Restore context handler: SSE enable
INCLUDE 'ASM\SYS\res_mtrr.inc'  ; Restore context handler: MTRR for v. memory
INCLUDE 'ASM\SYS\res_pcie.inc'  ; Restore context handler: PCIe Packet Size
INCLUDE 'ASM\SYS\res_agp.inc'   ; Restore context handler: AGP Fast Write
INCLUDE 'ASM\SYS\sv_cfg.inc'    ; Saver handler: Save configuration file
INCLUDE 'ASM\SYS\res_mem.inc'   ; Restore context handler: Memory requested

INCLUDE 'ASM\SYS\lib_acpi.inc'  ; Library for ACPI tables interpreting
INCLUDE 'ASM\SYS\lib_pci.inc'   ; Library for PCI regs. operations support
INCLUDE 'ASM\SYS\lib_agp.inc'   ; Library for AGP operations support
INCLUDE 'ASM\SYS\lib_pcie.inc'  ; Library for PCI Express operations support
INCLUDE 'ASM\SYS\lib_text.inc'  ; Library for built text strings support
INCLUDE 'ASM\SYS\lib_inp.inc'   ; Library for keyboard and mouse support
INCLUDE 'ASM\SYS\lib_out.inc'   ; Library for UEFI simple text output support
INCLUDE 'ASM\SYS\lib_mem.inc'   ; Library for UEFI memory info and allocate
INCLUDE 'ASM\SYS\lib_file.inc'  ; Library for UEFI file operations support
INCLUDE 'ASM\SYS\lib_beep.inc'  ; Library for sound generation by PC speaker
INCLUDE 'ASM\SYS\lib_tsc.inc'   ; Library for timings and delays by TSC
INCLUDE 'ASM\SYS\lib_rtc.inc'   ; Library for get date and time
INCLUDE 'ASM\SYS\lib_msg.inc'   ; Library for errors and complete messages
INCLUDE 'ASM\SYS\lib_stxt.inc'  ; Library for save report as TXT file
INCLUDE 'ASM\SYS\lib_sbmp.inc'  ; Library for save screen image as BMP file
INCLUDE 'ASM\SYS\lib_efi.inc'   ; Library for use EFI service routines

INCLUDE 'ASM\INFO\sysinfo.inc'  ; Get system information strings
INCLUDE 'ASM\INFO\inf_csm.inc'  ; Get info handler for: UEFI CSM presence
INCLUDE 'ASM\INFO\inf_cpu.inc'  ; Get info handler for: CPU name string
INCLUDE 'ASM\INFO\inf_cclk.inc' ; Get info handler for: CPU measured clock
INCLUDE 'ASM\INFO\inf_cfea.inc' ; Get info handler for: CPU advanced feat.
INCLUDE 'ASM\INFO\inf_vmod.inc' ; Get info handler for: Video mode parms.
INCLUDE 'ASM\INFO\inf_agp.inc'  ; Get info handler for: AGP settings
INCLUDE 'ASM\INFO\inf_pcie.inc' ; Get info handler for: PCI Express settings
INCLUDE 'ASM\INFO\inf_mem.inc'  ; Get info handler for: All/free memory size
INCLUDE 'ASM\INFO\inf_vmem.inc' ; Get info handler for: Video memory size
INCLUDE 'ASM\INFO\inf_mtrr.inc' ; Get info handler for: Video mem. MTRR set.
INCLUDE 'ASM\INFO\inf_benc.inc' ; Get info handler for: Video mem. benchmark
INCLUDE 'ASM\INFO\inf_show.inc' ; Show system info and benchmarks results

INCLUDE 'ASM\DRAW\scrcopy.inc'  ; Screen data copy from RAM to Video RAM
INCLUDE 'ASM\DRAW\fastupdt.inc' ; Fast update transit buf and Video RAM
INCLUDE 'ASM\DRAW\drawbenc.inc' ; Special draw box, used for benchmarks
INCLUDE 'ASM\DRAW\drawbox.inc'  ; Draw box, operation in the transit buffer
INCLUDE 'ASM\DRAW\drawchar.inc' ; Draw char, operation in the transit buffer
INCLUDE 'ASM\DRAW\drawsprt.inc' ; Draw and Get sprite, op. in the trans. buf.
INCLUDE 'ASM\DRAW\bmp2sprt.inc' ; Convert BMP format to sprite
INCLUDE 'ASM\DRAW\drawmous.inc' ; Draw mouse cursor, op. in the trans. buf.
INCLUDE 'ASM\DRAW\shiftbox.inc' ; Copy in the private buffer, for line fall

INCLUDE 'ASM\BLT\bltbenc.inc'   ; Special draw box, used for BLT benchmarks

; Note. Extreme Mode 2 is change status, base and size

INCLUDE 'ASM\EXM2\extreme.inc'  ; Sequencer for set extreme / restore context
INCLUDE 'ASM\EXM2\chk_base.inc' ; Check required/possible VRAM BAR relocation
INCLUDE 'ASM\EXM2\chk_size.inc' ; Check required/possible VRAM BAR resize
INCLUDE 'ASM\EXM2\set_emap.inc' ; Request MMIO region by UEFI firmware
INCLUDE 'ASM\EXM2\set_base.inc' ; Relocation VRAM BAR
INCLUDE 'ASM\EXM2\set_size.inc' ; Resize VRAM BAR
INCLUDE 'ASM\EXM2\skp_mtrr.inc' ; Restore after normal MTRR settings
INCLUDE 'ASM\EXM2\st2_mtrr.inc' ; Set extreme MTRR settings
INCLUDE 'ASM\EXM2\res_base.inc' ; Restore VRAM BAR base address after extreme
INCLUDE 'ASM\EXM2\res_size.inc' ; Restore VRAM BAR size after extreme
INCLUDE 'ASM\EXM2\res_emap.inc' ; Release MMIO region by UEFI firmware
INCLUDE 'ASM\EXM2\rs2_mtrr.inc' ; Restore MTRR settings after extreme

; Note. Extreme Mode 3 is change status without change base and size

INCLUDE 'ASM\EXM3\st3_pci.inc'  ; Extreme Mode 3 PCI bridge ranges change
INCLUDE 'ASM\EXM3\st3_mtrr.inc' ; Extreme Mode 3 MTRR ranges change
INCLUDE 'ASM\EXM3\rs3_pci.inc'  ; Extreme Mode 3 PCI bridge ranges restore
INCLUDE 'ASM\EXM3\rs3_mtrr.inc' ; Extreme Mode 3 MTRR ranges restore

INCLUDE 'ASM\DUMP\regdump.inc'  ; Write debug dump: CPU registers RAX-R15
INCLUDE 'ASM\DUMP\memdump.inc'  ; Write debug dump: Memory 256 bytes at RSI

;---------------------------------------------------------------------------;
;                 DATA SECTION, EXTERNAL MODULES CONNECT.                   ;
;---------------------------------------------------------------------------;
section '.data' data readable writeable

INCLUDE 'ASM\NAMES\names_e.inc' ; Copyright and names for UEFIMARK EXT. mode

INCLUDE 'ASM\SYS\dat_efi.inc'   ; Constants for UEFI support, GUIDs
INCLUDE 'ASM\SYS\dat_strn.inc'  ; Text strings for program start and exit
INCLUDE 'ASM\SYS\dat_msg.inc'   ; Text strings: errors and complete messages
INCLUDE 'ASM\SYS\dat_cpu.inc'   ; Constants for CPU identification
INCLUDE 'ASM\SYS\dat_seq.inc'   ; Tables for sequences execution
INCLUDE 'ASM\SYS\dat_cfg.inc'   ; Tables for interpreting config file
INCLUDE 'ASM\SYS\dat_rtc.inc'   ; Tables for built date and time strings
INCLUDE 'ASM\SYS\dat_stxt.inc'  ; Tables for save report to TXT file
INCLUDE 'ASM\SYS\dat_sbmp.inc'  ; Tables for save screen image to BMP file

INCLUDE 'ASM\INFO\dat_istr.inc' ; System information strings
INCLUDE 'ASM\INFO\dat_iseq.inc' ; Tables for get system information

INCLUDE 'ASM\DRAW\dat_draw.inc' ; Video control variables and tables

INCLUDE 'ASM\EXM2\dat_eseq.inc' ; Tables for set extreme context and restore
INCLUDE 'ASM\EXM3\dat_esq3.inc' ; Same for mode 3: chg. status w/o relocation

INCLUDE 'ASM\DUMP\dat_dump.inc' ; Data for write debug dump

INCLUDE 'ASM\SYS\dat_vars.inc'  ; Constants and variables, last for (?) data

;---------------------------------------------------------------------------;
;                       RELOCATION ELEMENTS SECTION.                        ;
;---------------------------------------------------------------------------;
section '.reloc' fixups data discardable
