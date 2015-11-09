# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase
from common.ConfigReader import CConfigReader
import json

#LinkID唯一性
class CCheckLinkIDUnique(platform.TestCase.CTestCase):
    def _do(self):
        
            sqlcmd = """
                select count(*) from (
                    select link_id  from rdb_link group by link_id having count(*) > 1
                ) as a;
                """
        
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False

#LinkID内容合理性
class CCheckLinkIDValid(platform.TestCase.CTestCase):

    def _do(self):
            
        sqlcmd = """

            select link_id as linkid from rdb_link 
                where (link_id >> 32) <> link_id_t

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckLinkCountTile(platform.TestCase.CTestCase):

    def _do(self):
            
        sqlcmd = """
            select * from (
                select  cast(((link_id >> 32) & (-1)) as int),count(*) as count 
                    from rdb_link 
                    group by cast(((link_id >> 32) & (-1)) as int)
            ) a where count > 65535;

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckLinkIDContinous(platform.TestCase.CTestCase):

    def _do(self):        
        sqlcmd = "select count(*) from rdb_link;"
        all_link_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = "select count(*) from id_fund_link where (version = current_database()) and (((tile_id >> 28) & 15) = 14);"
        new_link_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        if (all_link_count == new_link_count):
            sqlcmd = """
                        select count(*)
                        from 
                        (
                            select link_id_t, link_array, generate_series(1,tile_link_count) as seq_nm
                            from
                            (
                                select link_id_t, count(*) as tile_link_count, array_agg((a.link_id & ((1::bigint << 32) - 1))) as link_array
                                from
                                (
                                    select a.link_id_t, b.common_main_link_attri, a.link_id
                                    from rdb_link a
                                    left join rdb_link_with_all_attri_view b
                                    on a.link_id = b.link_id
                                    order by link_id_t, common_main_link_attri, link_id
                                )as a
                                group by link_id_t
                            )as b
                        ) a 
                        where link_array[seq_nm] != seq_nm;
                    """
            return 0 == self.pg.getOnlyQueryResult(sqlcmd)
        else:
            return True

            
#TileID检查
class CCheckTileID(platform.TestCase.CTestCase):  
    def _do(self):
        
        sqlcmd = """

                select result.link_id_t from (
                    select link.link_id,link.link_id_t,link.start_node_id,node.node_id,node.node_id_t 
                        from rdb_link link,rdb_node node
                     where link.start_node_id=node.node_id
                ) as result
                where result.link_id_t <> result.node_id_t;

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

#DisplayClass有效值检查
class CCheckDisplayclassValid(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_display_class;
                ALTER TABLE rdb_link
                ADD CONSTRAINT check_display_class CHECK (display_class = ANY (ARRAY[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,30,31]));
     
                 """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            self.pg.commit()
            return 1

#Link起始Node有效值检查    
class CCheckStartNodeValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select * from (
                    select a.start_node_id,b.node_id from rdb_link a
                    left join
                    rdb_node b
                    on a.start_node_id=b.node_id) as result
                 where result.node_id is null;

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

#Link终点Node有效值检查
class CCheckEndNodeValid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select * from (
                    select a.end_node_id,b.node_id from rdb_link a
                    left join 
                    rdb_node b
                    on a.end_node_id=b.node_id
                ) as result
                where result.node_id is null;

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

#Link终点TileID检查    
class CCheckEndNodeTileID(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                
                select * from (
                    select link.link_id,link.end_node_id,link.end_node_id_t,node.node_id,node.node_id_t 
                        from rdb_link link,rdb_node node
                    where link.end_node_id=node.node_id
                ) as result 
                where result.end_node_id_t <> result.node_id_t

                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

#road_type有效值检查    
class CCheckRoadType(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_road_type;
                ALTER TABLE rdb_link
                      ADD CONSTRAINT check_road_type CHECK (road_type = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]));
             
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            self.pg.commit()
            return 1
        
#road_type值完备性检查
class CCheckRoadTypeValid(platform.TestCase.CTestCase):
    def _do(self):
                    
            sqlcmd = """
                   select count(a.road_type)
                    from
                        (select unnest(ARRAY[0,1,2,3,4,6,8,10]) as road_type) as a
                    left join
                        (select distinct road_type from rdb_link) as b
                    on a.road_type = b.road_type
                    where b.road_type is null;
                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False

class CCheckRoadTypeValid_TA(platform.TestCase.CTestCase):
    def _do(self):
                    
            sqlcmd = """
                   select count(a.road_type)
                    from
                        (select unnest(ARRAY[0,2,3,4,6,7]) as road_type) as a
                    left join
                        (select distinct road_type from rdb_link) as b
                    on a.road_type = b.road_type
                    where b.road_type is null;
                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckRoadTypeValid_Nostra(platform.TestCase.CTestCase):
    def _do(self):
                    
            sqlcmd = """

                   select count(a.road_type)
                    from
                        (select unnest(ARRAY[0,2,3,4,6,8,10]) as road_type) as a
                    left join
                        (select distinct road_type from rdb_link) as b
                    on a.road_type = b.road_type
                    where b.road_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False

class CCheckRoadTypeValid_NI(platform.TestCase.CTestCase):
    def _do(self):
                    
        sqlcmd = """

               select count(a.road_type)
                from
                    (select unnest(ARRAY[0,1,2,3,4,6,7,8,9,10,14]) as road_type) as a
                left join
                    (select distinct road_type from rdb_link) as b
                on a.road_type = b.road_type
                where b.road_type is null;                    
            """

        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt != 0)
        
class CCheckRoadTypeValid_RDF(platform.TestCase.CTestCase):
    def _do(self):
                    
            sqlcmd = """
                   select count(a.road_type)
                    from
                        (select unnest(ARRAY[0,2,3,4,6,7,8,10,12,14]) as road_type) as a
                    left join
                        (select distinct road_type from rdb_link) as b
                    on a.road_type = b.road_type
                    where b.road_type is null;
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False

