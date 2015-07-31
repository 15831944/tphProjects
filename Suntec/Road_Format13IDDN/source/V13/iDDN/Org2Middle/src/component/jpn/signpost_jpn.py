# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author: liuxinxing
'''
from base import comp_base

class comp_signpost_jpn(comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        comp_base.__init__(self, 'SignPost')
    
    def _DoCreateTable(self):
        self.CreateTable2('signpost_tbl')
        
    def _Do(self):
        self.log.info('Begin convert SignPost from road_dir...')
        
        self.CreateFunction2('mid_convert_signpost')
        
        self.pg.callproc('mid_convert_signpost')
        
        self.pg.commit2()
        
        self.log.info('End convert SignPost from road_dir.')
        
        return 0
    