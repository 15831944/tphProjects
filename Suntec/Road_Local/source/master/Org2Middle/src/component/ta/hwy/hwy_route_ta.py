# -*- coding: UTF-8 -*-
'''
Created on 2015-3-18

@author: hcz
'''
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID


class HwyRouteTa(HwyRouteRDF):
    '''Make Path of main link.
       And make road_code for Highway Road
    '''

    def __init__(self, data_mng):
        '''
        Constructor
        '''
        HwyRouteRDF.__init__(self, data_mng)

    def _get_route_start(self, path_id, path):
        start_pos = self._get_first_org_facility_in(path_id, path)
        if start_pos >= 0:
            return start_pos
        else:
            return HwyRouteRDF._get_route_start(self, path_id, path)

    def _get_route_end(self, path_id, path):
        start_pos = self._get_last_org_facility_out(path_id, path)  # is是
        if start_pos >= 0:
            return start_pos
        else:
            return HwyRouteRDF._get_route_end(self, path_id, path)

    def _get_first_org_facility_in(self, path_id, path):
        '''取得元设施号相同的最后一个合流点(参看org_is表) '''
        if path_id == 79:
            pass
        facil_id = self.G.get_org_facil_id(path[0])
        if not facil_id:
            return -1
        facil_in_pos = 0
        node_idx = 1
        while node_idx <= len(path) - 1:
            node = path[node_idx]
            temp_facil_id = self.G.get_org_facil_id(node)
            if not temp_facil_id or facil_id != temp_facil_id:
                break
            # 取得设施种别(IC/JCT/SA/toll)
            facil_types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            # 注：这里还没考虑SAPA/收费站
            if(self._is_jct_in(facil_types) or
               self._is_ic_in(facil_types)):
                facil_in_pos = node_idx
            node_idx += 1
        if facil_in_pos == 0:
            facil_in_pos = node_idx - 1
        return facil_in_pos

    def _get_last_org_facility_out(self, path_id, path):
        '''取得元设施号相同的第一个分歧(参看org_is表)'''
        facil_id = self.G.get_org_facil_id(path[-1])
        if path_id == 79:
            pass
        if not facil_id:
            return -1
        facil_out_pos = len(path) - 1
        node_idx = len(path) - 2
        while node_idx > 0:
            node = path[node_idx]
            temp_facil_id = self.G.get_org_facil_id(node)
            if not temp_facil_id or facil_id != temp_facil_id:
                break
            # 取得设施种别(IC/JCT/SA/toll)
            facil_types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            # 注：这里还没考虑SAPA/收费站
            if(self._is_jct_out(facil_types) or
               self._is_ic_out(facil_types)):
                facil_out_pos = node_idx
            node_idx -= 1
        if facil_out_pos == len(path) - 1:
            facil_out_pos = node_idx + 1
        return facil_out_pos
