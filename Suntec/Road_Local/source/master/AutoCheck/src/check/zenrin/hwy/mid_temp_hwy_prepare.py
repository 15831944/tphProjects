# -*- coding: UTF8 -*-
'''
Created on 2015��7��16��

@author: PC_ZH
'''
import platform.TestCase


#==============================================================================
# CCheckFacilityNode
# 检查所有收录Hwy node（非起始点）均为分歧点
# 起点进入link的path_id不同于node所在path_id
# 非起点的脱出link的path_id不同于node所在path_id
#==============================================================================
class CCheckFacilityNode(platform.TestCase.CTestCase):
    '''检查所有收录node均为分歧点'''
    def _do(self):
        sqlcmd = '''
        select road_code, node_id, seq_nm as seq,
               array_agg(outlink_path) as outlink_path,
               array_agg(inlink_path) as inlink_path
         from (
                 select info.road_code, node.facility_id,
                        node.node_id, info.seq_nm,
                        case
                         when info1.road_code is null then 0
                         else info1.road_code
                        end as outlink_path,
                        case
                          when info2.road_code is null then 0
                          else info2.road_code
                        end as inlink_path
                 from mid_hwy_org_facility_node as node
                 left join hwy_link_road_code_info as info
                 on node.path_id = info.road_code and
                    node.node_id = info.node_id
                 left join link_tbl as t1  -- Out link
                 on (node.node_id = t1.s_node and t1.one_way_code in (1, 2))
                   or(node.node_id = t1.e_node and t1.one_way_code in (1,3))
                 left join link_tbl as t2  -- In link
                 on (node.node_id = t2.e_node and t2.one_way_code in (1,2))
                   or(node.node_id = t2.s_node and t2.one_way_code in (1,3))
                 left join hwy_link_road_code_info as info1  -- Out Road_code
                 on t1.link_id = info1.link_id
                 left join hwy_link_road_code_info as info2  -- In Road_code
                 on t2.link_id = info2.link_id
                 order by node.path_id ,node.node_id
            )as a
        group by road_code, seq, node_id
        order by road_code, seq, node_id
        '''
        for row in self.pg.get_batch_data(sqlcmd):
            node_path = row[0]
            # node_id = row[1]
            seq = row[2]
            outlink_path = row[3]
            inlink_path = row[4]
            if not node_path:
                return False
            node_set = set([node_path])
            if 0 == seq:  # Start Node of Path
                in_set = set(inlink_path)
                insert_set = in_set.intersection(node_set)
                if insert_set and insert_set != set([0]):
                    self.pg.log.error('CCheckFacilityNode')
                    return False
            else:  # Other Node(Include last Node)
                if node_set == set(outlink_path):
                    self.pg.log.error('CCheckFacilityNode')
                    return False
        return True


#==============================================================================
# CCheckSaFacilityNode
#zenrin/hwy_prepare/  当org_sa 连接mid_hwy_org_facility_node时node_id是否有空值
#==============================================================================
class CCheckSaFacilityNode(platform.TestCase.CTestCase):
    ''' '''
    def _Do(self):
        sqlcmd = '''
        SELECT a.pathid, a.dirflag, a.pointid, b.node_id
        FROM org_sa as a
        LEFT JOIN mid_hwy_org_facility_node as b
        ON a.pathid = b.path_id and a.dirflag = b.dirflag
           and a.pointid = b.facility_id
        WHERE node_id is null
        ORDER BY a.pathid, a.dirflag, a.pointid
        '''
        self.pg.execute(sqlcmd)
        row = self.pg.fetchall()
        if row:
            self.pg.log.error('CCheckSaFacilityNode')
            return False
        return True
