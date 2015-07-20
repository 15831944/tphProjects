# -*- coding: UTF-8 -*-
'''
Created on 2015-7-8

@author: road
'''
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF_HKG
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_2000M
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_2500M
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_S
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_SE


class HwyRouteNi(HwyRouteRDF_HKG):
    def __init__(self, data_mng,
                 min_distance=ROUTE_DISTANCE_2000M,
                 margin_dist=ROUTE_DISTANCE_2500M,
                 ItemName='HwyRouteNi'):
        '''
        '''
        HwyRouteRDF_HKG.__init__(self, data_mng, min_distance,
                             margin_dist, ItemName)

    def _merge_short_path(self, u, v, new_path_id):
        path_G = self.data_mng.get_path_graph()  # 路径的图
        old_path_ids = []
        new_path = None
        # merged_pathes = []
        length = path_G.get_length(u, v)
        path_id = path_G.get_path_id(u, v)
        while True:
            conn_type = path_G.get_path_conn_type((u, v))
            if(conn_type == HWY_PATH_CONN_TYPE_NONE or
               conn_type == HWY_PATH_CONN_TYPE_SE):
                node_list = path_G.get_node_list(u, v)
                # 小于最短长度(2Km)、# 无Highway Exit POI（小于2.5Km）
                if(length <= self.min_distance or
                   not self.has_hwy_exit_poi(node_list)):
                    old_path_ids.append(path_id)
                    new_path = None
                    break
                else:
                    self.log.warning('Length is %s. path_id=%s'
                                     % (length, path_id))
                    break
            elif conn_type == HWY_PATH_CONN_TYPE_S:  # 起点有相接
                # 和进入路径合并起来
                # 注：这里缺少判断多条相交，以后要补上
                in_pathes = path_G.in_edges(u, data=True)
                if len(in_pathes) > 1:
                    self.log.warning('In path > 1. path_id=%s, node=%s'
                                     % (path_id, u))
                    break
                else:
                    p, u, data = in_pathes[0]
                    max_u, max_v = self._get_max_len_path(p, u)
                    if (u, v) != (max_u, max_v):
                        old_path_ids.append(path_id)
                        max_len = path_G.get_length(max_u, max_v)
                        if max_len > ROUTE_DISTANCE_2500M:
                            merge_path_id = path_G.get_path_id(p, max_u)
                            old_path_ids.append(merge_path_id)
                            merge_path_id = path_G.get_path_id(max_u, max_v)
                            old_path_ids.append(merge_path_id)
                            new_u, new_v = path_G.merge_edges([p,
                                                               max_u, max_v],
                                                              new_path_id)
                            new_path = (new_u, new_v)
                        break
                    else:
                        in_path_id = data.get('path_id')
                        old_path_ids += [in_path_id, path_id]
                        new_u, new_v = path_G.merge_edges([p, u, v],
                                                          new_path_id)
                        new_path = (new_u, new_v)
            else:  # 终点有相接
                # 和脱出路径合并起来
                # 注：这里缺少判断多条相交，以后要补上
                out_pathes = path_G.out_edges(v, data=True)
                if len(out_pathes) > 1:
                    self.log.warning('Out path > 1. path_id=%s' % path_id)
                    break
                else:
                    v, child, data = out_pathes[0]
                    max_u, max_v = self._get_max_len_path(v, child,
                                                          reverse=True)
                    if (u, v) != (max_u, max_v):
                        old_path_ids.append(path_id)
                        max_len = path_G.get_length(max_u, max_v)
                        if max_len > ROUTE_DISTANCE_2500M:
                            merge_path_id = path_G.get_path_id(max_u, max_v)
                            old_path_ids.append(merge_path_id)
                            merge_path_id = path_G.get_path_id(max_v, child)
                            old_path_ids.append(merge_path_id)
                            new_u, new_v = path_G.merge_edges([max_u, max_v,
                                                               child],
                                                              new_path_id)
                            new_path = (new_u, new_v)
                        break
                    else:
                        out_path_id = data.get('path_id')
                        old_path_ids += [path_id, out_path_id]
                        new_u, new_v = path_G.merge_edges([u, v, child],
                                                          new_path_id)
                    new_path = (new_u, new_v)
            length = path_G.get_length(new_u, new_v)
            if length > self.min_distance:
                break
            path_id = new_path_id
            u, v = new_u, new_v
        return new_path, old_path_ids

    def has_hwy_exit_poi(self, path):
        '''判断该高速线路是否存在【Highway Exit POI】'''
        for node in path[1:-1]:  # 第一个点和最后一个不考虑
            if self.G.get_org_facil_id(node):
                return True
        return False
