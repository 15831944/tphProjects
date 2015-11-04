@echo off
@rem exit /B 0

set basedir=C:\zhaojie\git

echo "DataStat begin..."
echo %basedir%\DataStat\src
cd %basedir%\DataStat\src
python main.py
echo "DataStat end."

if NOT %ERRORLEVEL%==0 (
echo "dump pass..."
) else (
echo "dump begin..."
echo %basedir%\dump\src
cd %basedir%\dump\src
python main.py
echo "dump end."
)

exit /B %ERRORLEVEL%

exit /B 0