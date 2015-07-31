# -*- coding: cp936 -*-
'''
Created on 2012-3-21

@author: zhangliang
'''

import component

class comp_guideinfo_lane_jpn(component.guideinfo_lane.comp_guideinfo_lane):
    '''诱导车线信息类(日本版)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.guideinfo_lane.comp_guideinfo_lane.__init__(self)
      
    def _DoCreateTable(self):
        
        component.guideinfo_lane.comp_guideinfo_lane._DoCreateTable(self)
        
        # 这个表不包含中间link的数量
        self.CreateTable2('temp_node_lane')
        
        # 只存id和中间link的数量
        self.CreateTable2('temp_node_lane_midcount')
        
        return 0
     
    def _DoCreateFunction(self):
        # 此函数把Arrow值1到8，转成0度， 45度...315度， 但是当Arrow为"0"(未调查)是返回NULL
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
   
        # 把Lane信息写入lane_tbl
        self.__CreateGuideInfoLane()
        return 0
    
    def __CreateTempNodeLane(self):
        sqlcmd = """
                insert into temp_node_lane(gid, objectid, fromlinkid, tolinkid, passlid, lanedir, 
                            applane, rightchg, leftchg, passable, nodeid, from_nodeid)
                (
                SELECT A.gid, A.objectid, fromlinkid, tolinkid, passlid, lanedir
                       , applane, rightchg, leftchg, passable
                       , rdb_get_connected_nodeid_ipc(B.fromnodeid, B.tonodeid, c.fromnodeid, c.tonodeid) as nodeid
                       , rdb_get_connected_nodeid_ipc(D.fromnodeid, D.tonodeid, E.fromnodeid, E.tonodeid) as from_node_id
                  From (
                    SELECT gid, objectid, fromlinkid, tolinkid, 
                           mid_get_new_passlid(midlinkid, tolinkid, ',')  as passlid, 
                           lanedir, applane, rightchg, leftchg, passable, 
                           rdb_get_connected_linkid_ipc(tolinkid, midlinkid) as connected_linkid,
                           rdb_get_connected_from_linkid_ipc(fromlinkid, midlinkid) as from_connected_linkid
                      FROM road_lan
                  ) as A
                  LEFT JOIN road_rlk as B
                  ON fromlinkid = b.objectid
                  LEFT JOIN road_rlk as C
                  ON connected_linkid = C.objectid
                  LEFT JOIN road_rlk as D
                  ON tolinkid = D.objectid
                  LEFT JOIN road_rlk as E
                  ON from_connected_linkid = E.objectid
                  order by A.gid
                );     
                """
        #wb_log.log(self.ItemName, 'Now it is inserting to temp_node_lane...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            exit(1)
        else:
            self.pg.commit2()
            
        return 0
    
    def __CreateGuideInfoLane(self):
        # 把Lane信息写入lane_tbl
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
    
    
    
    