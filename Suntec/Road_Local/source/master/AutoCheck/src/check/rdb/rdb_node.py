# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckNodeIDUnique(platform.TestCase.CTestCase):
    def _do(self):
        
            sqlcmd = """
                select count(*) from (
                   select node_id  from rdb_node group by node_id having count(*) > 1
                ) as a;
                """
        
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckNodeIDValid(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
                select count(*) from rdb_node where (node_id >> 32) <> node_id_t;
                """
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckNodeTile(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
               select node_id from rdb_node where (node_id >> 32) <> node_id_t;

                """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0

class CCheckHighwayFacility(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join 
                            rdb_highway_facility_data b
                            on a.node_id=b.node_id
                        ) as result 
                        where node2 is not null
                    ) as result 
                    where (extend_flag & 1) <> 1
                ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)


class CCheckPark(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.park_node_id node2 
                            FROM rdb_node a
                            left join (
                                select * from rdb_park_node
                                where park_node_type <> 0
                            ) b
                            on a.node_id = b.park_node_id
                        ) as result 
                        where node2 is not null
                    ) as result 
                    where (extend_flag >> 1) & 1 <> 1
                ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
    
class CCheckCrossname(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_crossname
                            ) b
                            on a.node_id=b.node_id
                         ) as result 
                         where node2 is not null
                    ) as result 
                    where ((extend_flag >> 2) & 1) <> 1
                ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckTowardname(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
            select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_towardname
                            )  b
                            on a.node_id=b.node_id
                            )as result 
                            where node2 is not null
                    ) as result 
                    where ((extend_flag >> 3) & 1) <> 1
            ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckSpotguide(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_spotguidepoint
                            )  b
                            on a.node_id=b.node_id
                        )as result 
                        where node2 is not null
                    ) as result 
                    where ((extend_flag >> 4) & 1) <> 1
                ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckSignpost(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_signpost
                            )  b
                            on a.node_id=b.node_id
                         )as result 
                         where node2 is not null
                    ) as result 
                    where ((extend_flag >> 5) & 1) <> 1
                ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckLane(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_lane
                            )  b
                            on a.node_id=b.node_id
                            )as result 
                            where node2 is not null
                    ) as result 
                    where ((extend_flag >> 6) & 1) <> 1
                ) as d;

                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)
        
class CCheckRoadStructure(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_road_structure
                            )  b
                            on a.node_id=b.node_id
                            )as result
                            where node2 is not null
                    ) as result 
                    where ((extend_flag >> 7) & 1) <> 1
                 ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckBuildingStructure(platform.TestCase.CTestCase):
    def _do(self):
       
        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_building_structure
                            )  b
                            on a.node_id=b.node_id
                            )as result 
                            where node2 is not null
                    ) as result 
                    where ((extend_flag >> 8) & 1) <> 1
                ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckSignpostUC(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_signpost_uc
                            )  b
                            on a.node_id=b.node_id
                            )as result 
                            where node2 is not null
                    ) as result 
                    where ((extend_flag >> 11) & 1) <> 1
                ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckBoundaryNode(platform.TestCase.CTestCase):
    def _do(self):
        return 1
        sqlcmd = """
                    select count(*)
                    from rdb_node as a
                    left join 
                    (
                        select distinct node_id
                        from rdb_node as a
                        inner join rdb_link as b
                        on a.node_id in (b.start_node_id, b.end_node_id) and a.node_id_t != b.link_id_t
                    )as b
                    on a.node_id = b.node_id
                    where (((a.extend_flag >> 12) & 1) = 1 and b.node_id is null)
                       or (((a.extend_flag >> 12) & 1) = 0 and b.node_id is not null);
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckCaution(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_caution
                            )  b
                            on a.node_id=b.node_id
                            )as result 
                            where node2 is not null
                    ) as result 
                    where ((extend_flag >> 13) & 1) <> 1
                 ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)

class CCheckForceGuide(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """

                select count(*) from (
                    select * from (
                        select * from (
                            SELECT a.node_id node1, a.extend_flag,b.node_id node2 FROM rdb_node a
                            left join (
                                select distinct node_id from rdb_guideinfo_forceguide
                            )  b
                            on a.node_id=b.node_id
                            )as result 
                            where node2 is not null
                    ) as result 
                    where ((extend_flag >> 15) & 1) <> 1
                 ) as d;
                """
        return 0 == self.pg.getOnlyQueryResult(sqlcmd)


class CCheckTrafficLight(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
        select count(*) from (
            select 
            (select count(*) from rdb_node where ((extend_flag >> 9) & 1) = 1) as count_trf
            ,(select count(*) from rdb_node) as count_all
        ) a where (count_trf * 1.0) / count_all  > 0.01
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)

class CCheckTrafficLight_JPN(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
        select count(*) from (
            select 
            (select count(*) from rdb_node where ((extend_flag >> 9) & 1) = 1) as count_trf
            ,(select count(*) from rdb_node) as count_all
        ) a where (count_trf * 1.0) / count_all  > 0.04
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckTrafficLight_RDF_SGP(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
        select count(*) from (
            select 
            (select count(*) from rdb_node where ((extend_flag >> 9) & 1) = 1) as count_trf
            ,(select count(*) from rdb_node) as count_all
        ) a where (count_trf * 1.0) / count_all  > 0.06
                 """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
        
class CCheckToll(platform.TestCase.CTestCase):
    def _do(self):

            if self.pg.CreateFunction_ByName('CheckToll_ETC') == -1:
                return 0
            if self.pg.CreateFunction_ByName('CheckToll_toll') == -1:
                return 0
                        
            if self.pg.callproc('CheckToll_ETC') == -1 or self.pg.callproc('CheckToll_toll') == -1:
                return 0
                
            return 1

