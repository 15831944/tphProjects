@echo off
path = %path%;C:\Python26
cd /d %~dp0
set srcdir=C:\My\jepg
set destdir=C:\My\jepg2
@echo python CombinePics_Sensis.py /srcdir %srcdir% /destdir %destdir% >> %~dp0\sensis.txt

set host=172.26.179.201
set dbname=BRA_RDF_2014Q4_ORG 
set user=postgres
set password=pset123456
set gjvsrcdir=C:\my\17CY_picture_operation_2\2D_picture_14Q4\compression_picture\2DGJ_14Q4_SA_OUTPUT\2DGJ_14Q4_SA\SA
set gjvdestdir=C:\my\17CY_picture_operation_2\2D_picture_14Q4\PIC_OUTPUT_DAT\2DGJ
set ejvsrcdir=C:\my\17CY_picture_operation_2\2D_picture_14Q4\2DJ_14Q4_SA_TOTAL_WITH_SAR_OUPUT\ARGENTINA
set ejvdestdir=C:\my\17CY_picture_operation_2\2D_picture_14Q4\PIC_OUTPUT_DAT\2DJ\ARGENTINA
set logfile=%~dps0\Here.txt
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\BAHRAIN\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\BAHRAIN>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\EGYPT\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\EGYPT>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\ISRAEL\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\ISRAEL>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\KUWAIT\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\KUWAIT>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\MOROCCO\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\MOROCCO>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\OMAN\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\OMAN>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\QATAR\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\QATAR>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\SAUDI_ARABIA\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\SAUDI_ARABIA>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\SOUTH_AFRICA\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\SOUTH_AFRICA>>%logfile%
python CombinePics_Here.py CombinePics_Here.py /host %host% /dbname %dbname% /user %user% /password %password% /ejvsrcdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT\2DJ_2015Q1_ASIA\UNITED_ARAB_EMIRATES\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_5x3 /ejvdestdir C:\My\work\20150409\2DJ_2015Q1_ASIA_OUTPUT_DAT\UNITED_ARAB_EMIRATES>>%logfile%
pause