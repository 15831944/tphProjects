# -*- coding: UTF8 -*-
'''
Created on 2012-12-6

@author: yuanrui
'''
import base
import os

class comp_topo_msm(base.component_base.comp_base):
    '''
    make topology data
    '''
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Topo')

    def _Do(self):
                   
        self.log.info(self.ItemName + ': Now it is making topology data...')
        
        self.CreateIndex2('org_processed_line_folder_idx')
        self.CreateIndex2('org_processed_line_folder_link_id_idx')
        
        #self.log.info('11')
        self.CreateTable2('temp_topo_folderid')
        self.CreateTable2('temp_topo_link_point')   
        self.CreateTable2('temp_topo_point')
        self.CreateTable2('temp_topo_point_3cm')
        #self.log.info('22')
        self.CreateIndex2('temp_topo_point_3cm_ix_idx')
        self.CreateIndex2('temp_topo_point_3cm_iy_idx')
        self.CreateIndex2('temp_topo_point_3cm_ix_m_idx')
        self.CreateIndex2('temp_topo_point_3cm_iy_m_idx')
        #self.log.info('33')
        self.CreateTable2('temp_topo_near_point')
        self.CreateIndex2('temp_topo_near_point_aid_idx')
        self.CreateIndex2('temp_topo_near_point_bid_idx')
        #self.log.info('44')
        self.CreateTable2('temp_topo_node')
        self.CreateTable2('temp_topo_point_node')
        self.CreateTable2('temp_topo_walk_point')
        #self.log.info('55')
        self.CreateFunction2('temp_make_malsing_node')
        self.pg.callproc('temp_make_malsing_node')
        self.pg.commit2()
        #self.log.info('66')
        self.CreateIndex2('temp_topo_node_node_id_idx')
        self.CreateIndex2('temp_topo_node_the_geom_idx')
        self.CreateIndex2('temp_topo_point_node_x_idx')
        self.CreateIndex2('temp_topo_point_node_y_idx')
        self.CreateIndex2('temp_topo_point_node_z_idx')
        #self.log.info('77')
        self.CreateTable2('temp_topo_link')
        
        self.CreateIndex2('temp_topo_link_new_link_id_idx')
        self.CreateIndex2('temp_topo_link_folder_link_id_idx')
        self.CreateIndex2('temp_topo_link_the_geom_idx')
        
        self.log.info(self.ItemName + ': end of making topology data...')
      
