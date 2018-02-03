;===========================================================================;
;                                                                           ;
;       DATA FOR WRITE REGISTERS AND MEMORY DUMP, FOR DEBUG PURPOSE.        ;
;                                                                           ;
;===========================================================================;

Debug_Dump_Names_1:
DB 'RAX' , 0
DB 'RBX' , 0
DB 'RCX' , 0
DB 'RDX' , 0
DB 'RSI' , 0
DB 'RDI' , 0
DB 'RBP' , 0
DB 'RSP' , 0
DB 'R8 ' , 0
DB 'R9 ' , 0
DB 'R10' , 0
DB 'R11' , 0
DB 'R12' , 0
DB 'R13' , 0
DB 'R14' , 0
DB 'R15' , 0

Debug_Dump_Names_2:
DB  0Dh,0Ah,0Ah,'Debug dump:',0Dh,0Ah,0
