# -*- coding: UTF8 -*-
'''
Created on 2012-12-3

@author: yuanrui
'''

import psycopg2
import common.Logger
import common.sql
import threading
BATCH_SIZE = 1024 * 10


class CDB(object):
    __instance = None
    
    @staticmethod
    def instance():
        if CDB.__instance is None:
            CDB.__instance = CDB()
        return CDB.__instance
    
    def __init__(self, flag = 1):
        self.log      = common.Logger.CLogger.instance().logger('DataBase')
        self.recordno  = 0
        
        self.connected = False
 
        self.srv_path = common.ConfigReader.CConfigReader.instance().getPara('db')
            
        if self.srv_path == '':
            self.log.error("Doesn't exist host IP")
            #common.ConfigReader.CConfigReader.instance().proj_exit(1)
            exit(1)
        
        self.objSqlLoader = common.sql.CSqlLoader()
        self.objSqlLoader.init()
    
    def __delete__(self):
        self.close()
         
    def connect(self):
        '''connect to DB'''
        self.conn  = psycopg2.connect(self.srv_path)   
        self.pgcur = self.conn.cursor()
        self.connected = True
        
    def deletetabledata(self, table_name):
        '''delete contents of table'''
        sqlcmd = "delete from " + table_name
        try:
            self.pgcur.execute(sqlcmd)
            self.conn.commit()
            return 0
        except Exception,ex:
            print '%s:%s' % (Exception, ex)      
            #raise Exception, 'database operate wrong'
            self.log.error('SQL deletetabledata error:' + sqlcmd) 
            raise
        
    def query(self, sqlcmd):
        '''query'''
        try:
            self.pgcur.execute(sqlcmd)
            return 0
        except Exception,ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('SQL query error:' + sqlcmd) 
            raise
            
    def insert(self, table_name, col_list = [], parameters = []):
        '''insert into table
           table_name: table name
           col_list:   field list
           parameters: parameters
        '''
        p_list  = []
        for temp in col_list:
            p_list.append('%s')
        
        sqlcmd  = 'INSERT INTO ' + table_name + '(' + ','.join(col_list) + ')' 
        sqlcmd += 'values' + '('  + ','.join(p_list)+ ')'
        
        return self.execute(sqlcmd, parameters)
    
    def execute(self, sqlcmd, parameters = []):
        '''execute commands '''
        try:
            if parameters:
                self.pgcur.execute(sqlcmd, parameters)
            else:
                self.pgcur.execute(sqlcmd)
            return 0
        except Exception,ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('SQL execute error:' + sqlcmd) 
            raise
        
    def getcnt(self):
        '''get row count'''
        return self.pgcur.rowcount
         
    def callproc(self, proc_name, parameters = []):
        '''execute procedures'''
        try:
            self.pgcur.callproc(proc_name, parameters)
