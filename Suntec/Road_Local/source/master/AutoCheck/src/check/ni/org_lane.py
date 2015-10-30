# coding:utf-8

import platform.TestCase

class CCheckTable(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_ln')

class CCheckTableCount(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_ln
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)>0

class CCheckInlinkOutlinkEqual(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_ln where inlinkid=outlinkid
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckArrowinfo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_ln where arrowinfo not in ('1','2','3','4','5','6','7','8','9','a','b','c')
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckLaneinfoBuslaneAllNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd='''
                select count(1) from org_ln where laneinfo='0000000000000000' and buslane='0000000000000000'
               '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckLaneinfoBuslanelength(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln where length(laneinfo)<>16 or length(buslane)<>16
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckLaneinfoBuslaneRepeat(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from
                (
                    select gid,unnest(regexp_split_to_array(substr(laneinfo,1,lanenum::int),'')) as li,
                    unnest(regexp_split_to_array(substr(buslane,1,lanenum::int),'')) as bl 
                    from org_ln
                )
                a
                where li='1' and bl='1'
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
        
class CCheckInlinkidNodeid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln a join org_r b on a.inlinkid=b.id where a.nodeid not in (b.snodeid,b.enodeid)
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckInlinkOutlinkJoin(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r'''
                select count(1) from org_ln a join org_n b on a.nodeid=b.id 
                where passlid='' and not regexp_split_to_array(node_lid, E'\\|+')@>array[outlinkid::text]
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckLaneinfo(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln where substr(laneinfo,lanenum::int+1)<>repeat('0',16-lanenum::int)
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckBuslane(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln where substr(buslane,lanenum::int+1)<>repeat('0',16-lanenum::int)
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckLanenumlr(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln where lanenuml::int<0 or lanenumr::int<0
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln where inlinkid='' or nodeid='' or
                outlinkid='' or lanenum='' or
                laneinfo='' or buslane='' or
                lanenuml='' or lanenumr='' or arrowinfo=''
                '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
class CCheckLinkDuplicate(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r'''
                select count(1) from
                (
                    select gid from 
                    (
                        select * from 
                        (
                            select gid,unnest(array_prepend(inlinkid::text,array_append(regexp_split_to_array(passlid,E'\\|+'),outlinkid::text))) as linkid from org_ln 
                        ) a where linkid<>''
                    ) b
                    group by gid,linkid
                    having count(1)>1
                ) c
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckPasslidPasslid2(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln where passlid='' and passlid2<>''
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckLanenum(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(1) from org_ln where lanenum::int<1
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0
    
'''class CCheckInlinkPasslinkOutlink(platform.TestCase.CTestCase):
    def _do(self):'''
