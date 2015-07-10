# -*- coding: cp936 -*-
'''
Created on 2012-3-6

@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log


class rdb_guideinfo_spotguide(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'SpotGuidepoint'
                          , 'spotguide_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_spotguidepoint'
                          , 'gid'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('SPOTGUIDE'))
        
    def Do(self):
        if self.CreateTable2('rdb_guideinfo_spotguidepoint') == -1:
            return -1
        
        sqlcmd = """
                  insert into rdb_guideinfo_spotguidepoint
                         (  in_link_id
                           ,in_link_id_t
                           ,node_id
                           ,node_id_t
                           ,out_link_id
                           ,out_link_id_t
                           ,"type"
                           ,passlink_count
                           ,pattern_id
                           ,arrow_id
                           ,point_list
                           ,is_exist_sar
                          )
                  SELECT  a.tile_link_id
                        , a.tile_id
                        , b.tile_node_id
                        , b.tile_id
                        , c.tile_link_id
                        , c.tile_id
                        , s."type"
                        , s.passlink_cnt
                        , d.gid
                        , e.gid
                        , f.data
                        , s.is_exist_sar
                    FROM 
                    spotguide_tbl as s
                    LEFT JOIN rdb_tile_link as a
                    ON cast(s.inlinkid as bigint) = a.old_link_id
                    LEFT JOIN rdb_tile_node as b
                    ON cast(s.nodeid as bigint) = b.old_node_id
                    LEFT JOIN rdb_tile_link as c
                    ON cast(s.outlinkid as bigint) = c.old_link_id
                    LEFT JOIN rdb_guideinfo_pic_blob_bytea as d
                    on lower(s.patternno) = lower(d.image_id)
                    LEFT JOIN rdb_guideinfo_pic_blob_bytea as e
                    on lower(s.arrowno) = lower(e.image_id)
                    LEFT JOIN temp_point_list as f
                    on lower(s.arrowno) = lower(f.image_id)
                    where d.gid is not null and e.gid is not null
                    order by s.gid;
                  """
        
        rdb_log.log(self.ItemName, 'insert data to spot guide ...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'query the spot guide data failed.', 'error')
            return -1
        
        self.pg.commit2()
        
        self.CreateIndex2('rdb_guideinfo_spotguidepoint_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_spotguidepoint_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_spotguidepoint_node_id_t_idx')
        
        rdb_log.log(self.ItemName, 'insert data to spot guide end.', 'info')
    
    def _DoCheckValues(self):
        'check字段值'
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists check_type;
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT check_type CHECK (type = ANY (ARRAY[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]));
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
            
    def _DoContraints(self):
        '添加外键'
        # 检查Arrow图片ID
        self._DoCheckArrowID()
        
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists rdb_guideinfo_spotguidepoint_in_link_id_fkey;           
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_in_link_id_fkey FOREIGN KEY (in_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists rdb_guideinfo_spotguidepoint_pattern_id_fkey;
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_pattern_id_fkey FOREIGN KEY (pattern_id)
                      REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists rdb_guideinfo_spotguidepoint_node_id_fkey;
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_node_id_fkey FOREIGN KEY (node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists rdb_guideinfo_spotguidepoint_out_link_id_fkey;    
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT rdb_guideinfo_spotguidepoint_out_link_id_fkey FOREIGN KEY (out_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
        
    def _DoCheckArrowID(self):
        '检查Arrow图片ID.'
        rdb_log.log(self.ItemName, 'Check Arrow_ids of SpotGuide.', 'info') 
        sqlcmd = """
                select 
                    (
                    select count(gid)
                     FROM rdb_guideinfo_spotguidepoint
                    ),
                    (SELECT count(gid)
                      FROM rdb_guideinfo_spotguidepoint
                      where arrow_id is null or arrow_id = 0
                    );
                """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            all_num   = row[0]
            arrow_num = row[1]
            # 所有的Arrow_id都为0
            if all_num == arrow_num:
                return 0
            # 没有Arrow_id为0, 那就给Arrow_id创建外键
            if arrow_num == 0:
                sqlcmd = """
                        ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists rdb_guideinfo_spotguidepoint_arrow_id_fkey;
                        ALTER TABLE rdb_guideinfo_spotguidepoint
                          ADD CONSTRAINT rdb_guideinfo_spotguidepoint_arrow_id_fkey FOREIGN KEY (arrow_id)
                              REFERENCES rdb_guideinfo_pic_blob_bytea (gid) MATCH FULL
                              ON UPDATE NO ACTION ON DELETE NO ACTION;
                        """
                self.pg.execute2(sqlcmd) 
                self.pg.commit2()
                return 0
            # 有一部分Arrow_id为0, 输出Warning提示
            rdb_log.log(self.ItemName, 'There are ' + str(arrow_num) +' records(Arrow_id is 0 or NUll).', 'warning')
         
        return 0

