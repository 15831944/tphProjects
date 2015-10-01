# -*- coding: UTF-8 -*-
'''
Created on 2015��7��3��

@author: PC_ZH
'''

from component.ni.hwy.hwy_graphy_ni import HwyGraphNi
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY
from component.rdf.hwy.hwy_def_rdf import HWY_ROAD_TYPE_HWY1
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SAPA
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_RAMP
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN1
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_MAIN2
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_VIRTUAl_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SERVICE_ROAD
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_graph_rdf import HWY_UPDOWN_CODE
from component.jdb.hwy.hwy_graph import MAX_CUT_OFF
from component.jdb.hwy.hwy_graph import MIN_CUT_OFF_CHN
from component.rdf.hwy.hwy_def_rdf import SIMILAR_DEGREE_NONE
HWY_UPDOWN = "updown"


class HwyGraphZenrin(HwyGraphNi):
    ''' '''
    def __init__(self, data=None, **attr):
        HwyGraphNi.__init__(self, data, **attr)

    def _all_facil_path(self, u, v, road_code,
                        code_field=HWY_ROAD_CODE,
                        reverse=False, cutoff=MAX_CUT_OFF):
        MAX_COUNT = 2
        MAX_TOLLGATE_NUM = 2
        sapa_link = False
        exist_sapa_facil = False
        both_sapa_cnt = 0
        if cutoff < 1:
            return
        if reverse:  # 逆
            source = u
            visited = [v, u]
        else:  # 顺
            source = v
            visited = [u, v]
        if source not in self:
            return
        if self.is_hwy_inout(visited, reverse):  # 本线直接和一般道直接相连
            yield visited[1:], HWY_IC_TYPE_IC
            return
        if self.is_jct(visited, road_code, code_field, reverse):  # 本线和本线直接相连
            yield visited[1:], HWY_IC_TYPE_JCT
        # visited = [source]
        # stack = [iter(self[source])]
        nodes = self._get_not_main_link(visited[-1], code_field, reverse)
        if not nodes:  # 没有分歧/合流
            return
        stack = [iter(nodes)]
        while stack:
            if both_sapa_cnt > 0:
                # 双向的SAPA link，一条当两条权值算
                cutoff2 = cutoff - both_sapa_cnt
                if cutoff2 < MIN_CUT_OFF_CHN:
                    cutoff2 = MIN_CUT_OFF_CHN
            else:
                cutoff2 = cutoff
            curr_inout_flag = False
            ignore_inner = False
            children = stack[-1]
            child = next(children, None)
            temp_path = visited + [child]
            node_cnt = temp_path.count(child)
            if child is None:
                if(len(visited) > 1 and
                   self._is_both_dir_sapa(visited[-2], visited[-1],
                                          reverse)):
                    both_sapa_cnt -= 1
                stack.pop()
                visited.pop()
            elif(len(visited) >= 2 and child == visited[-2] and  # 折返
                 not self._is_cuted_road_end(visited[-1])):  # 不是断头路的最后一个点
                # 即不是断头路的最后一个点, 不能折返
                continue
            elif node_cnt > MAX_COUNT:
                continue
            elif(node_cnt > 1 and
                 self._is_link_repeat(temp_path[:-2], u, v) and
                 set._has_other_path(u, v, reverse)):
                continue
            elif not self.check_regulation(temp_path, reverse):
                continue
            elif child == visited[1]:
                path = visited + [child]
                if self._is_sapa_path(temp_path, road_code,
                                      code_field, reverse):
                    yield path[1:], HWY_IC_TYPE_PA
                    exist_sapa_facil = True
                continue
            elif self.get_tollgate_num(temp_path) > MAX_TOLLGATE_NUM:
                continue
            elif len(visited) <= cutoff2:
                # 取得link
                if reverse:  # 逆
                    out_edge = (child, visited[-1])
                else:  # 顺
                    out_edge = (visited[-1], child)
                # 本线和SAPA link直接相连
                if(len(visited) == 2 and
                   self.is_sapa_link(out_edge[0], out_edge[1])):
                    # not self.has_edge(out_edge[1], out_edge[0])):
                    sapa_link = True
                if self.is_jct(temp_path, road_code, code_field, reverse):
                    for uturn_info in self.get_uturns(temp_path, road_code,
                                                      code_field, reverse):
                        uturn_flg = uturn_info[-1]
                        if uturn_flg:
                            yield temp_path[1:], HWY_IC_TYPE_UTURN
                        else:
                            yield temp_path[1:], HWY_IC_TYPE_JCT
                    if self._is_sapa_path(temp_path, road_code,
                                          code_field, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_PA
                        exist_sapa_facil = True
                elif self.is_same_road_code(temp_path, road_code,  # 回到当前线路
                                            code_field, reverse):
                    if self._is_sapa_path(temp_path, road_code,
                                          code_field, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_PA
                        exist_sapa_facil = True
                    else:
                        # print temp_path[1:]
                        # 辅路、类辅路设施
                        yield temp_path[1:], HWY_IC_TYPE_SERVICE_ROAD
                        continue
                # 和一般道交汇
                if self.is_hwy_inout(temp_path, reverse):
                    yield temp_path[1:], HWY_IC_TYPE_IC
                    if self.is_city_hwy_ic(temp_path, reverse):
                        pass
                    else:
                        continue
                    curr_inout_flag = True
                if len(visited) < cutoff2:
                    if self.is_virtual_jct(child, road_code,
                                           code_field, reverse):
                        yield temp_path[1:], HWY_IC_TYPE_VIRTUAl_JCT
                    # 取得非本线
                    nodes = self._get_not_main_link(child, code_field,
                                                    reverse, ignore_inner)
                    if nodes:
                        visited.append(child)
                        stack.append(iter(nodes))
                        if self._is_both_dir_sapa(visited[-2], visited[-1],
                                                  reverse):
                            both_sapa_cnt += 1
                    else:
                        if not curr_inout_flag:  # 当前点已经是出入口，不再判断
                            # 和一般道相连
                            if reverse:  # 逆
                                u = temp_path[-1]
                                v = temp_path[-2]
                            else:  # 顺
                                u = temp_path[-2]
                                v = temp_path[-1]
                            if self.get_normal_link(u, v, reverse):
                                yield temp_path[1:], HWY_IC_TYPE_IC
                                continue
                elif len(visited) == cutoff2:
                    if(len(visited) > 1 and
                       self._is_both_dir_sapa(visited[-2], visited[-1],
                                              reverse)):
                        both_sapa_cnt -= 1
                    stack.pop()
                    visited.pop()
        # 存在SAPA link，但是没有生成SAPA设施
        if sapa_link and not exist_sapa_facil:
            for path in self._all_sapa_path(u, v, road_code,
                                            code_field, reverse,
                                            cutoff=MAX_CUT_OFF / 2):
                # 保证另头是SAPA link
                if reverse:
                    u, v = path[-1], path[-2]
                else:
                    u, v = path[-2], path[-1]
                if self.is_sapa_link(u, v):
                    exist_sapa_facil = True
                    yield path, HWY_IC_TYPE_PA
            if not exist_sapa_facil:
                print ('Exist SAPA Link, but no SAPA Facility. u=%s,v=%s'
                       % (u, v))

    def is_city_hwy_ic(self, path, reverse=False):
        '''高速和城市高速的jct、ramp、sapa相连 '''
        if reverse:  # 逆
            edges_iter = self.in_edges_iter(path[-1], True)
            # in_edge = (path[-1], path[-2])
        else:  # 顺
            edges_iter = self.out_edges_iter(path[-1], True)
            # in_edge = (path[-2], path[-1])
        for temp_u, temp_v, data in edges_iter:
            if reverse:  # 逆
                temp_path = path + [temp_u]
            else:  # 顺
                temp_path = path + [temp_v]
            # 规制
            if not self.check_regulation(temp_path, reverse):
                continue
            road_type = data.get(HWY_ROAD_TYPE)
            link_type = data.get(HWY_LINK_TYPE)
            #与城市高速的JCT/RAMP/SAPA相连
            if(road_type == HWY_ROAD_TYPE_HWY1 and
               link_type in (HWY_LINK_TYPE_JCT,
                             HWY_LINK_TYPE_RAMP,
                             HWY_LINK_TYPE_SAPA)):
                return True
        return False

    def get_uturns(self, path, road_code,
                   code_field=HWY_ROAD_CODE, reverse=False):
        if reverse:
            in_edge = (path[1], path[0])
        else:
            in_edge = (path[0], path[1])
        node = path[-1]
        main_nodes = self.get_main_link(node, road_code, code_field,
                                        False, reverse)
        if main_nodes:
            for e_v in main_nodes:
                if not reverse:
                    out_edge = (node, e_v)
                else:
                    out_edge = (e_v, node)
                if self._is_uturn(in_edge, out_edge, reverse):
                    yield e_v, True  # U-turn
                else:
                    yield e_v, False  # JCT

    def _is_uturn(self, in_edge, out_edge, reverse):
        # 进入本线和退出本线road_code相同,updown_c不同
        if set(in_edge) & set(out_edge):  # 相交
            return False
        data1 = self[in_edge[0]][in_edge[1]]
        data2 = self[out_edge[0]][out_edge[1]]
        if (data1.get(HWY_ROAD_CODE) == data2.get(HWY_ROAD_CODE) and
            data1.get(HWY_UPDOWN) != data2.get(HWY_UPDOWN)):
            return True
        else:
            return False
