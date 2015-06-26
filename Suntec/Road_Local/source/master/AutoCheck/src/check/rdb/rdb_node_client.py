# -*- coding: UTF8 -*-
'''
Created on 2011-12-22

@author: liuxinxing
'''

import platform.TestCase

class CCheckNodeID(platform.TestCase.CTestCase):
    def _do(self):

        sqlcmd = """
                      
                  (SELECT count(a.gid)as count 
                          FROM rdb_node as a
                      left join 
                      rdb_node_client as b
                      on a.node_id_t = b.node_id_t and (a.node_id & ((1::bigint << 32) - 1)) = b.node_id 
                  )
                    union
                  (
                      select count(*) as count from rdb_node 
                  ) 
                """

        if self.pg.execute(sqlcmd) == -1:
            return 0
        else:
            if self.pg.getcnt() == 1:
                return 1
            else:
                return 0
        
class CCheckExtendFlag(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
 
                select * from (   
                      SELECT a.extend_flag as extendflag1,b.extend_flag as extendflag2
                          FROM rdb_node as a
                      left join rdb_node_client as b
                      on a.node_id_t = b.node_id_t and (a.node_id & ((1::bigint << 32) - 1)) = b.node_id 
                ) as c 
                where c.extendflag1 <> c.extendflag2;
                """

            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0
                        
class CCheckGeomBlob(platform.TestCase.CTestCase):
    def _do(self):

            sqlcmd = """
  
                    SELECT *
                        FROM rdb_node_client
                    where octet_length(geom_blob) <> 4 ;

                """

            if self.pg.execute(sqlcmd) == -1:
                return 0
            else:
                if self.pg.getcnt() == 0:
                    return 1
                else:
                    return 0
                        
class CCheckGeomBlobExample(platform.TestCase.CTestCase):
    def _do(self):
     
            if self.pg.CreateFunction_ByName('byte2int') == -1:
                return 0
            
            sqlcmd = """
                       
            select count(*) from (
                select * from 
                    (select byte2int(geom_blob) as xy from rdb_node_client) a
                where  (strx(xy) < 0 or  strx(xy) > 4096)
                        or (stry(xy) < 0 or  stry(xy) > 4096)
                        
                union all
                
                select * from 
                    (select byte2int(geom_blob) as xy from rdb_node_client
                         where ((extend_flag >> 12) & 1) = 1) b 
                where  (strx(xy) <> 0 and strx(xy) <> 4096) 
                        and (stry(xy) <> 0 and stry(xy) <> 4096)
            ) c ;

            """
                    
            self.pg.execute(sqlcmd)
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            return False