# -*- coding: cp936 -*-
'''
Created on 2014-4-22

@author: zhaojie
'''
from base import comp_base
class comp_park_axf(comp_base):
    '''illust
    '''

    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'park')
        
    def _DoCreateTable(self):
        self.CreateTable2('park_link_tbl')     
        self.CreateTable2('park_node_tbl')
        self.CreateTable2('park_polygon_tbl')
        self.CreateTable2('temp_park_node')
        self.CreateTable2('temp_park_link')
        return 0
    
    def _Do(self):
        self.__make_temp_node_for_base()
        self.__make_temp_link_for_base()
        
        self.__make_park_link_for_middle()
        self.__make_park_node_for_middle()
        self.__make_park_polygon_for_middle() 
        return 0
    
    def __make_temp_node_for_base(self):
        self.log.info('start make temp_park_node for base_node!')
        
#        self.CreateFunction2('mid_make_temp_park_node')
#        self.pg.callproc('mid_make_temp_park_node')
#        self.pg.commit2()
        
        self.log.info('end make temp_park_node for base_node!')
        return 0
       
    def __make_temp_link_for_base(self):
        self.log.info('start make temp_park_link for base_link!')
        
#        self.CreateFunction2('mid_make_temp_park_link')
#        self.pg.callproc('mid_make_temp_park_link')
#        self.pg.commit2()
        
        self.log.info('end make temp_park_link for base_link!')
        return 0
    
    def __make_park_node_for_middle(self):
        self.log.info('start make park_node_tbl for middle!')
        
#        self.CreateFunction2('mid_park_get_linkarray')
#        self.CreateFunction2('mid_make_park_node_tbl')
#        self.pg.callproc('mid_make_park_node_tbl')
#        self.pg.commit2()
        
        self.log.info('end make park_node_tbl for middle!')
        return 0
    
    def __make_park_link_for_middle(self):
        self.log.info('start make park_link_tbl for middle!')
        
#        self.CreateFunction2('mid_make_park_link_tbl')
#        self.pg.callproc('mid_make_park_link_tbl')
#        self.pg.commit2()
        
        self.log.info('end make park_link_tbl for middle!')
        return 0
    
    def __make_park_polygon_for_middle(self):
        self.log.info('start make park_polygon_tbl for middle!')
        
#        self.CreateFunction2('mid_make_park_polygon_tbl')
#        self.pg.callproc('mid_make_park_polygon_tbl')
#        self.pg.commit2()
        
        self.log.info('end make park_polygon_tbl for middle!')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('park_link_id_idx')
        self.CreateIndex2('park_node_id_idx')
        self.CreateIndex2('park_polygon_id_idx')
        return 0