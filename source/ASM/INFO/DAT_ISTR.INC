;===========================================================================;
;                                                                           ;
;             TEXT STRINGS FOR SYSTEM INFORMATION VISUALIZING.              ;
;                                                                           ;
;===========================================================================;

;---------- Common strings -------------------------------------------------;

String_Info_1 DB  '     Benchmarking video memory...',0
String_Info_2 DB  'Press any key, F9=TXT, F10=BMP...',0

;---------- UEFImark Extreme Edition support -------------------------------;

String_Not_Ext  DB  'Pass 1: firmware context',0
String_Ext      DB  'Pass 2: remapped context',0

;---------- UEFI configuration ---------------------------------------------;

String_CSM_On   DB  'CSM found',0
String_CSM_Off  DB  'UEFI starts without CSM',0

;---------- CPU clock ------------------------------------------------------;

String_CCLK     DB  'TSC: ',0
String_CCLK_1   DB  ' MHz',0

;---------- CPU features ---------------------------------------------------;

String_EM64T       DB  'iEM64T',0
String_AMD64       DB  'AMD64',0
String_MMX         DB  'MMX',0
String_SSE         DB  'SSE',0
String_SSE2        DB  'SSE2',0
String_SSE3        DB  'SSE3',0
String_SSSE3       DB  'SSSE3',0
String_SSE41       DB  'SSE4.1',0
String_SSE42       DB  'SSE4.2',0
String_SSE4A       DB  'SSE4A',0
String_SSE5        DB  'SSE5',0
String_3DNow       DB  '3DNow',0
String_3DNowP      DB  '3DNow+',0
String_AVX         DB  'AVX',0
; This from v1.07
String_AVX2        DB  'AVX2',0
String_AVX512F     DB  'AVX512F',0
String_AVX512DQ    DB  'AVX512DQ',0
String_AVX512IFMA  DB  'AVX512IFMA',0
String_AVX512PF    DB  'AVX512PF',0
String_AVX512ER    DB  'AVX512ER',0
String_AVX512CD    DB  'AVX512CD',0
String_AVX512BW    DB  'AVX512BW',0
String_AVX512VL    DB  'AVX512VL',0
String_AVX512VBM   DB  'AVX512VBM',0


;  X1 = Strung type: 0=Unconditional, 1=Conditional
;  X2 = CPUID Selector: 0=Standard, 1=Extended
;  X3 = Register selector after CPUID: 0=EDX, 1=ECX
;  X4 = Bit number for register EDX or ECX
;  X5 = Pointer to text string
;  Value 0FFh means stop
;
;  MACRO  CFEAT  X1, X2, X3, X4, X5
;  {
;  DB  X1 SHL 7 + X2 SHL 6 + X3 SHL 5 + X4
;  DQ  X5
;  }
;
;  MACRO  CFEATSTOP
;  {
;  DB  0FFh
;  }

; v1.07 modification, 16-bit tags for new CPUID functions support
; [n] means bitfield size, not-listed values reserved
; X1[4] = Strung type: 0=Unconditional, 1=Conditional
; X2[4] = CPUID Selector: 0=Standard (0), 1=Extended (80000000h), 2=Fnc. 7
; X3[3] = Register selector after CPUID: 0=EAX (n/a), 1=EBX, 2=ECX, 3=EDX 
; X4[5] = Bit number for register EAX/EBX/ECX/EDX
; X5[64] = Pointer to text string
; Tag value 0FFFFh means stop

MACRO  CPU_FEATURE  X1, X2, X3, X4, X5
{
DW  X1 SHL 12 + X2 SHL 8 + X3 SHL 5 + X4
DQ  X5
}

MACRO  CPU_FEATURE_STOP
{
DW  0FFFFh
}

Selector_CPU_Features:
DQ  Intel_CPU_Features
DQ  AMD_CPU_Features

Intel_CPU_Features:
; First string, removed
; Intel EM64T = CPUID#80000001h, EDX.29
CPU_FEATURE  1 , 1 , 3 , 29 , String_EM64T
; MMX = CPUID#1, EDX.23
CPU_FEATURE  1 , 0 , 3 , 23 , String_MMX
; SSE = CPUID#1, EDX.25
CPU_FEATURE  1 , 0 , 3 , 25 , String_SSE
; SSE2 = CPUID#1, EDX.26
CPU_FEATURE  1 , 0 , 3 , 26 , String_SSE2
; SSE3 = CPUID#1, ECX.0
CPU_FEATURE  1 , 0 , 2 , 00 , String_SSE3
; SSSE3 = CPUID#1, ECX.9
CPU_FEATURE  1 , 0 , 2 , 09 , String_SSSE3
; SSE4.1 = CPUID#1, ECX.19
CPU_FEATURE  1 , 0 , 2 , 19 , String_SSE41
; SSE4.2 = CPUID#1, ECX.20
CPU_FEATURE  1 , 0 , 2 , 20 , String_SSE42
; AVX = CPUID#1, ECX.28
CPU_FEATURE  1 , 0 , 2 , 28 , String_AVX
; AVX2        = CPUID#7, EBX.5                                    ; From v1.07
CPU_FEATURE  1 , 2 , 1 , 05 , String_AVX2  
; Stop, end of list
CPU_FEATURE_STOP


