'''
Created on 2014-3-4

@author: hongchenzai
'''
from base import proj_base


class proj_mmi_module(proj_base.proj_base_module):
    '''
    MapmyIndia PROJECT Module
    '''

    def __init__(self, component_factory):
        '''
        Constructor
        '''
        proj_base.proj_base_module.__init__(self,
                                            'proj_mmi_module',
                                            component_factory
                                            )

    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """
        if self.comp_factory != None:
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('sidefiles'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
            
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            
            # Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            
            #guide
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost_UC'))
            self.AddComponent(self.comp_factory.CreateOneComponent('spotguide_tbl'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))

            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))

            self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
            
            pass
