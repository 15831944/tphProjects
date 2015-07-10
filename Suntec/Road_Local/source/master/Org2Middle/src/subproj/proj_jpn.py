# -*- coding: cp936 -*-
'''
Created on 2012-3-20

@author: zhangliang
'''
import subproj.proj_base

class proj_jpn_module(subproj.proj_base.proj_base_module):
    '''
    classdocs
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self, 'proj_jpn_module', component_factory)
    
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
            # import forceguide patch
            self.AddComponent(self.comp_factory.CreateOneComponent('forceguide_patch'))
            # sidefiles
            self.AddComponent(self.comp_factory.CreateOneComponent('sidefiles'))
            # Toll
            self.AddComponent(self.comp_factory.CreateOneComponent('Toll'))
            # 转换SRID
            self.AddComponent(self.comp_factory.CreateOneComponent('Trans_SRID'))
            self.AddComponent(self.comp_factory.CreateOneComponent('slope'))
            # 分割Link
            self.AddComponent(self.comp_factory.CreateOneComponent('Split_Link'))
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))
            # Tile
            self.AddComponent(self.comp_factory.CreateOneComponent('Tile'))
            # 有插图的node点的名称存在插图数据里，所以先把插图数据库导入数据库,再做字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide_Tollstation'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Etc'))
            # 字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))
           
            # GuideInfo
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Alert'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Zone'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('guideinfo_signpost_uc'))
            self.AddComponent(self.comp_factory.CreateOneComponent('fac_name'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_NaturalGuidence'))
            self.AddComponent(self.comp_factory.CreateOneComponent('hook_turn'))
            
            # Admin
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            
            # Regulation
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))            
            # Link 
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            # Node
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            # highway
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))
            # Caution
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            # link name
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Name'))
            # Shield
            self.AddComponent(self.comp_factory.CreateOneComponent('Shield'))
            
            # link_split
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            # link_merge
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))
            
            # mainnode
            self.AddComponent(self.comp_factory.CreateOneComponent('MainNode'))
            pass
