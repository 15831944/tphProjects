# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-6-30

@author: wushengbing
'''
import platform.TestCase



class CCheckTable(platform.TestCase.CTestCase):
    
    def _do(self):
        return self.pg.IsExistTable('org_cross_name')
   
class CCheckTableCount(platform.TestCase.CTestCase):
   
    def _do(self):
        sqlcmd='''
                select count(1) from org_cross_name
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0


class CCheckTnodeFromnode(platform.TestCase.CTestCase):
    '''检查Tnode  Fromnode 是否在同一条link上'''
    def _do(self):
        
        sqlcmd = '''
            select count(*) from 
            (
                select * from org_cross_name as a
                left join org_road as r
                on  ( r.snodeno = a.tonodeno and r.enodeno = a.fromnodeno
                      or 
                      r.enodeno = a.tonodeno and r.snodeno = a.fromnodeno
                    )
                    and r.meshcode = a.meshcode 

            ) as a
            where linkno is null
            
        '''
        return self.pg.getOnlyQueryResult(sqlcmd) == 0
    


    
    