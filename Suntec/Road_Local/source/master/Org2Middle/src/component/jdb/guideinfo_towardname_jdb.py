# -*- coding: cp936 -*-
'''
Created on 2012-3-28

@author: hongchenzai
'''
import component.component_base

class comp_guideinfo_towardname_jdb(component.component_base.comp_base):
    '''继承于comp_guideinfo_towardname_jpn
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self,'Guideinfo_TowardName')
        
    def _Do(self):
        self.__CreateTempNodeTowardName()
   
        # 把方面名称信息写入towardname_tbl
        self.__CreateGuideInfoTowardName()
        return 0
    def _DoCreateTable(self):
        
        self.CreateTable2('towardname_tbl')   
        # 这个表不包含中间link的数量
        self.CreateTable2('temp_node_towardname')
        
        return 0
        
    def _DoCreateFunction(self):
        'Create Function'
        self.CreateFunction2('rdb_get_connected_linkid_ipc')       
        self.CreateFunction2('rdb_get_connected_nodeid_ipc')           
        self.CreateFunction2('rdb_get_connected_from_linkid_ipc')     
        self.CreateFunction2('mid_getSplitLinkIDarray_dir')       
        self.CreateFunction2('mid_get_new_passlid_dir')
        
        
        return 0

    def __CreateTempNodeTowardName(self):
        sqlcmd = """
                INSERT INTO temp_node_towardname(
                            gid, objectid, fromlinkid, tolinkid, midlinkid, midcount,link_dir_kib,
                            guideclass, name_kanji, toward_name, boardptn_c, board_seq, nodeid, from_nodeid)
                (
                SELECT  A.gid, A.objectid, fromlinkid, tolinkid, midlinkid, midcount, link_dir_kib, guideclass
                       , name_kanji, sp_name as toward_name, boardptn_c, board_seq 
                       , (case when link_dir_kib[1] = 1 then B.to_node_id else B.from_node_id end) as nodeid
                       , (case when link_dir_kib[midcount+2] = 1 then C.from_node_id else C.to_node_id end) as from_nodeid
                  From (
                        SELECT gid, objectid, fromlinkid, tolinkid, midlinkid, midcount, link_dir_kib,guideclass, 
                               name_kanji, sp_name, boardptn_c, board_seq 
                          FROM temp_road_dir
                          where mid_isNotAllDigit(name_kanji) = true
                       ) as A
                  LEFT JOIN road_link_4326 as B
                  ON fromlinkid = b.objectid
                  LEFT JOIN road_link_4326 as C
                  ON tolinkid = C.objectid
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
                            direction, guideattr, namekind, namekind2,toward_name, "type")
                (
                SELECT objectid, node_id, fromlinkid, tolinkid, passlid,
                       (CASE WHEN passlid is null  or passlid = '' THEN 0 ELSE array_upper(regexp_split_to_array(passlid, E'\\\|+'), 1) END) as passlink_cnt,
                        0, 0, 0, 0, toward_name, 1
                FROM (
                    SELECT temp_node_towardname.gid, objectid, A.tile_link_id as fromlinkid, 
                            B.tile_node_id as node_id, C.tile_link_id as tolinkid, 
                           mid_get_new_passlid_dir(midlinkid, link_dir_kib, ',')  as passlid,
                           guideclass, toward_name, boardptn_c, board_seq
                      FROM temp_node_towardname
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
    

        
    