# -*- coding: cp936 -*-
'''
Created on 2013-8-30

@author: HCZ
'''
import subproj.proj_base


class proj_jdb_module(subproj.proj_base.proj_base_module):
    '''
    classdocs
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self, 'proj_jdb_module', component_factory)
    
    def ProcLinkTbl(self):
        return 0
    
    def ProcNodeTbl(self):
        return 0
    
    def ProcRegulationRelationTbl(self):
        return 0
    
    def ProcRegulationItemTbl(self):
        return 0
    
    def ProcGuideInfo(self):
        for comp in self.component_list:
            comp.Make()
        return 0
    
    def ProcNameDic(self):
        return 0
    
    def ConstructComponent(self):
        """
        You can add new component depend on your requirements
        """

        if self.comp_factory != None:
            # 预处理
            self.AddComponent(self.comp_factory.CreateOneComponent('PreProcess'))
            self.AddComponent(self.comp_factory.CreateOneComponent('sidefiles'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
            self.AddComponent(self.comp_factory.CreateOneComponent('slope'))
            self.AddComponent(self.comp_factory.CreateOneComponent('stopsign'))
            
            # 转换SRID
            self.AddComponent(self.comp_factory.CreateOneComponent('Trans_SRID'))
            # Admin
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            
            # 分割Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Split_Link'))
            # Tile
            self.AddComponent(self.comp_factory.CreateOneComponent('Tile'))
            
            # Regulation
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))
            # Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            # Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            
            # GuideInfo
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_building'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Trans_DirGuide'))# 转换方面名称
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('guideinfo_signpost_uc'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('Trans_HighwayNode'))# 转换Highway Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway_Prepare'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Alert'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Zone'))
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_NaturalGuidence'))
            self.AddComponent(self.comp_factory.CreateOneComponent('hook_turn'))
            
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            # link_split
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            # link_merge
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))
            # mainnode
            self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
            #Supplement lane arrow
            self.AddComponent(self.comp_factory.CreateOneComponent('arrow_optimize'))
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            self.AddComponent(self.comp_factory.CreateOneComponent('fac_namenew'))
            self.AddComponent(self.comp_factory.CreateOneComponent('update_patch_sapa_link'))


