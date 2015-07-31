'''
Created on 2012-3-23

@author: sunyifeng
'''

import base
import common
import io
import os

class comp_node_toll_jdb(base.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Toll_Node')
        
    def _DoCreateTable(self):
        if self.CreateTable2('road_hwynode') == -1:
            return -1
        
        if self.CreateTable2('road_tollnode') == -1:
            return -1
        
        if self.CreateTable2('road_dicin') == -1:
            return -1
        
        return 0
    
    def _DoCreateIndex(self):
        
        if self.CreateIndex2('road_tollnode_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('road_hwynode_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('road_dicin_road_code_seq_num_idx') == -1:
            return -1
    
    def _DoCreateFunction(self):

        return 0
    
    def _Do(self):
        
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        highway_path = common.GetPath('highway')
        toll_path = common.GetPath('toll')
        
        f_tollnode = io.open(os.path.join(highway_path, 'tollnode.txt'), 'r', 8192, 'euc-jp')
        pgcur.copy_from(f_tollnode, 'road_tollnode', '\t', "", 8192, None)
        pg.commit2()
        f_tollnode.close()
        
        f_hwynode = io.open(os.path.join(highway_path, 'hwynode.txt'), 'r', 8192, 'euc-jp')
        pgcur.copy_from(f_hwynode, 'road_hwynode', '\t', "", 8192, None)
        pg.commit2()
        f_hwynode.close()
        
        f_dicin = io.open(os.path.join(toll_path, 'hwymode', 'dic.in'), 'r', 8192, 'euc-jp')
        pgcur.copy_from(f_dicin, 'road_dicin', ' ', "", 8192, None)
        pg.commit2()
        f_dicin.close()

        return 0
    

    
