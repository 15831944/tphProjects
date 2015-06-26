# -*- coding: cp936 -*-
'''
Created on 2012-3-21

@author: zhangliang
'''

import component.default.guideinfo_lane
from component.jdb.guideinfo_lane_arrow_optimize_jdb import comp_guideinfo_lane_arrow_optimize_jdb


class comp_guideinfo_lane_jdb(component.default.guideinfo_lane.comp_guideinfo_lane):
    '''�յ�������Ϣ��(�ձ���)
    '''

    def __init__(self):
        '''
        Constructor
        '''
       
        component.default.guideinfo_lane.comp_guideinfo_lane.__init__(self)
      
    def _DoCreateTable(self):
        
        component.default.guideinfo_lane.comp_guideinfo_lane._DoCreateTable(self)
        # ���������м��м�link��������������node
        self.CreateTable2('temp_lane_strand_tbl')
        # ����������м�link������
        self.CreateTable2('temp_node_lane')
        
        # ֻ��id���м�link������
        self.CreateTable2('temp_node_lane_midcount')
        
        return 0
     
    def _DoCreateFunction(self):
        # �˺����Ѷ��ԭʼ���ݱ�����һ����ʱ��guidelane���У�
        self.CreateFunction2('make_temp_lane_strand_tbl')
        self.pg.callproc('make_temp_lane_strand_tbl')
        # �˺�����Arrowֵ1��8��ת��0�ȣ� 45��...315�ȣ� ���ǵ�ArrowΪ"0"(δ����)�Ƿ���NULL
        self.CreateFunction2('rdb_Lane_ArrowInfo_2Angle')
        
        #self.CreateFunction2('rdb_cnv_lane_passable_ipc')
        
        #self.CreateFunction2('rdb_cnv_lane_arrow_ipc')
        
        self.CreateFunction2('rdb_get_connected_linkid_ipc')
        
        self.CreateFunction2('rdb_get_connected_nodeid_ipc')
            
        self.CreateFunction2('rdb_get_connected_from_linkid_ipc')
        
        self.CreateFunction2('mid_getSplitLinkIDArray')
        
        self.CreateFunction2('mid_get_new_passlid')
        
        return 0 
        
    
    def _Do(self):
        self.__CreateTempNodeLane()
   
        # ��Lane��Ϣд��lane_tbl
        self.__CreateGuideInfoLane()
        self.__check_passlink()
        return 0
    
    def __CreateTempNodeLane(self):
        sqlcmd = """
                 insert into temp_node_lane(objectid, fromlinkid, tolinkid, passlid, lanedir, 
                            applane, rightchg, leftchg, passable, nodeid, from_nodeid)
                (
                SELECT A.objectid, from_link_id, to_link_id, temppasslid, lanedir_c
                       , approach_lane, a.right_change, a.left_change, from_lane_passable
                       , rdb_get_connected_nodeid_ipc(B.from_node_id, B.to_node_id, c.from_node_id, c.to_node_id) as nodeid
                       , rdb_get_connected_nodeid_ipc(D.from_node_id, D.to_node_id, E.from_node_id, E.to_node_id) as from_node_id
                  From (
                    SELECT objectid, from_link_id, to_link_id, 
                           mid_get_new_passlid(passlid, to_link_id, ',')  as temppasslid, 
                           lanedir_c, approach_lane, right_change, left_change, from_lane_passable, 
                           rdb_get_connected_linkid_ipc(to_link_id, passlid) as connected_linkid,
                           rdb_get_connected_from_linkid_ipc(from_link_id, passlid) as from_connected_linkid
                      FROM temp_lane_strand_tbl
                  ) as A
                  LEFT JOIN road_link as B
                  ON A.from_link_id = b.objectid
                  LEFT JOIN road_link as C
                  ON connected_linkid = C.objectid
                  LEFT JOIN road_link as D
                  ON A.to_link_id = D.objectid
                  LEFT JOIN road_link as E
                  ON from_connected_linkid = E.objectid
                  order by A.objectid
                );
                """
        #wb_log.log(self.ItemName, 'Now it is inserting to temp_node_lane_jdb...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            exit(1)
        else:
            self.pg.commit2()
            
        return 0
    
    def __CreateGuideInfoLane(self):
        # ��Lane��Ϣд��lane_tbl
        sqlcmd = """
                INSERT INTO lane_tbl(
                            id, inlinkid, nodeid, outlinkid, passlid, passlink_cnt, 
                            lanenum, laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo)
                (
                SELECT  objectid, A.tile_link_id, B.tile_node_id, C.tile_link_id, 
                        passlid, 
                        (CASE WHEN passlid is null or passlid = '' THEN 0 ELSE array_upper(regexp_split_to_array(passlid, E'\\\|+'), 1) END) as passlink_cnt,
                        applane::smallint, passable, 
                        rdb_Lane_ArrowInfo_2Angle(lanedir), 
                        leftchg::smallint, rightchg::smallint, '00000000000000000'
                  FROM (
                       select objectid, fromlinkid, nodeid, tolinkid, passlid, lanedir, 
                              applane, rightchg, leftchg, passable, from_nodeid
                         from temp_node_lane
                         --where rdb_Lane_ArrowInfo_2Angle(lanedir) is not null
                         order by objectid
                       ) as ln
                  LEFT JOIN middle_tile_link AS A
                  ON fromlinkid = A.old_link_id and nodeid = Any(A.old_s_e_node)
                  LEFT JOIN middle_tile_node AS B
                  ON nodeid = B.old_node_id
                  LEFT JOIN middle_tile_link AS C
                  ON tolinkid = C.old_link_id and from_nodeid = Any(C.old_s_e_node)
                  order by objectid
                );
                """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        return 0

    def __check_passlink(self):
        self.CreateFunction2('mid_check_guide_item_new')
        check_sql = '''
            select mid_check_guide_item_new({0});
        '''
#         try:
        self.pg.execute2(check_sql.format("\'lane_tbl\'"))
        row = self.pg.fetchone2()
        if row[0]:
            self.log.warning('passlink is not continuous,num=%s!!!', row[0])
            pass
#         except Exception:
#             self.log.error('passlink is not continuous!!!')
#             self.pg.commit2()
#         return
