# -*- coding: UTF8 -*-
'''
Created on 2015-5-7

@author: zhaojie
'''

import platform.TestCase

class CChecksignpost_ucValues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                 from signpost_uc_tbl
                 where sp_name is null and route_no1 is null and route_no2 is null
                    and route_no3 is null and  route_no4 is null and exit_no is null;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CChecksignpost_ucContraints(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (                
                    select id
                     from signpost_uc_tbl as a
                     left join link_tbl as b
                     on a.inlinkid = b.link_id and a.nodeid in (b.s_node, b.e_node) and b.one_way_code <> 4
                     left join link_tbl as c
                     on a.outlinkid = c.link_id and c.one_way_code <> 4
                     left join node_tbl as d
                     on a.nodeid = d.node_id
                     where b.link_id is null or c.link_id is null or d.node_id is null
                     
                     union
                     
                     select id
                     from
                     (
                         select id,unnest(string_to_array(passlid,'|')::bigint[]) as pass_link
                         from signpost_uc_tbl
                         where passlid is not null
                     )as a
                     left join link_tbl as b
                     on a.pass_link = b.link_id and b.one_way_code <> 4
                     where b.link_id is null
                 )temp;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CChecksignpost_ucLogic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select id
                     from signpost_uc_tbl
                     where inlinkid = outlinkid
                     
                     union
                     
                     select id 
                     from signpost_uc_tbl as a
                     left join link_tbl as b
                     on a.inlinkid = b.link_id and 
                        ((a.nodeid = b.s_node and b.one_way_code in (1,3)) or 
                        (a.nodeid = b.e_node and b.one_way_code in (1,2)))
                     where b.link_id is null
                 )temp;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    