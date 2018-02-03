;===========================================================================;
;                                                                           ;
;                  TABLES FOR BUILT DATE AND TIME STRINGS.                  ;
;                                                                           ;
;===========================================================================;

;--- Months names ---
String_Months:
DB  'January'   ,0
DB  'February'  ,0
DB  'March'     ,0
DB  'April'     ,0
DB  'May'       ,0
DB  'June'      ,0
DB  'July'      ,0
DB  'August'    ,0
DB  'September' ,0
DB  'October'   ,0
DB  'November'  ,0
DB  'December'  ,0

;--- Week days names ---
String_Days:
DB  'Sunday'    ,0
DB  'Monday'    ,0
DB  'Tuesday'   ,0
DB  'Wednesday' ,0
DB  'Thursday'  ,0
DB  'Friday'    ,0
DB  'Saturday'  ,0

;--- Table used for Day of Week calculation ---
January_Days:	
DB  31	; January
February_Days:
DB  28	; February (updated dynamically 28/29)
DB  31	; March
DB  30	; April
DB  31	; May
DB  30	; June
DB  31	; July
DB  31	; August
DB  30	; September
DB  31	; October
DB  30	; November
DB  31	; December
