# -*- coding: cp936 -*-
'''
Created on 2012-3-20

@author: zhangliang
'''
import subproj.proj_base

class proj_axf_module(subproj.proj_base.proj_base_module):
    '''北美module
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self, 'proj_axf_module', component_factory)
    
    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """
        if self.comp_factory != None:
            #new id
            self.AddComponent(self.comp_factory.CreateOneComponent('NewID'))
            # 字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
            # 道路名称和番号
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Name'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('slope'))
            self.AddComponent(self.comp_factory.CreateOneComponent('stopsign'))
            
            # GuideInfo
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('guideinfo_signpost_uc'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Alert'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Zone'))
            # SpotGuide
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_building'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_NaturalGuidence'))
            self.AddComponent(self.comp_factory.CreateOneComponent('hook_turn'))
            
            # Regulation
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))          
            # Link 
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            # Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            # Admin
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            # Caution
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            # shield
            self.AddComponent(self.comp_factory.CreateOneComponent('Shield'))
            
            # link_split
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            # link_merge
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))

            pass

            
            
