@echo off

set src_host=172.26.185.137
set src_dbname=J_2013Spring_Incremental_Updating
set dst_host=172.26.185.137
set dst_dbname=J_2013Spring_0054_0105
set backupfile=C:\database_backup\J_2013Spring_0054_0102.backup

set tableoption=--table=basemap_* --table=citymap_* --table=middlemap_* --table=topmap_* --table=road_* --table=vics_link_tbl --table=vics_ipc_link_tbl

@rem pg_dump --ignore-version --host=%src_host% --port=5432 -Upostgres -Fc --file=%backupfile% %tableoption% %src_dbname%
pg_restore --ignore-version --host=%dst_host% --port=5432 -Upostgres -Fc --dbname=%dst_dbname% %backupfile%


set basedir=D:\workspace\WebGis\proj\Source_Branch\incremental_updating

echo "org2middle begin..."
echo %basedir%\Org2Middle\src
cd /d %basedir%\Org2Middle\src
python main.py
echo "org2middle end."

echo "middle2rdb begin..."
echo %basedir%\Middle2RDB\src
cd /d %basedir%\Middle2RDB\src
python main.py
echo "middle2rdb end."

echo "autocheck begin..."
echo %basedir%\AutoCheck\src
cd /d %basedir%\AutoCheck\src
python main.py
echo "autocheck end."

pause
