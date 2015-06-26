# -*- coding: cp936 -*-
'''
Created on 2012-11-13

@author: zhangliang
'''
import component.component_base

class comp_vics(component.component_base.comp_base):
    '''
    vics tmc middle table
    '''

    def __init__(self):
        '''
        Constructors
        '''
        component.component_base.comp_base.__init__(self, 'vics tmc info')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('vics_ipc_link_tbl')
        
        self.CreateTable2('vics_link_tbl')
        
    def _DoCreateIndex(self):
        self.CreateIndex2('vics_link_tbl_linkid_dir_idx')  
        self.CreateIndex2('vics_ipc_link_tbl_linkid_dir_idx')  
        return 0 
       
    def _DoCreateFunction(self):
        return 0
    