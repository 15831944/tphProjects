@echo off
@rem exit /B 0

set putty_path=C:\zhaojie\git\F18\Road\source\master\make_route\config
set dest_host=192.168.10.16
set dest_pw=pset123456
set dest_rdb_host=192.168.10.17
set dest_rdb_pw=pset123456

@echo on
cd %putty_path%
putty.exe -ssh -pw %dest_pw% -m data_dump_copy_rdb.sh pset@%dest_host%
echo %ERRORLEVEL%

putty.exe -ssh -pw %dest_rdb_pw% -m data_restore_copy_rdb.sh pset@%dest_rdb_host%
echo %ERRORLEVEL%

exit /B 0