# -*- coding: UTF-8 -*-
'''
Created on 2015-1-7

@author: hcz
'''
import json
import component.component_base
from component.rdf.hwy.hwy_graph_rdf import is_cycle_path
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import NAME_TYPE_SHIELD
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SIDE
from component.rdf.hwy.hwy_def_rdf import LAST_SEQ_NUM
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_1000M
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_1500M
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_2000M
from component.rdf.hwy.hwy_def_rdf import ROUTE_DISTANCE_2500M
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_NAME_SPLIT
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_S
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_E
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_CONN_TYPE_SE
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_graph_rdf import HWY_PATH_ID
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_VIRTUAl_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_NONE
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_LENGTH
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_NUMS
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_NAMES
from component.rdf.hwy.hwy_def_rdf import ANGLE_135


class HwyRouteRDF(component.component_base.comp_base):
    '''Path of main link
    '''

    def __init__(self, data_mng,
                 min_distance=ROUTE_DISTANCE_2000M,
                 margin_dist=ROUTE_DISTANCE_1500M,
                 ItemName='HwyRouteRDF'):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, ItemName)
        self.data_mng = data_mng
        if self.data_mng:
            self.G = self.data_mng.get_graph()
        else:
            self.G = None
        self.min_distance = min_distance
        self.s_e_margin_dist = margin_dist

    def _Do(self):
        if not self.data_mng:
            self.log.error('HWY data manager is None.')
            return
        if not self.G:
            if self.G is not None:
                self.log.warning('Graph is empty(No Hwy Link).')  # 没有高速道路
                # 创建一张空表
                self.CreateTable2('hwy_link_road_code_info')
            else:
                self.log.error('Graph is none.')
            return
        self._make_main_path()
        # 加载Ramp/JCT/SAPA link
        self.data_mng.load_hwy_ic_link()
        self.data_mng.load_hwy_inout_link()
        # 处理侧道
        self._make_side_path()
        # 合并or删除较短的路径
        self._del_short_path()
        self._temp_update_geom()
        # ## 裁剪路径
        self.data_mng.load_exit_poi_name()
        self.data_mng.load_exit_name()
        self.data_mng.load_tollgate()
        self.data_mng.load_hwy_path_id()
        self.data_mng.load_hwy_regulation()
        self._cut_path()
        # ## 生成road_code
        self._make_road_code()
        # ## 生成link对应的road_code
        self._make_link_road_code_info()

    def _make_main_path(self):
        '''按相似度把所以本线link串起来'''
        self.log.info('Start Highway Main Link Path.')
        self.CreateTable2('mid_temp_hwy_main_path')
        self.CreateTable2('mid_temp_hwy_main_path_attr')
        path_id = 1
        for u, v in self.G.edges_iter():
            if self.G.has_edge(v, u):  # 双向通行
                continue
            data = self.G[u][v]
            if not self.G.is_hwy_main_link(data):  # 非高速本线
                continue
            if self.G.is_hov(u, v, data):  # HOV道路
                continue
            if self.G.is_hwy_inner_link(data):
                continue
            if self.G.is_referenceed(data):  # 被使用过的link
                continue
            path = self.G.get_main_path_by_similar(u, v)
            if not path:
                self.log.error('No Main Path. u=%s, v=%s' % (u, v))
            else:
                # 删除头尾的Inner link
                path = self._del_inner_link(self.G, path)
                self.set_ref(self.G, path)
                # store path
                self._store_path(self.G, path_id, path)
                path_id += 1
        # commit
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_main_path_path_id_idx')
        # self.CreateIndex2('mid_temp_hwy_main_path_link_id_idx')
        self.log.info('End Make Highway Main Link Path.')

    def _del_inner_link(self, G, path):
        '''删除头尾的Inner Link.'''
        # ## 删除开头的Inner Link
        start_idx = 0
        while start_idx < len(path) - 1:
            u = path[start_idx]
            v = path[start_idx + 1]
            data = self.G[u][v]
            if not G.is_hwy_inner_link(data):
                break
            start_idx += 1
        # ## 删除结尾的Inner Link
        end_idx = len(path) - 1
        while end_idx > start_idx + 1:
            u = path[end_idx - 1]
            v = path[end_idx]
            data = self.G[u][v]
            if not G.is_hwy_inner_link(data):
                break
            end_idx -= 1
        return path[start_idx:end_idx+1]

    def set_ref(self, G, path):
        for u, v in zip(path[0:-1], path[1:]):
            G.set_ref(u, v)

    def _store_path(self, G, path_id, path):
        linkids = G.get_linkids(path)
        seq_nm = 0
        path_len = len(path)
        if seq_nm > LAST_SEQ_NUM:
            self.log.error('Seq_nm is more than 9999. node=%s' % path[-1])
            return
        for seq_nm in range(0, path_len):
            node_id = path[seq_nm]
            if seq_nm == 0:
                link_id = None
            else:
                link_id = linkids[seq_nm - 1]
            # if seq_nm == path_len - 1:
            #    seq_nm = LAST_SEQ_NUM  # 9999: 最后一条link/node
            self._insert_path_link((path_id, node_id, link_id, seq_nm))
        # 路径情报
        path_attr = self._get_path_attr(G, path)
        length, path_name, path_number, sort_name, sort_num = path_attr
        fazm, tazm = self._get_path_angle(G, path)
        self._insert_path_info((path_id, length,
                                path_name, path_number,
                                sort_name, sort_num,
                                fazm, tazm))

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
            self._add_path_name(path_name, names)
            self._add_path_name(path_number, numbers)
            if link_length:
                path_length += link_length
            else:
                self.log.error('No Length. edge=(%s, %s)' % (u, v))
                return None
        rst_names = self._get_max_count_names(path_name)
        rst_nums = self._get_max_count_nums(path_number)
        if rst_names:
            json_name = json.dumps([rst_names], ensure_ascii=False,
                                   encoding='utf8', sort_keys=True)
        else:
            json_name = None
        if rst_nums:
            json_num = json.dumps([rst_nums], ensure_ascii=False,
                                  encoding='utf8', sort_keys=True)
        else:
            json_num = None
        sort_name = self._get_sort_name(rst_names)
        sort_num = self._get_sort_name(rst_nums)
        return path_length, json_name, json_num, sort_name, sort_num

    def _add_path_name(self, path_name_dict, names):
        if not names:
            return
        for name_dict in names:
            name_type = name_dict.get("type")
            if name_type in (NAME_TYPE_OFFICIAL, NAME_TYPE_SHIELD):
                key = json.dumps(name_dict, ensure_ascii=False,
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
                name_dict = json.loads(name, encoding='utf8')
                names.append(name_dict)
                max_cnt = cnt
            else:
                break
        # 按名称语种
        names.sort(cmp=lambda x, y: cmp(x.get('lang'), y.get('lang')),
                   reverse=True)
        return names

    def _get_max_count_nums(self, path_num_dict):
        nums = self._get_max_count_names(path_num_dict)
        for num in nums:
            # 去掉shield
            num['val'] = num.get('val').split('\t')[-1]
        return nums

    def _get_sort_name(self, path_name):
        '''取得排序用名称'''
        names = []
        for name_dict in path_name:
            name = name_dict.get('val')
            if name not in names:
                names.append(name)
        if names:
            return HWY_NAME_SPLIT.join(names)
        else:
            return None

    def _get_path_angle(self, G, path):
        '''fazm: 第一条link的起始角度(车流方向)
           tazm: 最后一条link的终点角度(车流方向)
        '''
        fazm = G.get_zm_angle((path[0], path[1]), path[0])
        tazm = G.get_zm_angle((path[-2], path[-1]), path[-1])
        return fazm, tazm

    def _get_path_length(self, path):
        '''取得长度'''
        path_length = 0
        for u, v in zip(path[0:-1], path[1:]):
            data = self.G[u][v]
            link_length = data.get(HWY_LINK_LENGTH)
            if link_length:
                path_length += link_length
            else:
                self.log.error('No Length. edge=(%s, %s)' % (u, v))
                return None
        return path_length

    def _make_side_path(self):
        '''计算辅路'''
        side_path, main_path, path_node_list = self._get_side_path()
        # 更新图内侧道的link_type
        self._update_link_type(path_node_list, HWY_LINK_TYPE_SIDE)
        # 保存辅路信息
        self._update_path(side_path, side_path_flg=HWY_TRUE)
        # ## 和侧道并行的主路，要重新拼接
        self.data_mng.load_hwy_path()  # 加载高速线路图
        path_G = self.data_mng.get_path_graph()  # 路径的图
        new_path_id = self.data_mng.get_max_path_id() + 1
        old_path_ids = []
        new_path_list = []
        for path_id, node_list in main_path.iteritems():
            s_node = node_list[0]
            e_node = node_list[-1]
            new_path = path_G.merge_path_by_similir(s_node, e_node)
            # new_path = self._recal_main_path(path_G, s_node, e_node)
            if not new_path or len(new_path) <= 2:  # 路径没有变化
                continue
            # ## 路径有变化: 老的路径要删除，新的路径要保存
            # 老的路径ID
            old_path_ids += self._get_path_id(path_G, new_path)
            u, v = path_G.merge_edges(new_path, new_path_id)
            new_path_list.append((u, v))
            new_path_id += 1
        # ## Delete old path
        del_flg = HWY_TRUE
        del_path = [(p, del_flg) for p in old_path_ids]
        self._update_path(del_path)
        # ## 保存新路径
        self._store_new_path(path_G, new_path_list)
        self.pg.commit2()

    def _update_link_type(self, path_node_list, link_type=HWY_LINK_TYPE_SIDE):
        for path in path_node_list:
            self.G.add_path(path, link_type=link_type)

    def _get_path_id(self, path_G, new_path):
        path_id_list = []
        for u, v in zip(new_path[0:-1], new_path[1:]):
            path_id_list.append(path_G.get_path_id(u, v))
        return path_id_list

    def _recal_main_path(self, G, u, v):
        if self.G.has_edge(v, u):  # 双向通行
            return []
        data = self.G[u][v]
        if self.G.is_referenceed(data):  # 被使用过的link
            return []
        path = self.G.get_main_path_by_similar(u, v)
        if not path:
            self.log.error('No Main Path. u=%s, v=%s' % (u, v))
        return path

    def _get_side_path(self):
        side_dict = {}
        main_path = {}  # 需重新拼接的线路
        path_node_list = []
        for side_path_info in self.data_mng.get_side_path():
            path_id, path, other_path_id, other_path = side_path_info
            if((path_id, other_path_id) in side_dict or
               (other_path_id, path_id) in side_dict):
                continue
            delete_flag = HWY_FALSE
            if not self._is_same_direction(path, other_path):
                continue
            # 首尾点在另一条线路里，且不是另一条线路的首尾
            if path[0] != other_path[0] or path[-1] != other_path[-1]:
                if self._is_short_path(path):
                    delete_flag = HWY_TRUE
                side_dict[(path_id, other_path_id)] = (path_id,
                                                       delete_flag)
                path_node_list.append(path)
                # 有一头的端点相同
                if path[0] == other_path[0] or path[-1] == other_path[-1]:
                    main_path[other_path_id] = other_path
                continue
            # ## 首尾重叠
            curr_num = self._get_hwy_node_num(path)
            other_num = self._get_hwy_node_num(other_path)
            del_path1 = True
            if curr_num > other_num:
                del_path1 = True
            elif curr_num < other_num:
                del_path1 = False
            else:
                length1 = self._get_path_length(path)
                length2 = self._get_path_length(other_path)
                if length1 > length2:
                    del_path1 = True
                elif length1 < length2:
                    del_path1 = False
                else:
                    self.log.error('Same length. path_id=%s, other_path_id=%s'
                                   % (path_id, other_path_id))
                self.log.warning('Same Hwy Node Number. path_id=%s, '
                                 'other_path_id=%s' % (path_id, other_path_id))
            if del_path1:
                if self._is_short_path(path):
                    delete_flag = HWY_TRUE
                side_dict[(path_id, other_path_id)] = (path_id,
                                                       delete_flag)
                main_path[other_path_id] = other_path
                path_node_list.append(path)
            else:
                if self._is_short_path(other_path):
                    delete_flag = HWY_TRUE
                side_dict[(path_id, other_path_id)] = (other_path_id,
                                                       delete_flag)
                main_path[path_id] = path
                path_node_list.append(other_path)
        side_path = [(path_id, delete_flag)
                     for (path_id, delete_flag) in side_dict.itervalues()]
        return side_path, main_path, path_node_list

    def _is_same_direction(self, path1, path2):
        '''首尾相交的两条路径是同向的'''
        if is_cycle_path(path2):  # 环
            self.log.error('Path2 is cycle.')
        else:  # 非环
            if path2.index(path1[0]) < path2.index(path1[-1]):
                return True
        return False

    def _is_short_path(self, path):
        # 长度小于1.5Km的辅路，从高速本线中去除
        if self._get_path_length(path) <= self.min_distance:
            return True
        return False

    def _get_hwy_node_num(self, path):
        '''取得线路上有Ramp、JCT、SAPAlink的Node点个数。'''
        num = 0
        G = self.data_mng.get_graph()
        for node in path[1:-1]:
            if len(G.in_edges(node)) >= 2:  # 有多条进入link
                num += 1
            elif len(G.out_edges(node)) >= 2:  # 有多条退出link
                num += 1
        return num

    def _del_short_path(self):
        '''删除或者合并'''
        self.log.info('Start Delete Short Path.')
        path_G = self.data_mng.get_path_graph()  # 路径的图
        new_path_list = []
        old_path_ids = []
        new_path_id = self.data_mng.get_max_path_id() + 1
        for path_info in self.data_mng.get_path_distance(ROUTE_DISTANCE_2500M):
            path = path_info[2]
            u, v = path[0], path[-1]
            if not path_G.has_edge(u, v):  # 路径已经被合并
                continue
            temp_path, temp_path_ids = self._merge_short_path(u, v,
                                                              new_path_id)
            if temp_path:
                new_path_list.append(temp_path)
            old_path_ids += temp_path_ids
            new_path_id += 1
        # ## Delete old path
        del_flg = HWY_TRUE
        del_pathes = [(p_id, del_flg) for p_id in old_path_ids]
        self._update_path(del_pathes)
        # ## 保存新路径
        self._store_new_path(path_G, new_path_list)
        self.log.info('End Delete Short Path.')

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
                # 存在Highway Exit POI
                has_exit_poi = self.has_hwy_exit_poi(node_list)
                if has_exit_poi:
                    break
                if length <= self.min_distance:
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

    def _cut_path(self):
        '''裁剪两头'''
        self.log.info('Start Cut Path.')
        self.CreateTable2('mid_temp_hwy_main_cut_path_attr')
        for path_id, path in self.data_mng.get_path():
            if is_cycle_path(path):
                # 保存路径
                self._store_cut_path(self.G, path_id, path,
                                     0, len(path) - 1)
                continue
            start_idx = self._get_route_start(path_id, path)
            end_idx = self._get_route_end(path_id, path)
            if(start_idx >= end_idx or
               end_idx >= len(path) or end_idx < 0 or
               start_idx >= len(path) or start_idx < 0):
                self.log.warning('Error Path. path=%s' % path_id)
                continue
            # 保存路径
            self._store_cut_path(self.G, path_id, path,
                                 start_idx, end_idx)
        self.pg.commit2()
        self.log.info('End Cut Path.')

    def _get_route_start(self, path_id, path):
        '''裁剪头'''
        types_list = self.get_start_facil_types_list(path_id, path,
                                                     self.s_e_margin_dist)
        # 断头
        if self._is_no_head(path, types_list):
            self.log.warning('No Head Highway. path_id=%s' % path_id)
            return 0
        last_ic_out_idx = -1
        last_jct_out_idx = -1
        first_jct_in_idx = -1
        for node_idx, length, facil_types in types_list:
            if length <= ROUTE_DISTANCE_1000M:
                if self._is_jct_out(facil_types):
                    last_jct_out_idx = node_idx
                if first_jct_in_idx < 0 and self._is_jct_in(facil_types):
                    first_jct_in_idx = node_idx
                if self._is_ic_out(facil_types):
                    last_ic_out_idx = node_idx
        # 存在JCT出口，找下个同JCT入口
        jct_in_idx = self._get_start_jct_in(path_id, path,
                                            last_jct_out_idx,
                                            first_jct_in_idx)
        if jct_in_idx >= 0:
            return jct_in_idx
        # 有IC出口， 找下个同IC入口
        if(last_ic_out_idx >= 0 and
           first_jct_in_idx < 0 and
           first_jct_in_idx < 0):
            next_ic_in = self._get_next_ic_in(path_id, path, last_ic_out_idx)
            if next_ic_in >= 0:
                ic_len = self._get_path_length(path[:next_ic_in])
                if ic_len > self.s_e_margin_dist:
                    self.log.warning('Next IC IN too far. path_id=%s,'
                                     'length=%s, next_ic_in=%s' %
                                     (path_id, ic_len, path[next_ic_in]))
                else:
                    return next_ic_in
            else:
                if type(self) != HwyRouteRDF_HKG:
                    self.log.warning('No Next IC IN. path_id=%s' %
                                     (path_id))
        # 合并连续的多个入口
        node_idx = self._merge_facil_in(path, path_id, types_list)
        if node_idx < 0:
            # 取link_type变化点
            node_idx = self._get_start_link_type_change(path, path_id,
                                                        types_list,
                                                        self.s_e_margin_dist)
            if node_idx < 0:
                # 取得第二个点
                node_idx = self._get_start_second_node(path, path_id,
                                                       types_list)
        return node_idx

    def _get_route_end(self, path_id, path):
        '''裁剪尾'''
        types_list = self.get_end_facil_types_list(path_id, path,
                                                   self.s_e_margin_dist)
        # 断尾
        if self._is_no_tail(path, types_list):
            # 取最后一个出口
            self.log.warning('No Tail Highway. path_id=%s' % path_id)
            return self._get_last_facil_out(path_id, path)
        last_ic_in_idx = -1
        last_jct_in_idx = -1
        first_jct_out_idx = -1
        for node_idx, length, facil_types in types_list:
            if length <= ROUTE_DISTANCE_1000M:
                if self._is_jct_in(facil_types):
                    last_jct_in_idx = node_idx
                if first_jct_out_idx < 0 and self._is_jct_out(facil_types):
                    first_jct_out_idx = node_idx
            if self._is_ic_in(facil_types):
                last_ic_in_idx = node_idx
        # 有JCT入口，取前一个JCT出口
        prev_out_idx = self._get_end_jct_out(path_id, path,
                                             last_jct_in_idx,
                                             first_jct_out_idx)
        if prev_out_idx >= 0:
            return prev_out_idx
        # 有IC入口， 找前一个同IC出口
        if(last_ic_in_idx >= 0 and
           last_jct_in_idx < 0 and
           first_jct_out_idx < 0):
            prev_ic_in = self._get_prev_ic_out(path_id, path, last_ic_in_idx)
            if prev_ic_in >= 0:
                ic_len = self._get_path_length(path[prev_ic_in:])
                if ic_len > self.s_e_margin_dist:
                    self.log.warning('Prev IC Out too far. path_id=%s, '
                                     'length=%s, next_ic_in=%s' %
                                     (path_id, ic_len, path[prev_ic_in]))
                else:
                    return prev_ic_in
            else:
                if type(self) != HwyRouteRDF_HKG:
                    self.log.warning('No Prev IC Out. path_id=%s' % (path_id))
        # 合并连续的多个入口
        node_idx = self._merge_facil_out(path, path_id, types_list)
        if node_idx < 0:
            # 取link_type变化点
            node_idx = self._get_end_link_type_change(path, path_id,
                                                      types_list,
                                                      self.s_e_margin_dist)
            if node_idx < 0:
                # 取得倒第二个点
                node_idx = self._get_end_second_node(path, path_id, types_list)
        return node_idx

    def _is_no_head(self, path, types_list):
        ''' 判定道路是否有头尾
        '''
        if not types_list:
            return True
        node_idx, length, types = types_list[0]
        if node_idx != 0:
            return False
        # if self._is_jct_in(types) or self._is_ic_in(types):
        #    return True
        return False

    def _is_no_tail(self, path, types_list):
        ''' 判定道路是否有尾
        '''
        if not types_list:
            return True
        node_idx, length, types = types_list[0]
        if node_idx != len(path) - 1:
            return False
        # if self._is_jct_out(types) or self._is_ic_out(types):
        #    return True
        return False

    def _get_last_facil_out(self, path_id, path):
        node_idx = len(path) - 1
        while node_idx >= 0:
            node = path[node_idx]
            types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            if types:
                if(self._is_ic_out(types) or
                   self._is_jct_out(types) or
                   self._is_sapa_out(types) or
                   self._is_tollgate(types)):
                    return node_idx
            node_idx -= 1
        return -1

    def get_start_facil_types_list(self, path_id, path, margin_distance):
        '''[(node_idx, length, types)]'''
        types_list = []
        node_idx = 0
        length = 0.0
        while node_idx < len(path):
            if node_idx > 0:
                length += self.G.get_length(path[node_idx - 1],
                                            path[node_idx])
            if length > margin_distance:
                break
            node = path[node_idx]
#             # 存在Exit Name(Exit No)
#             if node_idx > 0 and self.G.is_exit_name(node):
#                 # 起始位置
#                 types_list.append((node_idx, length, types))
            # 取得设施种别(IC/JCT/SA/toll)
            types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            if types:
                types_list.append((node_idx, length, types))
            elif node_idx == 0:
                self.log.warning('No Facility for node=%s, path_id=%s'
                                 % (node, path_id))
            node_idx += 1
        return types_list

    def get_end_facil_types_list(self, path_id, path, margin_distance):
        '''[(node_idx, length, types)]'''
        node_idx = len(path) - 1
        length = 0.0
        types_list = []
        while node_idx >= 0 and length <= self.s_e_margin_dist:
                node = path[node_idx]
                if node_idx < len(path) - 1:
                    length += self.G.get_length(path[node_idx],
                                                path[node_idx + 1])
                if length > margin_distance:
                    break
#                 # 存在Exit Name(Exit No)
#                 if self.G.get_exit_poi_name(node) or self.G.is_exit_name(node):
#                     # 起始位置
#                     return node_idx
                # 取得设施种别(IC/JCT/SA/toll)
                types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
                if types:
                    types_list.append((node_idx, length, types))
                elif node_idx == len(path) - 1:
                    self.log.warning('No Facility for node=%s, path_id=%s' %
                                     (node, path_id))
                node_idx -= 1
        return types_list

    def _get_start_jct_in(self, path_id, path,
                          last_jct_out_idx, first_jct_in_idx):
        if last_jct_out_idx <= 0 or first_jct_in_idx == 0:
            return -1
        next_jct_idx = self._get_next_jct_in(path_id, path,
                                             last_jct_out_idx,
                                             first_jct_in_idx)
        if next_jct_idx < 0:
            self.log.warning('No Next JCT IN. path_id=%s, Jct_node=%s' %
                             (path_id, path[last_jct_out_idx]))
            next_jct_idx = self._get_next_ic_in(path_id, path,
                                                last_jct_out_idx,
                                                first_jct_in_idx)
        if next_jct_idx < 0:
            self.log.warning('No Next JCT IN and IC IN. '
                             'path_id=%s, Jct_node=%s' %
                             (path_id, path[last_jct_out_idx]))
        else:
            jct_length = self._get_path_length(path[last_jct_out_idx:
                                                    next_jct_idx+1])
            if jct_length > ROUTE_DISTANCE_2500M:  # 距离大于2.5Km
                self.log.warning('Next JCT too far. path_id=%s, length=%s,'
                                 'jct_out=%s, jct_in=%s' %
                                 (path_id, jct_length,
                                  path[last_jct_out_idx],
                                  path[next_jct_idx]))
        return next_jct_idx

    def _get_end_jct_out(self, path_id, path,
                         last_jct_in_idx, first_jct_out_idx):
        if(last_jct_in_idx < 0 or
           last_jct_in_idx == len(path) - 1 or
           first_jct_out_idx == len(path) - 1):
            return -1
        if last_jct_in_idx >= 0:
            prev_jct_idx = self._get_prev_jct_out(path_id, path,
                                                  last_jct_in_idx,
                                                  first_jct_out_idx)
            if prev_jct_idx < 0:
                prev_jct_idx = self._get_prev_ic_out(path_id, path,
                                                     last_jct_in_idx,
                                                     first_jct_out_idx)
            if prev_jct_idx < 0:
                self.log.warning('No Prev JCT out and IC out.'
                                 'path_id=%s, Jct_node=%s' %
                                 (path_id, path[last_jct_in_idx]))
            else:
                jct_length = self._get_path_length(path[prev_jct_idx:
                                                        last_jct_in_idx+1])
                if jct_length > ROUTE_DISTANCE_2500M:  # 距离大于2.5Km
                    self.log.warning('Prev JCT too far. path_id=%s, length=%s,'
                                     'jct_out=%s, jct_in=%s' %
                                     (path_id, jct_length,
                                      path[prev_jct_idx],
                                      path[last_jct_in_idx]))
                return prev_jct_idx
        return -1

    def get_last_facil_types_list(self, path_id, path, margin_distance):
        types_list = []
        node_idx = len(path) - 1
        length = 0
        while node_idx >= 0:
            if node_idx < len(path) - 1:
                length += self.G.get_length(path[node_idx],
                                            path[node_idx + 1])
            if length > margin_distance:
                break
            node = path[node_idx]
            # 取得设施种别(IC/JCT/SA/toll)
            types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            if types:
                types_list.append((node_idx, length, types))
            elif node_idx == len(path) - 1:
                self.log.error('No Facility for node=%s, path_id=%s'
                               % (node, path_id))
            node_idx -= 1

    def _get_start_second_node(self, path, path_id, types_list):
        # 本线和本线直接相连的JCT, 且有多条退出本线，前推一个点
        if not types_list:
            self.log.error('No Faciliy. path_id=%s' % path_id)
            return 1
        node_idx = types_list[0][0]
        node = path[node_idx]
        in_nodes = self.G._get_not_main_link(node, HWY_PATH_ID, reverse=True)
        if len(in_nodes) <= 0:  # 无进入Ramp
            # ## 下个点是不是收费站
            if len(types_list) > 1:
                next_types = types_list[1][2]
                next_idx = types_list[1][0]
                if next_idx == node_idx + 1 and self._is_tollgate(next_types):
                    return node_idx
            # 第一条link太长
            if node_idx < len(path) - 1:
                link_len = self.G.get_length(node, path[node_idx+1])
                if link_len < self.s_e_margin_dist:
                    return node_idx + 1
            return node_idx
        else:
            return node_idx

    def _get_end_second_node(self, path, path_id, types_list):
        # 本线和本线直接相连的JCT, 且有多条退出本线，前推一个点
        if not types_list:
            self.log.error('No Faciliy. path_id=%s' % path_id)
            return 1
        node_idx = types_list[0][0]
        node = path[node_idx]
        out_nodes = self.G._get_not_main_link(node, HWY_PATH_ID)
        if len(out_nodes) <= 0:  # 无进退出Ramp
            # 第一条link太长
            if node_idx > 0:
                link_len = self.G.get_length(path[node_idx - 1], node)
                if link_len < self.s_e_margin_dist:
                    return node_idx - 1
            return node_idx
        else:
            return node_idx

    def _get_last_node(self, path, path_id, types_list):
        '''取得出口/分歧'''
        rst_pos = 0
        for type_idx in range(0, len(types_list)):
            types = types_list[type_idx][2]
            if self._is_ic_out(types):
                if type_idx != 0:
                    rst_pos = type_idx
            if self._is_jct_out(types):
                if type_idx != 0:
                    rst_pos = type_idx
                    break
                else:
                    node = types_list[type_idx][0]
                    out_edges = self.G.out_edges(node)
                    # 有多条进入link
                    if len(out_edges) > 1 or self._is_ic_out(types):
                        node_idx = types_list[type_idx][1]
                        return node_idx
            if self._is_sapa_out(types):
                rst_pos = type_idx
                break
            if self._is_sapa_in(types):
                break
            if self._is_tollgate(types):
                if type_idx != 0:
                    node_idx = types_list[type_idx-1][1]
                    return node_idx
                else:  # 最后一个点就是收费站
                    node_idx = types_list[type_idx][1]
                    return node_idx
        # 第一个之后的HwyNode, 肯定是多条link的合流点
        if rst_pos > 0:
            node_idx = types_list[rst_pos][1]
            return node_idx
        if len(types_list) > 1:
            second_node_idx = types_list[1][1]
            if second_node_idx == 0:
                second_node_idx = 1
        else:
            second_node_idx = 1
        # 查找link_type变化点
        u, v = path[-2], path[-1]
        link_type1 = self.G[u][v].get(HWY_LINK_TYPE)
        length = self.G.get_length(u, v)
        node_idx = len(path) - 2
        while node_idx > second_node_idx and length <= self.s_e_margin_dist:
            u, v = path[node_idx - 1], path[node_idx]
            data = self.G[u][v]
            link_type2 = data.get(HWY_LINK_TYPE)
            if link_type1 != link_type2:
                return node_idx
            link_type1 = link_type2
            length += self.G.get_length(u, v)
            node_idx -= 1
        if not types_list:
            self.log.error('No Faciliy. path_id=%s' % path_id)
            return len(path) - 1
        node_idx = types_list[0][1]
        if self._is_jct_in(types):
            node = path[node_idx]
            # 本线和本线直接相连的JCT, 且有多条退出本线，后移一个点
            out_nodes = self.G.get_main_link(node, path_id, HWY_PATH_ID)
            if len(out_nodes) > 1:
                return node_idx + 1
        return node_idx

    def _is_tollgate(self, types):
        return self._check_facil_type(types, [HWY_IC_TYPE_TOLL],
                                      HWY_INOUT_TYPE_NONE)

    def _is_ic_in(self, types):
        return self._check_facil_type(types, [HWY_IC_TYPE_IC],
                                      HWY_INOUT_TYPE_IN)

    def _is_ic_out(self, types):
        return self._check_facil_type(types, [HWY_IC_TYPE_IC],
                                      HWY_INOUT_TYPE_OUT)

    def _is_sapa_in(self, types):
        return self._check_facil_type(types,
                                      [HWY_IC_TYPE_SA, HWY_IC_TYPE_PA],
                                      HWY_INOUT_TYPE_IN)

    def _is_sapa_out(self, types):
        return self._check_facil_type(types,
                                      [HWY_IC_TYPE_SA, HWY_IC_TYPE_PA],
                                      HWY_INOUT_TYPE_OUT)

    def _is_jct_out(self, types):
        '''JCT出口/分歧'''
        return self._check_facil_type(types, [HWY_IC_TYPE_JCT],
                                      HWY_INOUT_TYPE_OUT)

    def _is_jct_in(self, types):
        return self._check_facil_type(types, [HWY_IC_TYPE_JCT],
                                      HWY_INOUT_TYPE_IN)

    def _is_virtual_jct_in(self, types):
        return self._check_facil_type(types, [HWY_IC_TYPE_VIRTUAl_JCT],
                                      HWY_INOUT_TYPE_IN)

    def _is_virtual_jct_out(self, types):
        return self._check_facil_type(types, [HWY_IC_TYPE_VIRTUAl_JCT],
                                      HWY_INOUT_TYPE_IN)

    def _check_facil_type(self, types, facilcls, inout):
        for temp_facilcls, temp_inout in types:
            if(temp_facilcls in facilcls and
               temp_inout == inout):
                return True
        return False

    def _get_next_jct_in(self, path_id, path, jct_out_idx, f_jct_in_idx):
        '''取得下一个 【JCT合流】'''
        node_idx = jct_out_idx + 1
        # 取一下JCT入口
        while node_idx < len(path):
            node = path[node_idx]
            # 取得设施种别
            types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            if types:
                if self._is_jct_in(types):
                    return node_idx
                if self._is_virtual_jct_in(types):
                    return node_idx
                if(f_jct_in_idx >= 0 and
                   f_jct_in_idx < node_idx):
                    pass
                else:
                    if self._is_ic_out(types):
                        break
                if(self._is_jct_out(types) or
                   self._is_sapa_out(types)):
                    break
                # 存在Exit POI / Exit Name(Exit No)
                if(node_idx > 0 and
                   (self.G.get_exit_poi_name(node) or
                    self.G.is_exit_name(node))
                   ):
                    return node_idx
            node_idx += 1
        return -1

    def _get_next_ic_in(self, path_id, path, out_idx, f_jct_in_idx=-1):
        # 取下一个IC入口
        if out_idx > 0:
            if(self.G.get_exit_poi_name(path[out_idx]) or
               self.G.is_exit_name(path[out_idx])):
                return -1
        node_idx = out_idx + 1
        while node_idx < len(path):
            node = path[node_idx]
            # 取得设施种别
            types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            if types:
                if self._is_ic_in(types):
                    return node_idx
                if f_jct_in_idx >= 0 and f_jct_in_idx < node_idx:
                    pass
                else:
                    if self._is_ic_out(types):
                        break
                if(self._is_jct_out(types) or
                   self._is_sapa_out(types) or
                   self._is_ic_out(types)):
                    break
                # 存在Exit POI/Exit Name(Exit No)
                if self.G.get_exit_poi_name(node) or self.G.is_exit_name(node):
                    break
            node_idx += 1
        return -1

    def _merge_facil_in(self, path, path_id, types_list):
        '''合并头几个连续的合流，返回其中的最后一个'''
        rst_pos = 0
        for type_idx in range(0, len(types_list)):
            types = types_list[type_idx][2]
            if self._is_jct_in(types):
                if type_idx > 0:
                    rst_pos = type_idx
                    node_idx = types_list[rst_pos][0]
                    return node_idx
                else:
                    node = path[types_list[type_idx][0]]
                    in_edges = self.G.in_edges(node)
                    # 有多条进入link
                    if(len(in_edges) > 1):
                        node_idx = types_list[rst_pos][0]
                        return node_idx
                    next_node = path[types_list[type_idx][0] + 1]
                    if self.G.is_hwy_inout([next_node, node], True):
                        node_idx = types_list[rst_pos][0]
                        return node_idx
            if type_idx > 0 and self._is_tollgate(types):
                node_idx = types_list[rst_pos][0]
                return node_idx
            if(type_idx > 0 and
               (self._is_jct_out(types) or
                self._is_sapa_out(types) or
                self._is_ic_out(types))
               ):
                break
            rst_pos = type_idx
        if rst_pos > 0:
            node_idx = types_list[rst_pos][0]
            return node_idx
        else:
            return -1

    def _merge_facil_out(self, path, path_id, types_list):
        '''合并头几个连续的合流，返回其中的最后一个'''
        rst_pos = 0
        for type_idx in range(0, len(types_list)):
            types = types_list[type_idx][2]
            node = path[types_list[type_idx][0]]
            # 存在Exit POI / Exit Name(Exit No)
            if self.G.get_exit_poi_name(node) or self.G.is_exit_name(node):
                node_idx = types_list[type_idx][0]
                return node_idx
            if self._is_jct_out(types):
                if type_idx > 0:
                    node_idx = types_list[type_idx][0]
                    return node_idx
                else:
                    out_edges = self.G.out_edges(node)
                    # 有多条退出link
                    if len(out_edges) > 1:
                        node_idx = types_list[type_idx][0]
                        return node_idx
                    prev_node = path[types_list[type_idx][0] - 1]
                    if self.G.is_hwy_inout([prev_node, node], False):
                        node_idx = types_list[type_idx][0]
                        return node_idx
            if self._is_tollgate(types):
                node_idx = types_list[rst_pos][0]  # 收费站的前一个
                return node_idx
            if(type_idx > 0 and
               (self._is_jct_in(types) or
                self._is_sapa_in(types) or
                self._is_ic_in(types))
               ):
                break
            rst_pos = type_idx
        if rst_pos > 0:
            node_idx = types_list[rst_pos][0]
            return node_idx
        else:
            return -1

    def _get_start_link_type_change(self, path, path_id,
                                    types_list, margin_dist):
        '''求link种别变化的地方'''
        # 第一个之后的HwyNode, 肯定是多条link的合流点
        if len(types_list) > 1:
            second_node_idx = types_list[1][0]
            if second_node_idx == len(path) - 1:
                second_node_idx = len(path) - 2
        else:
            second_node_idx = len(path) - 2
        # 查找link_type变化点
        u, v = path[0], path[1]
        link_type1 = self.G[u][v].get(HWY_LINK_TYPE)
        length = self.G.get_length(u, v)
        node_idx = 0
        while(node_idx < second_node_idx and
              length <= margin_dist):
            u, v = path[node_idx], path[node_idx + 1]
            if self.G.is_tollgate(u):
                return -1
            data = self.G[u][v]
            link_type2 = data.get(HWY_LINK_TYPE)
            if link_type1 != link_type2:
                return node_idx
            link_type1 = link_type2
            length += self.G.get_length(u, v)
            node_idx += 1
        return -1

    def _get_end_link_type_change(self, path, path_id,
                                  types_list, margin_dist):
        '''求link种别变化的地方'''
        if len(types_list) > 1:
            second_node_idx = types_list[1][0]
            if second_node_idx == 0:
                second_node_idx = 1
        else:
            second_node_idx = 1
        # 查找link_type变化点
        u, v = path[-2], path[-1]
        link_type1 = self.G[u][v].get(HWY_LINK_TYPE)
        length = self.G.get_length(u, v)
        node_idx = len(path) - 2
        while node_idx > second_node_idx and length <= self.s_e_margin_dist:
            u, v = path[node_idx - 1], path[node_idx]
            if self.G.is_tollgate(v):
                return -1
            data = self.G[u][v]
            link_type2 = data.get(HWY_LINK_TYPE)
            if link_type1 != link_type2:
                return node_idx
            link_type1 = link_type2
            length += self.G.get_length(u, v)
            node_idx -= 1
        return -1

    def _get_prev_jct_out(self, path_id, path, l_jct_in, f_jct_out_idx):
        '''取得上一个 【JCT出口/分歧】'''
        node_idx = l_jct_in - 1
        while node_idx >= 0:
            node = path[node_idx]
            # 取得设施种别
            types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            if types:
                if self._is_jct_out(types):
                    return node_idx
                if self._is_virtual_jct_out(types):
                    return node_idx
                if f_jct_out_idx >= 0 and f_jct_out_idx > node_idx:
                    pass
                else:
                    if self._is_ic_in(types):
                        break
                if(self._is_jct_in(types) or
                   self._is_sapa_in(types)):
                    break
                # 存在Exit POI/Exit Name(Exit No)
                if self.G.get_exit_poi_name(node) or self.G.is_exit_name(node):
                    break
                if self._is_ic_out(types):
                    break
            node_idx -= 1
        return -1

    def _get_prev_ic_out(self, path_id, path, l_jct_in, f_jct_out_idx=-1):
        '''取得上一个 【JCT出口/分歧】'''
        node_idx = l_jct_in - 1
        while node_idx >= 0:
            node = path[node_idx]
            # 取得设施种别
            types = self.G.get_facil_types(node, path_id, HWY_PATH_ID)
            if types:
                if self._is_ic_out(types):
                    return node_idx
                if f_jct_out_idx >= 0 and f_jct_out_idx > node_idx:
                    pass
                else:
                    if self._is_ic_in(types):
                        break
                if(self._is_jct_in(types) or
                   self._is_sapa_in(types)):
                    break
                # 存在Exit POI/Exit Name(Exit No)
                if self.G.get_exit_poi_name(node) or self.G.is_exit_name(node):
                    break
            node_idx -= 1
        return -1

    def _make_road_code(self):
        '''road_code按番号、名称排序'''
        self.CreateTable2('road_code_info')
        sqlcmd = """
        INSERT INTO road_code_info(road_name, road_number, path_id,
                                   sort_name, sort_number)
        (
        SELECT road_name, road_number, path_id,
               sort_name, sort_number
          FROM mid_temp_hwy_main_cut_path_attr
          order by length(sort_number), sort_number, sort_name, path_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_link_road_code_info(self):
        self.CreateTable2('hwy_link_road_code_info')
        sqlcmd = """
        INSERT INTO hwy_link_road_code_info(road_code, node_id, link_id,
                                            seq_nm, the_geom)
        (
        SELECT road_code, node_id, e.link_id,
               seq_num - 1, the_geom
          FROM (
            SELECT road_code, nodes[seq_num] as node_id,
                   link_ids[seq_num] as link_id, seq_num
              FROM (
                SELECT road_code, a.path_id,
                       nodes[start_idx+1:end_idx+1],
                       link_ids[start_idx+1:end_idx+1],
                       generate_series(1, end_idx - start_idx +1) as seq_num
                  FROM mid_temp_hwy_main_cut_path_attr AS a
                  LEFT JOIN (
                    SELECT path_id, array_agg(node_id) as nodes,
                           array_agg(link_id) as link_ids
                      FROM (
                        SELECT path_id, node_id, link_id
                          FROM mid_temp_hwy_main_path
                          ORDER BY path_id, seq_nm
                    ) AS a
                    GROUP BY path_id
                  ) AS b
                  ON a.path_id = b.path_id
                  LEFT JOIN road_code_info as c
                  on a.path_id = c.path_id
                  --WHERE road_code = 1
              ) as d
          ) AS e
          LEFT JOIN link_tbl
          ON e.link_id = link_tbl.link_id
          ORDER BY road_code, seq_num
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 去掉seq_nm = 0的link_id和the_geom
        sqlcmd = """
        UPDATE hwy_link_road_code_info SET the_geom = null, link_id = NULL
        WHERE seq_nm = 0;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('hwy_link_road_code_info_the_geom_idx')

    # ========================================================================
    # 保存数据,更新数据库
    # ========================================================================
    def _insert_path_link(self, params):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_main_path(
                    path_id, node_id, link_id, seq_nm)
            VALUES (%s, %s, %s, %s);
        """
        self.pg.execute2(sqlcmd, params)

    def _insert_path_info(self, params):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_main_path_attr(
                 path_id, length, road_name, road_number,
                 sort_name, sort_number, fazm, tazm)
          VALUES (%s, %s, %s, %s,
                  %s, %s, %s, %s);
        """
        self.pg.execute2(sqlcmd, params)

    def _update_path(self, path_info, side_path_flg=HWY_FALSE):
        sqlcmd = """
        UPDATE mid_temp_hwy_main_path_attr
          SET side_path_flg = %s, delete_flag = %s
           WHERE path_id = %s;
        """
        for (path_id, delete_flag) in path_info:
            self.pg.execute2(sqlcmd, (side_path_flg, delete_flag, path_id))
        self.pg.commit2()

    def _store_new_path(self, path_G, new_path_list):
        # ## 保存新路径
        for u, v in new_path_list:
            # 判断是否存在：后续的合并，有可能把前面的新路径也给合并了。
            if path_G.has_edge(u, v):
                path_id = path_G.get_path_id(u, v)
                # 新路径的所有点
                new_node_list = path_G.get_node_list(u, v)
                # 保存新路径
                self._store_path(self.G, path_id, new_node_list)

    def _store_cut_path(self, G, path_id, path, start_idx, end_idx):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_main_cut_path_attr(
                    path_id, length, road_name, road_number,
                    sort_name, sort_number, start_idx, end_idx)
         VALUES(%s, %s, %s, %s,
                %s, %s, %s, %s);
        """
        path_attr = self._get_path_attr(G, path[start_idx:end_idx+1])
        length, path_name, path_number, sort_name, sort_num = path_attr
        self.pg.execute2(sqlcmd, (path_id, length, path_name, path_number,
                                  sort_name, sort_num, start_idx, end_idx))

    def _temp_update_geom(self):
        sqlcmd = """
        SELECT AddGeometryColumn('','mid_temp_hwy_main_path',
                                 'the_geom','4326','LINESTRING',2);
        update mid_temp_hwy_main_path as a set the_geom = link_tbl.the_geom
          from link_tbl
          where a.link_id  = link_tbl.link_id;
        CREATE INDEX mid_temp_hwy_main_path_the_geom_idx
          ON mid_temp_hwy_main_path
          USING gist
          (the_geom);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def has_hwy_exit_poi(self, path):
        '''判断该高速线路是否存在【Highway Exit POI】'''
        for node in path[1:]:  # 第一个点不考虑
            if self.G.get_org_facil_id(node):
                return True
        return False

    def _get_max_len_path(self, u, v, reverse=False):
        path_G = self.data_mng.get_path_graph()
        max_u, max_v = None, None
        max_length = 0
        if not reverse:  # 正
            out_pathes = path_G.out_edges(v)
            for v, c in out_pathes:
                length = path_G.get_length(u, v)
                angle = path_G.get_angle((u, v), (v, c))
                if(length > max_length and
                   self.G.bigger_hwy_main_min_angle(angle)):
                    max_u, max_v = v, c
                    max_length = length
        else:  # 逆
            in_pathes = path_G.in_edges(u)
            for p, u in in_pathes:
                length = path_G.get_length(p, u)
                angle = path_G.get_angle((p, u), (u, v))
                if(length > max_length and
                   self.G.bigger_hwy_main_min_angle(angle)):
                    max_u, max_v = p, u
                    max_length = length
        return max_u, max_v


# =============================================================================
# HwyRouteRDF_HKG
# =============================================================================
class HwyRouteRDF_HKG(HwyRouteRDF):
    def __init__(self, data_mng,
                 min_distance=ROUTE_DISTANCE_2000M,
                 margin_dist=ROUTE_DISTANCE_2500M,
                 ItemName='Highway_Route_HKG'):
        '''
        '''
        HwyRouteRDF.__init__(self, data_mng, min_distance,
                             margin_dist, ItemName)
        self.s_e_margin_dist = margin_dist

    def _get_start_jct_in(self, path_id, path,
                          last_jct_out_idx, first_jct_in_idx):
        '''起点是JCT IN，且不在其他高速上。'''
        if first_jct_in_idx == 0:  # 第一个设施是JCT入口
            other_main_link = self.G.get_main_link(path[0], path_id,
                                                   HWY_PATH_ID,
                                                   same_code=False,
                                                   reverse=True)
            if other_main_link:  # 起点在其他高速上
                if(len(other_main_link) > 1 or  # 多条进入本线
                   self._exist_in_ic_link(path, first_jct_in_idx)):
                    return first_jct_in_idx
                else:
                    return -1
            else:
                return first_jct_in_idx
        return -1

    def _get_next_ic_in(self, path_id, path, out_idx, f_jct_in_idx=-1):
        return -1

    def _merge_facil_in(self, path, path_id, types_list):
        ''' 1. 如果入口是本线直接和一般道相连， 出口退到1.5公里内的第二个入口
            2. JCT点和另一条本线相连， 出口退到1.5公里内的第二个入口
        '''
        node_idx = -1
        updown_conn_direct_flg = False
        types = types_list[0][2]
        other_main_link = self.G.get_main_link(path[0], path_id,
                                               HWY_PATH_ID,
                                               reverse=True)
        normal_link = self.G.get_normal_link(path[0], path[1], reverse=True)
        if not other_main_link:
            facil_num = 0
            if self._is_jct_in(types):
                facil_num += 1
            if self._is_ic_in(types):
                facil_num += 1
            if self._is_sapa_in(types):
                facil_num += 1
            if self._is_tollgate(types):
                facil_num += 1
            if facil_num > 1:  # 入口设施大于1
                return 0
            if not normal_link:  # 本线和一般道, 不直接相连
                if self._exist_in_ic_link(path):  # 有进入的IC link(Ramp/JCT)
                    return 0
            else:  # 直接和一般道相连
                # 有进入的IC link(Ramp/JCT)
                if self._exist_in_ic_link(path):
                    self.log.warning('Exist in IC link. node=%s' % path[0])
                    return 0
        else:  # 有其他线路本线
            # 上下行直接相连
            if self._updown_line_conn_direct(path, other_main_link,
                                             reverse=True):
                updown_conn_direct_flg = True
            else:
                facil_num = 0
                if self._is_ic_in(types):
                    facil_num += 1
                if self._is_sapa_in(types):
                    facil_num += 1
                if facil_num > 0:
                    return 0
                # 本线点JCT
                if self._is_main_node_jct(path[0]):
                    next_jct = self._get_next_jct_node_idx(types_list, 1,
                                                           HWY_INOUT_TYPE_IN)
                    if next_jct > 0:
                        return next_jct
                    else:
                        next_in_idx = self._get_next_node_idx(types_list, 1,
                                                              HWY_INOUT_TYPE_IN
                                                              )
                        # 该设施点存是在进入的Ramp/SAPA/JCT Link
                        if(next_in_idx > 0 and
                           self._exist_in_ic_link(path, next_in_idx)):
                            return next_in_idx
                    return 0
        # 取得下个入口
        for type_idx in range(1, len(types_list)):
            types = types_list[type_idx][2]
            if(self._is_jct_out(types) or
               self._is_ic_out(types) or
               self._is_sapa_out(types)):
                if types_list[type_idx][0] > 1:
                    break
            # 入口
            if(self._is_jct_in(types) or
               self._is_ic_in(types) or
               self._is_sapa_in(types)):
                node_idx = types_list[type_idx][0]
                break
            if self._is_tollgate(types):
                node_idx = types_list[type_idx][0]
                if node_idx <= 1:
                    node_idx = 0
                else:
                    node_idx = -1
                break
        # 取得后方 Highway Exit POI
        exit_poi_idx = self.get_exit_poi_idx(path, node_idx,  reverse=True)
        if exit_poi_idx > 0:
            if exit_poi_idx == 1:
                self.log.warning('HWY POI Exit is at Position 1 of HWY Road.'
                                 'node=%s' % path[exit_poi_idx])
            else:
                return exit_poi_idx - 1  # 线路的起点在出口POI的前个点(车流方向的后面)
        if node_idx < 0 and updown_conn_direct_flg:
            node_idx = 1
        return node_idx

    def _get_end_jct_out(self, path_id, path,
                         last_jct_in_idx, first_jct_out_idx):
        '''终点设施是JCT OUT，且不在其他高速上。'''
        if first_jct_out_idx == len(path) - 1:  # 最后一个设施是JCT出口
            other_main_link = self.G.get_main_link(path[-1], path_id,
                                                   HWY_PATH_ID,
                                                   same_code=False,
                                                   reverse=False)
            if other_main_link:  # 起点在其他高速上
                if(len(other_main_link) > 1 or  # 多条退出本线
                   self._exist_out_ic_link(path, first_jct_out_idx)):
                    return first_jct_out_idx
                else:
                    return -1
            else:
                return first_jct_out_idx
        return -1

    def _get_prev_ic_out(self, path_id, path, out_idx, f_jct_in_idx=-1):
        return -1

    def _merge_facil_out(self, path, path_id, types_list):
        ''' 1. 如果入口是本线直接和一般道相连， 出口退到1.5公里内的第二个入口
            2. JCT点和另一条本线相连， 出口退到1.5公里内的第二个入口
        '''
        updown_conn_direct_flg = False
        node_idx = -1
        types = types_list[0][2]
        other_main_link = self.G.get_main_link(path[-1], path_id,
                                               HWY_PATH_ID,
                                               same_code=False)
        if not other_main_link:  # 无其他线路本线
            facil_num = 0
            if self._is_jct_out(types):
                facil_num += 1
            if self._is_ic_out(types):
                facil_num += 1
            if self._is_sapa_out(types):
                facil_num += 1
            if self._is_tollgate(types):
                facil_num += 1
            if facil_num > 1:  # 入口设施大于1
                return len(path) - 1
            normal_link = self.G.get_normal_link(path[-2], path[-1])
            if not normal_link:  # 本线和一般道, 不直接相连
                # 有进入的IC link(Ramp/JCT)
                if self._exist_out_ic_link(path, len(path) - 1):
                    return len(path) - 1
            else:  # 直接和一般道相连
                # 有退出的IC link(Ramp/JCT)
                if self._exist_out_ic_link(path, len(path) - 1):
                    self.log.warning('Exist Out IC link. node=%s' % path[-1])
                    return len(path) - 1
        else:  # 有取得本线
            # 上下行直接相连
            if self._updown_line_conn_direct(path, other_main_link):
                updown_conn_direct_flg = True
            else:
                facil_num = 0
                if self._is_ic_out(types):
                    facil_num += 1
                if self._is_sapa_out(types):
                    facil_num += 1
                if self._is_tollgate(types):  # 收费站和JCT出口一起
                    facil_num += 1
                if facil_num > 0:
                    return len(path) - 1
                # 本线点JCT
                inout = HWY_INOUT_TYPE_OUT
                if self._is_main_node_jct(path[-1]):
                    next_jct = self._get_next_jct_node_idx(types_list, 1,
                                                           inout)
                    if next_jct > 0:
                        return next_jct
                    else:
                        next_out_idx = self._get_next_node_idx(types_list, 1,
                                                               inout
                                                               )
                        # 该设施点存在脱出的Ramp/SAPA/JCT Link
                        if(next_out_idx > 0 and
                           self._exist_out_ic_link(path, next_out_idx)):
                            return next_out_idx
                    return len(path) - 1
        # 取得前个出口
        for type_idx in range(1, len(types_list)):
            types = types_list[type_idx][2]
            if(self._is_jct_in(types) or
               self._is_ic_in(types) or
               self._is_sapa_in(types)):
                if other_main_link:  # JCT
                    break
            # 出口
            if(self._is_jct_out(types) or
               self._is_ic_out(types) or
               self._is_sapa_out(types)):
                node_idx = types_list[type_idx][0]
                break
            if self._is_tollgate(types):
                node_idx = types_list[type_idx][0]
                # 收费站有出口名称
                if self.G.get_exit_poi_name(path[node_idx]):
                    pass
                else:
                    # 收费站在倒数第一个，第二个
                    if node_idx >= len(path) - 2:
                        node_idx = len(path) - 1
                    else:
                        node_idx = -1
                break
        # 取得前方 Highway Exit POI
        exit_poi_idx = self.get_exit_poi_idx(path, node_idx)
        if exit_poi_idx >= 0:
            return exit_poi_idx
        if node_idx < 0 and updown_conn_direct_flg:
            node_idx = len(path) - 2
        return node_idx

    def _get_next_jct_node_idx(self, types_list, next_type_idx=1,
                               jct_inout=HWY_INOUT_TYPE_IN):
        '''取下个JCT
           @jct_inout=HWY_INOUT_TYPE_IN: JCT 入口
           @jct_inout=HWY_INOUT_TYPE_OUT: JCT 出口
        '''
        if next_type_idx < len(types_list):
            types = types_list[next_type_idx][2]
            if jct_inout == HWY_INOUT_TYPE_IN:
                if self._is_jct_in(types):
                    node_idx = types_list[next_type_idx][0]
                    return node_idx
            elif jct_inout == HWY_INOUT_TYPE_OUT:
                if self._is_jct_out(types):
                    node_idx = types_list[next_type_idx][0]
                    return node_idx
            else:
                self.log.error('Error JCT INOUT_C')
        # 下个设施不是JCT
        return -1

    def _get_next_node_idx(self, types_list, next_type_idx=1,
                           inout=HWY_INOUT_TYPE_IN):
        '''取得下个对应的设施
           @inout=HWY_INOUT_TYPE_IN: 取入口设施
           @inout=HWY_INOUT_TYPE_OUT: 取出口设施
        '''
        if next_type_idx < len(types_list):
            types = types_list[next_type_idx][2]
            if inout == HWY_INOUT_TYPE_IN:
                if(self._is_jct_in(types) or
                   self._is_ic_in(types) or
                   self._is_sapa_in(types)):
                    node_idx = types_list[next_type_idx][0]
                    return node_idx
            elif inout == HWY_INOUT_TYPE_OUT:
                if(self._is_jct_out(types) or
                   self._is_ic_out(types) or
                   self._is_sapa_out(types)):
                    node_idx = types_list[next_type_idx][0]
                    return node_idx
            else:
                self.log.error('Error JCT INOUT_C')
        # 下个设施不是想要的入口/出口
        return -1

    def _exist_in_ic_link(self, path, node_idx=0):
        '''进入的Ramp/JCT link.'''
        node = path[node_idx]
        out_edge = (node, path[node_idx + 1])
        in_nodes = self.G._get_not_main_link(node, HWY_PATH_ID, reverse=True)
        for in_node in in_nodes:
            data = self.G[in_node][node]
            if self.G.is_normal_inner_link(data):
                continue
            temp_path = [out_edge[1], out_edge[0], in_node]
            if self.G.check_regulation(temp_path, reverse=True):
                return True
            else:
                self.log.warning('regulation. node=%s.' % node)
                pass
        return False

    def _exist_out_ic_link(self, path, node_idx=None):
        '''退出的Ramp/JCT link.'''
        node = path[node_idx]
        in_edge = (path[node_idx - 1], node)
        out_nodes = self.G._get_not_main_link(node, HWY_PATH_ID)
        for out_node in out_nodes:
            data = self.G[node][out_node]
            if self.G.is_normal_inner_link(data):
                continue
            temp_path = list(in_edge) + [out_node]
            if self.G.check_regulation(temp_path, reverse=False):
                return True
            else:
                # self.log.warning('regulation. node=%s.' % node)
                pass
        return False

    def get_exit_poi_idx(self, path, e_idx, reverse=False):
        '''取得前方 Highway Exit POI.'''
        if e_idx >= 0:
            if not reverse:  # 前方(车流)
                node_idx = len(path) - 1
                while node_idx > e_idx:
                    if self.G.get_exit_poi_name(path[node_idx]):
                        return node_idx
                    node_idx -= 1
            else:  # 后方(车流)
                node_idx = 1
                while node_idx < e_idx:
                    if self.G.get_exit_poi_name(path[node_idx]):
                        return node_idx
                    node_idx += 1
        return None

    def _is_main_node_jct(self, node):
        '''本线点JCT(一条线路的头和另一条线路的尾相连)'''
        in_main_nodes = self.G.get_main_link(node, None,
                                             HWY_PATH_ID,
                                             same_code=False,
                                             reverse=True
                                             )
        out_main_nodes = self.G.get_main_link(node, None,
                                              HWY_PATH_ID,
                                              same_code=False,
                                              )
        # 一进一出
        if len(in_main_nodes) == 1 and len(out_main_nodes) == 1:
            if self._is_one_in_one_out(node):
                return True
            # 直线(180度)
            in_edge = (in_main_nodes[0], node)
            out_edge = (node, out_main_nodes[0])
            angle = self.G.get_angle(in_edge, out_edge)
            if self.G.is_straight(angle):
                self.log.info('straight. node=%s' % node)
                return True
        return False

    def _is_one_in_one_out(self, node):
        in_edges = self.G.in_edges(node)
        out_edges = self.G.out_edges(node)
        if len(in_edges) == 1 and len(out_edges) == 1:
            return True
        return False

    def _updown_line_conn_direct(self, path, other_main_link, reverse=False):
        '''上下行本线，直接相连.
           @path: 本线路径
        '''
        if len(other_main_link) != 1:
            return False
        # 本线和当前本线不可能通行
        if reverse:  # 逆/头
            temp_path = path[::-1] + other_main_link
        else:
            temp_path = path + other_main_link
        if not self.G.check_regulation(temp_path, reverse):
            return True
        # 两条本线的夹角小于60度
        if reverse:  # 逆/头
            in_edge = (other_main_link[0], path[0])
            out_edge = (path[0], path[1])
        else:  # 顺/尾
            in_edge = (path[-2], path[-1])
            out_edge = (path[-1], other_main_link[0])
        angle = self.G.get_angle(in_edge, out_edge)
        if not self.G.bigger_hwy_main_min_angle(angle):
            return True
        # 两条本线link的名称相似
        if self._is_similar_name(in_edge, out_edge):
            # 夹角小于135度
            if self.G.check_angle(angle, ANGLE_135):
                self.log.warning('Name is similar. '
                                 'in_edge=%s, out_edge=%s, angle=%s'
                                 % (in_edge, out_edge, angle * 360.0 / 65535))
        return False

    def _is_similar_name(self, in_edge, out_edge):
        data1 = self.G[in_edge[0]][in_edge[1]]
        data2 = self.G[out_edge[0]][out_edge[1]]
        name1 = data1.get('names')
        name2 = data2.get('names')
        if self.G.is_similar_name(name1, name2):
            return True
        return False
