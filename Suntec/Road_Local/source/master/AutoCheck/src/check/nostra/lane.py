# -*- coding: UTF8 -*-
'''
Created on 2013-12-26

@author: minghongtu
'''
import platform.TestCase

class CCheckLaneInLinkType(platform.TestCase.CTestCase):
    
    def _do(self):
        
        sqlcmd = """
                select count(*)
                from lane_tbl as a left join link_tbl as b
                on a.inlinkid = b.link_id
                where b.link_type = 4 
                
                """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)
    
class CCheckGuideLanePathCnt(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select count(*)
                from
                (
                  SELECT array_upper(regexp_split_to_array(passlid, '\\\\|'),1) as lenth, passlink_cnt
                  FROM lane_tbl
                )as a
                where a.lenth <> passlink_cnt;
                
                """
        rec_cnt = self.pg.getOnlyQueryResult(sqlcmd)
        return (rec_cnt == 0)


class CCheckGuideLanePathLinkRe(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                SELECT regexp_split_to_array(passlid, '\\\\|') as passlinks
                FROM lane_tbl
                where passlid <> '';
                 
                 """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            print row[0]
            len1 = len(row[0])
            lent2 = len(set(row[0]))
            if  len1 != lent2:
                return False
        return True


class CCheckGuideLaneNodeForwarded(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = """
                select f.node_id
                from (
                    select c.node_id
                    from(
                        select unnest(nodes) as node_id
                        from(
                              SELECT inlinkid, array_agg(passlink_cnt) as a,array_agg( distinct nodeid) as nodes
                              FROM lane_tbl
                              group by inlinkid
                        ) as b
                    where 0 <> all(a)
                    ) as c
                    left join link_tbl as d
                    on c.node_id = d.s_node or c.node_id = d.e_node
                ) as f
                group by f.node_id having count(*) < 3
            
               """
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        if  len(rows) > 0:
            return False
        else:
            return True
    
    