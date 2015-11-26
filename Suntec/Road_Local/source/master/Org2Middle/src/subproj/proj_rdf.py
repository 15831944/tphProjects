# -*- coding: cp936 -*-
'''
Created on 2012-3-20

@author: zhangliang
'''
import subproj.proj_base


class proj_rdf_module(subproj.proj_base.proj_base_module):
    '''北美module
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self, 'proj_rdf_module', component_factory)

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
            self.AddComponent(self.comp_factory.CreateOneComponent('slope'))
            self.AddComponent(self.comp_factory.CreateOneComponent('stopsign'))
            
            # 字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))

            # Regulation
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            # Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            # Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('Update_sapa_link'))
            # Ramp RoadType
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp_RoadTypeFC'))
            # Node Height
            self.AddComponent(self.comp_factory.CreateOneComponent('Node_Height'))            
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            
            # GuideInfo
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('GuideInfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('guideinfo_signpost_uc'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_building'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_NaturalGuidence'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Zone'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Alert'))
            self.AddComponent(self.comp_factory.CreateOneComponent('hook_turn'))    
            
             
            # Admin
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution_highaccident'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway_Prepare'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('Name_filter_and_sort'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Detele_redundance_regulation'))
            # link_split
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            # link_merge
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))

            self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
            
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            pass


