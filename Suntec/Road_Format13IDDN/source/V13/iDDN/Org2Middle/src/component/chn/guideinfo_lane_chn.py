'''
Created on 2012-3-21

@author: zhangliang
'''
import component

class comp_guideinfo_lane_chn(component.guideinfo_lane.comp_guideinfo_lane):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.guideinfo_lane.comp_guideinfo_lane.__init__(self)
        
    def __Do(self):
        return 0