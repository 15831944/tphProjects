# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-6-18

@author: wushengbing
'''
import platform.TestCase
        

class CCheckPoiRec(platform.TestCase.CTestCase):
    '''检查POI表单记录是否为空'''
    def _do(self):
        sqlcmd = '''
               select count(*) from org_poi           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count > 0)

class CCheckPoiNumber(platform.TestCase.CTestCase):
    '''检查poi_id是否有重复'''
    def _do(self):
        sqlcmd = '''
               select count(*) 
               from 
               (
                 select poi_id from org_poi
                 group by poi_id having count(*)>1 
                ) as a        
               
               '''               
           
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)          

        return ( rec_count == 0 )


class CCheckPoiSub_cat(platform.TestCase.CTestCase):
    ''' 检查cat_code的种类个数是否合理 '''
    def _do(self):
        sqlcmd = '''
               select count(*)
              from 
              (select distinct sub_cat from  org_poi) AS A
               '''
        cat_code_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (cat_code_count > 1) 
    
    
    
class CCheckPoiName(platform.TestCase.CTestCase):
    '''poi name是否为空'''
    def _do(self):
        sqlcmd = '''
               select count(*) from org_poi
               where offi_name is null           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count == 0)    
    
 