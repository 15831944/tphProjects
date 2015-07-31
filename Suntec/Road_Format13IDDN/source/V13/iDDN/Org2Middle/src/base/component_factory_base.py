'''
Created on 2012-3-23

@author: hongchenzai
'''
import component_base

class comp_factory_base(object):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        
    def CreateOneComponent(self, comp_name):
        if comp_name == 'Guideinfo_Lane':
            return component_base.comp_base()
        
        if comp_name == 'Link':
            return component_base.comp_base()
        
        return None