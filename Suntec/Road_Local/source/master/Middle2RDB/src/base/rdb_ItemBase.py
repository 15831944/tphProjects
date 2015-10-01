# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
import common.log
from common import rdb_database
from common import rdb_log
from common import rdb_common

ITEM_EXTEND_FLAG_IDX = {'HWY_FACILITY':0,    # 高速施設
                        'PARKER':      1,    # 停车场
                        'CROSSNAME':   2,    #交叉点名
                        'TOWARDNAME':  3,    # 方面名称
                        'SPOTGUIDE':   4,    # iLLUST图
                        'SIGNPOST':    5,    # 方面看板誘導情報
                        'LANE':        6,    # LANE
                        'ROADSTRUCT':  7,    # 道路構造物誘導情報
                        'BUILDING':    8,    # BUILDING
                        'SIGNAL':      9,    # 信号机
                        'TOLL':        10,   # 收费站
                        'SIGNPOST_UC': 11,   # 北美方面看板
                        'CAUTION':     13,   # Caution
                        'FORCE_GUIDE': 15    # force guide
                        }
class ItemBase(object):
    '''
    classdocs
    '''

    def __init__(self, ItemName = 'ItemBase', s_tbl = None, s_col = None, \
                 d_tbl = None, d_col = None, flg = False, extend_idx = None):
        '''
        Constructor
        '''
        self.pg       = rdb_database.rdb_pg()
        self.log      = common.log.common_log.instance().logger(ItemName)
        self.ItemName = ItemName
        self.src_tbl  = s_tbl
        self.src_col  = s_col
        self.dest_tbl = d_tbl
        self.dest_col = d_col
        self.LinkCheckFlg  = flg         # 是否需要check InLinkId 和  OutLinkId是否相同
        self.NodeExtendIdx = extend_idx  # NodeExtendIdx=None时，不检查
    
    def Make(self):
        try:
            self.StartMake()
            
            self.pg.connect2()
            
            bDoResult = self.Do_CreateTable()
            bDoResult = self.Do_CreateFunction()
            bDoResult = self.Do() 
            bDoResult = self.Do_CreatIndex()
            
            self.pg.close2()
            self.EndMake()
            
            return bDoResult
        except:
            rdb_log.log(self.ItemName, 'Fail to make ' + self.ItemName + '.', 'exception')
            self.pg.closeall()
            self.item_exit(1)
    
    def Check(self):
        try:
            self.pg.connect2()
            self.StartCheck()
            bDoResult = self._DoCheckValues()
            bDoResult = self._DoCheckNumber()
            bDoResult = self._DoContraints()
            bDoResult = self._DoCheckLogic()
            bDoResult = self._DoCheckNodeExtendFlg()
            self.pg.close2()
            self.EndCheck()
            return bDoResult
        except:
            rdb_log.log(self.ItemName, 'Fail to check ' + self.ItemName + '.', 'exception')
            self.pg.closeall()
            #self.item_exit(1)
    
    def StartMake(self):
        rdb_log.log(self.ItemName, 'Start make ' + self.ItemName + '.', 'info')
        
        return 0
    
    def EndMake(self):
        rdb_log.log(self.ItemName, 'End make ' + self.ItemName + '.', 'info')
        return 0
    
    def StartCheck(self):
        rdb_log.log(self.ItemName, 'Start check ' + self.ItemName + '.', 'info')
        return 0
    
    def EndCheck(self):
        rdb_log.log(self.ItemName, 'End check ' + self.ItemName + '.', 'info')
        return 0
    
    def Do_CreateTable(self):
        '创建表.'
        return 0
    def Do_CreateFunction(self):
        '创建相关数据库函数.'
        return 0
    
    def Do(self):
        return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        return 0
    
    def _DoCheckValues(self):
        'check字段值'
        return 0
    
    def _DoCheckNumber(self):
        'check条目'
        
        if (self.src_tbl == None or self.src_tbl == '' or 
            self.src_col == None or self.src_col == '' or
            self.dest_tbl == None or self.dest_tbl == '' or
            self.dest_col == None or self.dest_col == ''):
            return 0
        
        sqlcmd = "SELECT "
        sqlcmd = sqlcmd + "(SELECT count(" + self.src_col +") FROM " + self.src_tbl +") as src_num, "
        sqlcmd = sqlcmd + "(SELECT count(" + self.dest_col +") FROM " + self.dest_tbl +") as desc_num;"
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            src_num  = row[0]
            desc_num = row[1]
            if src_num != desc_num:
                rdb_log.log(self.ItemName, 'Number of ' + self.src_tbl + ': ' + str(src_num) + ','\
                                           'But Number of ' + self.dest_tbl + ': ' + str(desc_num) + '.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Number of ' + self.src_tbl + ' and ' + self.dest_tbl + ': ' + str(src_num) + '.', 'info')
        else:
            rdb_log.log(self.ItemName, "Can't find Number of " + self.src_tbl + ' or ' + self.dest_tbl +'.', 'warning')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        return 0
    
    def _DoCheckLogic(self):
        '检查数据逻辑'
        return 0
    
    def _DoCheckNodeExtendFlg(self):
        '判断node表的Extend_flag标志位是否被置上。'
        if self.NodeExtendIdx == None or self.dest_tbl == None:
            return 0
        
        sqlcmd ="""
                select count(node_id)
                from (
                    SELECT a.node_id
                      FROM rdb_guideinfo_xxx as a
                      left join rdb_node as b
                      on a.node_id = b.node_id
                      where extend_flag & (1 << %s) = 0
                ) as a;
        """
        sqlcmd = sqlcmd.replace('rdb_guideinfo_xxx', self.dest_tbl)
        self.pg.execute2(sqlcmd, (self.NodeExtendIdx,))
        row = self.pg.fetchone2()
        if row:
            if row[0] > 0:
                rdb_log.log(self.ItemName, "Node Extend_Flag has not been set for " + self.dest_tbl + '.', 'warning')
            else:
                rdb_log.log(self.ItemName, "Check Node Extend_Flag for " + self.dest_tbl + '.', 'info')
        return 0
    
    def CreateTable1(self, table_name):
        if self.pg.CreateTable1_ByName(table_name) == -1:
            rdb_log.log(self.ItemName, 'Create table ' + table_name +' failed.', 'error')
            return -1
        else :
            return 0
            
    def CreateFunction1(self, function_name):
        if self.pg.CreateFunction1_ByName(function_name) == -1:
            rdb_log.log(self.ItemName, 'Create function ' + function_name + ' failed.', 'error')
            return -1
        
        return 0
    
    def CreateIndex1(self, index_name):
        if self.pg.CreateIndex1_ByName(index_name) == -1:
            rdb_log.log(self.ItemName, 'Create index ' + index_name + ' failed.', 'error')
            return -1
        return 0
    
    def CreateTable2(self, table_name):
        if self.pg.CreateTable2_ByName(table_name) == -1:
            rdb_log.log(self.ItemName, 'Create table ' + table_name +' failed.', 'error')
            return -1
        else :
            return 0
            
    def CreateFunction2(self, function_name):
        if self.pg.CreateFunction2_ByName(function_name) == -1:
            rdb_log.log(self.ItemName, 'Create function ' + function_name + ' failed.', 'error')
            return -1
        
        return 0
    
    def CreateIndex2(self, index_name):
        if self.pg.CreateIndex2_ByName(index_name) == -1:
            rdb_log.log(self.ItemName, 'Create index ' + index_name + ' failed.', 'error')
            return -1
        return 0
    
    def item_exit(self, param):
        rdb_common.rdb_exit(param)
        return 0
    
    def _call_child_thread(self, _min_id, _max_id, sqlcmd, thread_num = 4, record_num = 1500000):
        global max_id, next_id, mutex
        next_id = _min_id
        max_id  = _max_id
       
        # 创建线程
        mutex   = threading.Lock()  
        threads = []
        i       = 0
        
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
    def __init__(self, sqlcmd, i, number = 1500000):
        threading.Thread.__init__(self)
        self.pg        = rdb_database.rdb_pg()
        self._sqlcmd   = sqlcmd
        self._number   = number + (200000 * i)
        
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
            to_id   = 0
            
            if max_id > (next_id + self._number) :
                to_id   = next_id + self._number
                next_id = next_id + self._number + 1
                #print from_id, to_id, self._number, next_id
            else:
                to_id   = max_id
                next_id = max_id + 1
                #print from_id, to_id, self._number, next_id
            mutex.release()
            
            if self.pg.execute2(self._sqlcmd, (from_id, to_id)) == -1:
                self.pg.close2()
                return -1
            else :
                self.pg.commit2()
        
        self.pg.close2()
        return 0