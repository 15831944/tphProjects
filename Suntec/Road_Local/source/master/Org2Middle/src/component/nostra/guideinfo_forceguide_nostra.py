# -*- coding: UTF8 -*-
'''
Created on 2015-4-27

@author: liushengqiang
'''



import component.default.guideinfo_forceguide

class comp_guideinfo_forceguide_nostra(component.default.guideinfo_forceguide.com_guideinfo_forceguide):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_forceguide.com_guideinfo_forceguide.__init__(self)
    
    def _Do(self):
        
        self._deal_temp_patch_force_guide_tbl()  
        
        return 0
