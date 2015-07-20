# -*- encoding: utf-8 -*-
import os
import re
import psycopg2
import csv

#mdb文件目录
dir = r'/home/kotaimen/mdb_file/'
mdb_tbl_dic = {}


def make_create_sql():
    if os.path.isfile(dir + 'create.sql'):
        os.remove(dir + 'create.sql')


    for mdb_file in os.walk(dir):
        if len(mdb_file[2]) >0:
            for file_p in mdb_file[2]:
                if file_p[-3:] == 'mdb':
                    print file_p
                    cmd = 'mdb-schema %s  >>/home/kotaimen/mdb_file/create.sql'
                    cmd = cmd % (dir + file_p)
                    print cmd
                    os.system(cmd)
                    cmd = 'mdb-tables -1 %s ' % (dir + file_p)
                    val = os.popen(cmd).read()
                    mdb_tbl_dic[file_p] = val.split('\n')
    print mdb_tbl_dic

def modefy_create_sql():
    sql_file_name = dir + 'create.sql'
    sql_file_name_des = sql_file_name + '_new'
    fobj = open(sql_file_name, 'r')
    fobj_des = open(sql_file_name_des, 'w')
    for eachline in fobj:
        #判断表名中是否含有空格
        if eachline.find('TABLE ') >= 0:
            if eachline.find(';') >= 0:
                start_loc = eachline.find('TABLE ') + 6
                end_loc = eachline.find(';')
                tbl_name = eachline[start_loc:end_loc]
                eachline = eachline.replace(tbl_name, '"' + tbl_name + '"')
            else:
                start_loc = eachline.find('TABLE ') + 6
                end_loc = eachline.find('\n')
                tbl_name = eachline[start_loc:end_loc]
                eachline = eachline.replace(tbl_name, '"' + tbl_name + '"')

        if eachline.find('DROP TABLE') >= 0 :
            eachline = eachline.replace('DROP TABLE', 'DROP TABLE IF EXISTS')
        if eachline.find('Table') >= 0 :
            eachline = eachline.replace('Table', '"Table"')
        #create 语句，最后一行没有逗号
        if eachline.find('Text ') >= 0 and eachline.find(',') >0:
            loc = eachline.find('Text ')
            eachline = eachline[0:loc] + ' Text,\n'
        elif eachline.find('Text ') >= 0 and eachline.find(',') < 0:
            loc = eachline.find('Text ')
            eachline = eachline[0:loc] + ' Text \n'
        fobj_des.writelines(eachline)
    fobj.close()
    fobj_des.close()
    os.remove(sql_file_name)
    os.rename(sql_file_name_des, sql_file_name)

def make_insert_csv():
    for file_p in mdb_tbl_dic.keys():
        for tbl in mdb_tbl_dic[file_p]:
            if len(tbl) >0:
                cmd = 'mdb-export    %s %s >%s.csv' % (dir + file_p, '"' + tbl + '"', dir + '"' + tbl + '"')# tbl.replace(' ', '_').replace('&', '_'))
                os.system(cmd)

def modefy_insert_CSV():
    for sql_file in os.walk(dir):
        if len(sql_file[2]) >0:
            for file_p in sql_file[2]:
                if file_p[-3:] == 'csv' :
                    sql_file_name = dir + file_p
                    sql_file_name_des = sql_file_name + '_new'
                    fobj = open(sql_file_name, 'r')
                    fobj_des = open(sql_file_name_des, 'w')
                    for (num, val) in enumerate(fobj):
                        eachline = val
                        if num == 0:
                            col_list = eachline.split(',')
                            stat = 'COPY ' + '"' + (file_p[0:-4]) + '"' + ' (' #+ ('%s,'*len(line))[:-1]+')'
                            for col in col_list:
                                if col == 'Table':
                                    col = '"' + 'Table' + '"'
                                if col.find('\n') >= 0:
                                    col.replace('\n', '')
                                stat = stat + col + ','
                            stat = stat[:-2] + ')' + ' FROM STDIN WITH CSV ;\n'
                            eachline = stat

                        fobj_des.writelines(eachline)
                    fobj.close()
                    fobj_des.close()
                    os.remove(sql_file_name)
                    os.rename(sql_file_name_des, sql_file_name)


def insert_into_database():

    cmd = 'psql -h 172.26.11.205 -d ap_MapMyIndia_full_Sample -U postgres -f %s 2>>log.txt' % (dir + 'create.sql')
    os.system(cmd)

    for sql_file in os.walk(dir):
        if len(sql_file[2]) >0:
            for file_p in sql_file[2]:
                print file_p
                if file_p[-3:] == 'csv' :
                    cmd = 'psql -h 172.26.11.205 -d ap_MapMyIndia_full_Sample -U postgres -f %s 2>>log.txt' % (dir + '"' + file_p + '"')
                    os.system(cmd)


if __name__ == "__main__":
    #1.制作mdb文件中所包含TABLE的create脚本
    make_create_sql()
    #2.修改掉create脚本中的不合法字符
    modefy_create_sql()
    #3.将mdb中各表导出到csv文件中
    make_insert_csv()
    #4.修改csv脚本首行，改成copy形式
    modefy_insert_CSV()

    insert_into_database()
