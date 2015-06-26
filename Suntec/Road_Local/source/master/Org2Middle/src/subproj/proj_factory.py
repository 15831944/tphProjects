'''
Created on 2012-3-21

@author: zhangliang
'''

class proj_factory(object):
    '''
       This is project factory , it's determined which sub-project is running
    '''
    __instance = None

    @staticmethod
    def instance():
        if proj_factory.__instance is None:
            proj_factory.__instance = proj_factory()
        return proj_factory.__instance

    def __init__(self):
        '''
        Constructor
        '''
        pass
    
    def createProject(self, proj_name):
        import component.component_factory
        objCompFactory = eval('component.component_factory.comp_factory_%s()' % proj_name)
        exec('import subproj.proj_%s' % proj_name)
        objSubproj = eval('subproj.proj_%s.proj_%s_module(objCompFactory)' % (proj_name, proj_name))
        return objSubproj
