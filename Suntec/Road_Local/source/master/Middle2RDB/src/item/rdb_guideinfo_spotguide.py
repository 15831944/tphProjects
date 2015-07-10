# -*- coding: cp936 -*-
'''
Created on 2012-3-6
@author: sunyifeng
'''

from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log

tollStationImageName = 'toll_station_image' # 此图由机能组指定。
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
        self._GenerateSpotguidePointForTollStation()
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
                        , case 
                          when s.arrowno is null then 0    -- sensis, toll station
                          else e.gid end    -- rdf etc.
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
                    LEFT JOIN temp_guideinfo_pic_blob_id_mapping as d
                    on lower(s.patternno) = lower(d.image_id)
                    LEFT JOIN temp_guideinfo_pic_blob_id_mapping as e
                    on lower(s.arrowno) = lower(e.image_id)
                    LEFT JOIN temp_point_list as f
                    on lower(s.arrowno) = lower(f.image_id)
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
    
    # 将加油站作成spotguide点填充到中间表spotguide_tbl表中
    def _GenerateSpotguidePointForTollStation(self):
        # 从node_tbl搜刮出toll_flag=1的点。
        # 一些仕向地提供了toll station数据及元数据图片，这些数据在o2m的时候已被做到spotguide_tbl表中。
        # 为防止与这些toll station点重复，这里使用spotguide_tbl对搜出的toll_station列表进行了过滤。
        sqlcmd = """
                select a.node_id, array_agg(b.link_id) as slinkid, array_agg(b.one_way_code) as soneway, 
                array_agg(c.link_id) as elinkid, array_agg(c.one_way_code) as eoneway
                from
                node_tbl as a
                left join link_tbl as b
                on a.node_id=b.s_node
                left join link_tbl as c
                on a.node_id=c.e_node
                left join spotguide_tbl as d
                on a.node_id=d.nodeid
                where a.toll_flag=1
                and d.nodeid is null
                group by a.node_id
          """
          
        # arrowno字段留空，在作成rdb_guideinfo_spotguidepoint不考虑是否存在arrow图。
        # toll station 做成spotguide点时type定为12。
        # toll station 默认没有sar
        spotguide_tbl_insert_str = '''
                insert into spotguide_tbl(nodeid, inlinkid, outlinkid,
                                          passlid, passlink_cnt, direction,
                                          guideattr, namekind, guideclass,
                                          patternno, type,
                                          is_exist_sar)
                  values(%d, %d, %d,
                         '', 0, 0,
                         0, 0, 0,
                         %s, 12,
                         false)
            '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        for row in rows:
            node_id=row[0]
            sNodeLinkList = row[1] # 以此node为s_node的link列表
            sOneWayList = row[2] # 对应的one_way_code列表
            eNodeLinkList = row[3] # 以此node为e_node的link列表
            eOneWayList = row[4] # 对应的one_way_code列表
            
            inLinkList = []
            outLinkList = []
            # 根据s_node列表和它对应的one_way_code找出该node的inlink列表和outlink列表。
            for x in zip(sNodeLinkList, sOneWayList):
                if x[1] == 1:
                    inLinkList.append(x[0])
                    outLinkList.append(x[0])
                elif x[1] == 2:
                    outLinkList.append(x[0])
                elif x[1] == 3:
                    inLinkList.append(x[0])
                else:
                    continue
                
            # 根据e_node列表和它对应的one_way_code找出该node的inlink列表和outlink列表。
            for x in zip(eNodeLinkList, eOneWayList):
                if x[1] == 1:
                    inLinkList.append(x[0])
                    outLinkList.append(x[0])
                elif x[1] == 2:
                    inLinkList.append(x[0])
                elif x[1] == 3:
                    outLinkList.append(x[0])
                else:
                    continue
            
            # 根据求出的inlink列表和outlink列表作成inlink --> node --> outlink值对。 
            for oneInLink in inLinkList:
                for oneOutLink in outLinkList:
                    if oneInLink != oneOutLink:
                        if self.pg.execute2(spotguide_tbl_insert_str%\
                                            (node_id, oneInLink, oneOutLink, tollStationImageName)) == -1:
                            return -1
        
        self.pg.commit2()
        return 0
    
    def _DoCheckValues(self):
        'check字段值'
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_spotguidepoint DROP CONSTRAINT if exists check_type;
                ALTER TABLE rdb_guideinfo_spotguidepoint
                  ADD CONSTRAINT check_type CHECK (type = ANY (ARRAY[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12]));
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
            
    def _DoContraints(self):
        '添加外键'
        # 检查Arrow图片ID
        self.checkArrowIdAndAddConstraint()
        
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
        
    def checkArrowIdAndAddConstraint(self):
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
    