class CCheckRoadTypeValid_HKG(platform.TestCase.CTestCase):
    def _do(self):
                    
        sqlcmd = """
               select count(a.road_type)
                from
                    (select unnest(ARRAY[0,2,3,4,6,8,10,12,14]) as road_type) as a
                left join
                    (select distinct road_type from rdb_link) as b
                on a.road_type = b.road_type
                where b.road_type is null;
            """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
                        
class CCheckRoadTypeValid_MMI(platform.TestCase.CTestCase):
    def _do(self):
                    
            sqlcmd = """
                   select count(a.road_type)
                    from
                        (select unnest(ARRAY[0,2,3,4,6,7,8,10]) as road_type) as a
                    left join
                        (select distinct road_type from rdb_link) as b
                    on a.road_type = b.road_type
                    where b.road_type is null;
                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False                          
#一方通行Code有效值检查
class CCheckOneway(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_one_way;
                ALTER TABLE rdb_link
                      ADD CONSTRAINT check_one_way CHECK (one_way = ANY (ARRAY[0, 1, 2, 3]));
             
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            self.pg.commit()
            return 1

#一方通行Code值完备性检查
class CCheckOnewayValid(platform.TestCase.CTestCase):
    def _do(self):
         
            sqlcmd = """
                    select count(a.one_way)
                    from
                        (select unnest(ARRAY[0,1,2,3]) as one_way) as a
                    left join
                        (select distinct one_way from rdb_link) as b
                    on a.one_way = b.one_way
                    where b.one_way is null

                    union
                    
                    select count(*) from (
                        select a.oneway1,b.oneway4,c.all_cnt from (
                            select 1 as id, count(*) as oneway1 from rdb_link where one_way = 1
                        ) a
                        left join (
                            select 1 as id, count(*) as oneway4 from rdb_link where one_way = 0
                        ) b
                        on a.id = b.id
                        left join (
                            select 1 as id, count(*) as all_cnt from rdb_link 
                        ) c
                        on a.id = c.id
                    ) d 
                    where oneway1::float/all_cnt::float < 0.8 
                    or oneway4::float/all_cnt::float > 0.05;
                                     
                """

            self.pg.execute(sqlcmd)
            cnt = self.pg.getcnt()
            row = self.pg.fetchone()

            if row:
                if cnt == 1 and row[0] == 0:
                    return True
            return False
class CCheckOnewayValid_ta(platform.TestCase.CTestCase):
    def _do(self):
         
            sqlcmd = """
                    select count(a.one_way)
                    from
                        (select unnest(ARRAY[0,1,2,3]) as one_way) as a
                    left join
                        (select distinct one_way from rdb_link) as b
                    on a.one_way = b.one_way
                    where b.one_way is null

                    union
                    
                    select count(*) from (
                        select a.oneway1,b.oneway4,c.all_cnt from (
                            select 1 as id, count(*) as oneway1 from rdb_link where one_way = 1
                        ) a
                        left join (
                            select 1 as id, count(*) as oneway4 from rdb_link where one_way = 0
                        ) b
                        on a.id = b.id
                        left join (
                            select 1 as id, count(*) as all_cnt from rdb_link 
                        ) c
                        on a.id = c.id
                    ) d 
                    where oneway1::float/all_cnt::float < 0.7 
                    or oneway4::float/all_cnt::float > 0.06;
                                     
                """

            self.pg.execute(sqlcmd)
            cnt = self.pg.getcnt()
            row = self.pg.fetchone()

            if row:
                if cnt == 1 and row[0] == 0:
                    return True
            return False

class CCheckOnewayValid_Nostra(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                    select count(a.one_way)
                    from
                        (select unnest(ARRAY[1,2]) as one_way) as a
                    left join
                        (select distinct one_way from rdb_link) as b
                    on a.one_way = b.one_way
                    where b.one_way is null

                    union
                    
                    select count(*) from (
                        select a.oneway1,b.oneway4,c.all_cnt from (
                            select 1 as id, count(*) as oneway1 from rdb_link where one_way = 1
                        ) a
                        left join (
                            select 1 as id, count(*) as oneway4 from rdb_link where one_way = 0
                        ) b
                        on a.id = b.id
                        left join (
                            select 1 as id, count(*) as all_cnt from rdb_link 
                        ) c
                        on a.id = c.id
                    ) d 
                    where oneway1::float/all_cnt::float < 0.8 
                    or oneway4::float/all_cnt::float > 0.01;                          
                    
                """

            self.pg.execute(sqlcmd)
            cnt = self.pg.getcnt()
            row = self.pg.fetchone()

            if row:
                if cnt == 1 and row[0] == 0:
                    return True
            return False

