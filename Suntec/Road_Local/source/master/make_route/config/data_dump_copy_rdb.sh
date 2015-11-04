#! bin/sh

dest_name=CHN_NAVINFO_2015Q1_0082_0005_102

cd ~
echo 'dest_host=192.168.10.16' > $dest_name.sh
echo 'dest_name=CHN_NAVINFO_2015Q1_0082_0005_102' >> $dest_name.sh
echo './../../usr/local/pgsql/bin/pg_dump --ignore-version --host=$dest_host --port=5432 -Upostgres -Fc --file=./database_backup/$dest_name.backup --table=rdb_* --table=geometry_columns --table=id_fund_link --table=id_fund_node --table=temp_link_org_rdb --table=temp_split_newlink --table=temp_rdb_link_mapping_flag --table=temp_topo_link $dest_name' >> $dest_name.sh
sh $dest_name.sh
cp ./database_backup/$dest_name.backup /mnt/DB_Working
