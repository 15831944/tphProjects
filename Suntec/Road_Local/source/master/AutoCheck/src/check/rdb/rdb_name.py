# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-5-11

@author: wushengbing
'''
import platform.TestCase
        

class CCheckRdbNameRec(platform.TestCase.CTestCase):
    '''检查rdb_name表单记录是否为空'''
    def _do(self):
        sqlcmd = '''
               select count(*) from rdb_name           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count > 0)

class CCheckNameIdRec(platform.TestCase.CTestCase):
    '''检查name_id是否有重复'''
    def _do(self):
        sqlcmd = '''
               select count(*) 
               from 
               (
                 select distinct name_id from rdb_name  
                ) as a         
               
               '''
        name_id_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = '''
               select count(*) from  rdb_name           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)          

        return (name_id_count == rec_count )


class CCheckNameIdUnique(platform.TestCase.CTestCase):
    '''检查name_id和name是否唯一对应'''
    def _do(self):
        sqlcmd = '''
               select count(*)
              from 
              (select distinct name from  rdb_name) as a
               '''
        name_count = self.pg.getOnlyQueryResult(sqlcmd) 
        
        sqlcmd = '''
               select count(*)
              from 
              (select distinct name_id from  rdb_name) as b
               '''
        name_id_count = self.pg.getOnlyQueryResult(sqlcmd)       

        return (name_count==name_id_count) 
    
    
    
