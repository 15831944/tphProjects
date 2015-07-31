'''
Created on 2012-3-20

@author: zhangliang
'''
from base import proj_base
#import rdb_guideinfo_lane_chn

class proj_chn_module(proj_base.proj_base_module):
    '''
    classdocs
    '''
    def __init__(self, component_factory):
        '''
        Constructor
        '''
        proj_base.proj_base_module.__init__(self, 'proj_chn_module', component_factory)
    
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
    
        