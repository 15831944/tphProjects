@echo off
@rem exit /B 0

set basedir=C:\zhaojie\git

echo "org2middle begin..."
echo %basedir%\Org2Middle\src
cd %basedir%\Org2Middle\src
python main.py
echo "org2middle end."

if NOT %ERRORLEVEL%==0 (
echo "middle2rdb pass..."
) else (
echo "middle2rdb begin..."
echo %basedir%\Middle2RDB\src
cd %basedir%\Middle2RDB\src
python main.py
echo "middle2rdb end."
)
cd /d %basedir%
git config --local user.email "zhaojie@pset.suntec.net"
git config --local user.name "zj281224"
rm  log.txt
git log -1 >>log.txt

exit /B %ERRORLEVEL%

exit /B 0