;===========================================================================;
;                                                                           ;
;               MESSAGES STRINGS FOR PROGRAM START AND EXIT.                ;
;                                                                           ;
;===========================================================================;

Msg_OK:
DB  0Dh,0Ah
DB  'GOP video mode = '
EntryMode:
DB '          '   
DB  0Dh,0Ah
DB  'Xsize = '
EntryXsize:
DB '          '
DB  0Dh,0Ah
DB  'Ysize = '
EntryYsize:
DB '          '
DB  0Dh,0Ah
DB  'Press any key...'
DB  0

Msg_BAD:
DB  0Dh,0Ah
DB  'GOP access failed'
DB  0Dh,0Ah
DB  'Press any key...'
DB  0

Msg_CRLF:
DB  0Dh,0Ah
DB  0
   