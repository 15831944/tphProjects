# -*- coding: UTF-8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.ta.hwy.hwy_graph_ta import HwyGraphTa
from component.rdf.hwy.hwy_path_graph_rdf import HwyPathGraphRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_EXIT_NAME
from component.rdf.hwy.hwy_graph_rdf import HWY_JUNCTION_NAME


class HwyDataMngTa(HwyDataMngRDF):
    '''
    Highway Data Manager(TomTom)
    '''
    __instance = None

    def __init__(self, ItemName='HwyDataMngTa'):
        '''
        Constructor
        '''
        HwyDataMngRDF.__init__(self, ItemName)

    @staticmethod
    def instance():
        if HwyDataMngTa.__instance is None:
            HwyDataMngTa.__instance = HwyDataMngTa()
        return HwyDataMngTa.__instance

    def initialize(self):
        self._graph = HwyGraphTa()  # 高速link图
        self._path_graph = HwyPathGraphRDF(self._graph)  # 高速路径图

    def load_exit_poi_name(self):
        pass

    def load_exit_name(self):
        '''加载出口番号'''
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT node_id, name
          FROM mid_temp_hwy_exit_name_ta as a
          where is_exit_name = 'Y';
        """
        for exit_info in self.get_batch_data(sqlcmd):
            node_id, exit_name = exit_info
            data = {HWY_EXIT_NAME: exit_name}
            # data = {"exit_name": exit_name}
            if node_id in self._graph:
                self._graph.add_node(node_id, data)
            else:
                pass
                # self.log.warning('Node=%s does not in Graph.' % node_id)

    def load_junction_name(self):
        '''加载出口番号'''
        # 下面的SQL代码时临时
        sqlcmd = """
        SELECT node_id, name
          FROM mid_temp_hwy_exit_name_ta as a
          where is_junction_name = 'Y';
        """
        for exit_info in self.get_batch_data(sqlcmd):
            node_id, junction_name = exit_info
            data = {HWY_JUNCTION_NAME: junction_name}
            if node_id in self._graph:
                self._graph.add_node(node_id, data)
            else:
                pass
                # self.log.warning('Node=%s does not in Graph.' % node_id)
