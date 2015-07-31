# -*- coding: cp936 -*-
'''
Created on 2014-4-22

@author: zhaojie
'''
from base import comp_base
import common
class comp_park_jdb(comp_base):
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
        
        strrun_mode = common.common_func.GetPath('park_flag')
        if strrun_mode == 'true':
            self.__make_temp_node_for_base()
            self.__make_temp_link_for_base()
            
            self.__make_park_link_for_middle()
            self.__make_park_node_for_middle()
            self.__make_park_polygon_for_middle()
            pass
        return 0
    
    def __make_temp_node_for_base(self):
        self.log.info('start make temp_park_node for base_node!')
        
        self.CreateFunction2('mid_make_temp_park_node')
        self.pg.callproc('mid_make_temp_park_node')
        self.pg.commit2()
        
        self.log.info('end make temp_park_node for base_node!')
        return 0
       
    def __make_temp_link_for_base(self):
        self.log.info('start make temp_park_link for base_link!')
        
        self.CreateFunction2('mid_make_temp_park_link')
        self.pg.callproc('mid_make_temp_park_link')
        self.pg.commit2()
        
        self.log.info('end make temp_park_link for base_link!')
        return 0
    
    def __make_park_node_for_middle(self):
        self.log.info('start make park_node_tbl for middle!')
        
        self.CreateFunction2('mid_make_park_node_tbl')
        self.pg.callproc('mid_make_park_node_tbl')
        self.pg.commit2()
        
        self.log.info('end make park_node_tbl for middle!')
        return 0
    
    def __make_park_link_for_middle(self):
        self.log.info('start make park_link_tbl for middle!')
        
        self.CreateFunction2('mid_make_park_link_tbl')
        self.pg.callproc('mid_make_park_link_tbl')
        self.pg.commit2()
        
        self.log.info('end make park_link_tbl for middle!')
        return 0
    
    def __make_park_polygon_for_middle(self):
        self.log.info('start make park_polygon_tbl for middle!')
        
        self.CreateFunction2('mid_make_park_polygon_tbl')
        self.pg.callproc('mid_make_park_polygon_tbl')
        self.pg.commit2()
        
        self.log.info('end make park_polygon_tbl for middle!')
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('park_link_id_idx')
        self.CreateIndex2('park_node_id_idx')
        self.CreateIndex2('park_polygon_id_idx')
        return 0
    
    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
            
    def _DoCheckValues(self):
        self.log.info('start check values')
        sqlcmd = """
                 SELECT park_link_id
                 FROM park_link_tbl
                 WHERE park_s_node_id IS NULL or park_e_node_id is null
                 union
                 select park_link_id
                 from
                 (
                     select park_link_id,b.park_node_id as s_id,c.park_node_id as e_id
                     from park_link_tbl as a
                     left join
                     park_node_tbl as b
                     on a.park_s_node_id = b.park_node_id
                     left join
                     park_node_tbl as c
                     on a.park_e_node_id = c.park_node_id
                 )as d
                 where d.s_id is null or d.e_id is null;
                 """

        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('in park_link_tbl,park_s_node_id or park_e_node_id is error!')
            return -1
        self.log.info('end check values')