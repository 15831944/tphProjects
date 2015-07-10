# -*- coding: UTF8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
import subproj.proj_base


class proj_ni_module(subproj.proj_base.proj_base_module):
    '''
    TomTom PROJECT Module
    '''

    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self,
                                                    'proj_ni_module',
                                                    component_factory)

    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """
        if self.comp_factory:
            # ×Öµä
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
                        
            #guide
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_signpost_uc'))
			self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))

			#link
			self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            
            #admin
            self.AddComponent(self.comp_factory.CreateOneComponent('admin'))
            
            # Highway_Prepare
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway_Prepare'))
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))

            pass
