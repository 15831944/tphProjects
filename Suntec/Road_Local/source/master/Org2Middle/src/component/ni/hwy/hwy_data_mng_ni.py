# -*- coding: UTF-8 -*-
'''
Created on 2015-4-29

@author: hcz
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_EXIT_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_ORG_FACIL_ID


class HwyDataMngNi(HwyDataMngRDF):
    '''
    Highway Data Manager(TomTom)
    '''

    def __init__(self, ItemName='HwyDataMngNi'):
        '''
        Constructor
        '''
        HwyDataMngRDF.__init__(self, ItemName)

    @staticmethod
    def instance():
        if HwyDataMngRDF._instance is None:
            HwyDataMngRDF._instance = HwyDataMngNi()
        return HwyDataMngRDF._instance

    def load_exit_name(self):
        '''加载出口番号'''
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT node_id, name
          FROM mid_temp_hwy_exit_enter_poi_name_ni as a
          where kind = 'Exit';
        """
        for exit_info in self.get_batch_data(sqlcmd):
            node_id, exit_name = exit_info
            data = {HWY_EXIT_NAME: exit_name}
            # data = {"exit_name": exit_name}
            if node_id in self._graph:
                self._graph.add_node(node_id, data)
            else:
                pass

    def load_junction_name(self):
        pass

    def load_org_facil_id(self):
        '''加载元设施id'''
        sqlcmd = """
        SELECT node_id, poi_id
          FROM mid_temp_hwy_exit_enter_poi_name_ni
        """
        for facil_info in self.get_batch_data(sqlcmd):
            node, facil_id = facil_info
            data = {HWY_ORG_FACIL_ID: facil_id}
            if node in self._graph:
                self._graph.add_node(node, data)
