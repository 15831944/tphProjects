# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-6-29

@author: wushengbing
'''
import platform.TestCase


class CCheckTable(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_dest_guide')

   
class CCheckTableCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_dest_guide
               '''
        return self.pg.getOnlyQueryResult(sqlcmd) > 0

    
class CCheckTnodeInlink(platform.TestCase.CTestCase):
    '''检查tnode 是否在 inlink上'''
    def _do(self):
        sqlcmd = '''
            select count(*) 
            from  org_dest_guide as a
            join org_road as r
            on r.meshcode = a.meshcode and r.linkno = a.stotlinkno 
               and not (a.tnodeno = r.snodeno or a.tnodeno = r.enodeno)  
         '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
 
    
class CCheckTnodeOutlink(platform.TestCase.CTestCase):
    '''检查 tnode 是否在 outlink上'''
    def _do(self):
        sqlcmd = '''
            select count(*) 
            from  org_dest_guide as a
            join org_road as r
            on r.meshcode = a.meshcode and r.linkno = a.ttoelinkno 
               and not (a.tnodeno = r.snodeno or a.tnodeno = r.enodeno)  
         '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0  
    
  
    