class CCheckLinkCount(platform.TestCase.CTestCase):
    def _do(self):
        
      
        sqlcmd = """
                 
                select * from (
                    select node_id,link_num,branches from rdb_node                    
                ) as a where array_upper(branches,1)    <> link_num
                 """
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 0:
                return 1
            else:
                return 0 

class CCheckLink(platform.TestCase.CTestCase):
    def _do(self):       
      
            sqlcmd = """
                select c.link,c.node_id from (
                    select a.node_id,a.link,b.start_node_id,b.end_node_id from ( 
                        (select node_id,unnest(branches) as link from rdb_node) as a
                        inner join 
                        rdb_link as b
                        on a.link=b.link_id
                    ) where a.node_id <> b.start_node_id and a.node_id <> b.end_node_id
                ) as c
                        
                union
               
                select c.link_id,c.node_id from (
                    select a.link_id,b.node_id,b.branches from rdb_link a
                    inner join 
                    rdb_node  b
                    on a.start_node_id = b.node_id     
                ) as c 
                where not(c.link_id = any(branches))
                
                union
                
                select c.link_id,c.node_id from (
                    select a.link_id,b.node_id,b.branches from rdb_link a
                    inner join 
                    rdb_node  b
                    on a.end_node_id = b.node_id     
                ) as c 
                where not(c.link_id = any(branches));
                 """
                 
            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0 

class CCheckCoordinate(platform.TestCase.CTestCase):
    def _do(self):
        
            sqlcmd = """
                 select distinct ST_SRID(the_geom) from  rdb_node;
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

class CCheckNodeCount(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = '''
                select park_node_id from rdb_park_node;
                '''
        if self.pg.execute(sqlcmd) == -1:     
            sqlcmd = """
                    (
                        select count(*) as count_rdb from rdb_node
                    ) 
                    union
                    (
                        select count(*) as count_mid from node_tbl
                    ) 
                     """
        else:
            sqlcmd = """
                    (
                        select count(id)
                        from
                        (
                            select node_id as id from rdb_node
                            union
                            select park_node_id as id from rdb_park_node
                            where park_node_type = 0
                        )as a
                    ) 
                    union
                    (
                        select count(*) as count_mid from node_tbl
                    ); 
                     """
                 
        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0 

class CCheckAdjacentNode(platform.TestCase.CTestCase):
    def _do(self):
                
            sqlcmd = """
                select count(*) from (
                      select a.node1,a.node2,b.link_id as link1,c.node_id as node 
                      from (
                            select node1,node2,distance 
                            from (
                                   SELECT a.node_id as node1, b.node_id as node2,
                                          ST_Distance_Spheroid(a.the_geom,b.the_geom,'SPHEROID["WGS 84",6378137,298.257223563]') as distance,
                                          lonlat2pixel_tile(14::smallint, ((a.node_id_t >> 14) & 16383)::integer, (a.node_id_t & 16383)::integer, st_x(a.the_geom)::float, st_y(a.the_geom)::float, 4096::smallint) as geom1,
                                          lonlat2pixel_tile(14::smallint, ((b.node_id_t >> 14) & 16383)::integer, (b.node_id_t & 16383)::integer, st_x(b.the_geom)::float, st_y(b.the_geom)::float, 4096::smallint) as geom2
                                   FROM rdb_node as a
                                   left join rdb_node as b
                                       on a.node_id <> b.node_id and 
                                          ST_DWithin(b.the_geom, a.the_geom, 0.000007186444528518818301)
                                  where b.node_id is not null
                             ) as c 
                            where distance < 0.3 
                                and not ((x(geom1)=0 and x(geom2)=0) or (x(geom1)=4096 and x(geom2)=4096))
                                and not ((y(geom1)=0 and y(geom2)=0) or (y(geom1)=4096 and y(geom2)=4096))
                        ) a
                     left join rdb_link b
                         on (a.node1 = b.start_node_id and a.node2 = b.end_node_id) 
                         or (a.node2 = b.start_node_id and a.node1 = b.end_node_id)
                     left join (
                         select distinct node_id from (
                                select start_node_id as node_id from rdb_link where road_type in (0,1,2)
                                union
                                select end_node_id as node_id from rdb_link where road_type in (0,1,2)) h
                        ) c
                     on a.node1 = c.node_id 
                     or a.node2 = c.node_id            
                ) d
                where link1 is null and node is null;
                 """
                 
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False 

 
class CCheckDiff_NodeID(platform.TestCase.CTestCase):
    def _do(self):
                              
        sqlcmd = "select count(*) from rdb_node;"
        all_node_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = "select count(*) from id_fund_node where (version = current_database()) and (((tile_id >> 28) & 15) = 14);"
        new_node_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        sqlcmd = "select count(*) from id_fund_node where (((tile_id >> 28) & 15) = 14);"
        id_fund_count = self.pg.getOnlyQueryResult(sqlcmd)
        
        if (all_node_count == id_fund_count):
            return True
        elif (new_node_count * 1.0 / all_node_count) <= 0.05:
            return True
        else:
            return False

class CCheckDiff_NodeSame(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
                    select count(*)
                    from
                    (
                        select st_distance(a.the_geom, b.the_geom, true) as distance
                        from rdb_node as a
                        inner join id_fund_node as b
                        on a.node_id = b.node_id
                    )as t
                    where distance > 30;
                """
        rec_count = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_count == 0)
