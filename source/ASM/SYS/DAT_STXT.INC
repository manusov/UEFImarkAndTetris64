;===========================================================================;
;                                                                           ;
;              TABLES AND DATA FOR SAVE REPORT AS TEXT FILE.                ;
;                                                                           ;
;===========================================================================;

;---------- Strings for text report ----------------------------------------;

Report_Title     DB  'System information.',0Dh,0Ah,0
Report_Middle_2  DB  0Dh,0Ah
Report_Middle    DB  0Dh,0Ah,0

;---------- File name pattern ----------------------------------------------;

Name_TXT:
DB  'text____.txt',0
