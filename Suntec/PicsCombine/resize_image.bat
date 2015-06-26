@echo off
set path=%path%;C:\Python26
cd /d %~dp0

echo resizing ejv images to lower:
set srcdir=C:\My\20150417_asean_pic\2DJ_2015Q1_ASEAN_OUTPUT
set destdir=C:\My\20150417_asean_pic\2DJ_2015Q1_ASEAN_OUTPUT_resized
python CombinePics_Convert.py /srcdir %srcdir% /destdir %destdir%>>convert_ejv.txt
echo resize ejv images end.

echo resizing gjv images to lower:
set srcdir=C:\My\20150417_asean_pic\2DGJ_2015Q1_ASEAN_OUTPUT
set destdir=C:\My\20150417_asean_pic\2DGJ_2015Q1_ASEAN_OUTPUT_resized
python CombinePics_Convert.py /srcdir %srcdir% /destdir %destdir% >>convert_gjv.txt
echo resize ejv images end.