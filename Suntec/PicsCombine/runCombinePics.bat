@echo off
path = %path%;C:\Python26
cd /d %~dp0
set srcdir=C:\My\jepg
set destdir=C:\My\jepg2
python CombinePics_Sensis.py /srcdir %srcdir% /destdir %destdir% >> %~dp0\sensis.txt

set host=172.26.179.201
set dbname=BRA_RDF_2014Q4_ORG 
set user=postgres
set password=pset123456
set gjvsrcdir=C:\17CY_picture_operation\2D_picture_14Q4\compression_picture\2DGJ_14Q4_SA_OUTPUT\2DGJ_14Q4_SA\SA
set gjvdestdir=C:\17CY_picture_operation\2D_picture_14Q4\PIC_OUTPUT_DAT\2DGJ
set ejvsrcdir=C:\17CY_picture_operation\2D_picture_14Q4\2DJ_14Q4_SA_TOTAL_WITH_SAR_OUPUT\ARGENTINA
set ejvdestdir=C:\17CY_picture_operation\2D_picture_14Q4\PIC_OUTPUT_DAT\2DJ\ARGENTINA
set logfile=%~dps0\Here.txt
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /gjvsrcdir %gjvsrcdir%  /gjvdestdir %gjvdestdir% /ejvsrcdir %ejvsrcdir% /ejvdestdir %ejvdestdir% >>%logfile%
pause