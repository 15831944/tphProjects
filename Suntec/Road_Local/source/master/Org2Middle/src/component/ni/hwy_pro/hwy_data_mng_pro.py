# -*- coding: UTF-8 -*-
'''
Created on 2015-10-26

@author: hcz
'''
from component.rdf.hwy.hwy_data_mng_rdf import HwyDataMngRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
from component.ni.hwy.hwy_data_mng_ni import HwyDataMngNi
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
from component.jdb.hwy.hwy_graph import ONE_WAY_BOTH


class HwyDataMngNiPro(HwyDataMngNi):
    '''
    classdocs
    '''

    def __init__(self, ItemName='HwyDataMngPro'):
        '''
        Constructor
        '''
        HwyDataMngNi.__init__(self, ItemName)

    @staticmethod
    def instance():
        if HwyDataMngRDF._instance is None:
            HwyDataMngRDF._instance = HwyDataMngNi()
        return HwyDataMngRDF._instance

    def load_hwy_path_id(self):
        sqlcmd = """
        SELECT hw_pid, s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.inlinkid = link_id
        UNION
        SELECT hw_pid, outlinkid as link_id,
               s_node, e_node, one_way_code
           FROM mid_hwy_org_hw_junction_mid_linkid as a
           LEFT JOIN link_tbl as b
           ON a.outlinkid = link_id
        ORDER BY link_id;
        """
        for(path_id, s_node, e_node, one_way) in self.get_batch_data(sqlcmd):
            data = {HWY_PATH_ID: path_id}
            if one_way == ONE_WAY_POSITIVE:
                if s_node in self._graph:
                    self._graph.add_edge(s_node, e_node, data)
            elif one_way == ONE_WAY_RERVERSE:
                if e_node in self._graph:
                    self._graph.add_edge(e_node, s_node, data)
            elif one_way == ONE_WAY_BOTH:
                self._graph.add_edge(e_node, s_node, data)
