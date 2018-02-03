;===========================================================================;
;                                                                           ;
;        TABLES AND DATA FOR SAVE SCREEN IMAGE AS BMP GRAPHICS FILE.        ;
;                                                                           ;
;===========================================================================;

;---------------------------------------------------------------------------
; BMP file layout for 16M colors, 24 bits per pixel
;---------------------------------------------------------------------------
; Offset  Length        Value        Comment
;---------------------------------------------------------------------------
; 0000h   DB            'BM'         BMP file label
; 0002h   DD            ?            File size, bytes
; 0006h   DW            0,0          Reserved
; 000Ah   DD            00000036h    Offset of bitmap
;--- header start ---
; 000Eh   DD            00000028h    Length of header, bytes
; 0012h   DD            ?            Picture width, pixels
; 0016h   DD            ?            Picture height, pixels
; 001Ah   DW            1            Color planes
; 001Ch   DW            18h          Bits per pixel=24
; 001Eh   DD            0            Compression type, 0=without
; 0022h   DD            ?            Image size, bytes, exclude prefix+header
; 0026h   DD            2834         Horisontal resolution, virtual
; 002Ah   DD            2834         Vertical resolution, virtual
; 002Eh   DD            0            Total colors, not used here
; 0032h   DD            0            Used colors, not used here
;--- header stop, image start (palette not used for true color) ---
; 0036h   DB X*Y*3 DUP  ?            Pixels logical colors, from left down
;                                    corner, 3 bytes per pixel [ B, G, R ]
;---------------------------------------------------------------------------

; Total 14+40=54 bytes
Prefix_and_Header_Size = 54

Save_BMP_Header:
;--- Prefix ---
BMP_Signature         DB  'BM'       ; +0
BMP_File_Size         DD  0          ; +2, write required, f(video mode)
BMP_Reserved_1        DW  0,0        ; +6
BMP_Bitmap_Offset     DD  00000036h  ; +10
;--- Header ---
BMP_Header_Length     DD  00000028h	 ; +14
BMP_Pictire_Width     DD  0          ; +18, write required, f(video mode)
BMP_Pictire_Height    DD  0          ; +22, write required, f(video mode)
BMP_Color_Planes      DW  1          ; +26
BMP_Bits_per_Pixel    DW  18h        ; +28, write req. if sup. other modes
BMP_Compression_Type  DD  0          ; +30
BMP_Image_Size        DD  0          ; +34, write required, f(video mode)
BMP_H_Resolution      DD  2834       ; +38
BMP_V_Resolution      DD  2834       ; +42
BMP_Total_Colors      DD  0          ; +46
BMP_Used_Colors       DD  0          ; +50
;--- End of header ---

;---------- File name pattern ----------------------------------------------;

Name_BMP:
DB  'pict____.bmp' , 0
