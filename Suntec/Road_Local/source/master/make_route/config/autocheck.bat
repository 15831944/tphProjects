@echo off
@rem exit /B 0

set basedir=C:\zhaojie\git

echo "autocheck begin..."
echo %basedir%\AutoCheck\src
cd %basedir%\AutoCheck\src
python main.py
echo "autocheck end."
exit /B %ERRORLEVEL%
exit /B 0