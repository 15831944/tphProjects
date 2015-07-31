# -*- coding: cp936 -*-
'''
Created on 2012-3-14

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

class rdb_kind_code(ItemBase):
    '''
    kind_code类:处理各种种别人显示物的显示等级，显示层数等等。
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Kind_code')
        
    def Do_CreateTable(self):
        return self.CreateTable2('temp_kind_code')
    
    def Do_CreateFunction(self):
        return 0
    
    def Do(self):
        try :    
            stratify = rdb_common.Stratify()
            path = rdb_common.GetPath('kindcode')
            stratify.Open(path)
            kind_code_list = stratify.GetAllStratify() 
            sqlcmd = """
                     INSERT INTO temp_kind_code(
                                        kind_code
                                      , priority
                                      , name_type
                                      , kiwi_name
                                      , emg_code
                                      , emg_name
                                      , ni_code
                                      , ni_name
                                      , low_level
                                      , high_level
                                      , road_display_class)
                            VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);
                     """
            for kind_code_recod in kind_code_list:
                if self.pg.execute2(sqlcmd, kind_code_recod) == -1:
                    rdb_log.log(self.ItemName, 'Fail to insert record into temp_kind_code', 'error')
                    break
            self.pg.commit2()
        except:
            rdb_log.log(self.ItemName, 'Fail make temp_kind_code', 'error')
            return -1
        
        return 0
    
    def Check(self):
        return 0
    
    