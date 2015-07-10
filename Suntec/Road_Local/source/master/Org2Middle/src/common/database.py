# -*- coding: cp936 -*-
'''
Created on 2011-9-16
用来那连接数据库
@author: hongchenzai
'''

import psycopg2
import threading
import common.sql
import common.log
import common.config
BATCH_SIZE = 1024 * 10


class pg_client(object):
    __instance = None

    @staticmethod
    def instance():
        if pg_client.__instance is None:
            pg_client.__instance = pg_client()
        return pg_client.__instance
    
    @staticmethod
    def multi_execute(sqlcmd, min_gid, max_gid, thread_number=4, block_number=100000, logger=None):
        objExecute = CMultiPgExecute(sqlcmd, min_gid, max_gid, thread_number, block_number, logger)
        objExecute.execute()
    
    @staticmethod
    def multi_execute_in_tile(sqlcmd, tile_list_table, thread_number=4, logger=None):
        objExecute = CMultiPgExecuteInTile(sqlcmd, tile_list_table, thread_number, logger)
        objExecute.execute()

    def __init__(self, flag=1):
        self.log = common.log.common_log.instance().logger('DataBase')
        self.recordno = 0

        self.connected1 = False
        self.connected2 = False

        if flag != 1:  #  源数据库 和 目标数据库相同
            self.srv_path1 = self.srv_path2 = common.config.CConfig.instance().getPara('db1')
        else:  #  源数据库 和 目标数据库可以不相同
            self.srv_path1 = common.config.CConfig.instance().getPara('db1')
            self.srv_path2 = common.config.CConfig.instance().getPara('db2')

        if self.srv_path1 == '' or self.srv_path2 == '':
            raise Exception, "Doesn't exist host IP"

        self.SqlScriprt = common.sql.SqlScriprt.instance()

    def GetCountry(self):
        return common.config.CConfig.instance().getPara('proj_country')
        
    def __delete__(self):
        self.close1()
        self.close2()

    def connect1(self):
        '''连接源数据库'''
        if not self.connected1:
            self.conn1 = psycopg2.connect(self.srv_path1)  # 源数据库
            self.pgcur1 = self.conn1.cursor()
            self.connected1 = True

    def connect2(self):
        '''连接目标数据库'''
        if not self.connected2:
            self.conn2 = psycopg2.connect(self.srv_path2)  # 目标数据库
            self.pgcur2 = self.conn2.cursor()
            self.connected2 = True

    def deletetabledata(self, table_name):
        '''删除目标数据库表中的数据'''
        sqlcmd = "delete from " + table_name
        try:
            self.pgcur2.execute(sqlcmd)
            self.conn2.commit()
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            # raise Exception, 'database operate wrong'
            self.log.error('SQL deletetabledata error:' + sqlcmd)
            raise

    def deletetabledata1(self, table_name):
        '''删除目标数据库表中的数据'''
        sqlcmd = "delete from " + table_name
        try:
            self.pgcur1.execute(sqlcmd)
            self.conn1.commit()
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            # raise Exception, 'database operate wrong'
            self.log.error('SQL deletetabledata error:' + sqlcmd)
            raise

    def query(self, sqlcmd):
        '''在源数据库表查询数据'''
        try:
            self.pgcur1.execute(sqlcmd)
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('SQL query error:' + sqlcmd)
            raise

    def query_fromcur2(self, sqlcmd):
        '''在源数据库表查询数据'''
        try:
            self.pgcur2.execute(sqlcmd)
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('SQL query error:' + sqlcmd)
            raise

    def insert(self, table_name, col_list=[], parameters=[]):
        '''往目标数据库表插入数据
           table_name: 表名
           col_list:   字段列表
           parameters: 参数列表
        '''
        p_list = []
        for temp in col_list:
            p_list.append('%s')

        sqlcmd = 'INSERT INTO ' + table_name + '(' + ','.join(col_list) + ')'
        sqlcmd += 'values' + '(' + ','.join(p_list) + ')'

        return self.execute(sqlcmd, parameters)

    def execute1(self, sqlcmd, parameters=[]):
        '''往目标数据库表插入数据'''
        try:
            self.pgcur1.execute(sqlcmd, parameters)
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('SQL excute error:' + sqlcmd)
            raise

    def execute2(self, sqlcmd, parameters=[]):
        '''往目标数据库表插入数据'''
        try:
            if parameters:
                self.pgcur2.execute(sqlcmd, parameters)
            else:
                self.pgcur2.execute(sqlcmd)
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('SQL excute error:' + sqlcmd)
            raise
        
    def do_big_insert2(self, sqlcmd ):
        '''insert a large number of record to table, and commit it right away'''
        if 0 != self.execute2(sqlcmd):
            return -1
        self.commit2()
        return 0
        
    def getcnt1(self):
        '''往目标数据库表插入数据'''
        return self.pgcur1.rowcount
    def getcnt2(self):

        return self.pgcur2.rowcount

    def execute(self, sqlcmd, parameters=[]):
        '''往目标数据库表插入数据'''
        try:
            self.pgcur2.execute(sqlcmd, parameters)
            self.recordno += 1
            if self.recordno % 100000 == 0:
                self.commit()
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('SQL excute error:' + sqlcmd)
            raise
    def callproc(self, proc_name, parameters=[]):
        '''调用目标数据库的存储过程'''
        try:
            self.pgcur2.callproc(proc_name, parameters)
            self.recordno += 1
            return 0
        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('call process error:' + proc_name)
            raise

    def createtalbe(self, sqlcmd):
        try:
            self.pgcur2.execute(sqlcmd)
            return 0
        except:
            self.log.error('Create Table Failed' + sqlcmd)
            raise

    def fetchone(self):
        '''读一条记录'''
        return self.pgcur1.fetchone()

    def fetchone2(self):
        '''读一条记录'''
        return self.pgcur2.fetchone()

    def fetchone_fromcur2(self):
        '''读一条记录'''
        return self.pgcur2.fetchone()

    def fetchall(self):
        return self.pgcur1.fetchall()

    def fetchall2(self):
        return self.pgcur2.fetchall()

    def reconnect2(self):
        self.commit()
        self.close2()
        self.connect2()

    def commit1(self):
        # print self.recordno
        self.conn1.commit()

    def commit2(self):
        # print self.recordno
        self.conn2.commit()

    def commit(self):
        #print self.recordno
        self.conn2.commit()

    def close1(self):
        if self.connected1 == True:
            self.pgcur1.close()
            self.conn1.close()
            self.connected1 = False

    def close2(self):
        self.recordno = 0
        if self.connected2 == True:
            self.pgcur2.close()
            self.conn2.close()
            self.connected2 = False

    def closeall(self):
        # 关闭源数据库连接
        self.close1()
        # 关闭目标数据库连接
        self.close2()
    
    def analyze(self, table_name):
        sqlcmd = 'analyze "%s";' % table_name
        self.pgcur2.execute(sqlcmd)
        self.conn2.commit()

    def copy_to(self, file_object, table, sep='\t', null='\N', columns=None):
        self.pgcur1.copy_to(file_object, table, sep, null, columns)

    def copy_from1(self, file_object, table, sep='\t', null="", size=8192, columns=None):
        try:
            self.pgcur1.copy_from(file_object, table, sep, null, size, columns)
            return 0

        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            raise

    def copy_from2(self, file_object, table, sep='\t', null="",
                   size=8192, columns=None):
        try:
            self.pgcur2.copy_from(file_object, table, sep, null, size, columns)
            return 0

        except Exception, ex:
            print '%s:%s' % (Exception, ex)
            raise

    def create_index1(self, sqlcmd, index_name):
        """创建索引"""
        # check the index
        sqlstr = """select * from pg_indexes
                    where  indexname = %s
                 """

        try:
            # search the index by name
            self.pgcur1.execute(sqlstr, (index_name,))
            if self.pgcur1.rowcount == 1 :  # 索引已经存在
                # self.log.warning('Has been existed index "' + index_name + '"')
                return 0
            else:
                try:
                    self.pgcur1.execute(sqlcmd)
                    self.conn1.commit()
                    # self.log.info('Created index index "' + index_name + '"')
                    return 0
                except:
                    self.log.error('Created index ' + index_name + ' failed :' + sqlcmd)
                    raise
        except:
            self.log.error('Created index ' + index_name + ' failed :' + sqlcmd)
            raise

    def create_index2(self, sqlcmd, index_name):
        """创建索引"""
        # check the index
        sqlstr = """select * from pg_indexes
                    where  indexname = %s
                 """

        try:
            # search the index by name
            self.pgcur2.execute(sqlstr, (index_name,))
            if self.pgcur2.rowcount == 1 :  # 索引已经存在
                # self.log.warning('Has been existed index "' + index_name + '"')
                return 0
            else:
                try:
                    self.pgcur2.execute(sqlcmd)
                    self.conn2.commit()
                    # self.log.info('Created index "' + index_name + '"')
                    
                    #
                    sqlstr = "select tablename from pg_indexes where indexname = %s"
                    self.pgcur2.execute(sqlstr, (index_name,))
                    table_name = self.pgcur2.fetchone()[0]
                    self.analyze(table_name)
                    
                    return 0
                except:
                    self.conn2.rollback()
                    self.log.error('Created index ' + index_name + ' failed :' + sqlcmd)
                    raise
        except:
            self.conn2.rollback()
            self.log.error('Created index ' + index_name + ' failed :' + sqlcmd)
            raise

    def delete_index2(self, index_name):
        """删除索引"""
        sqlcmd = """select * from pg_indexes
                    where  indexname = %s
                 """
        try:
            # search the index by name
            self.pgcur2.execute(sqlcmd, (index_name,))
            if self.pgcur2.rowcount == 1 :  # 索引已经存在
                try:
                    self.pgcur2.execute("DROP INDEX " + index_name)
                    self.conn2.commit()
                    # self.log.info('DROP index "' + index_name + '"')
                    return 0
                except:
                    self.log.error('DROP index ' + index_name + ' failed ')
                    raise
            else:
                # doesn't exit this index
                return 0
        except:
            self.log.error('DROP index  ' + index_name + ' failed :' + sqlcmd)
            raise

    def CreateTalbe1(self, sqlcmd, table_name):
        '''主要用于创建临时表'''

        sqlstr = '''
                 SELECT  tablename
                 FROM  pg_tables
                 where tablename = %s
                 '''
        if self.execute1(sqlstr, (table_name,)) == -1:
            self.log.error('Search table  ' + table_name + ' failed :' + sqlstr)
            return -1
        else:
            if self.pgcur1.rowcount == 1 :  # 临时表存在已经存在
                try:
                    self.execute1('DROP TABLE ' + table_name + ' CASCADE')
                    self.commit1()
                    # self.log.info('DROP TABLE  ' + table_name)
                except:
                    self.log.error('DROP TABLE  ' + table_name + 'failed.')
                    raise

        try:
            # self.log.info('Now it is creating table ' + table_name +'...')
            if self.execute1(sqlcmd) == -1:
                # self.log.error('Create table ' + table_name + 'failed :' + sqlcmd)
                return -1
            self.commit1()
            # self.log.info('Create table ' + table_name + ' succeed')
            return 0
        except:
            self.log.error('Create table ' + table_name + 'failed :' + sqlcmd)
            raise


    def CreateTable1_ByName(self, table_name):
        if self.connected1 != True:
            self.log.error("Connect1 is not opened.")
            return -1

        sqlcmd = self.SqlScriprt.GetSqlScript(table_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for table " + table_name)
            return -1

        return self.CreateTalbe1(sqlcmd, table_name)

    def CreateIndex1_ByName(self, index_name):
        if self.connected1 != True:
            self.log.error("Connect1 is not opened.")
            return -1

        sqlcmd = self.SqlScriprt.GetSqlScript(index_name)
        if sqlcmd == None:
            # self.log.error("Doesn't exist SQL script for index " +  index_name)
            return -1
        return self.create_index1(sqlcmd, index_name)

    def CreateFunction1_ByName(self, function_name):
        if self.connected1 != True:
            self.log.error("Connect1 is not opened.")
            return -1

        sqlcmd = self.SqlScriprt.GetSqlScript(function_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for function " + function_name)
            return -1

        if self.execute1(sqlcmd) == -1:
            # self.log.error("Create function " +  function_name + " Failed.")
            return -1

        self.conn1.commit()
        # self.log.info("Create function " +  function_name + " Succeeded.")
        return 0

    def CreateTable2_ByName(self, table_name):
        if self.connected2 != True:
            self.log.error("Connect2 is not opened.")
            return -1

        # sqlcmd = sql_dict.GetSqlStr(table_name)
        sqlcmd = self.SqlScriprt.GetSqlScript(table_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for table " + table_name)
            raise Exception, "Doesn't exist SQL script for table " + table_name

        sqlstr = '''
                 SELECT  tablename
                 FROM  pg_tables
                 where tablename = %s
                 '''
        if self.execute2(sqlstr, (table_name,)) == -1:
            self.log.error('Search table  ' + table_name + ' failed :' + sqlstr)
            return -1
        else:
            if self.pgcur2.rowcount == 1 :  # 表存在已经存在
                # 删除 引用table_name的外键
                self.DropFKey2_ByName(table_name)
                # 删除 引用table_name的View
                self.DropView2_ByName(table_name)
                try:
                    self.execute2('DROP TABLE ' + table_name + ' CASCADE')
                    self.commit2()
                    # self.log.info('DROP TABLE  ' + table_name)
                except:
                    self.log.error('DROP TABLE  ' + table_name + 'failed.')
                    raise

        try:
            # self.log.info('Now it is creating table ' + table_name +'...')
            if self.execute2(sqlcmd) == -1:
                # self.log.error('Create table ' + table_name + 'failed :' + sqlcmd)
                return -1
            self.commit2()
            self.analyze(table_name)
            # self.log.info('Create table ' + table_name + ' succeed')
            return 0
        except:
            self.log.error('Create table ' + table_name + 'failed :' + sqlcmd)
            raise

    def CreateView2_ByName(self, table_name):
        if self.connected2 != True:
            self.log.error("Connect2 is not opened.")
            return -1

        # sqlcmd = sql_dict.GetSqlStr(table_name)
        sqlcmd = self.SqlScriprt.GetSqlScript(table_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for view " + table_name)
            raise Exception, "Doesn't exist SQL script for view " + table_name

        sqlstr = '''
                 SELECT  viewname
                 FROM  pg_views
                 where viewname = %s
                 '''
        if self.execute2(sqlstr, (table_name,)) == -1:
            self.log.error('Search view  ' + table_name + ' failed :' + sqlstr)
            return -1
        else:
            if self.pgcur2.rowcount == 1 :  # 表存在已经存在
                # 删除 引用table_name的View
                self.DropView2_ByName(table_name)
                try:
                    self.execute2('DROP VIEW ' + table_name)
                    self.commit2()
                    # self.log.info('DROP VIEW  ' + table_name)
                except:
                    self.log.error('DROP VIEW  ' + table_name + 'failed.')
                    raise

        try:
            # self.log.info('Now it is creating table ' + table_name +'...')
            if self.execute2(sqlcmd) == -1:
                # self.log.error('Create table ' + table_name + 'failed :' + sqlcmd)
                return -1
            self.commit2()
            # self.log.info('Create table ' + table_name + ' succeed')
            return 0
        except:
            self.log.error('Create view ' + table_name + 'failed :' + sqlcmd)
            raise

    def CreateIndex2_ByName(self, index_name):
        sqlcmd = self.SqlScriprt.GetSqlScript(index_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for index " + index_name)
            raise
        return self.create_index2(sqlcmd, index_name)

    def CreateFunction2_ByName(self, function_name):
        if self.connected2 != True:
            self.log.error("Connect2 is not opened.")
            raise

        sqlcmd = self.SqlScriprt.GetSqlScript(function_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for function " + function_name)
            raise Exception, "Doesn't exist SQL script for function " + function_name

        #
        sqlcmd_drop_function = self.__getDropFunctionSqlcmd(sqlcmd)
        if sqlcmd_drop_function:
            self.execute2(sqlcmd_drop_function)

        if self.execute2(sqlcmd) == -1:
            # self.log.error("Create function " +  function_name + " Failed.")
            return -1

        self.conn2.commit()
        # self.log.info("Create function " +  function_name + " Succeeded.")
        return 0

    def __getDropFunctionSqlcmd(self, create_cmd):
        sqlcmd = create_cmd.lower()
        index_func_name_end = sqlcmd.find(')')
        if index_func_name_end < 0:
            return

        index_func_name_begin = sqlcmd.find('create or replace function')
        if index_func_name_begin >= 0:
            index_func_name_begin += len('create or replace function')
            func_name = sqlcmd[index_func_name_begin:index_func_name_end + 1]
        else:
            index_func_name_begin = sqlcmd.find('create function')
            if index_func_name_begin >= 0:
                index_func_name_begin += len('create function')
                func_name = sqlcmd[index_func_name_begin:index_func_name_end + 1]
        
        #if 'default' appear,cut it 
        while True:
            if func_name.find('default') > 0:
                idefault_start = func_name.find('default')
                if func_name[idefault_start:len(func_name)].find(',') >= 0 :
                    idefault_end = idefault_start + func_name[idefault_start:len(func_name)].find(',')
                else:
                    idefault_end = len(func_name)-1
                func_name = func_name[0:idefault_start] + func_name[idefault_end:len(func_name)]
            else:   
                drop_cmd = 'drop function if exists' + func_name + ' CASCADE;'
                return drop_cmd

    def IsExistTable(self, table_name):
        if self.connected2 != True:
            self.log.error("Connect2 is not opened.")
            raise

        sqlstr = '''
                 SELECT  tablename
                 FROM  pg_tables
                 where tablename = %s
                 '''
        if self.execute2(sqlstr, (table_name,)) == -1:
            # self.log.error('Search table  ' + table_name + ' failed :' + sqlstr)
            return False
        else:
            if self.pgcur2.rowcount == 1 :  # 表存在已经存在
                return True

        return False
    
    def getMinMaxValue(self, table_name, column_name):
        sqlcmd = "SELECT min(%s), max(%s) FROM %s;" % (column_name, column_name, table_name)
        self.execute2(sqlcmd)
        row = self.fetchone2()
        min_value  = row[0]
        max_value  = row[1]
        return (min_value, max_value)

    def DropFKey2_ByName(self, table_name):
        "删除 引用table_name的所有外键。"
        if self.connected2 != True:
            self.log.error("Connect2 is not opened.")
            return -1

        if table_name == None or table_name == '':
            self.log.error("table_name is None.")
            return -1

        # find all fkey, which reference "table_name"
        sqlcmd = """
                select  b.conname as fkey_name, C.relname as table_name
                from (
                    SELECT relid, relname
                      FROM pg_statio_user_tables
                      where relname = %s
                ) AS A
                left join pg_constraint as B
                ON B.confrelid = A.relid
                left join pg_statio_user_tables as C
                ON B.conrelid = C.relid
                """
        self.execute2(sqlcmd, (table_name,))
        row = self.fetchone2()
        sqlcmd = ''
        while row:
            sqlcmd += self.GetDropFKeyStr(row[0], row[1])
            row = self.fetchone2()

        # print sqlcmd
        # Drop all fkey, which reference "table_name"
        if sqlcmd != '':
            if self.execute2(sqlcmd, table_name) == -1:
                return -1
            else:
                self.commit2()

        return 0

    def GetDropFKeyStr(self, fkey_name, table_name):
        if fkey_name == None or fkey_name == '':
            return ''

        if table_name == None or table_name == '':
            return ''

        return "ALTER TABLE " + table_name + " DROP CONSTRAINT if exists " + fkey_name + ";\n"

    def DropView2_ByName(self, table_name):
        "删除 引用table_name的所有View。"
        if self.connected2 != True:
            self.log.error("Connect2 is not opened.")
            return -1

        if table_name == None or table_name == '':
            self.log.error("table_name is None.")
            return -1

        # find all views, which reference "table_name"
        sqlcmd = """
                SELECT viewname
                 FROM pg_views
                 where definition like %s or definition like %s or definition like %s;
                 """
        # sqlcmd +=  ",%' or definition like '%" +table_name+ " %'"

        # print sqlcmd

        if self.execute2(sqlcmd, ("%" + table_name + " %", "%" + table_name + ",%", "%" + table_name + ")%")) == -1:
            return -1
        row = self.fetchone2()
        sqlcmd = ''
        while row:
            sqlcmd += self.GetDropViewStr(row[0])
            row = self.fetchone2()

        # print sqlcmd
        # Drop all views, which reference "table_name"
        if sqlcmd != '':
            if self.execute2(sqlcmd, table_name) == -1:
                return -1
            else:
                self.commit2()

        return 0

    def GetPKeyColumns(self, table_name):
        '''取得主键对应的字段名列表'''
        if table_name == None: return None
        sqlcmd = """
                select attname
                  from pg_attribute
                  where attrelid in (
                    SELECT relid
                      FROM pg_statio_user_tables
                      where relname = %s)

                  and attnum in (
                    SELECT regexp_split_to_table(indkey::character varying, E'\\ +')::integer
                      FROM pg_index
                      where indrelid in (
                        SELECT relid
                        FROM pg_statio_user_tables
                        where relname = %s
                        ) and indisprimary = true    -- primary key
                  )
        """
        col_list = []
        self.execute2(sqlcmd, (table_name, table_name,))
        rows = self.fetchall2()
        for row in rows:
            col_list.append(row[0])
        return col_list

    def GetDropViewStr(self, view_name):
        if view_name == None or view_name == '':
            return ''

        return "DROP VIEW " + view_name + ";\n"

    def Get_Version(self):
        '取得当前制做的数据版本号.'
        # srv_path2 格式: host='172.26.179.79' dbname='C_0_1.5.6_0037_0018' user='postgres' password='pset123456'
        dbname = self.srv_path2.split(' ')[1]
        version = dbname[dbname.find("'") + 1 : dbname.rfind("'")]
        return version

    def batch_query1(self, sql, batch_size=BATCH_SIZE):
        try:
            curs = self.conn1.cursor(name='batch1')
            curs.arraysize = batch_size
            curs.execute(sql)
            while True:
                rows = curs.fetchmany()
                if not rows:
                    break
                yield rows

        except psycopg2.DatabaseError as e:
            raise

        finally:
            curs.close()

    def batch_query2(self, sql, parameters=(), batch_size=BATCH_SIZE):
        try:
            curs = self.conn2.cursor(name='batch2')
            curs.arraysize = batch_size
            # print curs.mogrify(sql, parameters)
            curs.execute(sql, parameters)
            while True:
                rows = curs.fetchmany()
                if not rows:
                    break
                yield rows

        except psycopg2.DatabaseError as e:
            raise

        finally:
            if curs:
                curs.close()

    def get_batch_data2(self, sqlcmd, parameters=(), batch_size=BATCH_SIZE):
        row_data = list()
        for rows in self.batch_query2(sqlcmd, parameters, batch_size):
            for row in rows:
                if not row:
                    break
                row_data = row
                yield row_data

    def get_db_name2(self):
        return self._get_db_param('dbname')

    def get_db_host2(self):
        return self._get_db_param('host')

    def _get_db_param(self, mykey):
        if self.srv_path2:
            elements = self.srv_path2.split(' ')
            for element in elements:
                key_val = element.split('=')
                key = key_val[0]
                val = key_val[1]
                if key.strip() == mykey:
                    return val.strip().strip("'").strip('"')
        return ''

class CMultiPgExecute:
    def __init__(self, sqlcmd, min_gid, max_gid, thread_number, block_number, logger):
        self.sqlcmd = sqlcmd
        self.min_gid = min_gid
        self.max_gid = max_gid
        self.thread_number = thread_number
        self.block_number = block_number
        self.logger = logger
    
    def execute(self):
        self.sem = threading.Semaphore(self.thread_number)
        self.exception_flag = 0
        self.thread_list = {}
        
        current_gid = self.min_gid
        while (not self.exception_flag) and (current_gid <= self.max_gid):
            from_gid = current_gid
            to_gid = current_gid + self.block_number - 1
            
            self.sem.acquire()
            objThread = threading.Thread(target=self.__execute, args=(self.sqlcmd, from_gid, to_gid))
            self.thread_list[from_gid] = objThread
            objThread.start()
            
            current_gid = to_gid + 1
        
        for objThread in self.thread_list.values():
            objThread.join()
        
        if self.exception_flag:
            raise Exception, 'error happened while executing sql: \n%s' % self.sqlcmd
    
    def __execute(self, sqlcmd, from_gid, to_gid):
        #print sqlcmd, from_gid, to_gid
        try:
            try:
                objPg = pg_client()
                objPg.connect2()
                objPg.execute2(sqlcmd % (from_gid, to_gid))
                objPg.commit()
            finally:
                objPg.close2()
                del self.thread_list[from_gid]
                self.sem.release()
        except:
            self.exception_flag = 1
            if self.logger:
                self.logger.warning('error happened while executing sql:\n' + sqlcmd % (from_gid, to_gid))

class CMultiPgExecuteInTile:
    def __init__(self, sqlcmd, tile_list_table, thread_number, logger):
        self.sqlcmd = sqlcmd
        self.tile_list_table = tile_list_table
        self.thread_number = thread_number
        self.logger = logger
    
    def execute(self):
        self.sem = threading.Semaphore(self.thread_number)
        self.exception_flag = 0
        self.thread_list = {}
        
        objPg = pg_client()
        objPg.connect2()
        sqlcmd = """
                select array_agg(tile_id) as tile_array_list
                from %s
                group by tx >> 5, ty >> 5
                """ % self.tile_list_table
        tile_array_list = objPg.get_batch_data2(sqlcmd)
        for (tile_array,) in tile_array_list:
            if self.exception_flag:
                break
            else:
                self.sem.acquire()
                objThread = threading.Thread(target=self.__execute, args=(self.sqlcmd, tile_array))
                self.thread_list[tile_array[0]] = objThread
                objThread.start()
        
        for objThread in self.thread_list.values():
            objThread.join()
        
        if self.exception_flag:
            objPg.close2()
            raise Exception, 'error happened while executing sql: \n%s' % self.sqlcmd
    
    def __execute(self, sqlcmd, tile_array):
        #print sqlcmd, from_gid, to_gid
        try:
            try:
                objPg = pg_client()
                objPg.connect2()
                for tile_id in tile_array:
                    objPg.execute2(sqlcmd.replace('%s', str(tile_id)))
                objPg.commit()
            finally:
                objPg.close2()
                del self.thread_list[tile_array[0]]
                self.sem.release()
        except:
            self.exception_flag = 1
            if self.logger:
                self.logger.warning('error happened while executing sql:\n' + sqlcmd.replace('%s', str(tile_id)))
    