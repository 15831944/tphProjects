# -*- coding: UTF8 -*-
'''
Created on 2015-10-26

@author: hcz
'''

from component.rdf.hwy.hwy_ic_info_rdf import HwyICInfoRDF
from component.jdb.hwy.hwy_ic_info import ADD_NODE_POS_TYPE_NONE
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_IC
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_SAPA
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT


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

    def _get_out_node_add_info(self):
        '''设施出口附加情报'''
        add_info_list = []
        ic_node = self.first_facil.node_id
        facilcls = self.first_facil.facilcls
        inout = self.first_facil.inout
        pos_type = ADD_NODE_POS_TYPE_NONE  # 无料金所
        # 出口进入，[井]、[十]，[T]字型出口
        add_node_links = self._get_well_cross_add_info(ic_node, inout)
        if add_node_links:
            add_node, add_link, dis_s, dir_e = add_node_links[0]
            add_info = self._set_add_info(add_link, add_node, inout,
                                          0, pos_type, facilcls,
                                          [add_link], [add_node],
                                          dis_s, dir_e)
            add_info_list.append(add_info)
            return add_info_list
        for node_lid, link_lid, path_type in self.out_path_infos:
            if path_type != HWY_PATH_TYPE_IC:
                continue
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
            add_info = {}
            # ## 路径link为空, 如:【点JCT】、【出口直接和一般道相连】
            if not link_lid:
                add_node_links = self._get_special_out_node_add_info(node_lid,
                                                                     link_lid)
                if not add_node_links:
                    self.log.error('No Node Add Info(Out). node=%s' % ic_node)
                for add_node, add_link, dis_s, dir_e in add_node_links:
                    add_info = self._set_add_info(add_link, add_node, inout,
                                                  0, pos_type, facilcls,
                                                  [add_link], [add_node],
                                                  dis_s, dir_e)
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
            link_pos = self._get_add_info_link_pos(link_lid, end_pos,
                                                   ic_node, inout, path_type)
            if link_pos >= len(link_lid):
                link_pos = len(link_lid) - 1
                self.log.warning('No Add Info of IC OUT. node=%s,'
                                 'facilcls=%s, link_lid=%s' %
                                 (ic_node, facilcls, link_lid))
#             if link_pos >= len(link_lid):
#                 if path_type == HWY_PATH_TYPE_IC:
#                     rst_add_info = self._get_nwy_out_node_add_info(node_lid,
#                                                                    ramp=True)
#                     add_node_links = rst_add_info[1]
#                     if not add_node_links:
#                         self.log.error('No Node Add Info(Out). node=%s,'
#                                        'facilcls=%s, link_lid=%s'
#                                        % (ic_node, facilcls, link_lid))
#                     for add_node, add_link in add_node_links:
#                         add_info = self._set_add_info(add_link, add_node,
#                                                       inout,  0,
#                                                       pos_type, facilcls,
#                                                       link_lid,
#                                                       node_lid)
#                         add_info_list.append(add_info)
#                     continue
#                 else:
#                     if path_type == HWY_PATH_TYPE_JCT:
#                         pass
#                     else:
#                         self.log.warning('No Add Info. node=%s, facilcls=%s,'
#                                          ' link_lid=%s' %
#                                          (ic_node, facilcls, link_lid))
#                     continue
            if(path_type == HWY_PATH_TYPE_SAPA and
               link_pos == sapa_pos and
               sapa_node):
                add_node = sapa_node
            else:
                add_node = node_lid[link_pos]
            temp_link_lid = link_lid
            temp_node_lid = node_lid
            add_link = link_lid[link_pos]
            dir_s_node = node_lid[link_pos]
            dir_e_node = node_lid[link_pos + 1]
            # ## 求附加情报点和料金的关系
            # pos_type = self._get_pos_type(node_lid, link_pos)
            add_info = self._set_add_info(add_link, add_node, inout,
                                          link_pos, pos_type, facilcls,
                                          temp_link_lid, temp_node_lid,
                                          dir_s_node, dir_e_node)
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
        add_info_list = []
        pos_type = ADD_NODE_POS_TYPE_NONE  # 无料金所
        facilcls = self.first_facil.facilcls
        # [井]、[十]，[T]字型入口 (起始)
        add_node_links = self._get_well_cross_add_info(ic_node, inout)
        if add_node_links:
            add_node, add_link, dir_s, dir_e = add_node_links[0]
            add_info = self._set_add_info(add_link, add_node, inout,
                                          0, pos_type, facilcls,
                                          [add_link], [add_node],
                                          dir_s, dir_e)
            add_info_list.append(add_info)
            return add_info_list
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
                                                  dir_s, dir_e)
                    add_info_list.append(add_info)
                continue
            end_pos = len(link_lid)
            link_pos = self._get_add_info_link_pos(link_lid, end_pos,
                                                   ic_node, inout, path_type)
            if link_pos == len(link_lid):
                link_pos = len(link_lid) - 1
                self.log.warning('No Add Info of IC IN. node=%s, facilcls=%s,'
                                 'link_lid=%s.'
                                 % (ic_node, facilcls, link_lid))
#             if link_pos == len(link_lid):
#                 rst_add_info = self._get_nwy_in_node_add_info(node_lid,
#                                                               ramp=True)
#                 add_node_links = rst_add_info[1]
#                 if not add_node_links:
#                     self.log.warning('No Node Add Info(IN). node=%s,'
#                                      'linklid=%s' % (ic_node, link_lid))
#                 for add_node, add_link in add_node_links:
#                     add_info = self._set_add_info(add_link, add_node, inout,
#                                                   0, pos_type, facilcls,
#                                                   link_lid, node_lid)
#                     add_info_list.append(add_info)
#                 self.log.warning('No Add Info of IC IN. node=%s, link_lid=%s'
#                                  % (ic_node, link_lid))
#                 continue
            add_link = link_lid[link_pos]
            add_node = node_lid[link_pos]
            dir_s_node = node_lid[link_pos + 1]
            dir_e_node = node_lid[link_pos]
            # ## 求附加情报点和料金的关系
            # pos_type = self._get_pos_type(node_lid, link_pos)
            add_info = self._set_add_info(add_link, add_node, inout,
                                          link_pos, pos_type, facilcls,
                                          link_lid, node_lid,
                                          dir_s_node, dir_e_node)
            add_info_list.append(add_info)
        return add_info_list