class CCheckOnewayValid_RDF(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                    select count(a.one_way)
                    from
                        (select unnest(ARRAY[0,1,2,3]) as one_way) as a
                    left join
                        (select distinct one_way from rdb_link) as b
                    on a.one_way = b.one_way
                    where b.one_way is null

                    union
                    
                    select count(*) from (
                        select a.oneway1,b.oneway4,c.all_cnt from (
                            select 1 as id, count(*) as oneway1 from rdb_link where one_way = 1
                        ) a
                        left join (
                            select 1 as id, count(*) as oneway4 from rdb_link where one_way = 0
                        ) b
                        on a.id = b.id
                        left join (
                            select 1 as id, count(*) as all_cnt from rdb_link 
                        ) c
                        on a.id = c.id
                    ) d 
                    where oneway1::float/all_cnt::float < 0.8 
                    or oneway4::float/all_cnt::float > 0.05;
                                     
                """

            self.pg.execute(sqlcmd)
            cnt = self.pg.getcnt()
            row = self.pg.fetchone()

            if row:
                if cnt == 1 and row[0] == 0:
                    return True
            return False

class CCheckOnewayValid_RDF_SGP(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                    select count(a.one_way)
                    from
                        (select unnest(ARRAY[0,1,2,3]) as one_way) as a
                    left join
                        (select distinct one_way from rdb_link) as b
                    on a.one_way = b.one_way
                    where b.one_way is null

                    union
                    
                    select count(*) from (
                        select a.oneway1,b.oneway4,c.all_cnt from (
                            select 1 as id, count(*) as oneway1 from rdb_link where one_way = 1
                        ) a
                        left join (
                            select 1 as id, count(*) as oneway4 from rdb_link where one_way = 0
                        ) b
                        on a.id = b.id
                        left join (
                            select 1 as id, count(*) as all_cnt from rdb_link 
                        ) c
                        on a.id = c.id
                    ) d 
                    where oneway1::float/all_cnt::float < 0.5
                    or oneway4::float/all_cnt::float > 0.15;
                                     
                """

            self.pg.execute(sqlcmd)
            cnt = self.pg.getcnt()
            row = self.pg.fetchone()

            if row:
                if cnt == 1 and row[0] == 0:
                    return True
            return False
                        
class CCheckOnewayValid_MMI(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                    select count(a.one_way)
                    from
                        (select unnest(ARRAY[0,1,2]) as one_way) as a
                    left join
                        (select distinct one_way from rdb_link) as b
                    on a.one_way = b.one_way
                    where b.one_way is null

                    union
                    
                    select count(*) from (
                        select a.oneway1,b.oneway4,c.all_cnt from (
                            select 1 as id, count(*) as oneway1 from rdb_link where one_way = 1
                        ) a
                        left join (
                            select 1 as id, count(*) as oneway4 from rdb_link where one_way = 0
                        ) b
                        on a.id = b.id
                        left join (
                            select 1 as id, count(*) as all_cnt from rdb_link 
                        ) c
                        on a.id = c.id
                    ) d 
                    where oneway1::float/all_cnt::float < 0.8 
                    or oneway4::float/all_cnt::float > 0.01;
                                     
                """

            self.pg.execute(sqlcmd)
            cnt = self.pg.getcnt()
            row = self.pg.fetchone()

            if row:
                if cnt == 1 and row[0] == 0:
                    return True
            return False
        
class CCheckOnewayValid_MSM(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                    select count(a.one_way)
                    from
                        (select unnest(ARRAY[0,1,2]) as one_way) as a
                    left join
                        (select distinct one_way from rdb_link) as b
                    on a.one_way = b.one_way
                    where b.one_way is null

                    union
                    
                    select count(*) from (
                        select a.oneway1,b.oneway4,c.all_cnt from (
                            select 1 as id, count(*) as oneway1 from rdb_link where one_way = 1
                        ) a
                        left join (
                            select 1 as id, count(*) as oneway4 from rdb_link where one_way = 0
                        ) b
                        on a.id = b.id
                        left join (
                            select 1 as id, count(*) as all_cnt from rdb_link 
                        ) c
                        on a.id = c.id
                    ) d 
                    where oneway1::float/all_cnt::float < 0.8 
                    or oneway4::float/all_cnt::float > 0.02;
                                     
                """

            self.pg.execute(sqlcmd)
            cnt = self.pg.getcnt()
            row = self.pg.fetchone()

            if row:
                if cnt == 1 and row[0] == 0:
                    return True
            return False        
        
#功能等级有效值检查
class CCheckFunctionCode(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                    ALTER TABLE rdb_link DROP CONSTRAINT if exists check_function_code;
                    ALTER TABLE rdb_link
                      ADD CONSTRAINT check_function_code CHECK (function_code = ANY (ARRAY[1, 2, 3, 4, 5]));
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            self.pg.commit()
            return 1
    
#功能等级值完备性检查
class CCheckFunctionCodeValid(platform.TestCase.CTestCase):
    def _do(self):
            if self.pg.CreateFunction_ByName('CheckFunctionCodeValid') == -1:
                return 0
         
            sqlcmd = """

                    select * from CheckFunctionCodeValid();
                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 1:
                    return True
            return False
  
#Link长度检查  
class CCheckLinkLength(platform.TestCase.CTestCase):
    def _do(self):
         
        sqlcmd = """
               select * from rdb_link where link_length <= 1 and link_length > 500000 ;
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

#road_name_id有效值检查
class CCheckRoadNameID(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
            select tablename from pg_tables  where tablename like 'rdb_name_dictionary%';
                 """  
               
        self.pg.query(sqlcmd)
        language_table = []
        rows = self.pg.fetchall()
        if rows == None:
            return False
        for row in rows:
            language_table.append(row[0])

        # select * from (
        #    SELECT a.road_name_id,b.name_id FROM rdb_link a 
        #    left outer join rdb_name_dictionary_chinese b 
        #    on a.road_name_id=b.name_id
        #) as result 
        #where result.name_id is null                                
        # 统计所有语言表里name_id的数目
        sqlcmd = "SELECT count(gid) FROM rdb_link a left outer join ( "
        sqlcmd1 = "SELECT name_id from rdb_name_dictionary "
           
        for i, tbl in enumerate(language_table):           
            if i != 0:
                sqlcmd += ' UNION '
            sqlcmd += sqlcmd1.replace('rdb_name_dictionary', tbl)
        sqlcmd += ') b on a.road_name_id = b.name_id where b.name_id is null;'
                
        self.pg.query(sqlcmd)
        language_table = []
        row = self.pg.fetchone()
        if row:
            if row[0] == 0:
                return True
        else:
            return None
 
#road_number有效值检查
class CCheckRoadNumber(platform.TestCase.CTestCase):
    def _do(self):
        return 1
    
class CCheckLaneID(platform.TestCase.CTestCase):
    def _do(self):
   
        sqlcmd = """
               select * from (
                    SELECT a.lane_id as laneid1,b.lane_id as laneid2 FROM rdb_link a 
                    left join 
                    rdb_linklane_info b
                    on a.lane_id=b.lane_id
                ) as result
                where result.laneid2 is null;
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckToll(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_toll;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_toll CHECK (toll = ANY (ARRAY[0, 1, 2, 3]));
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            self.pg.commit()
            return 1  
    
class CCheckTollValid(platform.TestCase.CTestCase):
    def _do(self):
         
            sqlcmd = """

                select toll1,toll0 from 
                (
                    select 1 as id, count(*) as toll1 from rdb_link where toll=1
                ) as toll1
                
                left join 
                
                (
                    select 1 as id, count(*) as toll0 from rdb_link 
                ) as toll0
                
                on toll1.id=toll0.id;
                    
                """
        
            self.pg.query(sqlcmd)
            row = self.pg.fetchone()

            if row:
                if row[0] <> 0 and float(row[0]) / float (row[1]) < 0.01:
                    return True
            return False   
    
class CCheckRegulationFlag(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                    select * from (
                        select a.link_id,b.reg_link_id from (
                            select link_id from rdb_link where regulation_flag=True
                        ) as a 
                    left join 
                    (
                    select distinct unnest(string_to_array(key_string, ',')::bigint[]) as reg_link_id
                    from rdb_link_regulation
                    )as b
                    on a.link_id=b.reg_link_id 
                    ) as result
                    where reg_link_id is null
                    
                    union 
                    
                    select * from (
                        select a.link_id,b.reg_link_id from (
                            select link_id from rdb_link where regulation_flag=False
                        ) as a 
                    left join 
                    (
                    select distinct unnest(string_to_array(key_string, ',')::bigint[]) as reg_link_id
                    from rdb_link_regulation
                    )as b
                    on a.link_id=b.reg_link_id 
                    ) as result
                    where reg_link_id is not null;
                    
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckTileFlag(platform.TestCase.CTestCase):
    def _do(self):
        return 1 
        
class CCheckSpeedRegulationFlag(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                    select * from (
                        select a.link_id as link1,b.link_id as link2 from (
                            select link_id from rdb_link where speed_regulation_flag=True
                        ) as a 
                    left join 
                    rdb_cond_speed b
                    on a.link_id=b.link_id 
                    ) as result
                    where link2 is null
                    
                    union
                     
                    select * from (
                        select a.link_id as link1,b.link_id  as link2 from (
                            select link_id from rdb_link where speed_regulation_flag=False
                        ) as a 
                    left join 
                    rdb_cond_speed b
                    on a.link_id=b.link_id 
                    ) as result
                    where link2 is not null;
                    
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0 
    
class CCheckLinkAddInfoFlag(platform.TestCase.CTestCase):
    def _do(self):
       
        sqlcmd = """
                    select * from (
                        select a.link_id as link1,b.link_id as link2 from (
                            select link_id from rdb_link where link_add_info_flag=True
                        ) as a 
                    left join 
                    rdb_link_add_info b
                    on a.link_id=b.link_id 
                    ) as result
                    where link2 is null
                    
                    union
                        
                    select * from (
                        select a.link_id as link1,b.link_id  as link2 from (
                            select link_id from rdb_link where link_add_info_flag=False
                        ) as a 
                    left join 
                    rdb_link_add_info b
                    on a.link_id=b.link_id 
                    ) as result
                    where link2 is not null;
                    
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
  
class CCheckFAZM(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                select * from rdb_link  where fazm < -32768 or fazm > 32767;
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0    
    
class CCheckTAZM(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select * from rdb_link  where tazm < -32768 or fazm > 32767;
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0   
    
class CCheckFnLink(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                            
                select * from (
                        select link.start_node_id,link.fnlink,node.branches
                        from rdb_link as link
                        inner join 
                        rdb_node as node 
                        on link.start_node_id = node.node_id and link.fnlink is not null
                ) as a 
                where not (fnlink = any(branches));
            
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0 
    
class CCheckFnLinkCircle(platform.TestCase.CTestCase):
    def _do(self):
        return 1
    
class CCheckFnLinkTile(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                            
            select * from (
                select link_id,link_id_t from rdb_link
            ) as link
            inner  join
            (
                select fnlink,fnlink_t from rdb_link 
            ) as fnlink
            on link.link_id = fnlink.fnlink and link.link_id_t <> fnlink.fnlink_t;
            
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0   

class CCheckTnLink(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                            
                select * from (
                        select link.end_node_id,link.tnlink,node.branches
                        from rdb_link as link
                        inner join 
                        rdb_node as node 
                        on link.end_node_id = node.node_id and link.tnlink is not null
                ) as a 
                where not (tnlink = any(branches));
            
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
    
class CCheckTnLinkCircle(platform.TestCase.CTestCase):
    def _do(self):
        return 1  
    
class CCheckTnLinkTile(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                            
            select * from (
                select link_id,link_id_t from rdb_link
            ) as link
            inner  join 
            (
                select tnlink,tnlink_t from rdb_link 
            ) as tnlink
            on link.link_id = tnlink.tnlink and link.link_id_t <> tnlink.tnlink_t;
            
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0  
        
class CCheckLinkType(platform.TestCase.CTestCase):
    def _do(self):

    
        sqlcmd = """
                 ALTER TABLE rdb_link DROP CONSTRAINT if exists check_link_type;
                 ALTER TABLE rdb_link
                   ADD CONSTRAINT check_link_type CHECK (link_type = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]));
                  
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            self.pg.commit()
            return 1
        
class CCheckLinkTypeValid(platform.TestCase.CTestCase):
    def _do(self):
                    
            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[0,1,2,4,5,6,7]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 

class CCheckLinkTypeValid_RDF(platform.TestCase.CTestCase):
    def _do(self):
                    
            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[0,1,2,4,5,6]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 
        
class CCheckLinkTypeValid_JPN(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[1,2,4,5,6,7]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                   
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 

class CCheckLinkTypeValid_Nostra(platform.TestCase.CTestCase):
    def _do(self):
  
            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[0,1,2,4,5,6,7]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 

class CCheckLinkTypeValid_MMI(platform.TestCase.CTestCase):
    def _do(self):
  
            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[0,1,2,4,5,6,8]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 

class CCheckLinkTypeValid_MSM(platform.TestCase.CTestCase):
    def _do(self):
  
            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[0,1,2,4,5]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 

class CCheckLinkTypeValid_NI(platform.TestCase.CTestCase):
    def _do(self):
  
            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[0,1,2,3,4,5,6,7,8,9]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 

class CCheckLinkTypeValid_ZENRIN(platform.TestCase.CTestCase):
    def _do(self):
  
            sqlcmd = """
                   select count(a.link_type)
                    from
                        (select unnest(ARRAY[0,1,2,3,4,5,6,7]) as link_type) as a
                    left join
                        (select distinct link_type from rdb_link) as b
                    on a.link_type = b.link_type
                    where b.link_type is null;                    
                """

            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 
                                                
class CCheckExtendFlag(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                
                select * from (
                    select link_id as link,extend_flag as flag 
                    from rdb_link 
                    where extend_flag <> 0
                ) as a 
                where ((flag >> 2) & 7) not in (0,1,2,3)

                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0             

class CCheckShieldFlag(platform.TestCase.CTestCase):
    def _do(self):
       
        sqlcmd = """
                select count(a.link_id) from rdb_link a
                    left join  rdb_link_shield b
                    on a.link_id = b.link_id
                    where (b.link_id is not null and not a.shield_flag) or (b.link_id is null and a.shield_flag);
                 """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] == 0:
                return True
        return False 
    
class CCheckFAZMPath(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                select * from rdb_link  where fazm_path < -32768 or  fazm_path > 32767;
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0  
    
class CCheckTAZMPath(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select * from rdb_link  where tazm_path < -32768 or  tazm_path > 32767;
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0  
    
class CCheckTheGeom(platform.TestCase.CTestCase):
    def _do(self):
        
            if self.pg.CreateFunction_ByName('CheckTheGeom') == -1:
                return 0
                    
            if self.pg.callproc('CheckTheGeom') == -1:
                return 0
                
            return 1  
    
class CCheckTheCoordinate(platform.TestCase.CTestCase):
    def _do(self):
         
            sqlcmd = """
                 select distinct ST_SRID(the_geom) from  rdb_link;
                 """
                 
            self.pg.execute(sqlcmd)
            
            row = self.pg.fetchone()
            
            if self.pg.getcnt() == 1:
                
                if row:
                    if row[0] == 4326:
                        return True
                    return False

            else:       
                return 0
           
class CCheckLinkCount(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = '''
                select park_link_id from rdb_park_link;
                '''
        if self.pg.execute(sqlcmd) == -1:     
            sqlcmd = """
                    (
                        select count(gid) as count_rdb from rdb_link
                    ) 
                    union
                    (
                        select count(gid) as count_mid from link_tbl
                    );
                     """
        else:
            sqlcmd = """
                    (
                        select count(id)
                        from
                        (
                            select link_id as id from rdb_link
                            union
                            select park_link_id as id from rdb_park_link
                        )as a
                    ) 
                    union
                    (
                        select count(gid) as count_mid from link_tbl
                    ); 
                     """
                 
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0  
    
class CCheckRoadTypeMatching(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """

                select * from (
                    select distinct link_type from 
                    (
                        select * from rdb_link where road_type in (0,1)) as link
                    )as link
                where link_type not in (0,1,2,3,4,5,6,7,8)   
                    
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckRoadTypeMatching_MMI(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """

                select * from (
                    select distinct link_type from 
                    (
                        select * from rdb_link where road_type in (0,1)) as link
                    )as link
                where link_type not in (0,1,2,3,4,5,6,7,8)   
                    
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckRoadTypeMatching_Nostra(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """

                select * from (
                    select distinct link_type from 
                    (
                        select * from rdb_link where road_type in (0,1)) as link
                    )as link
                where link_type not in (0,1,2,3,4,5,6,7,13)   
                    
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
            
class CCheckRampClass(platform.TestCase.CTestCase):
    def _do(self):

            if self.pg.CreateFunction_ByName('check_ramp_atnode') == -1:
                return 0
        
            if self.pg.CreateFunction_ByName('CheckRamp') == -1:
                return 0
                              
            if self.pg.callproc('CheckRamp') == -1:
                return 0
                
            return 1   
    
class CCheckShapePoints(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select * from (
                    select ST_NPoints(the_geom) as num from rdb_link 
                ) as link
                where num > 1023 
                """
        
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0
 
class CCheckDuplicateLink(platform.TestCase.CTestCase):
    def _do(self):
                              
        sqlcmd = """
            select count(*) from (
                    select * from rdb_link as c
                    inner join (
                        select a.* from rdb_link as a
                        inner join rdb_link as b
                        on a.gid != b.gid 
                        and  a.start_node_id = b.start_node_id and a.end_node_id = b.end_node_id
                    ) as d
                    on c.gid != d.gid 
                    and  c.start_node_id = d.start_node_id and c.end_node_id = d.end_node_id 
                
                    union 
                
                    select * from rdb_link as c
                    inner join (
                        select a.* from rdb_link as a
                        inner join rdb_link as b
                        on a.gid != b.gid 
                        and  a.start_node_id = b.start_node_id and a.end_node_id = b.end_node_id
                    ) as d
                    on c.gid != d.gid 
                    and  c.start_node_id = d.end_node_id and c.end_node_id = d.start_node_id 
            ) e;                    
                """
        
        count_rec = self.pg.getOnlyQueryResult(sqlcmd)
        if count_rec > 0:
            return False
                
        return True

 
class CCheckDiff_LinkID(platform.TestCase.CTestCase):
    def _do(self):
                              
        sqlcmd = "select count(*) from rdb_link;"
        all_link_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = "select count(*) from id_fund_link where (version = current_database()) and (((tile_id >> 28) & 15) = 14);"
        new_link_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = "select count(*) from id_fund_link where (((tile_id >> 28) & 15) = 14);"
        id_fund_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        if (all_link_count == id_fund_count):
            return True
        elif (new_link_count * 1.0 / all_link_count) <= 0.05:
            return True
        else:
            return False

class CCheckDiff_LinkSame(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                    select count(*)
                    from
                    (
                        select st_distance(a.the_geom, b.the_geom, true) as distance
                        from rdb_link as a
                        inner join id_fund_link as b
                        on a.link_id = b.link_id
                    )as t
                    where distance > 30;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
class CCheckRdb_Link_road_name_id(platform.TestCase.CTestCase):
    '''检查rdb_link中road_name_id字段是否有数据存在'''
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from
                    (
                        select distinct road_name_id from rdb_link
                    )as a
                    
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count > 1)   

class CCheckRdb_Link_with_all_attri_view_road_name_id(platform.TestCase.CTestCase):
    '''检查rdb_Link_with_all_attri_view表单中road_name_id字段是否有数据存在'''
    def _do(self):
        sqlcmd = """
                    select count(*)
                    from
                    (
                        select distinct road_name_id from rdb_Link_with_all_attri_view
                    )as a
                    
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count > 1)      

class CCheckLinkAttriView_Forecast(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(*) from (
            select a.*
            from
            (
                select * from rdb_link_with_all_attri_view
                where forecast_flag is false
            )as a
            left join rdb_forecast_link b
            on a.link_id = b.link_id
            where b.link_id is not null
            
            union

            select a.*
            from
            (
                select * from rdb_link_with_all_attri_view
                where forecast_flag is true
            )as a
            left join rdb_forecast_link b
            on a.link_id = b.link_id
            where b.link_id is null           
        ) c;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckLinkAttriView_SSequence_Key(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(*) from (
            select a.*
            from
            (
                select * from rdb_link_with_all_attri_view
                where s_sequence_link_id != -1
            )as a
            left join rdb_link b
            on a.link_id = b.link_id
            where b.link_id is null
        ) c;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     
    
class CCheckLinkAttriView_SSequence_Valid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select * from rdb_link_with_all_attri_view a
        left join rdb_link_with_all_attri_view b
        on a.s_sequence_link_id = b.link_id
        where a.s_sequence_link_id != -1
        and a.start_node_id not in (b.start_node_id,b.end_node_id)
        and a.end_node_id not in (b.start_node_id,b.end_node_id);
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckLinkAttriView_ESequence_Key(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select count(*) from (
            select a.*
            from
            (
                select * from rdb_link_with_all_attri_view
                where e_sequence_link_id != -1
            )as a
            left join rdb_link b
            on a.link_id = b.link_id
            where b.link_id is null
        ) c;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0) 
    
class CCheckLinkAttriView_ESequence_Valid(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
        select * from rdb_link_with_all_attri_view a
        left join rdb_link_with_all_attri_view b
        on a.e_sequence_link_id = b.link_id
        where a.e_sequence_link_id != -1
        and a.start_node_id not in (b.start_node_id,b.end_node_id)
        and a.end_node_id not in (b.start_node_id,b.end_node_id);
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)     
        
class CCheckMatchingFlag_Val(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from rdb_link 
            where matching_flag not in (0,1);               
            """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)                  

class CCheckMatchingFlag_Null(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*) from rdb_link 
            where matching_flag != 0;                   
            """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckMatchingFlag_NotNull(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
               select count(a.matching_flag)
                from
                    (select unnest(ARRAY[0,1]) as matching_flag) as a
                left join
                    (select distinct matching_flag from rdb_link) as b
                on a.matching_flag = b.matching_flag
                where b.matching_flag is null;                   
            """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckABSLink(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
            select count(*)
            from rdb_link_with_all_attri_view a
            left join rdb_link_abs b
            on a.abs_link_id = b.abs_link_id 
            where a.abs_link_id != 0 and b.abs_link_id is null;                 
            """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)    

class CCheckRoadName_type(platform.TestCase.CTestCase):
    def _do(self):
        #type取值范围{'office_name','route_num','alter_name','bridge_name'}。
        sqlcmd = """
            select link_id, road_name
            from rdb_link
            where road_name is not null;                 
            """
        datas = self.pg.get_batch_data(sqlcmd) 

        for data in datas:
            link_id = data[0]
            road_name = data[1]
            
            find_flag = 0
            multi_name_info = json.loads(road_name)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    name_type = name_dict.get('type')
                    tts_type = name_dict.get('tts_type')                    
                    left_right = name_dict.get('left_right')

                    if name_type == 'office_name' or name_type == 'route_num'\
                        or name_type == 'alter_name' or name_type == 'bridge_name':
                        find_flag = 1
                    if tts_type == 'not_tts' or tts_type == 'text'\
                        or tts_type == 'phoneme' or tts_type == 'phoneme_text':
                        find_flag = 1
                    if left_right == 'left_right' or left_right == 'left_name'\
                        or left_right == 'right_name':
                        find_flag = 1
                                                
            if find_flag == 0:
                self.logger.info("RoadName: Wrong type/tts_type/left_right (id=%d)" % (link_id))
                return 0
        
        return 1 
           
class CCheckRoadName_TTS(platform.TestCase.CTestCase):
    def _do(self):
        #tts必须与非tts同时存在.
        sqlcmd = """
            select link_id, road_name
            from rdb_link
            where road_name is not null;                 
            """
        datas = self.pg.get_batch_data(sqlcmd) 

        for data in datas:
            link_id = data[0]
            road_name = data[1]
            tts_type_array = []
            multi_name_info = json.loads(road_name)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    tts_type = name_dict.get('tts_type')
                    tts_type_array.append(tts_type)
                    
                try:
                    if tts_type_array.index(u'phoneme') >= 0 or tts_type_array.index(u'phoneme_text') >= 0 \
                        or tts_type_array.index(u'text') >= 0:
                        # 'phoneme' or 'phoneme_text' was found.
                        try :
                            if tts_type_array.index(u'not_tts') >= 0:
                                pass
                        except:
                            # 'not_tts' not found.
                            self.logger.error("RoadName: Wrong TTS (only TTS, no name, id=%d)" % (link_id))
                            return 0
                except:
                    return 1
            
        return 1    

class CCheckRoadNumber_shield(platform.TestCase.CTestCase):
    def _do(self):
        # 道路番号必须有盾牌.
        sqlcmd = """
            select link_id, road_number
            from rdb_link
            where road_number is not null;                 
            """
        datas = self.pg.get_batch_data(sqlcmd) 

        for data in datas:
            link_id = data[0]
            road_number = data[1]
            tts_type_array = []
            
            road_number = road_number.replace('\t','\\t')
            multi_name_info = json.loads(road_number)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    tts_type = name_dict.get('tts_type')
                    type = name_dict.get('type')
                    val = name_dict.get('val')
                    
                    if tts_type == 'not_tts' and type == 'shield':
                        if not (val.find('\t') >= 0):
                            self.logger.error("RoadNumber: Wrong shield (no tab, id=%d)" % (link_id))
                            return 0
           
        return 1  

class CCheckRoadNumber_type(platform.TestCase.CTestCase):
    def _do(self):
        # 类型必须是'shield'.
        sqlcmd = """
            select link_id, road_number
            from rdb_link
            where road_number is not null;                 
            """
        datas = self.pg.get_batch_data(sqlcmd) 

        for data in datas:
            link_id = data[0]
            road_number = data[1]
            tts_type_array = []
            
            road_number = road_number.replace('\t','\\t')
            multi_name_info = json.loads(road_number)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    type = name_dict.get('type')
                    tts_type = name_dict.get('tts_type')
                    left_right = name_dict.get('left_right')
                     
                    if type == 'shield' :
                        find_flag = 1
                    if tts_type == 'not_tts' or tts_type == 'text'\
                        or tts_type == 'phoneme' or tts_type == 'phoneme_text':
                        find_flag = 1
                    if left_right == 'left_right' or left_right == 'left_name'\
                        or left_right == 'right_name':
                        find_flag = 1
                                                
            if find_flag == 0:
                self.logger.info("RoadNumber: Wrong type/tts_type/left_right (id=%d)" % (link_id))
                return 0
        
        return 1 

class CCheckRoadNumber_TTS(platform.TestCase.CTestCase):
    def _do(self):
        #tts必须与非tts同时存在.
        sqlcmd = """
            select link_id, road_number
            from rdb_link
            where road_number is not null;                 
            """
        datas = self.pg.get_batch_data(sqlcmd) 

        for data in datas:
            link_id = data[0]
            road_number = data[1]

            road_number = road_number.replace('\t','\\t')
            tts_type_array = []
            multi_name_info = json.loads(road_number)
            for one_name_info in multi_name_info:
                for name_dict in one_name_info:
                    tts_type = name_dict.get('tts_type')
                    tts_type_array.append(tts_type)
                    
                try:
                    if tts_type_array.index(u'phoneme') >= 0 or tts_type_array.index(u'phoneme_text') >= 0 \
                        or tts_type_array.index(u'text') >= 0:
                        # 'phoneme' or 'phoneme_text' was found.
                        try :
                            if tts_type_array.index(u'not_tts') >= 0:
                                pass
                        except:
                            # 'not_tts' not found.
                            self.logger.error("RoadNumber: Wrong TTS (only TTS, no number, id=%d)" % (link_id))
                            return 0
                except:
                    return 1
            
        return 1    

class CCheckNameUnique(platform.TestCase.CTestCase):
    def _do(self): 
        '''检查road_name和road_name_id是否一一对应'''
        sqlcmd1 = '''
            drop table if exists temp_road_name_check;
            create table temp_road_name_check
            (
                name_id integer,
                road_name varchar
            )
        ''' 
        self.pg.execute(sqlcmd1)
        self.pg.commit()
        
        sqlcmd = '''
            select distinct road_name_id, road_name 
            from rdb_link as a
            where road_name is not null;
        '''

        results = self.pg.get_batch_data(sqlcmd)
        for result in results:
            road_name_id = result[0]
            road_name = result[1]
            json_name = json.loads(road_name)
            sort = sorted(json_name,key=lambda e:e[0])
            self.pg.insert('temp_road_name_check',['name_id','road_name'],[road_name_id,str(sort)])
        self.pg.commit() 
        
        sqlcmd = '''
        select count(*)
        from
        (
            select count(*) from temp_road_name_check
            group by road_name having count(*) > 1
        ) as a
        ''' 
        return (self.pg.getOnlyQueryResult(sqlcmd) == 0)
    
class CCheckrdb_link_name_language(platform.TestCase.CTestCase):
    '''检查name_id和name是否唯一对应'''
    def _do(self):
        Flag = True
        pg_config = CConfigReader.instance()      
        sqlcmd_country = """
                        select distinct road_name
                        from rdb_link
                        where iso_country_code = '[country]'
                             and road_name is not null
                             and road_type <> 10;
                        """
        
        sqlcmd = '''
               select distinct iso_country_code
               from rdb_link;
               '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            language_code_list = pg_config.getcountry_language_code(row[0])
            sqlcmd_temp = sqlcmd_country.replace('[country]', row[0])
            self.pg.execute(sqlcmd_temp)
            lines = self.pg.fetchall()
            for line in lines:
                shield = json.loads(line[0], encoding='utf8')
                for shield_temp1 in shield:
                    for shield_temp2 in shield_temp1:
                        if shield_temp2['lang'] in language_code_list:
                            continue
                        else:
                            Flag = False
                            self.logger.error("name=%s error! country language = %s,country = %s",line[0],language_code_list,row[0])
     
        return Flag
    
class CCheckrdb_link_number_shield(platform.TestCase.CTestCase):
    '''检查name_id和name是否唯一对应'''
    def _do(self):
        Flag = True       
        pg_config = CConfigReader.instance()        
        sqlcmd_country = """
                        select distinct road_number
                        from rdb_link
                        where iso_country_code = '[country]'
                             and road_number is not null
                             and road_type <> 10;
                        """
        
        sqlcmd = '''
               select distinct iso_country_code
               from rdb_link;
               '''
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            shield_list = pg_config.getShield_id_list(row[0])
            sqlcmd_temp = sqlcmd_country.replace('[country]', row[0])
            self.pg.execute(sqlcmd_temp)
            lines = self.pg.fetchall()
            for line in lines:
                shield = json.loads(line[0].replace('\t', '\\t'), encoding='utf8')
                for shield_temp1 in shield:
                    for shield_temp2 in shield_temp1:
                        if shield_temp2['tts_type'] == 'not_tts':
                            shield_split = shield_temp2['val'].split('\t')
                            if str(shield_split[0]) in shield_list:
                                continue
                            else:
                                self.logger.error("shield=%s error!country shield = %s,country = %s",line, shield_list, row[0])
                                Flag = False
     
        return Flag 
           
class CCheckLinkCircle(platform.TestCase.CTestCase):
    '''检查link是否自身构成环'''
    def _do(self):
        sqlcmd = """
                SELECT count(link_id)
                  FROM rdb_link
                  where start_node_id = end_node_id;
        """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)  

class CCheckSapaLink_by_highway(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
                select count(*) from mid_temp_sapa_path_link
                where link_type = 5
        '''  
        rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec == 0)

    
class CCheckJctLink_by_highway(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
                select count(*) from mid_temp_jct_path_link
                where link_type = 5
        '''  
        rec = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec == 0)    
    
class CCheckDisplayclass(platform.TestCase.CTestCase):
    
    def _do(self):
        sqlcmd = '''
                select link_id,start_node_id,end_node_id,b.link_num,c.link_num,display_class
                from rdb_link a 
                join rdb_node b
                on a.start_node_id = b.node_id
                join rdb_node c
                on a.end_node_id = c.node_id
                where link_type=5 
                '''
        self.pg.execute(sqlcmd)
        results = self.pg.fetchall()
        sqlcmd_1='''
            select display_class 
            from rdb_link where link_id <> %d and link_type = 5
                and %d in (start_node_id,end_node_id)
                '''
        for result in results:
            if result[3]==2:
                display_class = self.pg.getOnlyQueryResult(sqlcmd_1%(result[0],result[1]))
                if display_class :
                    if display_class<>result[5]:
                        return False
            if result[4]==2:
                display_class = self.pg.getOnlyQueryResult(sqlcmd_1%(result[0],result[2]))
                if display_class :
                    if display_class<>result[5]:
                        return False
        return True
