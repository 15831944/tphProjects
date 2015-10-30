# -*- coding: UTF-8 -*-
'''
Created on 2015-7-8

@author: road
'''
import json
from component.rdf.hwy.hwy_route_rdf import HwyRouteRDF_HKG
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_2000M
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_2500M
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_S
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_SE
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import NAME_TYPE_ROUTE_NUM
from component.default.multi_lang_name import NAME_TYPE_SHIELD
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_LENGTH
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_NUMS
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_NAMES
from component.rdf.hwy.hwy_def_rdf import HWY_NAME_SPLIT


class HwyRouteNi(HwyRouteRDF_HKG):
    def __init__(self, data_mng,
                 min_distance=ROUTE_DISTANCE_2000M,
                 margin_dist=ROUTE_DISTANCE_2500M,
                 ItemName='HwyRouteNi'):
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

    def _add_path_name(self, path_name_dict, names,
                       name_types=[NAME_TYPE_OFFICIAL]):
        if not names:
            return
        for name_info in names:
            one_name = list()
            for name_dict in name_info:
                name_type = name_dict.get("type")
                if name_type in name_types:
                    one_name.append(name_dict)
            if not one_name:
                continue
            key = json.dumps(one_name, ensure_ascii=False,
                             encoding='utf8', sort_keys=True)
            if key in path_name_dict:
                path_name_dict[key] += 1
            else:
                path_name_dict[key] = 1

    def _get_max_count_names(self, path_name_dict):
        if not path_name_dict:
            return []
        names = []
        max_cnt = 0
        name_items = path_name_dict.items()
        # 按名称出现次数排序
        name_items.sort(cmp=lambda x, y: cmp(x[-1], y[-1]), reverse=True)
        for name, cnt in name_items:
            if cnt >= max_cnt:
                name_list = json.loads(name, encoding='utf8')
                names.append(name_list)
                max_cnt = cnt
            else:
                break
        # 按名称语种
#         names.sort(cmp=lambda x, y: cmp(x[0].get('lang'), y[0].get('lang')),
#                    reverse=True)
        return names

    def _get_max_count_nums(self, path_num_dict):
        nums = self._get_max_count_names(path_num_dict)
        for num_list in nums:
            # 去掉shield
            for num in num_list:
                num['val'] = num.get('val').split('\t')[-1]
        return nums

    def _get_sort_name(self, path_name):
        '''取得排序用名称'''
        names = []
        if not path_name:
            return None
        for name_dict_list in path_name:
            for name_dict in name_dict_list:
                name = name_dict.get('val')
                if name and name not in names:
                    names.append(name)
                break  # 不要翻译

        if names:
            return HWY_NAME_SPLIT.join(names)
        else:
            return None

    def _get_path_attr(self, G, path):
        '''取得属性(长度、番号、名称)'''
        path_length = 0
        path_name = {}
        path_number = {}
        for u, v in zip(path[0:-1], path[1:]):
            data = G[u][v]
            link_length = data.get(HWY_LINK_LENGTH)
            numbers = data.get(HWY_ROAD_NUMS)
            names = data.get(HWY_ROAD_NAMES)
            self._add_path_name(path_name, names, [NAME_TYPE_OFFICIAL])
            if not path_name:
                self._add_path_name(path_name, names, [NAME_TYPE_ROUTE_NUM])
            self._add_path_name(path_number, numbers, [NAME_TYPE_SHIELD])
            if link_length:
                path_length += link_length
            else:
                self.log.error('No Length. edge=(%s, %s)' % (u, v))
                return None
        rst_names = self._get_max_count_names(path_name)
        rst_nums = self._get_max_count_nums(path_number)
        if rst_names:
            json_name = json.dumps(rst_names, ensure_ascii=False,
                                   encoding='utf8', sort_keys=True)
        else:
            json_name = None
        if rst_nums:
            json_num = json.dumps(rst_nums, ensure_ascii=False,
                                  encoding='utf8', sort_keys=True)
        else:
            json_num = None
        sort_name = self._get_sort_name(rst_names)
        sort_num = self._get_sort_name(rst_nums)
        return path_length, json_name, json_num, sort_name, sort_num
