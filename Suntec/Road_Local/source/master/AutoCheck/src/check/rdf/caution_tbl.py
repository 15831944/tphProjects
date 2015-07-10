# -*- coding: UTF8 -*-
'''
Created on 2015-5-7

@author: zhaojie
'''

import platform.TestCase

class CCheckcautionValues(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from caution_tbl as a
                left join mid_admin_zone as b
                on a.strtts = b.ad_name and (b.ad_order in (0,1))
                where b.ad_code is null and a.data_kind = 4;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    
class CCheckcautionLogic(platform.TestCase.CTestCase):
    def _do(self):
        sqlcmd = '''
                select count(*)
                from
                (
                    select d.ad_code,end_code
                    from
                    (
                        select (case when s_node = nodeid then e_node else s_node end) as start_id,voice_id as end_code
                        from caution_tbl as a
                        left join link_tbl as b
                        on a.inlinkid = b.link_id
                        where a.data_kind = 4
                    )temp
                    left join node_tbl as c
                    on start_id = c.node_id
                    left join mid_admin_zone as d
                    on d.ad_order = 1 and ST_Contains(d.the_geom,c.the_geom)
                    where ad_code = end_code
                    
                    union
                    
                    select d.ad_code,end_code
                    from
                    (
                        select (case when s_node = nodeid then e_node else s_node end) as start_id,voice_id as end_code
                        from caution_tbl as a
                        left join link_tbl as b
                        on a.inlinkid = b.link_id
                        where a.data_kind = 4
                    )temp
                    left join node_tbl as c
                    on start_id = c.node_id
                    left join mid_admin_zone as d
                    on d.ad_order = 0 and ST_Contains(d.the_geom,c.the_geom)
                    where ad_code = end_code
                )temp;
                '''
        reg_number_count = self.pg.getOnlyQueryResult(sqlcmd)      

        return (reg_number_count == 0)
    