@echo off
@rem exit /B 0

set basedir=C:\zhaojie\git

echo "AutoArrange begin..."
echo %basedir%\AutoArrange\src
cd %basedir%\AutoArrange\src
python main.py
echo "AutoArrange end."

exit /B %ERRORLEVEL%

exit /B 0