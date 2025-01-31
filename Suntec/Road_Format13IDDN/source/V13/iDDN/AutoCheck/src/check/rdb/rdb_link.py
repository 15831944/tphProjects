# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

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
                                select link_id_t, count(*) as tile_link_count, array_agg(link_id) as link_array
                                from
                                (
                                    select link_id_t, common_main_link_attri, link_id
                                    from rdb_link_client
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
                ADD CONSTRAINT check_display_class CHECK (display_class = ANY (ARRAY[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,0]));
     
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
                    or oneway4::float/all_cnt::float > 0.03;
                                     
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
                    where oneway1::float/all_cnt::float < 0.8 
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
                where link_type not in (0,1,2,3,4,5,6,7)   
                    
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


