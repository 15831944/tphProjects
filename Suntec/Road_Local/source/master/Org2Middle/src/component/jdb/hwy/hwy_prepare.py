# -*- coding: UTF-8 -*-
'''
Created on 2013-10-30

@author: hongchenzai
'''
import component.component_base


class HwyPrepare(component.component_base.comp_base):
    '''Highway数据制前的准备工作
       road_hwynode   ==> mid_road_hwynode_middle_nodeid
       road_roadpoint ==> mid_road_roadpoint_middle_nodeid
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Highway_Prepare')

    def _Do(self):
        # HwyNode的node_id转成中间表的node_id
        self._make_hwy_node()
        self._make_access_point()
        # 高速特征点(高速出入口点和高速设施所在点), 避免link合并，把这些点合并了。
        self._make_all_hwy_Node()
        return 0

    def _make_hwy_node(self):
        '''HwyNode的node_id转成中间表的node_id。'''
        self.CreateTable2('mid_road_hwynode_middle_nodeid')
        sqlcmd = """
        INSERT INTO mid_road_hwynode_middle_nodeid(
                    objectid, node_id, hwymode_f,
                    road_code, road_seq, direction_c,
                    inout_c, approachnode_id, flownode_id,
                    tollclass_c, tollfunc_c, dummytoll_f,
                    guide_f, tollgate_lane, name_kanji,
                    name_yomi, dummyname_f, facilclass_c,
                    dummyfacil_f, the_geom)
        (
        SELECT objectid, tile_node_id as node_id, hwymode_f,
               road_code, road_seq, direction_c,
               inout_c, approachnode_id, flownode_id,
               tollclass_c, tollfunc_c, dummytoll_f,
               guide_f, tollgate_lane, name_kanji,
               name_yomi, dummyname_f, facilclass_c,
               dummyfacil_f, the_geom
          FROM highway_node_add_nodeid
          left join middle_tile_node
          on node_id = old_node_id
          order by objectid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # index
        self.CreateIndex2('mid_road_hwynode_middle_nodeid_node_id_idx')
        # 还要check 个数是否正确。
        # ...
        return 0

    def _make_access_point(self):
        self.CreateTable2('mid_access_point_middle_nodeid')
        sqlcmd = """
        INSERT INTO mid_access_point_middle_nodeid
        (
        SELECT a.objectid, tile_node_id as node_id, old_node_id,
               hwymode_f, road_code, road_seq,
               direction_c, inout_c, direction_kanji,
               direction_yomi, toilet_f, handicap_f,
               restaurant_f, cafeteria_f, souvenir_f,
               resting_f, nap_f, infodesk_f, information_f,
               shower_f, laundry_f, spa_f,
               fax_f, mailbox_f, cashservice_f,
               hwyoasis_f, carwash_f, gs_f,
               multiplestore_f, dogrun_f, a.the_geom
          FROM access_point_4326 as a
          inner join road_node_4326 as b
          on ST_Equals(b.the_geom, a.the_geom)
          inner join middle_tile_node
          on b.objectid = old_node_id
          ORDER BY a.objectid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_access_point_middle_nodei_road_code_road_seq_direction__idx')

    def _make_all_hwy_Node(self):
        # 高速特征点(高速出入口点和高速设施所在点)
        self.CreateTable2('mid_all_highway_node')
        sqlcmd = """
            INSERT INTO mid_all_highway_node(node_id)
            (
            SELECT distinct node_id
              FROM (
                    select node_id
                      from mid_road_hwynode_middle_nodeid
                    union
                    select node_id
                      from mid_access_point_middle_nodeid
              ) AS A
              order by node_id
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # Create index: node_id已经是primary key
        # 临时修改builder error: hcz
        if self.CreateTable2('highway_facility_tbl') == -1:
            return -1
        if self.CreateTable2('highway_sa_pa_tbl') == -1:
            return -1
        return 0
