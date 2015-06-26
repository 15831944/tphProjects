# -*- coding: UTF8 -*-
'''
Created on 2014-3-4

@author: hongchenzai
'''
import os
import pyodbc
import component.component_base
#used for pyodbc.connect
CONN_STR = "DRIVER={Microsoft Access Driver (*.mdb)};DBQ="
#psql program path
PSQL_PATH = r"C:\Program Files (x86)\PostgreSQL\9.0\bin"
#-f: sql script   -d: database   -L: log file   -o: output file
PSQL = '''psql.exe -f "%s" -h %s -U postgres -d "%s" -L "%s" -o "%s"'''

#dml statement
CREATE_SQL = '''CREATE TABLE "%s" (%s);\n'''
INSERT_SQL = '''INSERT INTO "%s" VALUES (%s);\n'''
SELECT_SQL = '''SELECT * FROM "%s";'''
DROP_SQL = '''DROP TABLE IF EXISTS "%s";'''


class comp_import_sidefile_mmi(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'sidefiles')
        self.mdb_path = ''
        self.sql_path = ''
        self.tbl_dict = {'phoneme': {'table_name': 'org_phoneme',
                                     'script': '',
                                     'cols': []  # 不包含Gid
                                     },
                         'regional_data': {'table_name': 'org_regional_data',
                                           'script': '',
                                           'cols': []
                                           },
                         'poi_info': {'table_name': 'org_poi_info',
                                      'script': '',
                                      'cols': []
                                      }
                         }

    def _Do(self):
        import common.common_func
        self.mdb_path = common.common_func.GetPath('mdb_path')
        self.sql_path = common.common_func.GetPath('temp_sql_path')
        if not self.mdb_path:  # 没有指定mdb文件目录
            self.log.error("Dose not indicate MDB path.")
            return 0
        else:
            if not os.path.exists(self.mdb_path):
                self.log.error("Dose not exist MDB path=%s" % self.mdb_path)
                return 0
        if not self.sql_path:
            self.sql_path = os.path.join(os.getcwd(), 'temp_sql')
        if not os.path.exists(self.sql_path):
            os.mkdir(self.sql_path)
        # 把MDB的数据导成SQL脚本
        self.delete_sub_file_folder(self.sql_path)
        self._drop_tables()
        self._mdb_2_files()
        self._create_tbls()
        self._file_2_psql()
        return 0

    def my_replace(self, myStr):
        '''
        regulate table name
        for example:
        xxx xxx -> xxx_xxx
        xxx.xxx -> xxx_xxx
        xxx&xxx -> xxx_xxx
        ...
        '''

        #filter illegal character
        def isLegal(myChr):
            '''
            character in [a-zA-Z0-9_] is legal
            '''
            return myChr.isalnum() or myChr == "_"

        #remove file suffix
        myList = list(myStr)
        illegalChrNum = 0

        for i in range(len(myList)):
            myList[i] = myList[i].lower()
            if not isLegal(myList[i]):
                illegalChrNum += 1
                #use "_" instead
                if illegalChrNum == 1:
                    myList[i] = "_"
                else:
                    myList[i] = ""

                if i < len(myList) - 1 and isLegal(myList[i + 1]):
                    illegalChrNum = 0
                else:
                    pass
            else:
                pass
        return reduce(lambda x, y: x + y, myList)

    def _mdb_2_files(self):
        '''
        read information from mdb file, then convert and write to sql
        '''
        poi_info_gid = 1
        self.log.info("Start MDB 2 File.")
        for names in os.walk(self.mdb_path):
            fileNames = [fileName for fileName in names[2] \
                         if (os.path.splitext(fileName)[1]) == ".mdb"]
            for fileName in fileNames:
                # 从名称里提起语种
                pos = fileName.lower().find('regional_data_')
                length = len('regional_data_')
                reg_lang_type = ''
                if pos == 0:
                    reg_lang_type = (os.path.splitext(fileName)[0])[length:]
                    data_type = 'regional_data'
                elif fileName.lower().find('phoneme') == 0:
                    data_type = 'phoneme'
                elif fileName.lower().find('poi_info') == 0:
                    data_type = 'poi_info'
                else:
                    self.log.error('Undefined data type. file=%s' % fileName)
                    continue

                #connect to mdb file
                try:
                    conn = pyodbc.connect(CONN_STR + os.path.join(names[0],
                                                                  fileName)
                                          )
                except pyodbc.DatabaseError as e:
                    print e.args
                    exit()
                cur = conn.cursor()

                #get table name
                tableNames = [row[2] for row in cur.tables() if row[3] == "TABLE"]
                for mdb_tbl_name in tableNames:
                    #create regular table name
                    name = os.path.splitext(fileName)[0]
                    data_file = self.my_replace(name + "_" + mdb_tbl_name)
                    self.log.info('Loading MDB 2 %s' % data_file)
                    admin_name = mdb_tbl_name
                    import codecs
                    temp_path = os.path.join(self.sql_path, data_file) + ".sql"
                    sqlFile = codecs.open(temp_path, "w+", 'utf-8')

                    dataDefStr = ''
                    cols = []
                    for column in cur.columns(mdb_tbl_name):
                        col_name = column[3].lower().replace(' ', '_')
                        col_type = column[5]
                        col_len = column[6]
                        
                        if col_name == 'table':
                            cols.append('"table"')
                        else:
                            cols.append(col_name)
                        if col_type.lower() == 'longchar':
                            dataDefStr += '''"%s" %s,''' % (col_name, 'text')
                        elif col_type.lower() == 'double':
                            dataDefStr += '''"%s" %s,''' % (col_name, 'double precision')
                        elif col_type.lower() == 'integer':
                            dataDefStr += '''"%s" %s,''' % (col_name, 'text')
                        else:
                            dataDefStr += '''"%s" %s (%d),''' % (col_name,
                                                                 col_type,
                                                                 col_len
                                                                 )
                    self._set_create_tbl_script(dataDefStr, data_type, cols)
                    # Get All Data
                    cur.execute(SELECT_SQL % (mdb_tbl_name))
                    for row in cur.fetchall():
                        #make insert statement
                        if 'poi_info' in data_file:
                            valuesStr = "$$%s$$," % str(poi_info_gid)
                            poi_info_gid += 1
                        else:
                            valuesStr = ""
                        for item in row:
                            if isinstance(item, unicode):
                                if item and item[-1] == '\\':
                                    self.log.warning('End of \\. id=%s' % row[0])
                                    item = item[:-1]
                                if 'poi_info' in data_file:
                                    item = item.replace('\n','')
                                    valuesStr += "$$%s$$," % item
                                else:
                                    valuesStr += "%s\t" % item
                            elif isinstance(item, int) or isinstance(item, float):
                                if 'poi_info' in data_file:
                                    valuesStr += "$$%s$$," % str(item)
                                else:
                                    valuesStr += "%s\t" % str(item)
                            else:
                                if 'poi_info' in data_file:
                                    valuesStr += "$$%s$$," % ''
                                else:
                                    valuesStr += "%s\t" % ''
                        if 'poi_info' in data_file:
                            valuesStr = valuesStr[:-1]  # 去掉最后一个逗号
                            sqlFile.write(INSERT_SQL % ('org_poi_info',
                                                        valuesStr)
                                          )
                        else:
                            if reg_lang_type:  # 区域语种
                                valuesStr += "%s\t" % reg_lang_type
                            valuesStr += "%s\t" % admin_name
                            valuesStr = valuesStr[:-1]  # 去掉最后一个TAB
                            valuesStr += '\n'
                            sqlFile.write(valuesStr)
                    sqlFile.close()
        self.log.info("Finish MDB To File.")

    def _set_create_tbl_script(self, col_def_str, data_type, cols):
        if data_type == 'phoneme':
            col_def_str += ' admin_name  character varying(100)'
            cols.append('admin_name')
        elif data_type == 'regional_data':
            col_def_str += ' regional_lang_type character varying(24),'
            cols.append('regional_lang_type')
            col_def_str += ' admin_name  character varying(100) '
            cols.append('admin_name')
        elif data_type == 'poi_info':
            col_def_str = col_def_str.rstrip(",")
        else:
            pass
        tbl_name = self.tbl_dict.get(data_type).get('table_name')
        # 添加gid
        col_def_str = ' gid serial not null primary key, ' + col_def_str
        sqlcmd = CREATE_SQL % (tbl_name, col_def_str)
        self.tbl_dict.get(data_type)['script'] = sqlcmd
        self.tbl_dict.get(data_type)['cols'] = cols

    def _create_tbls(self):
        for tbl in self.tbl_dict.itervalues():
            sql_script = tbl.get('script')
            if sql_script:
                self.pg.execute2(sql_script)
                self.pg.commit2()
                self.log.info('Drop table %s.' % tbl.get('table_name'))
            else:
                self.log.warning('Script is None')

    def _file_2_psql(self):
        self.log.info("Start Copying File To PSQL.")
        cwd = os.getcwd()
        psql_log = self._create_log_file()
        abs_path = os.path.abspath(self.sql_path)
        for sqlFile in os.listdir(abs_path):
            for data_type, tbl in self.tbl_dict.iteritems():
                if os.path.splitext(sqlFile)[0].find(data_type) == 0:
                    tbl_name = tbl.get('table_name')
                    cols = tbl.get('cols')
                    break
            if os.path.splitext(sqlFile)[1] == '.sql':  # 后缀是sql
                self.log.info('Copy %s 2 PSQL.' % sqlFile)
                if not tbl_name or not self.pg.IsExistTable(tbl_name):
                    self.log.warning('Does not Exist %s.' % tbl_name)
                    continue
                if 'poi_info' in tbl_name:
                    os.chdir(PSQL_PATH)
                    os.system(PSQL % (os.path.join(abs_path, sqlFile),
                                      self.pg.get_db_host2(),
                                      self.pg.get_db_name2(),
                                      psql_log,
                                      psql_log)
                              )
                    os.chdir(cwd)
                else:
                    f = open(os.path.join(abs_path, sqlFile))
                    self.pg.copy_from2(f, tbl_name, columns=cols)
                    self.pg.commit2()
        self.log.info("Finish Copying File 2 PSQL.")

    def _create_log_file(self):
        psql_log = os.path.join(os.path.abspath(self.sql_path), 'log')
        if os.path.isfile(psql_log):
            return psql_log
        else:
            f = open(psql_log, 'w')
            f.close()
        return psql_log

    def _drop_tables(self):
        '''
        drop tables
        used for test
        '''
        for tbl in self.tbl_dict.itervalues():
            table_name = tbl.get('table_name')
            self.pg.execute(DROP_SQL % (table_name))
            self.pg.commit2()
            self.log.info('Drop table %s.' % table_name)

    def delete_sub_file_folder(self, src):
        '''delete sub files and folders'''
        if os.path.isdir(src):
            for item in os.listdir(src):
                itemsrc = os.path.join(src, item)
                #self.delete_file_folder(itemsrc)
                if os.path.isfile(itemsrc):
                    try:
                        os.remove(itemsrc)
                    except:
                        pass
                if os.path.isdir(itemsrc):
                    try:
                        os.rmdir(src)
                    except:
                        pass
