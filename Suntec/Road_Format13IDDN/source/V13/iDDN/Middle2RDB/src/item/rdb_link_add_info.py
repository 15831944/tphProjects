# -*- coding: cp936 -*-
'''
Created on 2011-12-5

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_link_add_info(ItemBase):
    '''
    Link的付加信息类
    '''

    def __init__(self):
        ItemBase.__init__(self, 'Link_add_info')
       
    def Do_CreateTable(self):
        return self.CreateTable2('rdb_link_add_info')
    
    def Do_CreateFunction(self):
        'Create Function'
        if self.CreateFunction2('rdb_cnv_road_struct_code') == -1:
            return -1
        if self.CreateFunction2('rdb_cnv_path_extra_info') == -1:
            return -1        
        return 0
     
    def Do(self):
        sqlcmd1 = """
                INSERT INTO rdb_link_add_info(link_id, link_id_t, struct_code, 
                    shortcut_code, parking_flag, etc_lane_flag, path_extra_info)
                (
                  SELECT   tile_link_id, tile_id, struct_code
                        , 0 as shortcut_code
                        , 0, etc_lane_flag
                        , path_extra_info
                  FROM 
                  (
                     SELECT  a.link_id, 
                             rdb_cnv_road_struct_code(elevated, structure, tunnel, 
                                 rail_cross) AS struct_code,
                             (case when etc_only_flag = 1 then 1 
                                   when b.gid is not null then 2 
                                   else 0 
                             end) as etc_lane_flag,
                             rdb_cnv_path_extra_info(paved, uturn) AS path_extra_info                             
                     FROM link_tbl as a
                     left join (
                         select * from spotguide_tbl where type in (9,10)
                     ) as b
                     on a.link_id = b.inlinkid
                     where elevated <> 0 or tunnel <> 0 or structure <> 0 
                            or rail_cross <> 0
                            or (etc_only_flag <> 0 or b.gid is not null)
                            or paved = 0 or uturn = 1
                  ) AS A
                  LEFT JOIN rdb_tile_link
                  ON  old_link_id = link_id
                  ORDER BY tile_link_id
                );
            """
            
        if self.pg.execute2(sqlcmd1) == -1:
            rdb_log.log(self.ItemName, 'Insert record into rdb_link_add_info failed.', 'info') 
            return -1
        else:
            self.pg.commit2()
            
        return 0
    
    def Do_CreatIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_link_add_info_link_id_idx')
        self.CreateIndex2('rdb_link_add_info_link_id_t_idx')
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_link_add_info DROP CONSTRAINT if exists rdb_link_add_info_link_id_fkey;
                ALTER TABLE rdb_link_add_info
                  ADD CONSTRAINT rdb_link_add_info_link_id_fkey FOREIGN KEY (link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        # 检查link的link_add_info_flag
        self._DoCheckLinkAddInfoFlg()
        return 0
        
    def _DoCheckLinkAddInfoFlg(self):
        '检查link的link_add_info_flag'
        
        sqlcmd = """
                SELECT count(link_id)
                  from (
                    SELECT A.link_id, B.link_add_info_flag
                      FROM rdb_link_add_info AS A
                      LEFT JOIN rdb_link AS B
                      ON A.link_id = B.link_id
                    ) as c
                  where link_add_info_flag <> True;
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] > 0:
                rdb_log.log(self.ItemName, 'Not all link_add_info_flag have been set for link_add_info_flag.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Checked link_add_info_flag for link_add_info_flag.', 'info')
        return 0 
    
    def _DoCheckNumber(self):
        'check条目'
        
        sqlcmd = """
            select 
                (SELECT  count(a.link_id)
                  FROM link_tbl as a
                  left join (select * from spotguide_tbl where type in (9,10)) as b
                  on a.link_id = b.inlinkid
                  where elevated <> 0 or tunnel <> 0 or structure <> 0 or rail_cross <> 0 or (etc_only_flag <> 0 or b.gid is not null)
                  or paved = 0 or uturn = 1
                ) as src_num,
                (SELECT count(link_id) FROM rdb_link_add_info
                ) as desc_num;
            """
        
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            src_num  = row[0]
            desc_num = row[1]
            if src_num != desc_num:
                rdb_log.log(self.ItemName, 'Number of [elevated/tunnel/structure/rail_cross/etc_only_flag/paved/uturn]: ' + str(src_num) + ','\
                                           'But Number of rdb_link_add_info: ' + str(desc_num) + '.', 'warning')
            else:
                rdb_log.log(self.ItemName, 'Number of [elevated/tunnel/structure/rail_cross/etc_only_flag/paved/uturn] and rdb_link_add_info: ' + str(src_num) + '.', 'info')
        else:
            rdb_log.log(self.ItemName, "Can't find Number of [elevated/tunnel/structure/rail_cross/etc_only_flag/paved/uturn]  or  rdb_link_add_info .", 'warning')
        return 0