# -*- coding: cp936 -*-
'''
Created on 2012-3-22

@author: hongchenzai
'''
import component.component_base

class comp_link(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link')
        
    def _DoCreateTable(self):
        return self.CreateTable2('rdb_link')
    
    def _DoCreateFunction(self):
        'Create Function'
        if self.CreateFunction2('rdb_cnv_road_disp_class') == -1:
            return -1
        #if self.CreateFunction2('rdb_cnv_road_type') == -1:
        #    return -1
        #if self.CreateFunction2('rdb_cnv_link_type') == -1:
        #    return -1
        #if self.CreateFunction2('rdb_cnv_link_toll') == -1:
        #    return -1
        #if self.CreateFunction2('rdb_cnv_road_function_code') == -1:
        #    return -1
        if self.CreateFunction2('rdb_cnv_road_direction') == -1:
            return -1
        if self.CreateFunction2('rdb_conn_linkid') == -1:
            return -1
        if self.CreateFunction2('rdb_cal_zm') == -1:
            return -1
        return 0
    
    def _DoCreateIndex(self):
        '创建相关表索引.'
        self.CreateIndex2('rdb_link_link_id_idx')
        self.CreateIndex2('rdb_link_the_geom_idx')
        self.CreateIndex2('rdb_link_start_node_id_idx')
        self.CreateIndex2('rdb_link_end_node_id_idx')
        self.CreateIndex2('rdb_link_link_id_t_idx')
        return 0
    
    def _DoCheckValues(self):
        'check字段值'
        sqlcmd = """
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_toll;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_toll CHECK (toll = ANY (ARRAY[0, 1, 2, 3]));
                
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_display_class;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_display_class CHECK (display_class = ANY (ARRAY[10, 11, 12, 20, 21, 31, 32, 41, 42, 51, 52, 53, 54, 100, 101, 102, 103, 104, 0]));
                
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_road_type;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_road_type CHECK (road_type = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]));
                
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_link_type;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_link_type CHECK (link_type = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]));
                
                ALTER TABLE rdb_link DROP CONSTRAINT if exists check_function_code;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT check_function_code CHECK (function_code = ANY (ARRAY[1, 2, 3, 4, 5]));
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0
            
    def _DoContraints(self):
        '添加外键'
        sqlcmd = """
                -- Create FOREIGN KEY
                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_end_node_id_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_end_node_id_fkey FOREIGN KEY (end_node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_lane_id_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_lane_id_fkey FOREIGN KEY (lane_id)
                      REFERENCES rdb_linklane_info (lane_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_start_node_id_fkey;
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_start_node_id_fkey FOREIGN KEY (start_node_id)
                      REFERENCES rdb_node (node_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                  
                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_fnlink_fkey;    
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_fnlink_fkey FOREIGN KEY (fnlink)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                   
                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_tnlink_fkey;      
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_tnlink_fkey FOREIGN KEY (tnlink)
                      REFERENCES rdb_link (link_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                
                ALTER TABLE rdb_link DROP CONSTRAINT if exists rdb_link_road_name_id_fkey; 
                ALTER TABLE rdb_link
                  ADD CONSTRAINT rdb_link_road_name_id_fkey FOREIGN KEY (road_name_id)
                      REFERENCES rdb_name_dictionary_chinese (name_id) MATCH FULL
                      ON UPDATE NO ACTION ON DELETE NO ACTION;
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            return 0