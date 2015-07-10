# -*- coding: UTF8 -*-
'''
Created on 2012-3-20

@author: hcz
'''
import subproj.proj_base


class proj_nostra_module(subproj.proj_base.proj_base_module):
    '''
    classdocs
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self, 'proj_nostra_module',
                                            component_factory)

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
            # import update junction view
            self.AddComponent(self.comp_factory.CreateOneComponent('importupdatejv'))
            # illust name
            self.AddComponent(self.comp_factory.CreateOneComponent('illustname'))
            self.AddComponent(self.comp_factory.CreateOneComponent('slope'))
            self.AddComponent(self.comp_factory.CreateOneComponent('stopsign'))
            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Topo'))

            # 字典
            self.AddComponent(self.comp_factory.CreateOneComponent('Dictionary'))

            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Admin'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Regulation'))

            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Node'))
            #park
            self.AddComponent(self.comp_factory.CreateOneComponent('park'))

            # 诱导
            self.AddComponent(self.comp_factory.CreateOneComponent('CrossName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Alert'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Safety_Zone'))
            self.AddComponent(self.comp_factory.CreateOneComponent('hook_turn'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SpotGuide_Tollstation'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost_UC'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_TowardName'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_SignPost'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_ForceGuide'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_building'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_NaturalGuidence'))

            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Highway'))

            #
            self.AddComponent(self.comp_factory.CreateOneComponent('Ramp'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Split'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link_Merge'))

            pass
