# -*- coding: UTF8 -*-
'''
Created on 2013-12-2

@author: yuanrui
'''

import platform.TestCase

class CCheckRouteIDDuplication(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*) from (
                    select routeid from org_l_tran 
                    group by routeid having count(*) > 1
                ) a;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)

class CCheckFieldValid_rdlnclass(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where rdlnclass not in (11,12,13,14,21,22,23,24,26,27,28,41,42,43,44,46,51,61,62,63,71,72)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_oneway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where (oneway not in ('FT','TF') and oneway is not null)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_tollway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where tollway not in ('Y','N')
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_fm(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where fm not in (0,5,14,16,17)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_level(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where level not in (1,2,3,4,5)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_access(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where c_access not in ('Y','N') and c_access is not null
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_fw(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where fw not in (0,1,2,3,4,5,6,7,8,9,10,11,12,21)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_z_level(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where z_start not in (-1,0,1,2,3)
                or z_end not in (-1,0,1,2,3)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_admin_code(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where prov_code is null
                or amp_code is null
                or tam_code is null
                or postcode is null
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckFieldValid_rdlnwidth(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where rdlnwidth = 0
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckFieldValid_rdlnlane(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where rdlnlane = 0
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
        
class CCheckExpressway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where rdlnclass in (11,12,13,14) and level < 3
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckAccess(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from org_l_tran
                where c_access is not null and level not in (3,4,5)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckOverlay(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) 
            from (   
                select a.routeid as link1,b.routeid as link2,a.the_geom as geom1,b.the_geom as geom2 
                from (
                    SELECT routeid,the_geom,ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length
                    from org_l_tran 
                ) a
                left join (
                    SELECT routeid,the_geom,ST_Length_Spheroid(the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length
                    from org_l_tran
                ) b
                on a.routeid <> b.routeid and a.length = b.length 
                where b.routeid is not null
            ) a where st_equals(geom1,geom2)
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)

class CCheckCircle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from temp_topo_link a
            inner join temp_topo_link b
            on (a.start_node_id  = b.end_node_id and a.end_node_id = b.start_node_id)
            or (a.start_node_id = b.start_node_id and a.end_node_id = b.end_node_id)
            and a.routeid <> b.routeid
                ;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)
    
class CCheckFW(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_l_tran 
            where fw = 6 and oneway is null;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)  
    
class CCheckRTIC(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from org_rtic_link a
            left join org_l_tran b
            on cast(a.source_id as double precision) = b.routeid 
            where b.routeid is null;
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)  

class CCheckRTIC_linkdir(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            SELECT count(*)
              FROM org_rtic_link 
              where flag_u != '0' and dir_u = '0' 
              or flag_d != '0' and dir_d = '0';
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)  

class CCheckRTIC_kind(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            SELECT count(*)
              FROM org_rtic_link 
              where flag_u != '0' and kind_u = '0' 
              or flag_d != '0' and kind_d = '0';
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0) 

class CCheckRTIC_middle(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            SELECT count(*)
              FROM org_rtic_link 
              where flag_u != '0' and middle_u = '0' 
              or flag_d != '0' and middle_d = '0';
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0) 
                
class CCheckSPEED(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
              SELECT COUNT(*) FROM org_l_tran 
              WHERE SPEED > 160 OR SPEED < 0 
              OR SPD_LIMIT < 0 OR SPD_LIMIT > 160
                """
        reg_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (reg_count == 0)      