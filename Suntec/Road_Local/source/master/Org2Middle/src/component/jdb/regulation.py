# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author: liuxinxing
'''



import component.component_base

class comp_regulation_jdb(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
    
    def _DoCreateTable(self):
        
        self.CreateTable2('condition_regulation_tbl')
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
        return 0
    
    def _Do(self):
        self.__convert_condition_table()
        self.__convert_regulation_from_trn()
        self.__convert_regulation_from_owc()
        self.__convert_regulation_from_npc()
        
        return 0
    
    def __convert_condition_table(self):
        self.log.info('Begin convert condition_regulation_tbl...')
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateFunction2('mid_convert_weekflag')
        self.CreateFunction2('mid_convert_condition_regulation_table')
        
        self.pg.callproc('mid_convert_condition_regulation_table')
        
        self.pg.commit2()
        
        self.log.info('End convert condition_regulation_tbl.')
    
    def __convert_regulation_from_trn(self):
        self.log.info('Begin convert regulation from road_trn...')
        
        self.CreateFunction2('mid_convert_regulation_from_trn')
        self.CreateFunction2('mid_getConnectionNodeID')
        self.CreateFunction2('rdb_get_connected_nodeid_ipc')
        self.CreateFunction2('mid_getSplitLinkID')
        self.CreateFunction2('mid_getSplitLinkIDArray')
        
        self.pg.callproc('mid_convert_regulation_from_trn')
        
        self.pg.commit2()
        
        self.log.info('End convert regulation from road_trn.')
        
    def __convert_regulation_from_owc(self):
        self.log.info('Begin convert regulation from road_owc...')
        
        self.CreateFunction2('mid_convert_regulation_from_owc')
        
        self.pg.callproc('mid_convert_regulation_from_owc')
        
        self.pg.commit2()
        
        self.log.info('End convert regulation from road_owc.')
        
    def __convert_regulation_from_npc(self):
        self.log.info('Begin convert regulation from road_npc...')
        
        self.CreateFunction2('mid_convert_regulation_from_npc')
        
        self.pg.callproc('mid_convert_regulation_from_npc')
        
        self.pg.commit2()
        
        self.log.info('End convert regulation from road_npc.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
