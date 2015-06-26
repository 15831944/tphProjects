@echo off
set path=%path%;C:\Python26
cd /d %~dp0

echo creating ejv dat files:
set destdir=C:\My\20150409_mea_pic\2DJ_2015Q1_MEA_OUTPUT_DAT
set srcdir=C:\My\20150409_mea_pic\2DJ_2015Q1_MEA_OUTPUT_resized\2DJ_2015Q1_ASIA\UNITED_ARAB_EMIRATES\DRIVER_VIEW\LANDSCAPE\ASPECT_RATIO_4x3
python CombinePics_Here.py  /srcdir %srcdir% /destdir %destdir% >> %~dp0mea8_ejv.txt
echo create ejv dat files end.

echo creating gjv dat files:
set destdir=C:\My\20150409_mea_pic\2DGJ_2015Q1_MEA_OUTPUT_DAT
set srcdir=C:\My\20150409_mea_pic\2DGJ_2015Q1_MEA_OUTPUT_resized\2DGJ_2015Q1_MEA\MEA\MDPS\ASPECT RATIO 4x3
python CombinePics_Here.py  /srcdir "%srcdir%" /destdir %destdir% >> %~dp0mea8_gjv.txt
echo create gjv dat files end.
pause
