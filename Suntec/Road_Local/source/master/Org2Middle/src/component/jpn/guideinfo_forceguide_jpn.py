# -*- coding: cp936 -*-
'''
Created on 2012-3-26

@author: sunyifeng
'''

import component.component_base

class com_guideinfo_forceguide_jpn(component.component_base.comp_base):
    '''强制诱导
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_ForceGuide')
        
    def _Do(self):
    
        self.CreateTable2('force_guide_tbl')
        
        self.CreateFunction2('mid_getSplitLinkID')
        self.CreateFunction2('mid_get_new_passlid')
        self.CreateFunction2('mid_getConnectionNodeID')
        self.CreateFunction2('rdb_get_connected_nodeid_ipc')
        self.CreateFunction2('mid_convert_forceguide')
        
        self.pg.callproc('mid_convert_forceguide')
        self.pg.commit2()
        
        self.CreateIndex2('force_guide_tbl_node_id_idx')
        
        return 0
    