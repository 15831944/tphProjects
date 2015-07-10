# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
import common.common_func
import common.database

class comp_base(object):
    '''
    classdocs
    '''

    def __init__(self, ItemName='ItemBase'):
        '''
        Constructor
        '''
        self.pg = common.database.pg_client.instance()
        self.log = common.log.common_log.instance().logger(ItemName)
        self.ItemName = ItemName

    def Make(self):
        try:
            self.StartMake()

            self.pg.connect2()

            bDoResult = self._DoCreateTable()
            bDoResult = self._DoCreateFunction()
            bDoResult = self._Do()
            bDoResult = self._DoCreateIndex()

            self.pg.close2()
            self.EndMake()

            return bDoResult
        except:
            self.log.exception('Fail to make ' + self.ItemName + '.')
            self.pg.closeall()
            raise

    def Check(self):
        try:
            self.StartCheck()

            self.pg.connect2()

            self._DoCheckValues()
            self._DoCheckNumber()
            self._DoContraints()
            self._DoCheckLogic()

            self.pg.close2()
            self.EndCheck()

            return 0
        except:
            self.log.exception('Fail to check ' + self.ItemName + '.')
            self.pg.closeall()
            return 1

    def StartMake(self):
        self.log.info('Start make ' + self.ItemName + '.')

        return 0

    def EndMake(self):
        self.log.info('End make ' + self.ItemName + '.')
        return 0

    def StartCheck(self):
        self.log.info('Start check ' + self.ItemName + '.')
        return 0

    def EndCheck(self):
        self.log.info('End check ' + self.ItemName + '.')
        return 0

    def _DoCreateTable(self):
        '创建表.'
        return 0
    def _DoCreateFunction(self):
        '创建相关数据库函数.'
        return 0

    def _Do(self):
        return 0

    def _DoCreateIndex(self):
        '创建相关表索引.'
        return 0

    def _DoCheckValues(self):
        'check字段值'
        return 0

    def _DoCheckNumber(self):
        'check条目'
        return 0

    def _DoContraints(self):
        '添加外键'
        return 0

    def _DoCheckLogic(self):
        '检查数据逻辑'
        return 0

    def isCountry(self, country_name):
        if self.pg.GetCountry().lower() == country_name:
            return 1
        else:
            return 0
        
    def CreateTable1(self, table_name):
        if self.pg.CreateTable1_ByName(table_name) == -1:
            self.log.error('Create table ' + table_name + ' failed.')
            return -1
        else :
            return 0

    def CreateFunction1(self, function_name):
        if self.pg.CreateFunction1_ByName(function_name) == -1:
            self.log.error('Create function ' + function_name + ' failed.')
            return -1

        return 0

    def CreateIndex1(self, index_name):
        if self.pg.CreateIndex1_ByName(index_name) == -1:
            self.log.error('Create index ' + index_name + ' failed.')
            return -1
        return 0

    def CreateTable2(self, table_name):
        if self.pg.CreateTable2_ByName(table_name) == -1:
            self.log.error('Create table ' + table_name + ' failed.')
            return -1
        else :
            return 0

    def CreateView2(self, table_name):
        if self.pg.CreateView2_ByName(table_name) == -1:
            self.log.error('Create view ' + table_name + ' failed.')
            return -1
        else :
            return 0

    def CreateFunction2(self, function_name):
        if self.pg.CreateFunction2_ByName(function_name) == -1:
            self.log.error('Create function ' + function_name + ' failed.')
            return -1

        return 0

    def CreateIndex2(self, index_name):
        if self.pg.CreateIndex2_ByName(index_name) == -1:
            self.log.error('Create index ' + index_name + ' failed.')
            return -1
        return 0

    def item_exit(self, param):
        common.common_func.proj_exit(param)
        return 0

    def get_batch_data(self, sqlcmd, parameters=(), batch_size=common.database.BATCH_SIZE):
        row_data = list()
        for rows in self.pg.batch_query2(sqlcmd, parameters, batch_size):
            for row in rows:
                if not row:
                    break
                row_data = row
                yield row_data
    
    def GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is error! %s;',sqlcmd)

    def _call_child_thread(self, _min_id, _max_id, sqlcmd, thread_num=4, record_num=1500000):
        global max_id, next_id, mutex
        next_id = _min_id
        max_id = _max_id

        # 创建线程
        mutex = threading.Lock()
        threads = []
        i = 0

        # 启动线程
        while i < thread_num:
            threads.append(child_thread(sqlcmd, i, record_num))
            i += 1;

        # 启动线程
        for t in threads:
            t.start()

        # 等待子线程结束
        for t in threads:
            t.join()

        return 0

#####################################################################################
# 线程类
#####################################################################################
import threading
class child_thread(threading.Thread):
    def __init__(self, sqlcmd, i, number=1500000):
        threading.Thread.__init__(self)
        self.pg = common.database.pg_client()
        self._sqlcmd = sqlcmd
        self._number = number + (200000 * i)

    def run(self):
        self.pg.connect2()
        global max_id, next_id, mutex

        while True:
            # 加锁
            mutex.acquire()
            if next_id > max_id :
                mutex.release()
                break

            from_id = next_id
            to_id = 0

            if max_id > (next_id + self._number) :
                to_id = next_id + self._number
                next_id = next_id + self._number + 1
                print from_id, to_id, self._number, next_id
            else:
                to_id = max_id
                next_id = max_id + 1
                print from_id, to_id, self._number, next_id
            mutex.release()

            if self.pg.execute2(self._sqlcmd, (from_id, to_id)) == -1:
				self.pg.close2()
				return -1

            self.pg.commit2()

        self.pg.close2()
        return 0
