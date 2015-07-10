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
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
             
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            
            # Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            
            # guide
            self.AddComponent(self.comp_factory.CreateOneComponent('stopsign'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_signpost_uc'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_building'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_NaturalGuidence'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Alert'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Zone'))
            self.AddComponent(self.comp_factory.CreateOneComponent('hook_turn'))
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            self.AddComponent(self.comp_factory.CreateOneComponent('height'))
            self.AddComponent(self.comp_factory.CreateOneComponent('spotguide_tbl'))
            self.AddComponent(self.comp_factory.CreateOneComponent('signpost_tbl'))
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            # highway prepare
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway_Prepare'))
            
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
            
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            
            # Road_type and function_class change for ramp
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp'))
            
            pass
