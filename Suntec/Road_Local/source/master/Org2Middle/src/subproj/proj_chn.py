'''
Created on 2012-3-20

@author: zhangliang
'''
import subproj.proj_base

class proj_chn_module(subproj.proj_base.proj_base_module):
    '''
    classdocs
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        subproj.proj_base.proj_base_module.__init__(self, 'proj_chn_module', component_factory)
    
    def ProcLinkTbl(self):
        return 0
    
    def ProcNodeTbl(self):
        return 0
    
    def ProcRegulationRelationTbl(self):
        return 0
    
    def ProcRegulationItemTbl(self):
        return 0
    
    def ProcGuideInfo(self):
        return 0
    
    def ProcNameDic(self):
        return 0
    
        