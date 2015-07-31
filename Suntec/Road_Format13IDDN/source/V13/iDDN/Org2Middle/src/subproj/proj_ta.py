# -*- coding: cp936 -*-
'''
Created on 2012-8-27

@author: hongchenzai
'''
from base import proj_base

class proj_ta_module(proj_base.proj_base_module):
    '''
    TomTom PROJECT Module
    '''

    def __init__(self, component_factory):
        '''
        Constructor
        '''
        proj_base.proj_base_module.__init__(self, 'proj_ta_module', component_factory)

    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """
        if self.comp_factory != None:
            # jv files
            self.AddComponent(self.comp_factory.CreateOneComponent('sidefiles'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Sidefiles_Phoneme'))
            # 字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
            # Regulation
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            # Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))

            # GuideInfo
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('guideinfo_signpost_uc'))

            # TowarName一定要放在guideinfo_signpost_uc之后，因为使用到后者的方面名称
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            # Guideinfo_ForceGuide
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            # Admin
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))

            # Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp'))

            # SpotGuide
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))

            # 道路名称和番号
            # self.AddComponent(self.comp_factory.CreateOneComponent('Link_Name'))
            # shield
            # self.AddComponent(self.comp_factory.CreateOneComponent('Shield'))

            # Link_overlay_or_circle
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_overlay_or_circle'))

            # link_split
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            # link_merge
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))

            self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
