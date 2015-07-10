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
    
class CCheckOutlinkidPasslid(platform.TestCase.CTestCase):
    #检查outlink和passlink最后一条是否相连
    def _do(self):
        sqlcmd = r'''
                select count(1) from 
                (
                    select passlid_arr[array_upper(passlid_arr,1)] as linkid,outlinkid from 
                    (
                        select regexp_split_to_array(passlid,E'\\|+') as passlid_arr,outlinkid from org_ln where passlid<>''
                    ) a
                ) a
                join org_r b
                on a.linkid=b.id
                join org_r c
                on a.outlinkid=c.id
                where geometrytype(st_linemerge(st_collect(b.the_geom,c.the_Geom)))<>'LINESTRING'
                 '''
        return self.pg.getOnlyQueryResult(sqlcmd)==0

class CCheckInlinkPasslinkOutlink(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = r'''
                select gid from 
                (
                    select a.gid,geometrytype(st_linemerge(st_collect(the_geom))) from
                    (
                        select gid,unnest(array_prepend(inlinkid::text,array_append(regexp_split_to_array(passlid,E'\\|+'),outlinkid::text))) as linkid from org_ln --where gid<100000
                    ) a join org_r b on a.linkid=b.id and a.linkid<>''
                    group by a.gid
                ) b
                where geometrytype<>'LINESTRING'
                 '''
        self.pg.execute(sqlcmd)
        gid_list=self.pg.fetchall()
        for gid in gid_list:
            sqlcmd='''
                select inlinkid,passlid,outlinkid from org_ln where gid=%d
                   ''' % gid[0]
            self.pg.execute(sqlcmd)
            inlinkid,passlid,outlinkid=self.pg.fetchone()
            passlid_arr=passlid.split('|')
            link_arr=[inlinkid]+passlid_arr+[outlinkid]
            if len(link_arr)<>len(set(link_arr)):
                continue
            passlid0=passlid_arr[0]
            sqlcmd_1='''
                select case when geometrytype(st_linemerge(st_collect(a.the_geom,b.the_geom)))='LINESTRING'
                            then case when st_equals(b.snode_geom,a.snode_geom) or st_equals(b.snode_geom,a.enode_geom) then b.enodeid
                                      else b.snodeid end
                            else 'wrong' end  
                from 
                (
                    select 1 as id,a.snodeid,a.enodeid,b.the_geom as snode_geom,c.the_geom as enode_geom,a.the_geom from org_r a join org_n b on a.snodeid=b.id
                    join org_n c on a.enodeid=c.id where a.id='%s'
                ) a
                join
                (
                    select 1 as id,a.snodeid,a.enodeid,b.the_geom as snode_geom,a.the_geom from org_r a join org_n b on a.snodeid=b.id
                    where a.id='%s'
                ) b using(id)
                     '''
            node=self.pg.getOnlyQueryResult(sqlcmd_1%(inlinkid,passlid0))
            if node == 'wrong':
                return False
            for idx in range(len(link_arr))[2:]:
                linkid=link_arr[idx]
                sqlcmd_2='''
                    select case when st_equals(b.the_geom,c.the_geom) then a.enodeid
                                when st_equals(b.the_geom,d.the_geom) then a.snodeid
                                else 'wrong' end
                    from org_r a join org_n c on a.snodeid=c.id join org_n d on a.enodeid=d.id
                    ,(select the_geom from org_n where id='%s') b where a.id='%s' 
                         '''
                print gid,node,linkid
                node=self.pg.getOnlyQueryResult(sqlcmd_2%(node,linkid))
                if node=='wrong':
                    sqlcmd_3='''
                        select direction from org_r where id='%s'
                              '''%link_arr[idx-1]
                    if self.pg.getOnlyQueryResult(sqlcmd_3)=='1':
                        continue
                    return False
        return True

