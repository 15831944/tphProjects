# -*- coding: cp936 -*-
'''
Created on 2012-3-20

@author: zhangliang
'''
from base import proj_base


class proj_rdf_module(proj_base.proj_base_module):
    '''北美module
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        proj_base.proj_base_module.__init__(self, 'proj_rdf_module', component_factory)

    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """
        if self.comp_factory != None:
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('rawdata'))    #revise the rawdata about phoneme
            self.AddComponent(self.comp_factory.CreateOneComponent('ImportCSV')) 
            self.AddComponent(self.comp_factory.CreateOneComponent('ImportJCV'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Trans_Geom'))
            self.AddComponent(self.comp_factory.CreateOneComponent('trans_Srid'))
            
            # 字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))

            # Regulation
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            # Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            # Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            
            # GuideInfo
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('guideinfo_signpost_uc'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            
            # Admin
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            
            # Ramp RoadType
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp'))
            # link_split
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            # link_merge
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))

            self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
            pass


