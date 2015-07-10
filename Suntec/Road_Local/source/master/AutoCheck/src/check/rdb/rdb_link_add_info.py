# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckTableStruct(platform.TestCase.CTestCase):
    def _do(self):
        return self.pg.IsExistTable('rdb_link_add_info')

class CCheckLinkIDEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.link_id)
                    from rdb_link_add_info as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                    where b.link_id is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckLinkIDUnique(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(link_id)
                    from
                    (
                        select link_id, count(link_id) as cnt
                        from rdb_link_add_info
                        group by link_id
                    )as a
                    where cnt > 1
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.link_id)
                    from rdb_link_add_info as a
                    left join rdb_link as b
                    on a.link_id = b.link_id
                    where a.link_id_t != b.link_id_t
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckStructCodeEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(link_id)
                    from rdb_link_add_info
                    where struct_code not in (0,1,2,3,4,5,6,7,8)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckStructCodeComplete(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.struct_code)
                    from
                    (select unnest(ARRAY[0,1,3]) as struct_code) as a
                    left join
                    (select distinct struct_code from rdb_link_add_info) as b
                    on a.struct_code = b.struct_code
                    where b.struct_code is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckStructCodeComplete_Nostra(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.struct_code)
                    from
                    (select unnest(ARRAY[1,2,5]) as struct_code) as a
                    left join
                    (select distinct struct_code from rdb_link_add_info) as b
                    on a.struct_code = b.struct_code
                    where b.struct_code is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckStructCodeComplete_MMI(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.struct_code)
                    from
                    (select unnest(ARRAY[1,2,4]) as struct_code) as a
                    left join
                    (select distinct struct_code from rdb_link_add_info) as b
                    on a.struct_code = b.struct_code
                    where b.struct_code is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckStructCodeComplete_MSM(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.struct_code)
                    from
                    (select unnest(ARRAY[1]) as struct_code) as a
                    left join
                    (select distinct struct_code from rdb_link_add_info) as b
                    on a.struct_code = b.struct_code
                    where b.struct_code is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
        
class CCheckStructCodeComplete_RDF(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.struct_code)
                    from
                    (select unnest(ARRAY[1,2]) as struct_code) as a
                    left join
                    (select distinct struct_code from rdb_link_add_info) as b
                    on a.struct_code = b.struct_code
                    where b.struct_code is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
        
class CCheckShortcutCodeEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(link_id)
                    from rdb_link_add_info
                    where shortcut_code not in (0,1,2,3)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckParkingFlagEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(link_id)
                    from rdb_link_add_info
                    where parking_flag not in (0,1,2,3)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckEtcLaneFlagEffective(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(link_id)
                    from rdb_link_add_info
                    where etc_lane_flag not in (0,1,2)
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckEtcLaneFlagComplete(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.etc_lane_flag)
                    from
                    (select unnest(ARRAY[0,1,2]) as etc_lane_flag) as a
                    left join
                    (select distinct etc_lane_flag from rdb_link_add_info) as b
                    on a.etc_lane_flag = b.etc_lane_flag
                    where b.etc_lane_flag is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckEtcLaneFlagComplete_RDF(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.etc_lane_flag)
                    from
                    (select unnest(ARRAY[0,1]) as etc_lane_flag) as a
                    left join
                    (select distinct etc_lane_flag from rdb_link_add_info) as b
                    on a.etc_lane_flag = b.etc_lane_flag
                    where b.etc_lane_flag is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckEtcLaneFlagComplete_RDF_IDN(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(a.etc_lane_flag)
                    from
                    (select unnest(ARRAY[0,1]) as etc_lane_flag) as a
                    left join
                    (select distinct etc_lane_flag from rdb_link_add_info) as b
                    on a.etc_lane_flag = b.etc_lane_flag
                    where b.etc_lane_flag is null
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0 or rec_cnt == 1)
        
class CCheckEtcLaneFlagComplete_Nostra(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    select count(distinct etc_lane_flag) from rdb_link_add_info where etc_lane_flag <> 0;
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
   
class CCheckPavedFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info
                        where (path_extra_info & 1) = 1 
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join link_tbl b
            on c.old_link_id = b.link_id
            where b.link_id is not null and b.paved <> 0;
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckUturnFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info
                        where ((path_extra_info >> 1) & 1) = 1  
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join link_tbl b
            on c.old_link_id = b.link_id
            where b.link_id is not null and b.uturn <> 1;
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)  
    
class CCheckIPDFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info
                        where ((path_extra_info >> 2) & 1) = 1  
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join link_tbl b
            on c.old_link_id = b.link_id
            where b.link_id is not null and b.ipd = 0;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheck4WDFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info
                        where ((path_extra_info >> 4) & 1) = 1  
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join link_tbl b
            on c.old_link_id = b.link_id
            where b.link_id is not null and b.display_class != 19;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
        
class CCheckUrbanFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info
                        where ((path_extra_info >> 3) & 1) = 1  
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join link_tbl b
            on c.old_link_id = b.link_id
            where b.link_id is not null and b.urban = 0;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckAddInfo2Flag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info
                        where ((path_extra_info >> 15) & 1) = 1 
            ) a
            left join rdb_link_add_info2 b
            on a.link_id = b.link_id
            where b.link_id is null or b.link_add_info2 is null;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     
        
class CCheckRodizioFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info2
                        where (link_add_info2 & 1) = 1  
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join link_tbl b
            on c.old_link_id = b.link_id
            where b.link_id is not null and b.rodizio = 0;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckERPFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info2
                        where ((link_add_info2 >> 1) & 3) = 1  
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join link_tbl b
            on c.old_link_id = b.link_id
            where b.link_id is not null and b.erp = 0;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 

class CCheckSafetyZoneFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info2
                        where ((link_add_info2 >> 3) & 1) = 1  
            ) a
            left join rdb_guideinfo_safety_zone b
            on a.link_id = b.link_id
            where b.link_id is null;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckSafetyAlertFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                        select *
                        from rdb_link_add_info2
                        where ((link_add_info2 >> 5) & 1) = 1  
            ) a
            left join rdb_guideinfo_safetyalert b
            on a.link_id = b.link_id
            where b.link_id is null;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckStopSignFlag(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from (
                select *,((path_extra_info >> 6) & 3) as pos_flag
                from rdb_link_add_info
                where ((path_extra_info >> 6) & 3) != 0  
            ) a
            left join rdb_tile_link c
            on a.link_id = c.tile_link_id
            left join (
                 select link_id
                    ,case when 2 = any(pos_flag_array) and 3 = any(pos_flag_array) then 1
                    else pos_flag_array[1]
                    end as pos_type
                from (
                    select link_id,array_agg(pos_flag) as pos_flag_array 
                    from (
                        select distinct link_id,pos_flag
                        from stopsign_tbl
                    ) a
                    group by link_id 
                ) b
            ) b
            on c.old_link_id = b.link_id
            where b.link_id is null or a.pos_flag != b.pos_type;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    

class CCheckAddInfo2Valid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from rdb_link_add_info2 a
            left join rdb_link_add_info2 b
            on a.link_id = b.link_id
            where b.link_id is null;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)                      