AMD_CPU_Features:
; First string, removed
; AMD64 = CPUID#80000001h, EDX.29
CPU_FEATURE  1 , 1 , 3 , 29 , String_AMD64
; MMX = CPUID#1, EDX.23
CPU_FEATURE  1 , 0 , 3 , 23 , String_MMX
; SSE = CPUID#1, EDX.25
CPU_FEATURE  1 , 0 , 3 , 25 , String_SSE
; SSE2 = CPUID#1, EDX.26
CPU_FEATURE  1 , 0 , 3 , 26 , String_SSE2
; SSE3 = CPUID#1, ECX.0
CPU_FEATURE  1 , 0 , 2 , 00 , String_SSE3
; SSSE3 = CPUID#1, ECX.9
CPU_FEATURE  1 , 0 , 2 , 09 , String_SSSE3
; SSE4.1 = CPUID#1, ECX.19
CPU_FEATURE  1 , 0 , 2 , 19 , String_SSE41
; SSE4.2 = CPUID#1, ECX.20
CPU_FEATURE  1 , 0 , 2 , 20 , String_SSE42
; SSE4A = CPUID#80000001h, ECX.6
CPU_FEATURE  1 , 1 , 2 , 06 , String_SSE4A
; SSE5 = CPUID#80000001h, ECX.11
CPU_FEATURE  1 , 1 , 2 , 11 , String_SSE5
; 3DNow = CPUID#80000001h, EDX.31
CPU_FEATURE  1 , 1 , 3 , 31 , String_3DNow
; 3DNow+ = CPUID#80000001h, EDX.30
CPU_FEATURE  1 , 1 , 3 , 30 , String_3DNowP
; AVX = CPUID#1, ECX.28
CPU_FEATURE  1 , 0 , 2 , 28 , String_AVX
; AVX2        = CPUID#7, EBX.5                                    ; From v1.07
CPU_FEATURE  1 , 2 , 1 , 05 , String_AVX2  
; Stop, end of list
CPU_FEATURE_STOP


AVX512_CPU_Features_1:                                            ; From v1.07
; AVX512F     = CPUID#7, EBX.16    ; Foundation  
CPU_FEATURE  1 , 2 , 1 , 16 , String_AVX512F
; AVX512IFMA  = CPUID#7, EBX.21    ; Integer Fused Multiply and Add 
CPU_FEATURE  1 , 2 , 1 , 21 , String_AVX512IFMA 
; AVX512PF    = CPUID#7, EBX.26    ; Prefetch          
CPU_FEATURE  1 , 2 , 1 , 26 , String_AVX512PF
; AVX512ER    = CPUID#7, EBX.27    ; Exponential and Reciprocal  
CPU_FEATURE  1 , 2 , 1 , 27 , String_AVX512ER
; AVX512CD    = CPUID#7, EBX.28    ; Conflict Detection 
CPU_FEATURE  1 , 2 , 1 , 28 , String_AVX512CD
; Stop, end of list
CPU_FEATURE_STOP


AVX512_CPU_Features_2:
; AVX512DQ    = CPUID#7, EBX.17    ; Doublewords and Quadwords operations     
CPU_FEATURE  1 , 2 , 1 , 17 , String_AVX512DQ
; AVX512BW    = CPUID#7, EBX.30    ; Bytes and Words Operations 
CPU_FEATURE  1 , 2 , 1 , 30 , String_AVX512BW
; AVX512VL    = CPUID#7, EBX.31    ; Vector Length Control 
CPU_FEATURE  1 , 2 , 1 , 31 , String_AVX512VL
; AVX512VBM   = CPUID#7, ECX.1     ; Vector Byte Manipulation   
CPU_FEATURE  1 , 2 , 2 , 01 , String_AVX512VBM
; Stop, end of list
CPU_FEATURE_STOP


;  This is old format entries used before v1.07
;  Intel_CPU_Features:
;  ; First string, removed
;  ; Intel EM64T = CPUID#80000001h, EDX.29
;  CFEAT  1 , 1 , 0 , 29 , String_EM64T
;  ; MMX = CPUID#1, EDX.23
;  CFEAT  1 , 0 , 0 , 23 , String_MMX
;  ; SSE = CPUID#1, EDX.25
;  CFEAT  1 , 0 , 0 , 25 , String_SSE
;  ; SSE2 = CPUID#1, EDX.26
;  CFEAT  1 , 0 , 0 , 26 , String_SSE2
;  ; SSE3 = CPUID#1, ECX.0
;  CFEAT  1 , 0 , 1 , 00 , String_SSE3
;  ; SSSE3 = CPUID#1, ECX.9
;  CFEAT  1 , 0 , 1 , 09 , String_SSSE3
;  ; SSE4.1 = CPUID#1, ECX.19
;  CFEAT  1 , 0 , 1 , 19 , String_SSE41
;  ; SSE4.2 = CPUID#1, ECX.20
;  CFEAT  1 , 0 , 1 , 20 , String_SSE42
;  ; AVX = CPUID#1, ECX.28
;  CFEAT  1 , 0 , 1 , 28 , String_AVX
;  ; Stop
;  CFEATSTOP

