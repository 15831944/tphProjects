@echo off

set src_host=172.26.185.137
set src_dbname=J_2012A_Incremental_Updating
set dst_host=172.26.185.137
set dst_dbname=J_2013Spring_Incremental_Updating
set backupfile=D:\workspace\WebGis\proj\Source_Branch\incremental_updating\Middle2RDB\src\cache\id_fund.backup

set tableoption=--table=id_fund_*

pg_dump --ignore-version --host=%src_host% --port=5432 -Upostgres -Fc --file=%backupfile% %tableoption% %src_dbname%
pg_restore --ignore-version --host=%dst_host% --port=5432 -Upostgres -Fc --dbname=%dst_dbname% %backupfile%
