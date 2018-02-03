;===========================================================================;
;                                                                           ;
;                  EQUATIONS FOR UEFI ERRORS VISUALIZATION.                 ;
;                                                                           ;
;===========================================================================;

;--- Success status ---

EFI_SUCCESS = 0

;--- EFI error codes, status high bit=1 ---

NUMBER_OF_EFI_ERRORS      = 32
EFIERR                    = 8000000000000000h
EFI_LOAD_ERROR            = EFIERR or 1
EFI_INVALID_PARAMETER     = EFIERR or 2
EFI_UNSUPPORTED           = EFIERR or 3
EFI_BAD_BUFFER_SIZE       = EFIERR or 4
EFI_BUFFER_TOO_SMALL      = EFIERR or 5
EFI_NOT_READY             = EFIERR or 6
EFI_DEVICE_ERROR          = EFIERR or 7
EFI_WRITE_PROTECTED       = EFIERR or 8
EFI_OUT_OF_RESOURCES      = EFIERR or 9
EFI_VOLUME_CORRUPTED      = EFIERR or 10
EFI_VOLUME_FULL           = EFIERR or 11
EFI_NO_MEDIA              = EFIERR or 12
EFI_MEDIA_CHANGED         = EFIERR or 13
EFI_NOT_FOUND             = EFIERR or 14
EFI_ACCESS_DENIED         = EFIERR or 15
EFI_NO_RESPONSE           = EFIERR or 16
EFI_NO_MAPPING            = EFIERR or 17
EFI_TIMEOUT               = EFIERR or 18
EFI_NOT_STARTED           = EFIERR or 19
EFI_ALREADY_STARTED       = EFIERR or 20
EFI_ABORTED               = EFIERR or 21
EFI_ICMP_ERROR            = EFIERR or 22
EFI_TFTP_ERROR            = EFIERR or 23
EFI_PROTOCOL_ERROR        = EFIERR or 24
EFI_INCOMPATIBLE_VERSION  = EFIERR or 25
EFI_SECURITY_VIOLATION    = EFIERR or 26
EFI_CRC_ERROR             = EFIERR or 27
EFI_END_OF_MEDIA          = EFIERR or 28
EFI_END_OF_FILE           = EFIERR or 31
EFI_INVALID_LANGUAGE      = EFIERR or 32

;--- EFI warning codes, status high bit=0 ---

NUMBER_OF_EFI_WARNINGS    = 4
EFI_WARN_UNKNOWN_GLYPH    = 1
EFI_WARN_DELETE_FAILURE   = 2
EFI_WARN_WRITE_FAILURE    = 3
EFI_WARN_BUFFER_TOO_SMALL = 4

;--- Program operations names ---

NUMBER_OF_OPERATIONS      = 9
Op_Output_Message         = 1
Op_Initialize_Keyboard    = 2
Op_Initialize_Video       = 3
Op_Time_Stall             = 4
Op_Check_Keys_Press       = 5
Op_Locate_Handle          = 6
Op_Handle_Protocol        = 7
Op_Get_Video_Mode         = 8
Op_Set_Video_Mode         = 9

