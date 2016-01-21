@echo off
@rem exit /B 0

set basedir=C:\zhaojie\git

echo "DataStat begin..."
echo %basedir%\DataStat\src
cd %basedir%\DataStat\src
python main.py
echo "DataStat end."

exit /B %ERRORLEVEL%

exit /B 0