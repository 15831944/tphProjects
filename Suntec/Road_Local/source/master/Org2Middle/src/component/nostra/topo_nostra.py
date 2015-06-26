# -*- coding: UTF8 -*-
'''
Created on 2012-12-6

@author: yuanrui
'''
import component.component_base

class comp_topo_nostra(component.component_base.comp_base):
    '''
    make topology data
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Topo')

    def _Do(self):
                   
        self.log.info(self.ItemName + ': Now it is making topology data...')
        
        #print os.getcwd()
        file = open("./sql_script/nostra/make_topology_patch.sql")
        sqlcmd = file.read()
        file.close()
        sqlcmd = self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info(self.ItemName + ': end of making topology data...')
      
