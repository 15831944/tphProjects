# -*- coding: cp936 -*-
'''
Created on 2012-8-27

@author: hongchenzai
'''
import subproj.proj_base


class proj_ta_module(subproj.proj_base.proj_base_module):
    '''
    TomTom PROJECT Module
    '''

    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self, 'proj_ta_module', component_factory)

    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """
        if self.comp_factory != None:
            # jv files
            self.AddComponent(self.comp_factory.CreateOneComponent('sidefiles'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Sidefiles_Phoneme'))
            self.AddComponent(self.comp_factory.CreateOneComponent('slope'))
            self.AddComponent(self.comp_factory.CreateOneComponent('stopsign'))
            # 字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
            # Regulation
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            # Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            # Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            # update sapa link
            #self.AddComponent(self.comp_factory.CreateOneComponent('Update_sapa_link'))
            # Ramp
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp_RoadTypeFC'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Update_link_type'))
            # GuideInfo
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('guideinfo_signpost_uc'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Alert'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Zone'))
            self.AddComponent(self.comp_factory.CreateOneComponent('hook_turn'))
            self.AddComponent(self.comp_factory.CreateOneComponent('school_zone'))

            # TowarName一定要放在guideinfo_signpost_uc之后，因为使用到后者的方面名称
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            # Guideinfo_ForceGuide
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_NaturalGuidence'))
            # Admin
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_building'))
            # Highway Prepare
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway_Prepare'))
 
            # SpotGuide
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))
 
            # 道路名称和番号
            #self.AddComponent(self.comp_factory.CreateOneComponent('Link_Name'))
            #  shield
            #self.AddComponent(self.comp_factory.CreateOneComponent('Shield'))

            # Link_overlay_or_circle
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_overlay_or_circle'))
 
            # link_split
            self.AddComponent(self.comp_factory.CreateOneComponent('height'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Name_filter_and_sort'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Detele_redundance_regulation'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            # link_merge
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))
 
            self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))

            pass
