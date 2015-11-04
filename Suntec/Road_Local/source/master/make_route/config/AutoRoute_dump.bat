@echo off
@rem exit /B 0

set basedir=C:\zhaojie\git

echo "Route_dump begin..."
echo %basedir%\Route_dump\src
cd %basedir%\Route_dump\src
python main.py
echo "Route_dump end."

exit /B %ERRORLEVEL%

exit /B 0