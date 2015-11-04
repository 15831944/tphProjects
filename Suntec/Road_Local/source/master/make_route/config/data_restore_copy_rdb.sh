#! bin/sh

dest_name=CHN_NAVINFO_2015Q1_0082_0005_102

cd ~
cp /mnt/DB_Working/$dest_name.backup ./database_backup/
rm /mnt/DB_Working/$dest_name.backup

echo 'dest_name=CHN_NAVINFO_2015Q1_0082_0005_102' > $dest_name.sh
echo 'dest_rdb_host=192.168.10.17' >> $dest_name.sh
echo './../../usr/local/pgsql/bin/dropdb --host=$dest_host_rdb --port=5432 -Upostgres $dest_name' >> $dest_name.sh
echo './../../usr/local/pgsql/bin/createdb --host=$dest_host_rdb --port=5432 -Upostgres -Opostgres -EUTF8 -Tsample_postgis $dest_name' >> $dest_name.sh
echo './../../usr/local/pgsql/bin/pg_restore -c --ignore-version --host=$dest_rdb_host --port=5432 -Upostgres -Fc --dbname=$dest_name --jobs 2 ./database_backup/$dest_name.backup' >> $dest_name.sh
sh $dest_name.sh


