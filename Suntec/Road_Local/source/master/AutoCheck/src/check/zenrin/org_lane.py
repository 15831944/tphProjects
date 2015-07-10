# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-6-18

@author: wushengbing
'''
import platform.TestCase

class CCheckTable1(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_lane')
   
class CCheckTableCount1(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_lane
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0


class CCheckInlinkOutlinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_lane 
                where snodeno in (ysnodeno,yenodeno) 
                      and enodeno in (ysnodeno,yenodeno) 
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0


class CCheckLanenum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_lane where lanesu < 1
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0


class CCheckTable2(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_hwlane')

class CCheckTableCount2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_hwlane
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0 
       
class CCheckTnodeInInlink(platform.TestCase.CTestCase):
    '''检查tnode是否在inlink上'''
    def _do(self):
        sqlcmd = '''
            select count(*) 
            from org_hwlane as a
            join org_road as r
            on r.meshcode = a.meshcode and r.linkno = a.sroadno 
               and not (a.tnodeno = r.snodeno or a.tnodeno = r.enodeno) 
         '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckTnodeInOutlink(platform.TestCase.CTestCase):
    '''检查tnode是否在outlink上'''
    def _do(self):
        sqlcmd = '''
            select count(*) 
            from org_hwlane as a
            join org_road as r
            on r.meshcode = a.meshcode and r.linkno = a.eroadno 
               and not (a.tnodeno = r.snodeno or a.tnodeno = r.enodeno) 
         '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0        

     
class CCheckTnodeInOutlink1(platform.TestCase.CTestCase):
    '''检查分叉路口 tnode 是否在outlink上'''
    def _do(self):
        sqlcmd = ''' 
        select count(*) from
        (
            select meshcode,sroadno,unnest(array_agg(eroadno)) as eroadno ,
                   unnest(array_agg(tnodeno)) as tnodeno 
            from org_hwlane a
            join
            (
                select meshcode,sroadno 
                from org_hwlane a 
                join
                (
                    select meshcode,sroadno 
                    from org_hwlane 
                    group by meshcode,sroadno having count(1)=2
                ) b
                using(meshcode,sroadno)
                group by meshcode,sroadno having count(distinct(slaneinf))=1
            ) b
            using(meshcode,sroadno)
            group by meshcode,sroadno,slaneinf
        ) as aa
        join org_road as r
        on r.meshcode = aa.meshcode and r.linkno = aa.eroadno 
           and not (aa.tnodeno = r.snodeno or aa.tnodeno = r.enodeno)
        '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckTnodeInOutlink2(platform.TestCase.CTestCase):
    '''检查合并路口 tnode 是否在outlink上'''
    def _do(self):
        sqlcmd = '''
        select count(*) from
        (       
             select meshcode,eroadno,elaneinf,unnest(array_agg(sroadno)) as sroadno,
                    unnest(array_agg(tnodeno)) as tnodeno 
            from org_hwlane a 
            join
            (
                select meshcode,eroadno 
                from org_hwlane a 
                join
                (
                    select meshcode,eroadno 
                    from org_hwlane 
                    group by meshcode,eroadno having count(1)=2
                ) b
                using(meshcode,eroadno)
                group by meshcode,eroadno having count(distinct(elaneinf))=1
            ) b
            using(meshcode,eroadno)
            group by meshcode,eroadno,elaneinf
      ) as aa
      join org_road as r
      on r.meshcode = aa.meshcode and r.linkno = aa.eroadno 
         and not (aa.tnodeno = r.snodeno or aa.tnodeno = r.enodeno)      
     '''  
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
    