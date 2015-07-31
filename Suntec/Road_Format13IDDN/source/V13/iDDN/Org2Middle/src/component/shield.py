# -*- coding: cp936 -*-
'''
Created on 2013-1-11

@author: hongchenzai
'''
from base import comp_base

class comp_shield(comp_base):
    '''
    shield
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'Shield')
        
    def _DoCreateTable(self):
        # 道路名称与字典关系表
        self.CreateTable2('link_shield_tbl')
        
    def _DoCreateIndex(self):
        self.CreateIndex2('link_shield_tbl_link_id_idx')  
       
    def _DoCreateFunction(self):
        
        return 0
    
    
    
    
    
        