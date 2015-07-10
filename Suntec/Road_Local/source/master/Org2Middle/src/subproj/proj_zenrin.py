# -*- coding: UTF8 -*-
'''
Created on 2015-6-17

@author: lsq
'''
import subproj.proj_base


class proj_zenrin_module(subproj.proj_base.proj_base_module):
    '''
    ZENRIN PROJECT Module
    '''

    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self,
                                                    'proj_zenrin_module',
                                                    component_factory)

    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """
        if self.comp_factory:
            
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Spotguide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Signpost_uc'))
            pass
