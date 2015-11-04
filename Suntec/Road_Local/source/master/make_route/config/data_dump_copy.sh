#! bin/sh

src_name=C_NIMIF_15Q1_ORG

cd ~
echo 'src_host=192.168.10.19' > $src_name.sh
echo 'src_name=C_NIMIF_15Q1_ORG' >> $src_name.sh
echo './../../usr/local/pgsql/bin/pg_dump --ignore-version --host=$src_host --port=5432 -Upostgres -Fc --file=./database_backup/$src_name.backup --exclude-table=pg_* $src_name' >> $src_name.sh
sh $src_name.sh
cp ./database_backup/$src_name.backup /mnt/DB_Working
