# -*- coding: UTF-8 -*-
'''
Created on 2015-5-7

@author: liuxinxing
'''
import component.component_base

class comp_regulation_ni(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
    
    def _Do(self):
        self.__convert_condition_table()
        
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        self.__convert_regulation_access()
        self.__convert_regulation_oneway()
        self.__convert_regulation_linkrow()
        return 0
    
    def __convert_condition_table(self):
        self.log.info('convert condition_regulation_tbl...')
        
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        
        self.CreateFunction2('mid_get_access')
        self.CreateFunction2('mid_convert_condition_regulation_tbl')
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        self.pg.commit2()
        
        self.log.info('convert condition_regulation_tbl end.')
    
    def __convert_regulation_access(self):
        self.log.info('convert regulation of access...')
        
        self.CreateFunction2('mid_convert_regulation_access')
        self.pg.callproc('mid_convert_regulation_access')
        self.pg.commit2()
        
        self.log.info('convert regulation of access end.')
    
    def __convert_regulation_oneway(self):
        self.log.info('convert regulation of oneway...')
        
        self.CreateFunction2('mid_convert_regulation_oneway')
        self.pg.callproc('mid_convert_regulation_oneway')
        self.pg.commit2()
        
        self.log.info('convert regulation of oneway end.')
    
    def __convert_regulation_linkrow(self):
        self.log.info('convert regulation of linkrow...')
        
        self.CreateFunction2('mid_convert_regulation_linkrow')
        self.pg.callproc('mid_convert_regulation_linkrow')
        self.pg.commit2()
        
        self.log.info('convert regulation of linkrow end.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
    