;  AMD_CPU_Features:
;  ; First string, removed
;  ; AMD64 = CPUID#80000001h, EDX.29
;  CFEAT  1 , 1 , 0 , 29 , String_AMD64
;  ; MMX = CPUID#1, EDX.23
;  CFEAT  1 , 0 , 0 , 23 , String_MMX
;  ; SSE = CPUID#1, EDX.25
;  CFEAT  1 , 0 , 0 , 25 , String_SSE
;  ; SSE2 = CPUID#1, EDX.26
;  CFEAT  1 , 0 , 0 , 26 , String_SSE2
;  ; SSE3 = CPUID#1, ECX.0
;  CFEAT  1 , 0 , 1 , 00 , String_SSE3
;  ; SSSE3 = CPUID#1, ECX.9
;  CFEAT  1 , 0 , 1 , 09 , String_SSSE3
;  ; SSE4.1 = CPUID#1, ECX.19
;  CFEAT  1 , 0 , 1 , 19 , String_SSE41
;  ; SSE4.2 = CPUID#1, ECX.20
;  CFEAT  1 , 0 , 1 , 20 , String_SSE42
;  ; SSE4A = CPUID#80000001h, ECX.6
;  CFEAT  1 , 1 , 1 , 06 , String_SSE4A
;  ; SSE5 = CPUID#80000001h, ECX.11
;  CFEAT  1 , 1 , 1 , 11 , String_SSE5
;  ; 3DNow = CPUID#80000001h, EDX.31
;  CFEAT  1 , 1 , 0 , 31 , String_3DNow
;  ; 3DNow+ = CPUID#80000001h, EDX.30
;  CFEAT  1 , 1 , 0 , 30 , String_3DNowP
;  ; AVX = CPUID#1, ECX.28
;  CFEAT  1 , 0 , 1 , 28 , String_AVX
;  ; Stop
;  CFEATSTOP


;---------- UEFI video mode parameters -------------------------------------;

String_Vmode    DB  'GOP video mode ',0
String_Vmode_1  DB  ', X=',0
String_Vmode_2  DB  ', Y=',0
String_Vmode_3  DB  ', BPP=',0

;---------- AGP parameters -------------------------------------------------;

String_AGP_B    DB  'AGP  ',0            ; Bridge
String_AGP_A    DB  'Card ',0            ; Adapter
String_FastW    DB  'fast write at ',0
String_NotOpt   DB  'not optimized',0
String_AGP_2    DB  ', capable: ',0
String_FW       DB  'FW, ',0


;---------- PCI Express parameters -----------------------------------------;

String_PCIe_B   DB  'PCIe x',0           ; Bridge
String_PCIe_A   DB  'Card x',0           ; Adapter
String_PCIe_1   DB  ', ' ,0
String_PCIe_2   DB  'GT/s, TLP=',0
String_PCIe_3   DB  ', max: x',0

String_25GTS    DB  '2.5',0
String_50GTS    DB  '5.0',0
String_80GTS    DB  '8.0',0
String_xGTS     DB  '?',0

;---------- System memory size ---------------------------------------------;

String_Mem      DB  'UEFI memory: ',0
String_Mem_1    DB  ' KB, ACPI reserved: ',0
String_Mem_2    DB  ' KB',0
String_Mem_3    DB  ', physical address ',0
String_Mem_4    DB  ' bits',0

;---------- Video memory size and Protocol version -------------------------;

String_Vmem     DB  'Video memory used: ',0
String_Vmem_1   DB  ' KB',0
String_Vmem_2   DB  ', physical: ',0
String_Vmem_3   DB  ' KB or above',0
String_GOP      DB  ', Version ',0

;---------- Video memory MTRR settings -------------------------------------;

; This 4 strings must be sequental
String_MTRR     DB  'Video memory MTRR: ',0
String_MTRR_1   DB  'h-',0
String_MTRR_2   DB  'h, ',0
String_MTRR_3:
DB  'UC' , 0    ; 00=Uncacheable
DB  'WC' , 0    ; 01=Write Combining
DB  '?'  , 0    ; 02=Reserved
DB  '?'  , 0    ; 03=Reserved
DB  'WT' , 0    ; 04=Write Through
DB  'WP' , 0    ; 05=Write Protected
DB  'WB' , 0    ; 06=Write Back

String_MTRR_N   DB  'No vacant MTRR found',0

;---------- Video memory benchmarks ----------------------------------------;

String_Benc     DB  'Video memory write (MB/S): ',0
String_Benc_1   DB  ' (SSE), ',0
String_Benc_2   DB  ' (386), ',0
String_Benc_3   DB  ' (BLT)',0
String_Benc_U   DB  '? (benchmarks interrupted by user)',0
