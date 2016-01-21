# -*- coding: UTF8 -*-
'''
Created on 2015-10-26

@author: hcz
'''

from component.rdf.hwy.hwy_ic_info_rdf import HwyICInfoRDF
from component.jdb.hwy.hwy_ic_info import ADD_NODE_POS_TYPE_NONE
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_IC
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_JCT
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_SAPA
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SAPA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.ni.hwy_pro.hwy_facility_pro import HWY_UNKOWN_ID_NI


class HwyICInfoNIPro(HwyICInfoRDF):
    '''
    classdocs
    '''

    def __init__(self, ic_no, facility_id, facil_list,
                 data_mng, item_name='HwyICInfoNIPro'):
        '''
        Constructor
        '''
        HwyICInfoRDF.__init__(self, ic_no, facility_id, facil_list,
                              data_mng, item_name)

    def _set_jct_dummy_flg(self):
        if(self.first_facil.road_point == HWY_UNKOWN_ID_NI and
           self.first_facil.facilcls == HWY_IC_TYPE_JCT):
            self.dummy = HWY_TRUE
        else:
            HwyICInfoRDF._set_jct_dummy_flg(self)

    def set_node_add_info(self):
        '''制作 临时附加情报表：并切短Path'''
        HwyICInfoRDF.set_node_add_info(self)
        # 所有link都是两向link. 附加情报有多条, 中国就一个Case, 坐标: 97.199191,37.340645
        if self._is_special_ic_add_info():
            self.log.warning('Force using first link for add link.'
                             'node=%s, inout=%s' %
                             (self.first_facil.node_id, self.first_facil.inout)
                             )
            # 强制使用第一条link做为附加情报情报所在link
            # 其他属性暂时保持不变
            for add_info in self.add_infos:
                node_lid = add_info['node_lid']
                link_lid = add_info['link_lid']
                inout = add_info['in_out']
                if link_lid:
                    link_lid = map(int, link_lid.split(','))
                node_lid = map(int, node_lid.split(','))
                add_info['add_link'] = link_lid[0]
                add_info['add_node'] = node_lid[0]
                if inout == HWY_INOUT_TYPE_OUT:
                    dir_s_node = node_lid[0]
                    dir_e_node = node_lid[1]
                else:
                    dir_s_node = node_lid[1]
                    dir_e_node = node_lid[0]
                add_info['dir_s_node'] = dir_s_node
                add_info['dir_e_node'] = dir_e_node

    def _get_out_node_add_info(self):
        '''设施出口附加情报'''
        add_info_list = []
        ic_node = self.first_facil.node_id
        facilcls = self.first_facil.facilcls
        inout = self.first_facil.inout
        pos_type = ADD_NODE_POS_TYPE_NONE  # 无料金所
        last_links = set()
        if inout != HWY_INOUT_TYPE_OUT:
            return add_info_list
        in_link = self._get_facil_inlink()  # 本线进入link
        # 出口进入，[井]、[十]，[T]字型出口
        if facilcls == HWY_IC_TYPE_IC:
            add_node_links = self._get_well_cross_add_info(ic_node, inout)
            if add_node_links:
                add_node, add_link, dis_s, dir_e = add_node_links[0]
                add_info = self._set_add_info(add_link, add_node, inout,
                                              0, pos_type, facilcls,
                                              [add_link], [add_node],
                                              dis_s, dir_e,
                                              HWY_PATH_TYPE_IC, in_link)
                add_info_list.append(add_info)
                return add_info_list
        for node_lid, link_lid, path_type in self.out_path_infos:
            if path_type == HWY_PATH_TYPE_UTURN:
                if len(self.out_path_infos) > 1:
                    continue
                else:  # UTURN， 是该设施的唯一路径
                    if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                        path_type = HWY_PATH_TYPE_SAPA
                    else:
                        if facilcls != HWY_IC_TYPE_JCT:
                            self.log.error('Unknown UTURN Path. node_id=%s'
                                           % ic_node)
                        else:
                            path_type = HWY_PATH_TYPE_JCT
            add_info = {}
            # ## 路径link为空, 如:【点JCT】、【出口直接和一般道相连】
            if not link_lid:
                add_node_links = self._get_special_out_node_add_info(node_lid,
                                                                     link_lid)
                if not add_node_links:
                    self.log.error('No Node Add Info(Out). node=%s, '
                                   'facilcls=%s, path_type=%s' %
                                   (ic_node, facilcls, path_type))
                for add_node, add_link, dis_s, dir_e in add_node_links:
                    add_info = self._set_add_info(add_link, add_node, inout,
                                                  0, pos_type, facilcls,
                                                  [add_link], [add_node],
                                                  dis_s, dir_e,
                                                  path_type, in_link)
                    add_info_list.append(add_info)
                continue
            end_pos = len(link_lid)
            if path_type == HWY_PATH_TYPE_SAPA:
                sapa_pos_info = self.get_sapa_pos(node_lid, link_lid)
                sapa_pos, sapa_node = sapa_pos_info[0], sapa_pos_info[2]
                if sapa_pos < 0:
                    continue
                else:
                    end_pos = sapa_pos
            # ## 路径link不为空
            link_pos, ic_num = self._get_add_info_link_pos(link_lid, node_lid,
                                                           end_pos, ic_node,
                                                           inout, path_type)
            if(ic_num > 1 and path_type == HWY_PATH_TYPE_JCT):
                if facilcls in HWY_IC_TYPE_SAPA:
                    self.log.warning('No Add Info. node=%s, facilcls=%s,'
                                     'path_type=%s, link_lid=%s' %
                                     (ic_node, facilcls, path_type, link_lid))
                    continue
                # To Node点的退出本线link
            if link_pos >= len(link_lid):  # 没有找到后方唯一的
                # 注：ic_num是刚好最后一条link的后方设施数
                if path_type == HWY_PATH_TYPE_IC:
                    link_pos = len(link_lid) - 1
                    last_links.add(link_lid[-1])
                elif path_type == HWY_PATH_TYPE_JCT:
                    if self._is_pass_sapa_pos(link_lid):
                        # self.log.warning('Add Info, JCT Pass SAPA POS.'
                        #                 'node=%s, facilcls=%s,'
                        #                 'path_type=%s, link_lid=%s' %
                        #                 (ic_node, facilcls,
                        #                  path_type, link_lid))
                        continue
                    link_pos = len(link_lid) - 1
                    for node, link in self._get_2_main_link_of_jct(node_lid,
                                                                   inout):
                        t_link_lid = link_lid
                        t_node_lid = node_lid
                        add_link = link
                        add_node = node_lid[-1]
                        dir_s_node = node_lid[-1]
                        dir_e_node = node
                        # ## 求附加情报点和料金的关系
                        add_info = self._set_add_info(add_link, add_node,
                                                      inout, link_pos,
                                                      pos_type, facilcls,
                                                      t_link_lid, t_node_lid,
                                                      dir_s_node, dir_e_node,
                                                      path_type, in_link,
                                                      ic_num)
                        add_info_list.append(add_info)
                    continue
                else:  # SAPA/UTURN
                    self.log.warning('No Add Info. node=%s, facilcls=%s,'
                                     'path_type=%s, link_lid=%s' %
                                     (ic_node, facilcls, path_type, link_lid))
                    continue
            if facilcls in HWY_IC_TYPE_SAPA and path_type == HWY_PATH_TYPE_IC:
                # 出口经过SAPA POI所在link.
                sapa_pos_info = self.get_sapa_pos(node_lid, link_lid)
                sapa_pos = sapa_pos_info[0]
                if sapa_pos >= 0:
                    if link_pos >= sapa_pos:
                        continue
            if(path_type == HWY_PATH_TYPE_SAPA and
               link_pos == sapa_pos and
               sapa_node):
                add_node = sapa_node
                dir_s_node = sapa_node  # 靠近SAPA那个点，做为始点, 和link的one_way可能不一致
                if node_lid[link_pos + 1] != sapa_node:
                    dir_e_node = node_lid[link_pos + 1]
                else:
                    dir_e_node = node_lid[link_pos]
            else:
                if path_type == HWY_PATH_TYPE_IC:
                    link_pos = self._get_last_link_for_add_link(link_lid,
                                                                link_pos,
                                                                last_links)
                add_node = node_lid[link_pos]
                dir_s_node = node_lid[link_pos]
                dir_e_node = node_lid[link_pos + 1]
            temp_link_lid = link_lid
            temp_node_lid = node_lid
            add_link = link_lid[link_pos]
            # ## 求附加情报点和料金的关系
            add_info = self._set_add_info(add_link, add_node, inout,
                                          link_pos, pos_type, facilcls,
                                          temp_link_lid, temp_node_lid,
                                          dir_s_node, dir_e_node,
                                          path_type, in_link,
                                          ic_num)
            add_info_list.append(add_info)
        return add_info_list

    def _get_in_node_add_info(self):
        '''设施入口附加情报'''
        if self.second_facil:
            ic_node = self.second_facil.node_id
            inout = self.second_facil.inout
        else:
            ic_node = self.first_facil.node_id
            inout = self.first_facil.inout
        last_links = set()
        add_info_list = []
        pos_type = ADD_NODE_POS_TYPE_NONE  # 无料金所
        facilcls = self.first_facil.facilcls
        out_linkid = self._get_facil_outlink()  # 本线退出link
        # 出口的附加情报
        for node_lid, link_lid, path_type in self.in_path_infos:
            if path_type != HWY_PATH_TYPE_IC:
                continue
            add_info = {}
            if not link_lid:
                add_node_links = self._get_special_in_node_add_info(node_lid,
                                                                    link_lid)
                if not add_node_links:
                    G = self.data_mng.get_graph()
                    in_edges = G.in_edges(ic_node)
                    if not in_edges:  # 断头路
                        continue
                    self.log.error('No Node Add Info(IN). node=%s' % ic_node)
                for add_node, add_link, dir_s, dir_e in add_node_links:
                    add_info = self._set_add_info(add_link, add_node, inout,
                                                  0, pos_type, facilcls,
                                                  [add_link], [add_node],
                                                  dir_s, dir_e,
                                                  path_type, out_linkid)
                    add_info_list.append(add_info)
                continue
            end_pos = len(link_lid)
            link_pos, ic_num = self._get_add_info_link_pos(link_lid, node_lid,
                                                           end_pos, ic_node,
                                                           inout, path_type)
            if facilcls in HWY_IC_TYPE_SAPA:
                # 出口经过SAPA POI所在link.
                sapa_pos_info = self.get_sapa_pos(node_lid, link_lid)
                sapa_pos = sapa_pos_info[0]
                if sapa_pos >= 0:
                    if link_pos >= sapa_pos:
                        continue
            if link_pos >= len(link_lid):
                link_pos = len(link_lid) - 1
                last_links.add(link_lid[-1])
            link_pos = self._get_last_link_for_add_link(link_lid, link_pos,
                                                        last_links)
            add_link = link_lid[link_pos]
            add_node = node_lid[link_pos]
            dir_s_node = node_lid[link_pos + 1]
            dir_e_node = node_lid[link_pos]
            # ## 求附加情报点和料金的关系
            add_info = self._set_add_info(add_link, add_node, inout,
                                          link_pos, pos_type, facilcls,
                                          link_lid, node_lid,
                                          dir_s_node, dir_e_node,
                                          path_type, out_linkid,
                                          ic_num)
            add_info_list.append(add_info)
        return add_info_list

    def _get_last_link_for_add_link(self, link_lid, add_pos, last_links):
        '''一条路上有好多出口时，取最前方(最靠近高速本线)的那条link.'''
        if add_pos <= 0:
            return add_pos
        if not last_links:
            return add_pos
        link_idx = 0
        while link_idx < add_pos:
            if link_lid[link_idx] in last_links:
                return link_idx
            link_idx += 1
        return add_pos

    def _is_special_ic_add_info(self):
        '''所有link都是两向link. 附加情报有多条。'''
        if self.first_facil.facilcls != HWY_IC_TYPE_IC:
            return False
        first_links = set()
        add_links = set()
        for add_info in self.add_infos:
            add_link = add_info['add_link']
            node_lid = add_info['node_lid']
            link_lid = add_info['link_lid']
            inout = add_info['in_out']
            if link_lid:
                link_lid = map(int, link_lid.split(','))
            if add_link != link_lid[-1]:
                return False
            node_lid = map(int, node_lid.split(','))
            if inout == HWY_INOUT_TYPE_OUT:
                path = node_lid
            else:
                path = node_lid[::-1]
            # 路径上的所有link都是双向link
            if not self._all_link_are_both_dir(path):
                return False
            first_links.add(link_lid[0])
            add_links.add(add_link)
        if len(first_links) > 1 or len(add_links) < 2:
            return False
        # 附加link在同一条路上
        if self._is_on_same_path(add_links):
            return False
        # 该点有IC入口和出口
        node_id = self.first_facil.node_id
        facil_infos = self.data_mng.get_hwy_facils_by_nodeid(node_id)
        type_set = set()
        for facil_info in facil_infos:
            type_set.add((facil_info.facilcls, facil_info.inout))
        if((HWY_IC_TYPE_IC, HWY_INOUT_TYPE_OUT) not in type_set or
           (HWY_IC_TYPE_IC, HWY_INOUT_TYPE_IN) not in type_set):
            return False
        return True

    def _all_link_are_both_dir(self, path):
        if len(path) <= 1:
            return False
        G = self.data_mng.get_graph()
        for u, v in zip(path[:-1], path[1:]):
            if not G.has_edge(v, u):
                return False
        return True

    def _is_on_same_path(self, add_links):
        for add_info in self.add_infos:
            link_lid = add_info['link_lid']
            if link_lid:
                link_lid = map(int, link_lid.split(','))
            link_lid = set(link_lid)
            if link_lid.intersection(add_links) == set(add_links):
                return True
        return False

    def _is_pass_sapa_pos(self, link_lid):
        node_id = self.first_facil.node_id
        facil_infos = self.data_mng.get_hwy_facils_by_nodeid(node_id)
        hwy_data = self.data_mng
        for facil in facil_infos:
            road_code = facil.road_code
            updown = facil.updown
            if(facil.facilcls in HWY_IC_TYPE_SAPA and
               road_code == self.first_facil.road_code and
               updown == self.first_facil.updown):
                road_seq = facil.road_point
                sapa_node, sapa_link = hwy_data.get_sapa_postion(road_code,
                                                                 road_seq,
                                                                 updown)
                if sapa_link in link_lid:
                    return True
        return False
