# -*- coding: UTF8 -*-
'''
Created on 2014-5-16

@author: zhaojie
'''
import platform.TestCase
import platform.TestCaseManager

class CCheckNum(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
            select count(*) from rdb_park_node;
            """
    
        self.pg.execute(sqlcmd)
        rdb_node = self.pg.fetchone()[0]
        
        sqlcmd = """
            select count(*) from park_node_tbl;
            """
    
        self.pg.execute(sqlcmd)
        tbl_node = self.pg.fetchone()[0]
        
        sqlcmd = """
            select count(*) from rdb_park_link;
            """
    
        self.pg.execute(sqlcmd)
        rdb_link = self.pg.fetchone()[0]
        
        sqlcmd = """
            select count(*) from park_link_tbl;
            """
    
        self.pg.execute(sqlcmd)
        tbl_link = self.pg.fetchone()[0]
        
        return (rdb_node == tbl_node) and (rdb_link == tbl_link)
    
    
class CCheckParkID(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
            select count(*) from (
               select park_node_id  
               from rdb_park_node
               where (park_node_id::bit(32))::integer < (1<<23) 
               
               union
               
               select park_node_id  
               from rdb_park_node
               where (park_base_id::bit(32))::integer > (1<<23)
                   and park_node_type in (1,2,3)
            ) as a;
            """
    
        self.pg.execute(sqlcmd)
        parknodeid = self.pg.fetchone()[0]
        
        sqlcmd = """
            select count(*) from (
               select park_link_id  
               from rdb_park_link
               where (park_link_id::bit(32))::integer < (1<<23) 
            ) as a;
            """
    
        self.pg.execute(sqlcmd)
        parklinkid = self.pg.fetchone()[0]
        
        return (parknodeid + parklinkid) == 0

class CCheckConnectCorrect(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            CREATE OR REPLACE FUNCTION check_park_linkout(nodeid bigint,base_node_id bigint,
                                    num smallint ,node_type smallint,linkout_arry bigint[])
                RETURNS boolean
                LANGUAGE plpgsql
            AS $$
            DECLARE
                rec record;
            BEGIN
                if num is null or num = 0  then
                    return true;
                end if;
                for idx in 1..num loop
                    if (linkout_arry[idx]::bit(32))::integer > (1<<23) then
                        if node_type in (1,3) then
                            for rec in (
                                select park_link_id,park_link_oneway,park_s_node_id,park_e_node_id
                                from rdb_park_link
                                where park_link_id = linkout_arry[idx]
                            )loop
                                if nodeid = rec.park_s_node_id then
                                   if rec.park_link_oneway in (0,3) then
                                        return true;
                                    end if;
                                elsif nodeid = rec.park_e_node_id then
                                    if rec.park_link_oneway in (0,2) then
                                        return true;
                                    end if;
                                else
                                    return true;
                                end if;
                            end loop;
                        else
                            return true;
                        end if;
                    else
                        if node_type in (2,3) then
                            for rec in (
                                select link_id,one_way,start_node_id,end_node_id
                                from rdb_link
                                where link_id = linkout_arry[idx]
                            )loop
                                if base_node_id = rec.start_node_id then
                                    if rec.one_way in (0,3) then
                                        return true;
                                    end if;
                                elsif base_node_id = rec.end_node_id then
                                    if rec.one_way in (0,2) then
                                        return true;
                                    end if;
                                else
                                    return true;
                                end if;
                            end loop;
                        else
                            return true;
                        end if;
                    end if;
                end loop;
 
                return false;
            END;
            $$;
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit()
        
        sqlcmd = '''
            select count(*) from (
                select park_node_id
                from 
                (
                    select * from rdb_park_node where park_node_type in (1,2,3)
                ) as a
                where check_park_linkout(park_node_id,park_base_id,park_connect_link_num,
                                        park_node_type,park_connect_link_array)
            ) as temp;
            '''
        self.pg.execute(sqlcmd)
        num = self.pg.fetchone()[0]
        strcountey = platform.TestCaseManager.CTestCaseManager.instance().getCountry()
        if strcountey.lower() == "jpn":
            return num == 1
        else:
            return num == 0
    
class CCheckConnectivity(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(*) from (
                select park_link_id
                from rdb_park_link as a
                left join
                rdb_park_node as b
                on a.park_s_node_id = b.park_node_id or a.park_e_node_id = b.park_node_id
                where b.park_node_id is null
            ) as temp;
            '''
        self.pg.execute(sqlcmd)
        num = self.pg.fetchone()[0]
        
        return num == 0
    
class CCheckLinkAndNodeInPOlygon(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
            select count(*) from (
                select a.park_link_id as id
                from rdb_park_link as a
                left join
                rdb_park_region as b
                on a.park_region_id = b.park_region_id
                where b.park_region_id is null
                union
                select a.park_node_id as id
                from rdb_park_node as a
                left join
                rdb_park_region as b
                on a.park_region_id = b.park_region_id
                where b.park_region_id is null
            )as temp
            '''
        self.pg.execute(sqlcmd)
        num = self.pg.fetchone()[0]
        
        return num == 0
        