#            self.recordno  += 1
            return 0
        except Exception,ex:
            print '%s:%s' % (Exception, ex)
            self.log.error('call process error:' + proc_name) 
            raise
            
    def CreateTable(self, sqlcmd):
        try:
            self.pgcur.execute(sqlcmd)
            return 0
        except:
            self.log.error('Create Table Failed' + sqlcmd) 
            raise
                   
    def fetchone(self):
        '''read one record'''
        return self.pgcur.fetchone()
      
    def fetchall(self):
        return self.pgcur.fetchall()
    
    def reconnect(self):
        self.commit()
        self.close()
        self.connect()
               
    def commit(self):
        #print self.recordno
        self.conn.commit()
    
    def rollback(self):
        #print self.recordno
        self.conn.rollback()
    
    def close(self):
        if self.connected == True:
            self.pgcur.close()
            self.conn.close()
            self.connected = False
        
    def closeall(self): 
        # close database connection
        self.close()
        
    def copy_to(self, file_object, table, sep='\t', null='\N', columns=None):
        self.pgcur.copy_to(file_object, table, sep, null, columns)

    def copy_from(self, file_object, table, sep='\t', null="", size=8192, columns=None):
        try:
            self.pgcur.copy_from(file_object, table, sep, null, size, columns)
            return 0
            
        except Exception,ex:
            print '%s:%s' % (Exception, ex)
            raise
        
    def create_index(self, sqlcmd, index_name):
        """create indexes"""
        # check the index
        sqlstr = """select * from pg_indexes
                    where  indexname = %s
                 """
                
        try:
            # search the index by name
            self.pgcur.execute(sqlstr, (index_name,))
            if self.pgcur.rowcount == 1 :  
                #self.log.warning('Has been existed index "' + index_name + '"') 
                return 0
            else:
                try:
                    self.pgcur.execute(sqlcmd)
                    self.conn.commit()
                    #self.log.info('Created index index "' + index_name + '"') 
                    return 0
                except:
                    self.log.error('Created index ' + index_name + ' failed :' + sqlcmd) 
                    raise    
        except:
            self.log.error('Created index ' + index_name + ' failed :' + sqlcmd) 
            raise
           
    def delete_index(self, index_name):
        """delete indexes"""
        sqlcmd = """select * from pg_indexes
                    where  indexname = %s
                 """
        try:
            # search the index by name
            self.pgcur.execute(sqlcmd, (index_name,))
            if self.pgcur.rowcount == 1 :  # index already exist
                try:
                    self.pgcur.execute("DROP INDEX " + index_name)
                    self.conn.commit()
                    #self.log.info('DROP index "' + index_name + '"') 
                    return 0
                except:
                    self.log.error('DROP index ' + index_name + ' failed ' ) 
                    raise
            else:
                # doesn't exit this index
                return 0
        except:
            self.log.error('DROP index  ' + index_name + ' failed :' + sqlcmd) 
            raise
            
    def CreateTable1(self, sqlcmd, table_name):
        '''create tables''' 
        
        sqlstr = '''
                 SELECT  tablename
                 FROM  pg_tables 
                 where tablename = %s
                 '''
        if self.execute(sqlstr, (table_name,)) == -1:
            self.log.error('Search table  ' + table_name + ' failed :' + sqlstr)
            return -1
        else:
            if self.pgcur.rowcount == 1 :  # table already exist
                #delete foreign keys of table
                self.DropFKey_ByName(table_name)
                # delete views of table
                self.DropView_ByName(table_name)
                try:
                    self.execute('DROP TABLE ' +  table_name)
                    self.commit()
                    #self.log.info('DROP TABLE  ' + table_name)
                except:
                    self.log.error('DROP TABLE  ' + table_name + 'failed.')
                    raise
                    
        try:
            #self.log.info('Now it is creating table ' + table_name +'...')
            if self.execute(sqlcmd) == -1:
                #self.log.error('Create table ' + table_name + 'failed :' + sqlcmd)
                return -1
            self.commit()
            #self.log.info('Create table ' + table_name + ' succeed')
            return 0
        except:
            self.log.error('Create table ' + table_name + 'failed :' + sqlcmd)
            raise
        
    def CreateTable_ByName(self, table_name):
        if self.connected != True:
            self.log.error("Connect is not opened.")
            return -1
        
        sqlcmd = self.objSqlLoader.GetSqlScript(table_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for table " +  table_name)
            return -1
        
        return self.CreateTable1(sqlcmd, table_name)
    
    def CreateIndex_ByName(self, index_name):
        if self.connected != True:
            self.log.error("Connect is not opened.")
            return -1
        
        sqlcmd = self.objSqlLoader.GetSqlScript(index_name)
        if sqlcmd == None:
            #self.log.error("Doesn't exist SQL script for index " +  index_name)
            return -1
        return self.create_index(sqlcmd, index_name)
    
    def CreateView_ByName(self, table_name):
        if self.connected != True:
            self.log.error("Connect is not opened.")
            return -1
        
        #sqlcmd = sql_dict.GetSqlStr(table_name)
        sqlcmd = self.objSqlLoader.GetSqlScript(table_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for view " +  table_name)
            raise Exception, "Doesn't exist SQL script for view " +  table_name
        
        sqlstr = '''
                 SELECT  viewname
                 FROM  pg_views 
                 where viewname = %s
                 '''
        if self.execute(sqlstr, (table_name,)) == -1:
            self.log.error('Search view  ' + table_name + ' failed :' + sqlstr)
            return -1
        else:
            if self.pgcur.rowcount == 1 :  # table already exist
                # delete views of table
                self.DropView_ByName(table_name)
                try:
                    self.execute('DROP VIEW ' +  table_name)
                    self.commit()
                    #self.log.info('DROP VIEW  ' + table_name)
                except:
                    self.log.error('DROP VIEW  ' + table_name + 'failed.')
                    raise
                    
        try:
            #self.log.info('Now it is creating table ' + table_name +'...')
            if self.execute(sqlcmd) == -1:
                return -1
            self.commit()
    
            return 0
        except:
            self.log.error('Create view ' + table_name + 'failed :' + sqlcmd)
            raise
      
    def CreateFunction_ByName(self, function_name):
        if self.connected != True:
            self.log.error("Connect is not opened.")
            raise
        
        sqlcmd = self.objSqlLoader.GetSqlScript(function_name)
        if sqlcmd == None:
            self.log.error("Doesn't exist SQL script for function " +  function_name)
            raise Exception, "Doesn't exist SQL script for function " +  function_name
        
        if self.execute(sqlcmd) == -1:
 
            return -1
        
        self.conn.commit()

        return 0
    
    def IsExistTable(self, table_name):
        if self.connected != True:
            self.log.error("Connect is not opened.")
            raise
        
        sqlstr = '''
                 SELECT  tablename
                 FROM  pg_tables 
                 where tablename = %s
                 '''
        if self.execute(sqlstr, (table_name,)) == -1:
  
            return False
        else:
            if self.pgcur.rowcount == 1 :  # table already exist
                return True
           
        return False
    
    def getOnlyQueryResult(self, sqlcmd):
        self.execute(sqlcmd)
        row = self.fetchone()
        if row:
            return row[0]
        return 0
    
    def DropFKey_ByName(self, table_name):
        "drop foreign key of table"
        if self.connected != True:
            self.log.error("Connect is not opened.")
            return -1
        
        if table_name== None or table_name == '':
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
        self.execute(sqlcmd, (table_name,))
        row = self.fetchone()
        sqlcmd = ''
        while row:
            sqlcmd += self.GetDropFKeyStr(row[0], row[1])
            row = self.fetchone()

        # Drop all fkey, which reference "table_name"
        if sqlcmd != '':
            if self.execute(sqlcmd, table_name) == -1:
                return -1
            else:
                self.commit()
        
        return 0
 
    def GetDropFKeyStr(self, fkey_name, table_name):
        if fkey_name == None or fkey_name == '':
            return ''
        
        if table_name == None or table_name == '':
            return ''
        
        return "ALTER TABLE " + table_name + " DROP CONSTRAINT if exists " + fkey_name + ";\n"
    
    def DropView_ByName(self, table_name):
        "drop view of table"
        if self.connected != True:
            self.log.error("Connect is not opened.")
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
       
        if self.execute(sqlcmd, ("%"+table_name +" %", "%"+table_name +",%", "%"+table_name +")%")) == -1:
            return -1
        row = self.fetchone()
        sqlcmd = ''
        while row:
            sqlcmd += self.GetDropViewStr(row[0])
            row = self.fetchone()

        # Drop all views, which reference "table_name"
        if sqlcmd != '':
            if self.execute(sqlcmd, table_name) == -1:
                return -1
            else:
                self.commit()
        
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
        self.execute(sqlcmd, (table_name, table_name,))
        rows = self.fetchall()
        for row in rows:
            col_list.append(row[0])
        return col_list
    
    def GetDropViewStr(self, view_name):        
        if view_name == None or view_name == '':
            return ''
        
        return "DROP VIEW " + view_name + ";\n"  
         
    def Get_Version(self):
#       '取得当前制做的数据版本号.'
#        # srv_path2 ??: host='172.26.179.79' dbname='C_0_1.5.6_0037_0018' user='postgres' password='pset123456'
        dbname  = self.srv_path.split(' ')[1]
        version = dbname[dbname.find("'")+1 : dbname.rfind("'")]
        return version

    def RecreateCursor(self, my_cursor):
        '''游标重定'''
        self.pgcur = self.conn.cursor(my_cursor)

    def fetchmany(self, num):
        return self.pgcur.fetchmany(num)

    def get_batch_data(self, sqlcmd, parameters=(), batch_size=BATCH_SIZE):
        row_data = list()
        for rows in self.batch_query(sqlcmd, parameters, batch_size):
            for row in rows:
                if not row:
                    break
                row_data = row
                yield row_data

    def batch_query(self, sql, parameters=(), batch_size=BATCH_SIZE):
        try:
            curs = self.conn.cursor(name='batch')
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
            curs.close()

    def _call_child_thread(self, _min_id, _max_id, sqlcmd, thread_num = 4, record_num = 1500000):
        global max_id, next_id, mutex
        next_id = _min_id
        max_id = _max_id

        mutex = threading.Lock()
        threads = []
        i = 0

        try:
            while i < thread_num:
                threads.append(child_thread(sqlcmd, i, record_num))
                i += 1;
            
            for t in threads:
                t.start()
                                       
            for t in threads:
                t.join()
                
            for t in threads:
                if t.mark == 1:
                    return -1
        except:
            return -1
            
        return 1
    
    def GetRegionLayers(self):   
        # Get layer number. 
        sqlcmd = """
            select ltrim(rtrim(tablename,'_tbl'),'rdb_region_link_layer')
            from pg_tables 
            where tablename like 'rdb_region_link_layer%_tbl'
            order by tablename;
        """
        self.execute(sqlcmd)
        rows = self.fetchall()
        
        layer_list = []
        for row in rows:
            layer_list.append(row[0])
  
        return layer_list
    
class child_thread(threading.Thread):

    def __init__(self, sqlcmd, i, number = 150000):
        threading.Thread.__init__(self)
        self.pg = CDB()
        self._sqlcmd = sqlcmd
        self._number = number + (200000 * i)
        self.mark = 0
        
    def run(self):
        self.pg.connect()
        global max_id, next_id, mutex

        while True:

            mutex.acquire()
            if next_id > max_id :
                mutex.release()
                break

            from_id = next_id
            to_id = 0

            if max_id > (next_id + self._number) :
                to_id = next_id + self._number
                next_id = next_id + self._number + 1
                # print from_id, to_id, self._number, next_id
            else:
                to_id = max_id
                next_id = max_id + 1
                # print from_id, to_id, self._number, next_id
            mutex.release()

            try:
                self.pg.execute(self._sqlcmd, (from_id, to_id))
                self.pg.commit()
            except:
                self.mark = 1

        self.pg.close()
        return 0

    def rtnMark(self):
        return self.mark
