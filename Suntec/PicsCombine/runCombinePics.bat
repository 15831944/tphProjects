@echo off
path = %path%;C:\Python26
cd /d %~dp0
python CombinePics_Sensis.py /srcdir C:\My\jepg /destdir C:\My\jepg2 >> %~dp0\sensis.txt
python CombinePics_Here.py CombinePics_Here.py /host 172.26.179.201 /dbname BRA_RDF_2014Q4_ORG /user postgres /password pset123456 /gjvsrcdir C:\17CY_picture_operation\2D_picture_14Q4\compression_picture\2DGJ_14Q4_SA_OUTPUT\2DGJ_14Q4_SA\SA /gjvdestdir C:\17CY_picture_operation\2D_picture_14Q4\PIC_OUTPUT_DAT\2DGJ /ejvsrcdir C:\17CY_picture_operation\2D_picture_14Q4\2DJ_14Q4_SA_TOTAL_WITH_SAR_OUPUT\ARGENTINA /ejvdestdir C:\17CY_picture_operation\2D_picture_14Q4\PIC_OUTPUT_DAT\2DJ\ARGENTINA >>%~dps0\Here.txt
pause