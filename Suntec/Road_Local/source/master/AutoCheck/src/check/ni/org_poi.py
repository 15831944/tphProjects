# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-5-7

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
                 select distinct poi_id from org_poi 
                )          
               
               '''
        poi_id_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = '''
               select count(*) from org_poi           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)          

        return (poi_id_count == rec_count )


class CCheckKind(platform.TestCase.CTestCase):
    ''' 检查cat_code的种类个数是否合理 '''
    def _do(self):
        sqlcmd = '''
               select count(*)
              from 
              (select distinct kind from  org_poi) AS A
               '''
        cat_code_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (cat_code_count > 1) 
    
    
    
class CCheckName(platform.TestCase.CTestCase):
    '''poi name是否为空'''
    def _do(self):
        sqlcmd = '''
               select count(*) from org_poi
               where name is null           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count == 0)    
    
class CCheckLanguage(platform.TestCase.CTestCase):
    '''POI language是否只有ENG 和 CHI'''
    def _do(self):
        sqlcmd = '''
               select count(*) from org_poi
               where not (language = '1' and language = '3')           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count == 0)  
    
      
    
       
           
