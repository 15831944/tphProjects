# -*- coding: cp936 -*-
'''
Created on 2012-3-7

@author: sunyifeng
'''
from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX
from common import rdb_log

class rdb_guideinfo_lane(ItemBase):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Lane'
                          , 'lane_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_lane'
                          , 'guideinfo_id'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('LANE'))
        
    
    def Do_CreateTable(self):
        
        if self.CreateTable2('rdb_guideinfo_lane') == -1:
            return -1

        return 0
        
    def Do_CreatIndex(self):
        
        if self.CreateIndex2('rdb_guideinfo_lane_in_link_id_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_lane_node_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdb_guideinfo_lane_node_id_t_idx') == -1:
            return -1
        
        return 0
    
    def Do_CreateFunction(self):
        self.CreateFunction2('rdb_getpasslinkcnt')
        self.CreateFunction2('rdb_cnv_lane_passable')
        self.CreateFunction2('rdb_u_turn_dir')        
        self.CreateFunction2('rdb_make_arrowinfo_from_link_ipc')
        self.CreateFunction2('rdb_cal_zm')
        self.CreateFunction2('rdb_cal_zm_lane')
        self.CreateFunction2('rdb_convert_guidetype')
        return self.CreateFunction2('rdb_convert_angle')
        
    def Do(self):
        

        sqlcmd = """
                  insert into rdb_guideinfo_lane
                                (   guideinfo_id
                                  , in_link_id
                                  , in_link_id_t
                                  , node_id
                                  , node_id_t
                                  , out_link_id
                                  , out_link_id_t
                                  , lane_num
                                  , lane_info
                                  , arrow_info
                                  , lane_num_l
                                  , lane_num_r
                                  , passlink_count
                                  )
                  SELECT ln.gid
                       , a.tile_link_id as inlinkid
                       , a.tile_id as in_tile_id
                       , b.tile_node_id as nodeid
                       , b.tile_id as node_tile_id
                       , c.tile_link_id as outlinkid
                       , c.tile_id as out_tile_id
                       , ln.lanenum
                       , rdb_cnv_lane_passable(ln.lanenum, ln.laneinfo)
                       , case   when (ln.arrowinfo is null and h.guidetype is not null) then rdb_convert_guidetype(h.guidetype,cast(h.flag as smallint))
                                when (ln.arrowinfo is null and h.guidetype is null) then rdb_make_arrowinfo_from_link_ipc(ln.inlinkid,ln.nodeid,ln.passlid,ln.outlinkid)
                                else ln.arrowinfo 
                         end as arrowinfo
                       , ln.lanenuml
                       , ln.lanenumr
                       , rdb_getpasslinkcnt(ln.passlid, null)
                  FROM 
                  (select * from lane_tbl order by id) as ln
                  LEFT JOIN rdb_tile_link as a
                  ON a.old_link_id = cast(ln.inlinkid as bigint)
                  LEFT JOIN rdb_tile_node as b
                  ON b.old_node_id = cast(ln.nodeid as bigint)
                  LEFT JOIN rdb_tile_link as c
                  ON c.old_link_id = cast(ln.outlinkid as bigint)
                  left join (
                        select  a.inlinkid, a.outlinkid, a.passlid, a.arrowinfo,
                                b.guide_type as guidetype,
                                (case when (c.extend_flag & 1) = 0 then 0 else 1 end) as flag 
                        from (select * from lane_tbl where arrowinfo is null) as a
                        inner join force_guide_tbl b 
                            on  a.inlinkid = b.inlinkid 
                            and a.outlinkid = b.outlinkid 
                            and not (a.passlid is distinct from b.passlid)
                            and b.guide_type not in (0,1) and b.guide_type < 32
                        left join link_tbl as c
                            on a.inlinkid = c.link_id
                  ) h
                  on ln.inlinkid=h.inlinkid and ln.outlinkid=h.outlinkid and not (ln.passlid is distinct from h.passlid)
                  order by ln.gid             
                  """
        rdb_log.log(self.ItemName, 'Start making rdb_guideinfo_lane', 'info') 
        
        if self.pg.execute2(sqlcmd) == -1:
            rdb_log.log(self.ItemName, 'making rdb_guideinfo_lane data failed.', 'error')
            return -1
        
        self.pg.commit2()
    
        rdb_log.log(self.ItemName, 'succeeded to make rdb_guideinfo_lane.', 'info')
 
        return 0
    
    def _DoCheckValues(self):
        # check_arrow_info没法用，因为海外的arrow是个组合值
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists check_lane_num;
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT check_lane_num CHECK (lane_num <= 16 AND lane_num >= 1);
                  
                --ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists check_arrow_info;
                --ALTER TABLE rdb_guideinfo_lane
                --  ADD CONSTRAINT check_arrow_info CHECK (arrow_info = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]));
                
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists check_lane_num_l;
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT check_lane_num_l CHECK (lane_num_l >= (-7) AND lane_num_l <= 7);
                  
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists check_lane_num_r;
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT check_lane_num_r CHECK (lane_num_r >= (-7) AND lane_num_r <= 7);
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        return 0
    
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists rdb_guideinfo_lane_in_link_id_fkey;  
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT rdb_guideinfo_lane_in_link_id_fkey FOREIGN KEY (in_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                 
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists rdb_guideinfo_lane_node_id_fkey;      
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT rdb_guideinfo_lane_node_id_fkey FOREIGN KEY (node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_guideinfo_lane DROP CONSTRAINT if exists rdb_guideinfo_lane_out_link_id_fkey; 
                ALTER TABLE rdb_guideinfo_lane
                  ADD CONSTRAINT rdb_guideinfo_lane_out_link_id_fkey FOREIGN KEY (out_link_id)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
        
        

        