# -*- coding: cp936 -*-
'''
Created on 2012-3-6
@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log

class rdb_guideinfo_spotguide(ItemBase):
    def __init__(self):
        ItemBase.__init__(self, 'SpotGuidepoint'
                          , 'spotguide_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_spotguidepoint'
                          , 'gid'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('SPOTGUIDE'))
        
    def Do(self):
        rdb_log.log(self.ItemName, 'insert data to spot guide ...', 'info')
        self.CreateTable2('rdb_guideinfo_spotguidepoint')
        if self._GenerateSpotguidePointFromSpotguideTbl() == -1:
            return -1
        rdb_log.log(self.ItemName, 'insert data to spot guide end.', 'info')
    
    def _GenerateSpotguidePointFromSpotguideTbl(self):
        # 关于字段arrow_id：
        # 当spotguide_tbl.arrowno为空时，说明该仕向地未提供arrow图（如seisis）。
        # 此时是否能找到arrow图不作为过滤条件，并将arrow_id置为0。
        # 当spotguide_tbl.arrowno不为空时，说明该仕向地提供arrow图（如rdf）。
        # 此时必须找到spotguide_tbl.arrowno对应的图片才做spotguide点。
        # arrow_id设置为pic_blob表中对应的图片gid。
        sqlcmd = """
                  INSERT INTO rdb_guideinfo_spotguidepoint
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
                        , case 
                          when s.arrowno is null then 0    -- sensis, toll station
                          else e.gid end    -- rdf etc.
                        , s.is_exist_sar
                    FROM 
                    spotguide_tbl as s
                    LEFT JOIN rdb_tile_link as a
                    ON cast(s.inlinkid as bigint) = a.old_link_id
                    LEFT JOIN rdb_tile_node as b
                    ON cast(s.nodeid as bigint) = b.old_node_id
                    LEFT JOIN rdb_tile_link as c
                    ON cast(s.outlinkid as bigint) = c.old_link_id
                    LEFT JOIN temp_guideinfo_pic_blob_id_mapping as d
                    on lower(s.patternno) = lower(d.image_id)
                    LEFT JOIN temp_guideinfo_pic_blob_id_mapping as e
                    on lower(s.arrowno) = lower(e.image_id)
                    where d.gid is not null and    -- pattern illust must be found.
                          (s.arrowno is null   -- sensis, toll station
                           or e.gid is not null)   -- rdf etc.
                    order by s.gid;
                  """
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'query the spot guide data failed.', 'error')
            return -1
        
        self.pg.commit2()
        self.CreateIndex2('rdb_guideinfo_spotguidepoint_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_spotguidepoint_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_spotguidepoint_node_id_t_idx')
        return 0
    
    def _DoCheckValues(self):
        # 此处不再进行check，全部转移到autocheck的rdb_check模块中进行。
        return 0
            
    def _DoContraints(self):
        # 添加外键:
        # 由于添加了toll station，arrow_id部分为零也属于合理情况，故不为arrow_id建立外键。
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

