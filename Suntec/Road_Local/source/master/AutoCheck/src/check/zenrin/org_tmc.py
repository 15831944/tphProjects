# -*- coding: UTF8 -*-


import platform.TestCase

class CCheckorg_TableExists_rdstmc(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('org_rdstmc')
    
class CCheckorg_TableExists_locationtable(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('gewi_locationtable')

class CCheckorg_LinkValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from org_rdstmc a
                left join (
                    
                    select gid, meshcode, linkno
                    from org_road_croad
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_ferry
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_general1 
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_general2 
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_general3 
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_hiway1 
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_hiway2
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_motorcycle
                    
                    union
                    
                    select gid, meshcode, linkno
                    from org_road_thin
                ) b 
                on a.meshcode = b.meshcode and a.roadno = b.linkno
                where b.gid is null;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_SnodenoValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from org_rdstmc a
                left join org_node b
                on a.meshcode = b.meshcode and a.snodeno = b.nodeno
                where b.gid is null;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_EnodenoValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from org_rdstmc a
                left join org_node b
                on a.meshcode = b.meshcode and a.enodeno = b.nodeno
                where b.gid is null;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_LinkDirValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_rdstmc where link_dire not in ('+','-');
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_LocDirValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_rdstmc where loc_dire not in ('+','-','P','N');
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_LocCodeValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(a.gid)
                from org_rdstmc a
                left join gewi_locationtable b
                on a.loc_code = cast(b.location_code as integer)
                where b.location_code is null;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_SubType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from gewi_locationtable 
                where substring(sub_type,1,1) not in ('P','L','A');
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_LinkDupli(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select loc_code,loc_dire,meshcode,roadno,link_dire,count(*)
                from (
                    select meshcode,roadno,link_dire,loc_code,loc_dire 
                    from org_rdstmc
                ) a group by loc_code,loc_dire,meshcode,roadno,link_dire
                having count(*) > 1;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_RoadName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select * from gewi_locationtable where
             (road_name is not null and croad_name is null)
             or 
             (road_name is null and croad_name is not null);
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_FirstName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select * from gewi_locationtable where
            (first_name is not null and cfirst_name is null)
            or 
            (first_name is null and cfirst_name is not null);
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_SecondName(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select * from gewi_locationtable where
            (second_name is null and csecond_name is not null);
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_AreaReference(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
           select count(*) from (
                select distinct ltrim(area_reference,'0') as loc_code
                from gewi_locationtable where area_reference is not null
            ) a
            left join gewi_locationtable b
            on a.loc_code = b.location_code
            where substring(b.sub_type,1,1) != 'A';
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_LinearReference(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                select distinct ltrim(linear_reference,'0') as loc_code
                from gewi_locationtable where area_reference is not null
            ) a
            left join gewi_locationtable b
            on a.loc_code = b.location_code
            where substring(b.sub_type,1,1) != 'L';
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_NegOffset(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                select distinct ltrim(negative_offset,'0') as loc_code
                from gewi_locationtable where negative_offset is not null
            ) a
            left join gewi_locationtable b
            on a.loc_code = b.location_code
            where b.location_code is null;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckorg_PosOffset(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """      
            select count(*) from (
                select distinct ltrim(positive_offset,'0') as loc_code
                from gewi_locationtable where positive_offset is not null
            ) a
            left join gewi_locationtable b
            on a.loc_code = b.location_code
            where b.location_code is null;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)


