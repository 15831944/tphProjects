'''
Created on 2011-12-21

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log

class rdb_linklane_info(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Link Lane')
        
    def Do(self):
        
        self.CreateTempLinkLaneInfo()
        
        self.CreateLinkLaneInfo()
        
        return 0
    
    def CreateTempLinkLaneInfo(self):
        
        if self.CreateTable2('temp_rdb_linklane_info_p') == -1:
            exit(1)
        
        if self.CreateTable2('temp_rdb_linklane_info') == -1:
            exit(1)
        
        if self.CreateFunction2('rdb_linkdir') == -1:
            exit(1)
        
        if self.CreateFunction2('rdb_linkwidth') == -1:
            exit(1)
        
        sqlcmd = """
                insert into temp_rdb_linklane_info_p(disobey_flag, lane_dir, lane_up_down_distingush,
                ops_lane_number, ops_width, neg_lane_number, neg_width, old_link_id) 
                (
                select disobey_flag = 1 as disobey_flag, 
                    rdb_linkdir(one_way_code) as lane_dir, 
                    one_way_code in (2,3) as lane_up_down_distingush,
                    lane_num_s2e as ops_lane_number,
                    width_s2e as ops_width,
                    lane_num_e2s as neg_lane_number,
                    width_e2s as neg_width,
                    link_id as old_link_id
                from link_tbl order by disobey_flag, lane_dir, up_down_distinguish,
                     ops_lane_number, ops_width, neg_lane_number, neg_width
                );
                """
                
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd = """
                insert into temp_rdb_linklane_info(disobey_flag, lane_dir, lane_up_down_distingush,
                ops_lane_number, ops_width, neg_lane_number, neg_width, lane_id, old_link_id) 
                (
                    select disobey_flag, lane_dir, lane_up_down_distingush,
                                    ops_lane_number, ops_width, neg_lane_number, neg_width, min(lane_id),
                                    unnest(array_agg(old_link_id)) as old_link_id
                    from temp_rdb_linklane_info_p
                    group by 
                    disobey_flag, lane_dir, lane_up_down_distingush,
                    ops_lane_number, ops_width, neg_lane_number, neg_width
                )
                """
        
        rdb_log.log('Link Lane', 'Inserting record to temp_rdb_linklane_info...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            exit(1)
        else:
            self.pg.commit2()
#            self.CreateIndex2('temp_rdb_linklane_info_oldlinkid_idx')
    
    def CreateLinkLaneInfo(self):
        
        if self.CreateTable2('rdb_linklane_info') == -1:
            exit(1)
                
        sqlcmd = """
                insert into rdb_linklane_info(lane_id, disobey_flag, lane_dir, lane_up_down_distingush,
                ops_lane_number, ops_width, neg_lane_number, neg_width ) 
                (
                select distinct lane_id,
                disobey_flag, 
                lane_dir, 
                lane_up_down_distingush,
                ops_lane_number,
                ops_width,
                neg_lane_number,
                neg_width 
                from temp_rdb_linklane_info
                order by lane_id
                ) 
                """
        
        rdb_log.log('Link Lane', 'Inserting record to rdb_linklane_info...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            exit(1)
        else:
            self.pg.commit2()
        
        self.CreateIndex2('rdb_linklane_info_lane_id_idx')
