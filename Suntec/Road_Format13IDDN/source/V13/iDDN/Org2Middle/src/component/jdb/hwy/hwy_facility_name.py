# -*- coding: UTF-8 -*-
'''
Created on 2014年10月17日

@author: hcz
'''
import base
from component.jdb.hwy.hwy_data_mng import HwyDataMng
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import IC_TYPE_SMART_IC
from component.jdb.hwy.hwy_def import INOUT_TYPE_OUT
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import ROAD_CODE_NO_TOLL
LINK_TYPE_SAPA = 7


# =============================================================================
# HwyFacilityName
# =============================================================================
class HwyFacilityName(base.component_base.comp_base):
    '''
    HWY Facility Name, 给Toward Name使用。
    Include: 高速道设施和调整无料区间设施名称
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'HwyFacilityName')
        self._all_facils = []  # 当前点的所有并设
        self._all_pathes_dict = {}  # 当前点所有设施的所有路情况
        self._curr_node = None

    def _DoCreateIndex(self):
        self.CreateIndex2('mid_hwy_facility_name_for_tow_'
                          'nodeid_inlinkid_outlinkid_pas_idx')

        return 0

    def _Do(self):
        # 出口
        self.CreateTable2('mid_hwy_facility_name_for_towardname')
        for info in self._get_all_out_facility_nodes():
            node_id, inlinkid = info[0:2]
            self._set_all_facils(node_id)  # 获取点的所有设施(并设)
            for facil_info in self._all_facils:
                passlink_list = []
                outlink_list = []
                facilcls = facil_info.facilcls
                # SmartIC，且有并设，不收录
                if(facilcls == IC_TYPE_SMART_IC and
                   len(self._all_facils) > 1):
                    continue
                path_infos = self._get_pathes_info(facil_info)
                if not path_infos:
                    self.log.error("None Path. node_id=%s" % node_id)
                    continue
                for facil, link_lid, node_lid, path_type in path_infos:
                    # SAPA设施的IC路径， 如："亀山ＰＡ"
                    if self._is_sapa_icpath(facil_info, path_type):
                        continue
                    if path_type == 'UTURN':
                        # 判断UTurn路径是不是唯一的一条路径
                        if self._judge_uturn_path(facil_info, path_type):
                            path_type = 'SAPA'
                        else:
                            continue
                    # 高速出口路径的最后一个点在无料区间本线（通常发生在高速和无料区间交界处）不收录， 如：八竜ＩＣ, 高岡ＩＣ
                    if(path_type == 'IC' and
                       facil_info.inout == INOUT_TYPE_OUT and
                       self._is_on_no_toll_main_link(node_lid[-1])):
                        self.log.info('点在无料区间本线。node_id=%s,LastNode=%s,name=%s'
                                      % (facil_info.node_id, node_lid[-1],
                                         facil_info.facil_name))
                        continue
                    # SAPA设施，经过SAPA所在设施点的路径
                    if(path_type == 'SAPA' and
                       not self._is_through_sapa_pos(node_lid)):
                        continue
                    # 非SAPA设施，且路径经过SAPA Link的不收录
                    if(path_type != 'SAPA' and
                       self._is_through_sapa_link(link_lid)):
                        continue
                    # 判断路径的【环】
                    if not self._judge_cycle(facil_info, node_lid, path_type):
                        continue
                    out_link_pos = self._get_out_link_pos(node_lid,
                                                          link_lid,
                                                          node_id,
                                                          facil_info.inout,
                                                          path_type)
                    passlink, outlink = self._get_passlink_outlink(node_lid,
                                                                   link_lid,
                                                                   out_link_pos
                                                                   )
                    if not outlink:
                        self.log.error('outlink is none , node_id=%s, name=%s'
                                       % (node_id, facil_info.facil_name))
                        continue
                    passlink_list.append(passlink)
                    outlink_list.append(outlink)
                if not outlink_list:  # 没有找到路径
                    facil_name = facil_info.facil_name
                    if facil_name in ('観音ランプ', '八草東ＩＣ', '浜松いなさ北ＩＣ'):
                        self.log.warning('No OutLink. node_id=%s,name=%s'
                                         % (node_id, facil_name))
                    else:
                        self.log.error('No OutLink. node_id=%s,name=%s'
                                       % (node_id, facil_name))
                    continue
                # 获取JCT本线out_link
                main_outlinks = self._get_main_outlink_for_jct(facil_info)
                for outlink in main_outlinks:
                    passlink_list.append(())
                    outlink_list.append(outlink)
                self._insert_facility_name(inlinkid, node_id,
                                           passlink_list, outlink_list,
                                           facil_info.facilcls,
                                           facil_info.facil_name,
                                           facil_info.name_yomi)
        self.pg.commit2()
        return 0

    def _set_all_facils(self, node_id):
        if self._curr_node != node_id:
            hwy_data = HwyDataMng.instance()
            self._all_facils = hwy_data.get_hwy_facils_by_nodeid(node_id)
            self._set_all_pathes_info()
            self._curr_node = node_id

    def _set_all_pathes_info(self):
        '''当前点的所有设施的所有路径'''
        for facil_info in self._all_facils:
            path_infos = self.get_pathes_info_from_mng(facil_info)
            if len(path_infos) == 1:
                path_info = path_infos[0]
                path_type = path_info[3]
                if(path_type == 'UTURN' and
                   facil_info.facilcls in (IC_TYPE_SA, IC_TYPE_PA)):
                        # 修改 path_type: ['UTURN' ==> 'SAPA']
                        path_type = 'SAPA'
                        path_infos = [path_info[0:3] + (path_type,)]
            self._all_pathes_dict[facil_info] = path_infos

    def _get_pathes_info(self, facil_info):
        return self._all_pathes_dict.get(facil_info)

    def get_pathes_info_from_mng(self, facil_info):
        '''获取路径情报。'''
        if not facil_info:
            return []
        path_infos = []
        hwy_data = HwyDataMng.instance()
        if facil_info.inout == INOUT_TYPE_OUT:  # 出口(分歧)
            path_infos = hwy_data.get_ic_path_info(facil_info)
        else:  # 入口(合流)
            path_infos = hwy_data.get_ic_path_info(facil_info)
        return path_infos

    def _just_one_path(self, path_infos):
        if len(path_infos) == 1:
            return True
        return False

    def _is_sapa_icpath(self, facil_info, path_type):
        '''SAPA设施的IC路径'''
        if(facil_info.facilcls in (IC_TYPE_SA, IC_TYPE_PA)
           and path_type == 'IC'):
            if facil_info.facil_name != '亀山ＰＡ':
                self.log.info('SAPA设施的IC路径. name=%s' % facil_info.facil_name)
            return True
        else:
            return False

    def _judge_uturn_path(self, facil_info, path_type):
        '''判断uturn路径是不是唯一的一条路径。'''
        # ## UTurn
        if path_type == 'UTURN':
            path_infos = self._get_pathes_info(facil_info)
            if self._just_one_path(path_infos):  # UTURN， 是该设施的唯一路径
                if facil_info.facilcls not in (IC_TYPE_SA, IC_TYPE_PA):
                    self.log.error('Error: Unknown UTURN Path. node_id=%s'
                                   % facil_info.node_id)
                    return False
            else:
                return False  # 该设施有多条路径，不收录UTurn
        return True

    def _judge_cycle(self, facil_info, node_lid, path_type):
        if self._exist_cycle(node_lid):  # 当前路径里有【环】
            path_infos = self._get_pathes_info(facil_info)
            all_path = []
            if path_type == 'SAPA':
                all_path = self._get_all_path_through_sapa_pos(path_infos)
            else:
                all_path = self._get_all_path_no_through_sapa(path_infos)
            # 有所路径都有环，且当前路径只绕一圈
            if(self._all_path_exist_cycle(all_path) and
               self._just_once_cycle(node_lid)):
                pass
            else:
                return False
        return True

    def _get_main_outlink_for_jct(self, facil_info):
        '''获取JCT脱出本线link'''
        # 当前高速点上只有一个JCT设施 or 该JCT设施是父设施
        if not facil_info.facilcls == IC_TYPE_JCT:
            return []
        rst_outlinks = []
        if self._get_jct_num() == 1 or self._is_father_jct(facil_info):
            node_id = facil_info.node_id
            G = self._get_graph()
            # 取设施road_code和link的road_code相同的本线
            outlinkids = G.get_main_outlinkids(node_id, facil_info.road_code)
            if not outlinkids:
                # 取本线link
                outlinkids = G.get_main_outlinkids(node_id)
            # 判断link_id在当前HwyNode点上某一设施的某一条路径里
            for outlink in outlinkids:
                if not self._is_in_path(outlink):  # 不已经路径里
                    rst_outlinks.append(outlink)
            if rst_outlinks:
                return rst_outlinks
            else:
                # 判断所有脱出link都已经在路径里
                for outlink in G.get_outlinkids(node_id):
                    if not self._is_in_path(outlink):  # 不已经路径里
                        self.log.error('JCT有不在路径里有脱 出link。nodeid=%s,name=%s'
                                       % (node_id, facil_info.facil_name))
        return rst_outlinks

    def _is_father_jct(self, facil_info):
        '''该JCT设施是父设施, 或者点上只有一个设施。'''
        if facil_info.facilcls != IC_TYPE_JCT:
            return False
        if len(self._all_facils) == 1:  # 只有一个设施
            return True
        self.log.info('有多个JCT设旋. node_id=%s,name=%s'
                      % (self._curr_node, facil_info.facil_name))
        hwy_data = HwyDataMng.instance()
        if hwy_data.is_parent_facil(facil_info.road_code,
                                    facil_info.road_point,
                                    facil_info.updown):
            return True
        return False

    def _get_jct_num(self):
        jct_num = 0
        for temp_facil_info in self._all_facils:
            if temp_facil_info.facilcls == IC_TYPE_JCT:
                jct_num += 1
        return jct_num

    def _get_end_pos(self, path_type, node_lid, link_lid):
        end_pos = len(link_lid) - 1
        if path_type == 'SAPA':
            sapa_pos = self.get_sapa_pos(node_lid)
            if end_pos >= 0:
                end_pos = sapa_pos - 1
        return end_pos

    def get_sapa_pos(self, path):
        '''取得SAPA所在点的位置(index), 如果没有找到返回-1'''
        hwy_data = HwyDataMng.instance()
        for pos in range(0, len(path)):
            nodeid = path[pos]
            if hwy_data.is_sapa_pos_node(nodeid):
                return pos
        return -1

    def _get_same_facils(self, facil_info):
        # 取得并设(同点)，但不包含当前设施
        same_facil = []
        for facil in self._all_facils:
            if facil_info.road_point != facil.road_point:
                same_facil.append(facil)
        return same_facil

    def _get_facil_info(self, facility_id):
        '''从并设里，取设施情报'''
        hwy_data = HwyDataMng.instance()
        for facil in self._all_facils:
            road_code = facil.road_code
            road_point = facil.road_point
            temp_facil_id = hwy_data.get_facility_id(road_code, road_point)
            if temp_facil_id == facility_id:
                return facil
        return None

    def _is_smart_ic(self, facil_info):
        if not facil_info:
            return False
        if facil_info.facilcls == IC_TYPE_SMART_IC:
            return True
        else:
            return False

    def _is_through_sapa_link(self, lind_lid):
        '''路径经过SAPA link'''
        hwy_data = HwyDataMng.instance()
        for link_id in lind_lid:
            link_type = hwy_data.get_link_type(link_id)
            if link_type == LINK_TYPE_SAPA:
                return True
        return False

    def _is_through_sapa_pos(self, node_lid):
        '''路径经过SAPA设施所在位置'''
        hwy_data = HwyDataMng.instance()
        for node_id in node_lid:
            if hwy_data.is_sapa_pos_node(node_id):
                return True
        return False

    def _in_sapa_shortest_path(self, sapa_facil, link_id):
        '''link_id在SAPA的最短路径(经过SAPA设施所在点的最短路径和不经过SAPA设施所在点)里'''
        path_infos = self._get_pathes_info(sapa_facil)
        if not path_infos:
            return False
        shortest_path = self._get_shortest_path_no_through_sapa_pos(path_infos)
        if link_id in shortest_path:
            return True
        shortest_path = self._get_shortest_path_through_sapa_pos(path_infos)
        if link_id in shortest_path:
            return True
        return False

    def _get_shortest_path_through_sapa_pos(self, path_infos):
        '''经过SAPA设施所在点的最短路径'''
        shortest_path = ()
        shortest_len = 0
        for facil, link_lid, node_lid, path_type in path_infos:
            if self._is_through_sapa_pos(node_lid):
                if len(link_lid) < shortest_len or shortest_len == 0:
                    shortest_path = link_lid
                    shortest_len = len(link_lid)
        return shortest_path

    def _get_shortest_path_no_through_sapa_pos(self, path_infos):
        '''不经过SAPA设施所在点的最短路径. 返回的是[link序, node序】'''
        shortest_path = []
        shortest_len = 0
        for facil, link_lid, node_lid, path_type in path_infos:
            if not self._is_through_sapa_pos(node_lid):
                if len(link_lid) < shortest_len or shortest_len == 0:
                    shortest_path = link_lid
                    shortest_len = len(link_lid)
        return shortest_path

    def _all_through_sapa_pos(self, facil_info):
        '''SmartIC的每条路径都经过SAPA设施所在点.'''
        path_infos = self._get_pathes_info(facil_info)
        if not path_infos:
            self.log.error('没有路径')
        for facil, link_lid, node_lid, path_type in path_infos:
            if not self._is_through_sapa_pos(node_lid):
                return False
        return True

    def _all_path_exist_cycle(self, all_path):
        '''所有路径都有环。'''
        if not all_path:
            return False
        for nodes in all_path:
            if not self._exist_cycle(nodes):
                return False
        return True

    def _get_all_path(self, path_infos):
        all_path = []
        for facil, link_lid, node_lid, path_type in path_infos:
            all_path.append(node_lid)
        return all_path

    def _get_all_path_no_through_sapa(self, path_infos):
        all_path = []
        for facil, link_lid, node_lid, path_type in path_infos:
            if(not self._is_through_sapa_pos(node_lid)
               and not self._is_through_sapa_link(link_lid)):
                all_path.append(node_lid)
        return all_path

    def _get_all_path_through_sapa_pos(self, path_infos):
        '''获取所有通过SAPA设施所在点的路径'''
        all_path = []
        for facil, link_lid, node_lid, path_type in path_infos:
            if self._is_through_sapa_pos(node_lid):
                all_path.append(node_lid)
        return all_path

    def _just_once_cycle(self, nodes):
        '''只绕一圈: 只是简单判断一点出现三次，所以同个点如果出现在两个圈里，也被认为是出现三次。'''
        if not nodes:
            return True
        for node in nodes:
            if nodes.count(node) >= 3:
                return False
        return True

    def _exist_cycle(self, nodes):
        '''存在环划'''
        if self.get_simple_cycle(nodes):
            return True
        return False

    def get_simple_cycle(self, nodes):
        import networkx as nx
        G = nx.DiGraph()
        G.add_path(nodes)
        return list(nx.simple_cycles(G))

    def _is_node_in_cycle(self, nodes, node_id):
        '''点在环上'''
        cycles = self.get_simple_cycle(nodes)
        for cycle in cycles:
            if node_id in cycle:
                return True
        return False

    def _get_out_link_pos(self, node_lid, link_lid,
                          ic_node, inout, path_type):
        '''附加情报link的位置
           Out(出口): 后方设施要唯一, 前方没有其他分支
           In(入口): 前方设施要唯一
        '''
        end_pos = self._get_end_pos(path_type, node_lid, link_lid)
        link_idx = end_pos
        while link_idx >= 0:
            link_id = link_lid[link_idx]
            if inout == INOUT_TYPE_OUT:  # 出口
                # 取得link的后方所有设施(同点即并列)
                ic_num = self._get_bwd_facil_num(ic_node,
                                                 link_id,
                                                 path_type)
                if ic_num <= 1:  # 后方设施唯一
                    node_id = node_lid[link_idx]
                    # 有其他分支
                    if self._exist_other_branches(node_id, path_type):
                        # 当前link有的进入点(车流方向)有分支，所以out_link是当前link
                        return link_idx
                    else:  # 没有其他分支，继续
                        pass
                else:
                    break
            else:  # 入口
                ic_num = self._get_fwd_facil_num(None, link_id)
                if ic_num == 1:  # link前方就一个facility_id
                    break
                else:
                    # 取得link的前方所有设施(并列)
                    ic_num = self._get_fwd_facil_num(ic_node, link_id)
                    if ic_num == 1:  # 无并列
                        if self._is_bwd_ic_same_node(link_id):
                            print 'bwd_ic_same_node, link_id=%s' % link_id
                            break
            link_idx -= 1
        # 最后一根link是有多个设施经过的，所以要取最后一根的下一根(通常是脱出本线)
        if link_idx == len(link_lid) - 1:
            return link_idx + 1
        if link_idx != end_pos:  # SAPA所在的那根，有可能就是out_link
            link_idx += 1  # 当前link有后方设施多个，所以out_link是前一根(车流方向)
        return link_idx

    def _get_bwd_facil_num(self, ic_node, link_id, path_type):
        '''后方设施数
           ic_node: 设施的HighWay Node点,
                    ic_node无指定，link后方所有设施数目;
                    ic_node指定，和ic_node同点的后方设施(并列)
           link_id: Path路径上的某一条link
        '''
        facil_id_set = set()
        # # 取得前后方设施
        hwy_data = HwyDataMng.instance()
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        if len(bwd_list) == 1:
            return 1
        # 后方设施数
        for bwd in bwd_list:
            temp_node_id = bwd.get('node_id')
            temp_path_type = bwd.get('path_type')
            bwd_facil_id = bwd.get('facility_id')
            if ic_node != temp_node_id:
                continue
            if path_type in ('SAPA'):
                link_type = hwy_data.get_link_type(link_id)
                bwd_facil = self._get_facil_info(bwd_facil_id)
                if not bwd_facil:
                    pass
                if self._is_smart_ic(bwd_facil):  # 后方设施是SmartIC
                    # SmartIC的所有路径都通过，SAPA设施所在点
                    if self._all_through_sapa_pos(bwd_facil):
                        continue
                    else:  # 不是所有的路径都经过SAPA所在点
                        facil_id_set.add(bwd_facil_id)
                else:
                    # IC, JCT的路径
                    if temp_path_type != 'SAPA' and link_type == 7:
                        # 其他设施不应该通过SAPA Link，即去掉经过SAPA link的路径
                        continue
                    else:
                        if temp_path_type != 'UTURN':
                            facil_id_set.add(bwd_facil_id)
            elif path_type in ('JCT'):
                facil_id_set.add(bwd_facil_id)
            elif path_type in ('IC'):
                if temp_path_type == 'SAPA':
                    sapa_facil = self._get_facil_info(bwd_facil_id)
                    # 有
                    if self._in_sapa_shortest_path(sapa_facil, link_id):
                        facil_id_set.add(bwd_facil_id)
                    else:
                        pass
                elif temp_path_type != 'UTURN':
                    facil_id_set.add(bwd_facil_id)
            else:
                if temp_path_type != 'UTURN':
                    facil_id_set.add(bwd_facil_id)
        return len(facil_id_set)

    def _exist_other_branches(self, node_id, curr_path_type):
        '''脱出方向有其他分支，这些分支是不在当前HwyNode的所有设施的所有路径上的'''
        G = self._get_graph()
        if not G:
            return False
        # node_id的所有脱出link(road_type=9除外，即细街路2除外)
        out_links = G.get_outlinkids_except_narrow_road(node_id)
        if len(out_links) == 1:
            return False
        for out_link in out_links:
            # 判断out_link在ic_node点上某一设施的某一条路径里
            if not self._is_in_path(out_link):
                # 经过经过out_link的所有路径的path_type和curr_path_type一致
                if not self._same_path_type(out_link, curr_path_type):
                    return True
        return False

    def _same_path_type(self, link_id, curr_path_type):
        '''经过link_id的所有路径的path_type和curr_path_type一致'''
        hwy_data = HwyDataMng.instance()
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        if not bwd_list:
            return False
        for bwd in bwd_list:
            temp_path_type = bwd.get('path_type')
            if(temp_path_type != 'UTURN' and
               temp_path_type != curr_path_type):
                return False
        return True

    def _get_graph(self):
        hwy_data = HwyDataMng.instance()
        toll_G = hwy_data.get_graph()  # 有高速
        ic_node = self._curr_node
        no_toll_G = hwy_data.get_graph_no_toll()  # 无料区间
        if ic_node not in toll_G and ic_node not in no_toll_G:
            self.log.error('高速 点同时不在有料高速,也无料区间里。node_id=%s' % ic_node)
            return None
        if ic_node in toll_G:
            # inlink是有料高速本线
            for parent, node, data in toll_G.in_edges_iter(ic_node, True):
                if data.get('road_code') == self._all_facils[0].road_code:
                    return toll_G
        return no_toll_G

    def _is_in_path(self, link_id):
        '''判断link_id在当前HwyNode点上某一设施的某一条路径里'''
        for path_infos in self._all_pathes_dict.values():
            for facil, link_lid, node_lid, path_type in path_infos:
                if link_id in link_lid:
                    return True
        return False

    def _is_on_no_toll_main_link(self, node_id):
        '''点在无料区间本线上。'''
        hwy_data = HwyDataMng.instance()
        no_toll_G = hwy_data.get_graph_no_toll()  # 无料区间
        if node_id not in no_toll_G:
            return False
        # inlink是有料高速本线
        for parent, node, data in no_toll_G.in_edges_iter(node_id, True):
            if data.get('road_code') > ROAD_CODE_NO_TOLL:
                return True
        for parent, node, data in no_toll_G.out_edges_iter(node_id, True):
            if data.get('road_code') > ROAD_CODE_NO_TOLL:
                return True
        return False

    def _get_passlink_outlink(self, node_lid, link_lid, out_link_pos):
        out_link = None
        pass_link = []
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        if out_link_pos > 0 and out_link_pos == len(link_lid):
            outlinkids = G.get_main_outlinkids(node_lid[-1])
            if outlinkids:
                out_link = outlinkids[0]
            else:
                self.log.error('Error: None Out Link.')
            pass_link = link_lid
        else:
            pass_link = link_lid[:out_link_pos]
            out_link = link_lid[out_link_pos]
        return pass_link, out_link

    def _get_all_out_facility_nodes(self):
        sqlcmd = """
        SELECT distinct s_node_id, inlinkid
          FROM mid_hwy_ic_path
          where s_inout_c = 2
          order by s_node_id, inlinkid;
        """
        return self.get_batch_data(sqlcmd)

    def _insert_facility_name(self, inlinkid, node_id, passlink_list,
                              outlink_list, facilcls,
                              facil_name, name_yomi):
        sqlcmd = """
        INSERT INTO mid_hwy_facility_name_for_towardname(
                    nodeid, inlinkid, outlinkid,
                    passlid, passlink_cnt, facilcls,
                    sp_name)
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    mid_get_json_string_for_japan_names(array[%s], array[%s])
                    );
        """
        temp_set = set()
        for passlink, outlink in zip(passlink_list, outlink_list):
            passlink_cnt = len(passlink)
            passlink = '|'.join([str(l) for l in passlink])
            if (passlink, outlink) in temp_set:
                continue
            param = (node_id, inlinkid, outlink,
                     passlink, passlink_cnt, facilcls,
                     facil_name, name_yomi)
            self.pg.execute2(sqlcmd, param)
            temp_set.add((passlink, outlink))
