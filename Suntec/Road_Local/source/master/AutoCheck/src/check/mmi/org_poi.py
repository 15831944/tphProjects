# -*- coding: UTF8 -*-
'''
Created on 2015-4-22

@author: wushengbing
'''
import platform.TestCase
        

class CCheckPoiRec(platform.TestCase.CTestCase):
    '''检查POI表单记录是否为空'''
    def _do(self):
        sqlcmd = '''
               select count(*) from org_poi_point           
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
                 select distinct uid from org_poi_point 
                )          
               
               '''
        poi_id_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = '''
               select count(*) from org_poi_point           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)          

        return (poi_id_count == rec_count )


class CCheckCat_code(platform.TestCase.CTestCase):
    ''' 检查cat_code的种类个数是否合理 '''
    def _do(self):
        sqlcmd = '''
               select count(*)
              from 
              (select distinct cat_code from  org_poi_point) AS A
               '''
        cat_code_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (cat_code_count > 1) 
    
    
    
class CCheckStd_name(platform.TestCase.CTestCase):
    '''std_name是否为空'''
    def _do(self):
        sqlcmd = '''
               select count(*) from org_poi_point
               where std_name is null           
               '''
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (rec_count == 0)    
    
    
    
       
           
