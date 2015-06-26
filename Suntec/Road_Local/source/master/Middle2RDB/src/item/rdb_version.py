# -*- coding: cp936 -*-
'''
Created on 2014-12-15

@author: zhaojie
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

class rdb_version(ItemBase):
    '''
    rdb_slope
    '''
    def __init__(self):
        '''
        Constructor
        '''        
        ItemBase.__init__(self,'slope','','','','')
    
    def Do(self):
        '''
        Make rdb_version info 
        '''
        sqlcmd = '''
                drop table if exists rdb_version;
                CREATE TABLE rdb_version
                (
                  version_no varchar(512)
                );
                '''
        rdb_log.log(self.ItemName, 'Now create rdb_version...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        db_version = rdb_common.GetPath('version')
        
        sqlcmd = '''
                insert into rdb_version
                values(%s);
                '''
        rdb_log.log(self.ItemName, 'Now it is inserting to rdb_version...', 'info')
        db_version = "\'" + db_version + "\'"
        if self.pg.execute2(sqlcmd % (str(db_version),)) == -1:
            return -1
        else:
            self.pg.commit2()    

        return 0
    