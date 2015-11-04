#! bin/sh

src_name=C_NIMIF_15Q1_ORG
dest_name=C_NIMIF_15Q1_ORG_test

cd ~
cp /mnt/DB_Working/$src_name.backup ./database_backup/
rm /mnt/DB_Working/$src_name.backup

echo 'src_name=C_NIMIF_15Q1_ORG' > $dest_name.sh
echo 'dest_host=192.168.10.16' >> $dest_name.sh
echo 'dest_name=C_NIMIF_15Q1_ORG_test' >> $dest_name.sh
echo './../../usr/local/pgsql/bin/dropdb --host=$dest_host --port=5432 -Upostgres $dest_name' >> $dest_name.sh
echo './../../usr/local/pgsql/bin/createdb --host=$dest_host --port=5432 -Upostgres -Opostgres -EUTF8 -Tsample_postgis $dest_name' >> $dest_name.sh
echo './../../usr/local/pgsql/bin/pg_restore -c --ignore-version --host=$dest_host --port=5432 -Upostgres -Fc --dbname=$dest_name --jobs 2 ./database_backup/$src_name.backup' >> $dest_name.sh
sh $dest_name.sh


