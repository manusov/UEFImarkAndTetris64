;===========================================================================;
;                                                                           ;
;  MESSAGES STRINGS FOR ERRORS, WARNINGS AND OPERATIONS COMPLETE MESSAGES.  ;
;                     THIS MESSAGES IS UEFI API RELEVANT.                   ;
;                                                                           ;
;===========================================================================;

;--- EFI error codes, status high bit RAX.63=1 ---

EFI_Errors_Names:
DB  '?'                         , 0    ; 0=Invalid error code
DB  'EFI_LOAD_ERROR'            , 0    ; 1 
DB  'EFI_INVALID_PARAMETER'     , 0    ; 2 
DB  'EFI_UNSUPPORTED'           , 0    ; 3 
DB  'EFI_BAD_BUFFER_SIZE'       , 0    ; 4 
DB  'EFI_BUFFER_TOO_SMALL'      , 0    ; 5 
DB  'EFI_NOT_READY'             , 0    ; 6 
DB  'EFI_DEVICE_ERROR'          , 0    ; 7 
DB  'EFI_WRITE_PROTECTED'       , 0    ; 8 
DB  'EFI_OUT_OF_RESOURCES'      , 0    ; 9 
DB  'EFI_VOLUME_CORRUPTED'      , 0    ; 10
DB  'EFI_VOLUME_FULL'           , 0    ; 11
DB  'EFI_NO_MEDIA'              , 0    ; 12
DB  'EFI_MEDIA_CHANGED'         , 0    ; 13
DB  'EFI_NOT_FOUND'             , 0    ; 14
DB  'EFI_ACCESS_DENIED'         , 0    ; 15
DB  'EFI_NO_RESPONSE'           , 0    ; 16
DB  'EFI_NO_MAPPING'            , 0    ; 17
DB  'EFI_TIMEOUT'               , 0    ; 18
DB  'EFI_NOT_STARTED'           , 0    ; 19
DB  'EFI_ALREADY_STARTED'       , 0    ; 20
DB  'EFI_ABORTED'               , 0    ; 21
DB  'EFI_ICMP_ERROR'            , 0    ; 22
DB  'EFI_TFTP_ERROR'            , 0    ; 23
DB  'EFI_PROTOCOL_ERROR'        , 0    ; 24
DB  'EFI_INCOMPATIBLE_VERSION'  , 0    ; 25
DB  'EFI_SECURITY_VIOLATION'    , 0    ; 26
DB  'EFI_CRC_ERROR'             , 0    ; 27
DB  'EFI_END_OF_MEDIA'          , 0    ; 28
DB  '?'                         , 0    ; 29
DB  '?'                         , 0    ; 30
DB  'EFI_END_OF_FILE'           , 0    ; 31
DB  'EFI_INVALID_LANGUAGE'      , 0    ; 32

;--- EFI warning codes, status high bit RAX.63=0 ---

EFI_Warnings_Names:
DB  '?'                         , 0    ; 0=Invalid warning code
DB  'EFI_WARN_UNKNOWN_GLYPH'    , 0    ; 1
DB  'EFI_WARN_DELETE_FAILURE'   , 0    ; 2
DB  'EFI_WARN_WRITE_FAILURE'    , 0    ; 3
DB  'EFI_WARN_BUFFER_TOO_SMALL' , 0    ; 4

;--- Program operations names ---

Operations_Names:
DB  '?'                         , 0    ; 0=Invalid operation code
DB  'Output message'            , 0    ; 1
DB  'Initialize keyboard'       , 0    ; 2
DB  'Initialize video'          , 0    ; 3
DB  'Time stall'                , 0    ; 4
DB  'Check keys press'          , 0    ; 5
DB  'Locate handle'             , 0    ; 6
DB  'Handle protocol'           , 0    ; 7
DB  'Get video mode'            , 0    ; 8
DB  'Set video mode'            , 0    ; 9
DB  'Locate protocol'           , 0    ; 10
DB  'Open or create file'       , 0    ; 11
DB  'Close file'                , 0    ; 12
DB  'Read file'                 , 0    ; 13
DB  'Write file'                , 0    ; 14

;--- Save text report and screen image status ---

Save_File_Status:
DB  '?'                             , 0    ; 0=No file operations exec.
DB  'Text report saved OK, '        , 0    ; 1
DB  'Graphics screen saved OK, '    , 0    ; 2
DB  'Error saving text report'      , 0    ; 3
DB  'Error saving graphics screen'  , 0    ; 4

;--- Additional strings ---

Str_Operation  DB  'Operation = ' , 0
Str_Status     DB  'Status    = ' , 0

