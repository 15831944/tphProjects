# -*- coding: UTF8 -*-
'''
Created on 2012-12-10
CrossName类
@author: hongchenzai
'''
import platform.TestCase
import json


class CCheckNumber(platform.TestCase.CTestCase):
    '''检查[交叉点名称]的条目数。'''
    def _do(self):
        sqlcmd = """
                SELECT 
                 (
                 SELECT COUNT(gid)
                    from crossname_tbl
                    where namestr is not null
                 ) as mid_num,
                  (
                  SELECT COUNT(guideinfo_id)
                    from rdb_guideinfo_crossname
                    ) as rdb_num;
                """
                
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != row[1]:
                return False
            else:
                return True
        else:
            return False
   
class CCheckNameID(platform.TestCase.CTestCase):
    '''检查[交叉点名称]的Name ID'''
    def _do(self):
        sqlcmd = """
            SELECT count(a.*)
              FROM rdb_guideinfo_crossname as a
              left join temp_name_dictionary AS b
              ON a.name_id = new_name_id
              left join rdb_tile_node as c
              ON a.node_id = tile_node_id
              left join node_tbl as d
              on c.old_node_id = d.node_id
              where old_name_id <> d.name_id;
        """
        self.pg.query(sqlcmd)
        row = self.pg.fetchone()
        if row:
            if row[0] != 0:
                return False
            else:
                return True
        else:
            return False
        
class CCheckCross_name(platform.TestCase.CTestCase):
    '''  cross_name为json格式，并且名称不为空'''
    def _do(self):
        
        sqlcmd = """
                SELECT cross_name
                FROM rdb_guideinfo_crossname
                where cross_name is not null;
                """
              
        for row in self.pg.get_batch_data(sqlcmd):
            cross_name = row[0]
            name_lists = json.loads(cross_name)
            for name_list in name_lists:
                for name in name_list:
                    if not name.get('val'):
                        return False
        return True        

class CCheckNode_idTile(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
            select count(*)
            from
            (
               select node_id 
               from rdb_guideinfo_crossname 
               where (node_id >> 32) <> node_id_t
            ) a
            ;
                """  

        return (self.pg.getOnlyQueryResult(sqlcmd) == 0 )     


class CCheckIn_link_idTile(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
            select count(*)
            from
            (
                select in_link_id
                from
                (
                   select in_link_id,in_link_id_t 
                   from rdb_guideinfo_crossname
                   where in_link_id is not null 
                ) b
                where (b.in_link_id >> 32) <> b.in_link_id_t
            ) a
            ;
                """  

        return (self.pg.getOnlyQueryResult(sqlcmd) == 0 )  

    
class CCheckOut_link_idTile(platform.TestCase.CTestCase):
    def _do(self):
        
        sqlcmd = """
            select count(*)
            from
            (
                select out_link_id
                from
                (
                   select out_link_id,out_link_id_t 
                   from rdb_guideinfo_crossname
                   where out_link_id is not null 
                ) b
                where (b.out_link_id >> 32) <> b.out_link_id_t
            ) a
            ;
                """  

        return (self.pg.getOnlyQueryResult(sqlcmd) == 0 )  