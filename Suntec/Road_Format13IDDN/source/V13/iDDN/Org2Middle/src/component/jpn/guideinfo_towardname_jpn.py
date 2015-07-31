# -*- coding: cp936 -*-
'''
Created on 2012-3-28

@author: hongchenzai
'''
import component

class comp_guideinfo_towardname_jpn(component.guideinfo_towardname.comp_guideinfo_towardname):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.guideinfo_towardname.comp_guideinfo_towardname.__init__(self)
    
    def _DoCreateTable(self):
        
        component.guideinfo_towardname.comp_guideinfo_towardname._DoCreateTable(self)
        
        # 这个表不包含中间link的数量
        self.CreateTable2('temp_node_towardname')
        
        return 0
        
    def _DoCreateFunction(self):
        'Create Function'
        self.CreateFunction2('rdb_get_connected_linkid_ipc')
        
        self.CreateFunction2('rdb_get_connected_nodeid_ipc')
            
        self.CreateFunction2('rdb_get_connected_from_linkid_ipc')
        
        self.CreateFunction2('mid_getSplitLinkIDArray')
        
        self.CreateFunction2('mid_get_new_passlid')
        
        return 0
    
    def _Do(self):
        self.__CreateTempNodeTowardName()
   
        # 把方面名称信息写入towardname_tbl
        self.__CreateGuideInfoTowardName()
        return 0
    
    def __CreateTempNodeTowardName(self):
        sqlcmd = """
                INSERT INTO temp_node_towardname(
                            gid, objectid, fromlinkid, tolinkid, midlinkid, midcount,
                            guideclass, name_kanji, name_yomi, boardptn_c, board_seq, nodeid, from_nodeid)
                (
                SELECT  A.gid, A.objectid, fromlinkid, tolinkid, midlinkid, midcount,guideclass
                       , name_kanji, name_yomi, boardptn_c, board_seq 
                       , rdb_get_connected_nodeid_ipc(B.fromnodeid, B.tonodeid, c.fromnodeid, c.tonodeid) as nodeid
                       , rdb_get_connected_nodeid_ipc(D.fromnodeid, D.tonodeid, E.fromnodeid, E.tonodeid) as from_nodeid
                  From (
                        SELECT gid, objectid, fromlinkid, tolinkid, midlinkid, midcount, guideclass, 
                               name_kanji, name_yomi, boardptn_c, board_seq, 
                               rdb_get_connected_linkid_ipc(tolinkid, midlinkid) as connected_linkid,
                               rdb_get_connected_from_linkid_ipc(fromlinkid, midlinkid) as from_connected_linkid
                          FROM road_dir
                          where mid_isNotAllDigit(name_kanji) = true
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
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        return 0
        
    def __CreateGuideInfoTowardName(self):
        sqlcmd = """
                INSERT INTO towardname_tbl(
                            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                            direction, guideattr, namekind, name_id, "type")
                (
                SELECT objectid, node_id, fromlinkid, tolinkid, passlid,
                       (CASE WHEN passlid is null  or passlid = '' THEN 0 ELSE array_upper(regexp_split_to_array(passlid, E'\\\|+'), 1) END) as passlink_cnt,
                        0, 0, 0, name_id, 1
                FROM (
                    SELECT temp_node_towardname.gid, objectid, A.tile_link_id as fromlinkid, 
                            B.tile_node_id as node_id, C.tile_link_id as tolinkid, 
                           mid_get_new_passlid(midlinkid, tolinkid, ',')  as passlid,
                           guideclass, name_id, boardptn_c, board_seq
                      FROM temp_node_towardname
                      LEFT JOIN temp_toward_name
                      ON temp_node_towardname.gid = temp_toward_name.gid
                      LEFT JOIN middle_tile_link AS A
                      ON fromlinkid = A.old_link_id and nodeid = Any(A.old_s_e_node)
                      LEFT JOIN middle_tile_node AS B
                      ON nodeid = B.old_node_id
                      LEFT JOIN middle_tile_link AS C
                      ON tolinkid = C.old_link_id and from_nodeid = Any(C.old_s_e_node)
                     ) AS d
                order by objectid, node_id, fromlinkid, tolinkid, gid
                );
            """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        return 0
    