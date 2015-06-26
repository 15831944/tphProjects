# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author: liuxinxing
'''
import component.component_base

class comp_regulation_axf(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Regulation')
    
    def _DoCreateTable(self):
        self.CreateTable2('temp_condition_regulation_tbl')
        self.CreateTable2('condition_regulation_tbl')
        self.CreateTable2('regulation_relation_tbl')
        self.CreateTable2('regulation_item_tbl')
        
    def _Do(self):
        self.__convert_condition_table()
        self.__convert_regulation_on_node()
        self.__convert_regulation_on_cross()
        return 0
    
    def __convert_condition_table(self):
        self.log.info('Begin convert condition_regulation_tbl...')
        
        self.CreateFunction2('mid_get_access')
        self.CreateFunction2('mid_convert_condition_regulation_tbl')
        
        self.pg.callproc('mid_convert_condition_regulation_tbl')
        
        self.pg.commit2()
        
        self.log.info('End convert condition_regulation_tbl.')
    
    def __convert_regulation_on_node(self):
        self.log.info('Begin convert regulation on node...')
        
        self.CreateFunction2('mid_convert_regulation_on_node')
        
        self.pg.callproc('mid_convert_regulation_on_node')
        
        self.pg.commit2()
        
        self.log.info('End convert regulation on node.')
    
    def __convert_regulation_on_cross(self):
        self.log.info('Begin convert regulation on cross...')
                
        self.CreateFunction2('mid_findInnerPath')
        self.CreateFunction2('mid_convert_regulation_on_cross')
        
        self.pg.callproc('mid_convert_regulation_on_cross')
        
        self.pg.commit2()
        
        self.log.info('End convert regulation on cross.')
    
    def _DoCheckLogic(self):
        self.log.info('Check regulation...')
        
        self.CreateFunction2('mid_check_regulation_item')
        self.pg.callproc('mid_check_regulation_item')
        
        self.CreateFunction2('mid_check_regulation_condition')
        self.pg.callproc('mid_check_regulation_condition')
        
        self.log.info('Check regulation end.')
        return 0
    