# -*- coding: cp936 -*-
'''
Created on 2013-1-11

@author: hongchenzai
'''
import component.component_base

class comp_shield(component.component_base.comp_base):
    '''
    shield
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Shield')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('link_shield_tbl')
        
    def _DoCreateIndex(self):
        self.CreateIndex2('link_shield_tbl_link_id_idx')  
       
    def _DoCreateFunction(self):
        
        return 0
    
    
    
    
    
        