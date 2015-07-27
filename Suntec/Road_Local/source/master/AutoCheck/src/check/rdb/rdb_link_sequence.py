# -*- coding: UTF8 -*-
'''
Created on 2015-2-5

@author: zhaojie
'''

import platform.TestCase

    
def base_sequenceID(link_table, sequence_table):
    
    sqlcmd = """
            select count(*) 
            from (
                select a.link_id  
                from [link_sequence_tbl] as a
                left join [link_tbl] as b
                on a.link_id = b.link_id
                where b.link_id is null
                
                union
                
                select s_link_id  
                from [link_sequence_tbl] as a
                left join [link_tbl] as b
                on a.s_link_id = b.link_id
                where b.link_id is null and a.s_link_id <> -1
                
                union
                
                select e_link_id  
                from [link_sequence_tbl] as a
                left join [link_tbl] as b
                on a.e_link_id = b.link_id
                where b.link_id is null and a.e_link_id <> -1                    
                ) as temp1;
            """   
    return (sqlcmd.replace('[link_tbl]', link_table)).replace('[link_sequence_tbl]', sequence_table)

def compare_link_type():   
    sqlcmd = """
        CREATE OR REPLACE FUNCTION Check_sequence_link_type(type1 integer, type2 integer)
            RETURNS BOOLEAN
            LANGUAGE plpgsql
        AS $$
        DECLARE
        BEGIN
            if (type1 in (3,5,6,7) and type2 in (3,5,6,7)) or 
                (type1 not in (3,5,6,7) and type2 not in (3,5,6,7)) then
                return true;
            else
                return false;
            end if;
        END;
        $$;      
        """
    return sqlcmd

def base_sequence_type(link_table, sequence_table):
    
    sqlcmd = """
                select count(*) 
                from (                  
                        select a.link_id  
                        from [link_sequence_tbl] as a
                        left join [link_tbl] as b
                        on a.link_id = b.link_id
                        left join [link_tbl] as c
                        on a.s_link_id = c.link_id
                        where a.s_link_id <> -1 and 
                            not Check_sequence_link_type(b.link_type, c.link_type)
                        
                        union
                        
                        select a.link_id  
                        from [link_sequence_tbl] as a
                        left join [link_tbl] as b
                        on a.link_id = b.link_id
                        left join [link_tbl] as c
                        on a.e_link_id = c.link_id
                        where a.e_link_id <> -1 and 
                            not Check_sequence_link_type(b.link_type, c.link_type)                 
                    ) as temp1;
                """
    return (sqlcmd.replace('[link_tbl]', link_table)).replace('[link_sequence_tbl]', sequence_table)

def region_sequence_type(link_table, sequence_table):
    
    sqlcmd = """
                select count(*) 
                from (
                        select a.link_id  
                        from [link_sequence_tbl] as a
                        left join [link_mapping] as b
                        on a.link_id = b.region_link_id
                        left join [link_mapping] as c
                        on a.s_link_id = c.region_link_id
                        where a.s_link_id <> -1 and (b.region_link_id is null or  c.region_link_id is null)
                        
                        union
                        
                        select a.link_id  
                        from [link_sequence_tbl] as a
                        left join [link_mapping] as b
                        on a.link_id = b.region_link_id
                        left join [link_mapping] as c
                        on a.e_link_id = c.region_link_id
                        where a.e_link_id <> -1 and (b.region_link_id is null or  c.region_link_id is null)
                    ) as temp1;
                """
    return (sqlcmd.replace('[link_mapping]', link_table)).replace('[link_sequence_tbl]', sequence_table)

def base_sequence_oneway(link_table, sequence_table):
    
    sqlcmd = """
                select count(*) 
                from (                  
                        select a.link_id  
                        from [link_sequence_tbl] as a
                        join [link_tbl] as b
                        on a.link_id = b.link_id
                        where a.s_link_id <> -1 and b.one_way in (0,2)
                        
                        union
                        
                        select a.link_id  
                        from [link_sequence_tbl] as a
                        join [link_tbl] as b
                        on a.link_id = b.link_id
                        where a.e_link_id <> -1 and b.one_way in (0,3)
                        
                        union
                        
                        select link_id
                        from
                        (
                            select a.link_id, (case when c.start_node_id = c.end_node_id then c.one_way in (0)
                                                    when b.start_node_id = c.start_node_id then c.one_way in (0,3)
                                                    else c.one_way in (0,2) end) as flage_temp
                            from [link_sequence_tbl] as a
                            left join [link_tbl] as b
                            on a.link_id = b.link_id
                            left join [link_tbl] as c
                            on a.s_link_id = c.link_id
                            where a.s_link_id <> -1
                        )temp
                        where flage_temp = true
                        
                        union
                        
                        select link_id
                        from
                        (
                            select a.link_id, (case when c.start_node_id = c.end_node_id then c.one_way in (0)
                                                    when b.end_node_id = c.start_node_id then c.one_way in (0,3)
                                                    else c.one_way in (0,2) end) as flage_temp
                            from [link_sequence_tbl] as a
                            left join [link_tbl] as b
                            on a.link_id = b.link_id
                            left join [link_tbl] as c
                            on a.e_link_id = c.link_id
                            where a.e_link_id <> -1
                        )temp
                        where flage_temp = true
                                                
                                           
                    ) as temp1;
                """
    return (sqlcmd.replace('[link_tbl]', link_table)).replace('[link_sequence_tbl]', sequence_table) 
        
class CCheckLink_sequenceID(platform.TestCase.CTestCase):
    def _do(self):
                   
            self.pg.execute(base_sequenceID(r'rdb_link', r'rdb_link_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckLink_sequence_type(platform.TestCase.CTestCase):
    def _do(self):
                   
            self.pg.execute(compare_link_type())        
            self.pg.execute(base_sequence_type(r'rdb_link', r'rdb_link_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False

class CCheckLink_sequence_oneway(platform.TestCase.CTestCase):
    def _do(self):
                                       
            self.pg.execute(base_sequence_oneway(r'rdb_link', r'rdb_link_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False             
        
class CCheckLink_layer4_sequenceID(platform.TestCase.CTestCase):
    def _do(self):
        
            self.pg.execute(base_sequenceID(r'rdb_region_link_layer4_tbl', r'rdb_region_link_layer4_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckLink_layer4_sequence_type(platform.TestCase.CTestCase):
    def _do(self):
    
            self.pg.execute(compare_link_type())        
            self.pg.execute(region_sequence_type(r'rdb_region_layer4_link_mapping', r'rdb_region_link_layer4_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckLink_layer4_sequence_oneway(platform.TestCase.CTestCase):
    def _do(self):
                                       
            self.pg.execute(base_sequence_oneway(r'rdb_region_link_layer4_tbl', r'rdb_region_link_layer4_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
        
class CCheckLink_layer6_sequenceID(platform.TestCase.CTestCase):
    def _do(self):
        
            self.pg.execute(base_sequenceID(r'rdb_region_link_layer6_tbl', r'rdb_region_link_layer6_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckLink_layer6_sequence_type(platform.TestCase.CTestCase):
    def _do(self):
         
            self.pg.execute(compare_link_type())        
            self.pg.execute(region_sequence_type(r'rdb_region_layer6_link_mapping', r'rdb_region_link_layer6_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False
        
class CCheckLink_layer6_sequence_oneway(platform.TestCase.CTestCase):
    def _do(self):
                                       
            self.pg.execute(base_sequence_oneway(r'rdb_region_link_layer6_tbl', r'rdb_region_link_layer6_sequence'))
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False