@echo off
@rem exit /B 0

set basedir=C:\zhaojie\git

echo "autocheck begin..."
echo %basedir%\AutoCheck\src
cd %basedir%\AutoCheck\src
python main.py
echo "autocheck end."

cd /d %basedir%
git config --local user.email "zhaojie@pset.suntec.net"
git config --local user.name "zj281224"
rm  log_check.txt
git log -1 >>log_check.txt

exit /B %ERRORLEVEL%
exit /B 0