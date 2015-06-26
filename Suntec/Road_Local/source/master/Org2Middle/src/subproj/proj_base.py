'''
Created on 2012-3-20

@author: zhangliang
'''
import common.common_func
import common.log

class proj_base_module(object):
    '''
    This is project base class 
    '''

    def __init__(self, proj_name = 'proj_base_module', component_factory = None):
        '''
        Constructor
        '''
        self.item_name      = common.common_func.GetProjName() + '_' + common.common_func.getProjCountry()
        self.log            = common.log.common_log.instance().logger(proj_name)
        self.component_list = []
        self.comp_factory   = component_factory
    
    def ConstructComponent(self):
        if self.comp_factory != None:
            self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Lane'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_CrossName'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Caution'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Picture'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Signpost'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Spotguidepoint'))
            #self.AddComponent(self.comp_factory.CreateOneComponent('Guideinfo_Towardname'))
            self.AddComponent(self.comp_factory.CreateOneComponent('Link'))
        return 0
    
    def AddComponent(self, component):
        """
        You can add new component depend on your requirements
        """
        if component != None:
            self.component_list.append(component)
        else:
            raise Exception, 'null component'
    
    def Start2DealwithOriginalData(self):
        try:
            self.log.info('start to process ' + self.item_name + ' original data.')
            for comp in self.component_list:
                comp.Make()
            self.log.info('process ' + self.item_name + ' original data end.')
            return 0
        except:
            self.log.exception('error happened while processing ' + self.item_name + ' original data.')
            raise
    
    def Start2CheckMiddleData(self):
        self.log.info('start to check ' + self.item_name + ' middle data.')
        comp_error = []
        for comp in self.component_list:
            bResult = comp.Check()
            if bResult != 0:
                comp_error.append(comp)
        
        if len(comp_error) == 0:
            self.log.info('check result of middle data is ok.')
            return 0
        else:
            self.log.exception('middle data error.')
            for comp in comp_error:
                self.log.error('error happened while checking ' + comp.ItemName + ' middle data.')
            raise Exception, 'middle data error.'
               

        
        