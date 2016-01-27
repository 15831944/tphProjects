# -*- coding: UTF-8 -*-
'''
Created on 2015年9月22日

@author: hcz
'''
from common import cache_file
from component.rdf.hwy.hwy_def_rdf import HWY_PATH_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SAPA
from component.jdb.hwy.hwy_node_addinfo import HwyTollType
from component.jdb.hwy.hwy_node_addinfo import HwyNodeAddInfo
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_DOWN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.jdb.hwy.hwy_ic_info import convert_tile_id
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_ENTER
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_EXIT
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_JCT
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_PASS
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_SAPA
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_JCT
from component.jdb.hwy.hwy_graph import ONE_WAY_BOTH
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
ADD_INFO_CUTOFF = 10


class HwyNodeAddInfoRDF(HwyNodeAddInfo):
    '''
    Highway Node附加情报
    '''

    def __init__(self, data_mng, item_name='HwyNodeAddInfoRDF'):
        '''
        Constructor
        '''
        HwyNodeAddInfo.__init__(self, item_name)
        self.data_mng = data_mng
        self.file_obj = None

    def _Do(self):
        # 料金所的附加情报
        self.CreateTable2('highway_node_add_info')
        self._merge_temp_add_info()
        # self._make_both_2_inout()
        self.file_obj = cache_file.open('temp_highway_node_add_info')
        self._make_ics_node_add_info()  # 除料金外
        self._make_toll_node_add_info()  # 料金
        self._store_add_info()
        # 一条设施路径上，只能一个附加情报(非收费站)
        self._check_repeat_add_info()
        # 添加坐标信息(the_geom)——给QGis显示用
        self._add_the_geom()
        self._check_toll_add_info()
        if not self._check_facility_num():
            # 备分facility_num出错的记录
            self._backup_facility_num_failed_add_info()
            # 更新设施番号数目和seq_num
            self._update_facility_num()
        self._check_repeat_add_link()

    def _make_toll_node_add_info(self):
        return 0
        toll_data = self._get_toll_data()
        for toll in toll_data:
            node_id, dummy_toll_nodes = toll[0:2]
            up_downs, facility_ids = toll[2:4]
            toll_classes = toll[4]
            # class_names, etc_antennas, enters = toll[5:8]
            # exits, jcts, sa_pas = toll[8:11]
            unopen = toll[11][0]
            dummy = toll[12][0]
            link_id = toll[24]
            tile_id = toll[25]
            facil_names = toll[26]
            toll_name_flg = toll[17]
            no_toll_money = HWY_FALSE
            # ## 料金所功能種別
            toll_type = HwyTollType()
            toll_type_num = 1  # 料金功能种别数
            (toll_type.non_ticket_gate, toll_type.check_gate,
             toll_type.single_gate, toll_type.cal_gate,
             toll_type.ticket_gate, toll_type.nest,
             toll_type.uturn, toll_type.not_guide,
             toll_type.normal_toll, toll_type.etc_toll,
             toll_type.etc_section) = zip(*toll[13:24])[0]
            # ## 料金施設種別
            facility_num = 1  # 设施数
            seq_num = 1
            self._set_toll_class(toll_classes, toll_type)
            toll_type.unopen = unopen
            # 料金所名称
            facil_name = facil_names[0]
            addinfo_list = []
            # 收费站有多个设施番号 or 收费站有单独的名称(通常收费站和设施名不同)
            if(self._is_mult_facilitly_toll(facility_ids) or
               toll_name_flg):
                if not toll_name_flg:
                    toll_type.not_guide = HWY_TRUE
                else:
                    toll_type.not_guide = HWY_FALSE
                # toll_type没有指定，里面的字段默认都设为"0"
                toll_flag = HWY_FALSE
                toll_addinfo = AddInfoDataRDF(link_id, node_id,
                                              toll_flag, no_toll_money,
                                              tile_id=tile_id)
                addinfo_list.append(toll_addinfo)
            else:
                # # 设置 架空Flag
                # self._set_dummy_flag(toll_classes, toll_type, dummy)
                toll_flag = self._get_toll_flag(toll_type)
                facility_id = facility_ids[0]
                up_down = self._convert_updwon(up_downs)
                toll_addinfo = AddInfoDataRDF(link_id, node_id,
                                              toll_flag, no_toll_money,
                                              facility_num, up_down,
                                              facility_id, facil_name,
                                              seq_num, toll_type_num,
                                              toll_type, tile_id=tile_id)
                addinfo_list.append(toll_addinfo)
            for addinfo in addinfo_list:
                self._insert_add_info(addinfo)
        self.pg.commit2()

    def _check_repeat_add_info(self):
        '''一条设施路径上，只能一个附加情报(非收费站),
           if 有多个附加， 且前面附加情报的设施号>后面的加情报的设施号，删除前面。
           if 有多个附加， 且前面附加情报的设施号<=后面的加情报的设施号，删除前面
        '''
        prev_link = None
        for (link_lids, node_lids, add_links,
             add_nodes, facility_ids, facil_types,
             add_link_lids, ) in self._get_multi_add_info_facil():
            # facil_types: exit, exit, jct, sapa
            link_lid_list = [map(int, n_lid.split(','))
                             for n_lid in link_lids]
            # node_lid_list = [map(int, n_lid.split(','))
            #                  for n_lid in node_lids]
            add_link_set = set(add_links)
            add_facil_infos = zip(add_links, add_nodes,
                                  facility_ids, facil_types,
                                  add_link_lids)
            for link_lid in link_lid_list:
                inter_links = add_link_set.intersection(link_lid)
                inter_facils = self._get_inter_facil(inter_links,
                                                     add_facil_infos)
                ic_jct_facils, sapa_facils = inter_facils
                # 注：SAPA与IC/JCT可能同时存在, 所以要分开判断
                if len(ic_jct_facils) > 1:  # 一条路径上 ic/jct数目大于1
                    if self._over_add_link_lid(ic_jct_facils):
                        # 有多个设施， 种别不同或设施番号不同
                        self.log.warning('Mulit Add Info. link_lid=%s'
                                         % link_lid)
                    if len(ic_jct_facils) == 2:
                        # 附加情报路径有重叠/覆盖
                        # 一个是出口，一个是入口
                        temp_types = set(info[3] for info in ic_jct_facils)
                        if temp_types == set(['exit', 'enter']):
                            continue
                # 最多不能经过两个SAPA
                if len(sapa_facils) > 2:
                    if prev_link != link_lid[0]:
                        self.log.warning('SAPA Add Info > 2. link_lid=%s'
                                         % link_lid)
                        prev_link = link_lid[0]
                elif len(sapa_facils) > 1:
                    # 同个SAPA不应该出现多次
                    facil_ids = [info[2] for info in sapa_facils]
                    for facil_id in facil_ids:
                        if facil_ids.count(facil_id) > 1:
                            if prev_link != link_lid[0]:
                                self.log.warning('SAPA Add Info > 1. '
                                                 'link_lid=%s' % link_lid)
                                prev_link = link_lid[0]
                else:
                    pass

    def _get_inter_facil(self, inter_links, add_facil_infos):
        ic_jct_facil = set()
        sapa_facil = set()
        for link in inter_links:
            for add_facil_info in add_facil_infos:
                if link == add_facil_info[0]:
                    facil_type = add_facil_info[3]
                    if facil_type == 'sapa':
                        sapa_facil.add(add_facil_info)
                    elif facil_type in ('exit', 'enter', 'jct'):
                        ic_jct_facil.add(add_facil_info)
                    else:
                        self.log.error('Error Facil_type. link_id=%s'
                                       % link)
        return ic_jct_facil, sapa_facil

    def _over_add_link_lid(self, ic_jct_facils):
        '''附加link_lid有重叠or覆盖'''
        i = 0
        ic_jct_facils = list(ic_jct_facils)
        while i < len(ic_jct_facils) - 1:
            info = ic_jct_facils[i]
            add_link1 = info[0]
            facil_type1 = info[3]
            add_link_lid1 = info[4]
            if facil_type1 == 'enter':
                if add_link_lid1:
                    link_lid1 = str(add_link1) + ',' + add_link_lid1
                else:
                    link_lid1 = str(add_link1)
            else:
                if add_link_lid1:
                    link_lid1 = add_link_lid1 + ',' + str(add_link1)
                else:
                    link_lid1 = str(add_link1)
            j = i + 1
            while j < len(ic_jct_facils):
                info = ic_jct_facils[j]
                add_link2 = info[0]
                facil_type2 = info[3]
                add_link_lid2 = info[4]
                if not add_link_lid1:
                    add_link_lid1 = ''
                if facil_type2 == 'enter':
                    if add_link_lid2:
                        link_lid2 = str(add_link2) + ',' + add_link_lid2
                    else:
                        link_lid2 = str(add_link2)
                else:
                    if add_link_lid2:
                        link_lid2 = add_link_lid2 + ',' + str(add_link2)
                    else:
                        link_lid2 = str(add_link2)
                if(link_lid1 in link_lid2 or
                   link_lid2 in link_lid1):
                    types = set([facil_type1, facil_type2])
                    if types == set(['enter', 'exit']):  # 出口和入口
                        pass
                    else:
                        return True
                j += 1
            i += 1
        return False

    def _get_multi_add_info_facil(self):
        '''取得有多个附加情报的设施'''
        sqlcmd = """
        SELECT link_lids, node_lids,
               add_links, add_nodes,
               facility_ids, facil_types,
               add_link_lids--, dir_s_nodes,
               --dir_e_nodes
        FROM (
            SELECT road_code, road_seq,
                   facilcls_c, inout_c, node_id,
                   array_agg(add_link) AS add_links,
                   array_agg(add_node) AS add_nodes,
                   array_agg(facility_id) AS facility_ids,
                   array_agg(facil_type) AS facil_types,
                   array_agg(link_lid) AS add_link_lids,
                   array_agg(dir_s_node) as dir_s_nodes,
                   array_agg(dir_e_node) as dir_e_nodes
             FROM (
                SELECT distinct b.road_code, b.road_seq,
                       b.facilcls_c, b.inout_c, b.node_id,
                       a.link_id as add_link,
                       a.node_id as add_node,
                       facility_id,
                       (case when exit = 1 then 'exit'
                       when enter = 1 then 'enter'
                       when jct = 1 then 'jct'
                       when sa = 1 or pa = 1 then 'sapa'
                       else NULL end) as facil_type,
                       a.link_lid,
                       a.dir_s_node,
                       a.dir_e_node
                  FROM highway_node_add_info AS a
                  INNER JOIN mid_temp_hwy_ic_path_expand_link AS b
                  ON a.link_id = b.pass_link_id and
                     facility_id > 0 and
                     path_type <> 'UTURN'
                  --where link_id in (12886906, 79189159)
              ) as c
              group by road_code, road_seq, facilcls_c,
                       inout_c, node_id
              having count(*) > 1
         ) AS d
         LEFT JOIN (
            SELECT road_code, road_seq,
                   facilcls_c, inout_c, node_id,
                   array_agg(node_lid) as node_lids,
                   array_agg(link_lid) as link_lids
              FROM mid_temp_hwy_ic_path
              group by road_code, road_seq,
                   facilcls_c, inout_c, node_id
         ) as e
         ON d.road_code = e.road_code and
            d.road_seq = e.road_seq and
            d.facilcls_c = e.facilcls_c and
            d.inout_c = e.inout_c and
            d.node_id = e.node_id
         order by d.road_code, d.road_seq,
                  d.facilcls_c, d.inout_c, d.node_id;
        """
        return self.get_batch_data(sqlcmd)

    def _set_toll_class(self, toll_classes, hwy_toll_type):
        '''料金种别'''
        if set(toll_classes) == set([TOLL_CLS_PASS]):  # 料金所
            hwy_toll_type.gate = HWY_TRUE
        else:
            if set(toll_classes) == set([TOLL_CLS_ENTER,
                                         TOLL_CLS_EXIT]):  # 出/入
                hwy_toll_type.enter = HWY_TRUE
                hwy_toll_type.exit = HWY_TRUE
            elif set(toll_classes) == set([TOLL_CLS_ENTER]):  # 入口
                hwy_toll_type.enter = HWY_TRUE
            elif set(toll_classes) == set([TOLL_CLS_EXIT]):  # 出口
                hwy_toll_type.exit = HWY_TRUE
            elif set(toll_classes) == set([TOLL_CLS_JCT]):  # JCT/UTURN
                pass   # 什么种别都不做
            else:
                pass

    def _is_inout_common_toll(self, facility_ids, toll_classes):
        '''出口和入口共用收费站'''
        if set(toll_classes) == set(1, 2):
            return True
        return False

    def _is_mult_facilitly_toll(self, facility_ids):
        '''收费站有多个设施番号'''
        if len(set(zip(facility_ids))) > 1:
            return True
        else:
            return False

    def _convert_updwon(self, up_downs):
        rst_updown = None
        up_down_set = set(up_downs)
        if HWY_UPDOWN_TYPE_UP in up_down_set:
            if HWY_UPDOWN_TYPE_DOWN in up_down_set:
                rst_updown = 3
            else:
                rst_updown = 1
        elif HWY_UPDOWN_TYPE_DOWN in up_down_set:
            rst_updown = 0
        else:
            pass
        return rst_updown

    def _merge_temp_add_info(self):
        self.log.info('Merge Temp Node Add Info.')
        self.CreateTable2('mid_hwy_node_add_info_merged')
        file_obj = cache_file.open('mid_hwy_node_add_info_merged')
        data_mng = self.data_mng
        data_mng.load_add_info_link()
        for (ic_no, updown, facility_id,
             facilcls, in_out, link_lid,
             node_lid, add_node_id, add_link_id,
             dir_s_node, dir_e_node, facil_name,
             pos_type, pos_type_name, toll_flag,
             path_type, in_linkid, ic_num,
             all_link_lid) in self._get_temp_add_info_data():
            link_list = map(int, link_lid.split(','))
            node_list = map(int, node_lid.split(','))
            last_add_idx = -1
            curr_add_idx = len(link_list)
            link_idx = len(link_list) - 1
            if(path_type == HWY_PATH_TYPE_IC and
               facilcls not in (HWY_IC_TYPE_SAPA)):
                while link_idx >= 0:
                    link_id = link_list[link_idx]
                    if link_id == add_link_id:
                        curr_add_idx = link_idx
                        break
                    # 最后一条附加link所在位置
                    if(last_add_idx < 0 and
                       self.data_mng.is_add_info_link(link_id)):
                        last_add_idx = link_idx
                    link_idx -= 1
                if curr_add_idx < last_add_idx:
                    temp_link_id = link_list[last_add_idx]
                    # 所有的路径都经过该link
                    if self._all_pass_add_link(all_link_lid, temp_link_id):
                        # print 'Move add link:', link_lid
                        add_link_id = link_list[last_add_idx]
                        add_node_id = node_list[last_add_idx]
                        if in_out == HWY_INOUT_TYPE_IN:
                            dir_s_node = node_list[last_add_idx+1]
                            dir_e_node = node_list[last_add_idx]
                            ic_num = data_mng.get_enter_facil_num(add_link_id)
                        else:
                            dir_s_node = node_list[last_add_idx]
                            dir_e_node = node_list[last_add_idx+1]
                            ic_num = data_mng.get_exit_facil_num(add_link_id)
            if not facil_name:
                facil_name = ''
            params = (ic_no, updown, facility_id,
                      facilcls, in_out, link_lid,
                      node_lid, add_node_id, add_link_id,
                      dir_s_node, dir_e_node, facil_name,
                      pos_type, pos_type_name, toll_flag,
                      in_linkid, ic_num, path_type
                      )
            self._insert_merged_add_info(file_obj, params)
        self._store_merged_add_info(file_obj)

    def _make_both_2_inout(self):
        '''出入口附加情报link两头和一般道相通，即一般道能可以穿过这些link. 坐标：89.43317,42.92208'''
        cutoff_10 = ADD_INFO_CUTOFF
        G = self.data_mng.get_graph()
        for (link_id, u, v, one_way) in self._get_all_ic_add_info_link():
            if one_way in (ONE_WAY_POSITIVE, ONE_WAY_BOTH):
                pathes = list(G.all_path_2_inout([u, v],
                                                 HWY_ROAD_CODE,
                                                 False,
                                                 cutoff_10))
                for path in pathes:
                    path = path[::-1]
                    for temp_path in G.all_path_2_inout(path,
                                                        HWY_ROAD_CODE,
                                                        True,
                                                        cutoff_10):
                        if temp_path:
                            if self._get_toll_num(temp_path) < 2:
                                print link_id
                                break
            if one_way in (ONE_WAY_RERVERSE, ONE_WAY_BOTH):
                pathes = list(G.all_path_2_inout([v, u],
                                                 HWY_ROAD_CODE,
                                                 False,
                                                 cutoff_10))
                for path in pathes:
                    path = path[::-1]
                    for temp_path in G.all_path_2_inout(path,
                                                        HWY_ROAD_CODE,
                                                        True,
                                                        cutoff_10):
                        if temp_path:
                            if self._get_toll_num(temp_path) < 2:
                                print link_id
                                break

    def _get_toll_num(self, path):
        toll_nodes = set()
        G = self.data_mng.get_graph()
        for node in path:
            if G.is_tollgate(node):
                toll_nodes.add(node)
        return len(toll_nodes)

    def _all_pass_add_link(self, all_link_lid, add_link_id):
        for link_lid in all_link_lid:
            if link_lid:
                temp_lid = map(int, link_lid.split(','))
                if add_link_id not in temp_lid:
                    return False
            else:
                return False
        return True

    def _make_ics_node_add_info(self):
        """设施附加情报"""
        add_link_changed = {}
        for info in self._get_add_info_data():
            add_linkid, tile_id, add_node_ids, inout_list = info[0:4]
            if len(set(add_node_ids)) > 1:
                self.log.warning('Number of add node > 1. link_id=%s'
                                 % add_linkid)
            add_nodeid = add_node_ids[0]
            add_info_dict = {}
            add_info_list = zip(*info[4:15])
            path_types = info[15]
            # ## 按In/Out和path_type种别分类
            for i in range(0, len(inout_list)):
                inout = inout_list[i]
                path_type = path_types[i]
                key = (inout, path_type)
                if key in add_info_dict:
                    add_info_dict[key].append(add_info_list[i])
                else:
                    add_info_dict[key] = [add_info_list[i]]
            seq_num = 1
            rst_add_infos = []
            for key, add_info_list in add_info_dict.items():
                inout, path_type = key
                # ## JCT
                if path_type == HWY_PATH_TYPE_JCT:
                    jct_adds = self._set_jct_add_info(add_linkid, add_nodeid,
                                                      tile_id, add_info_list,
                                                      seq_num)
                    self._add_add_infos(rst_add_infos, jct_adds)
                    continue
                # ## SAPA
                elif path_type == HWY_PATH_TYPE_SAPA:
                    sapa_adds = self._set_sapa_add_info(add_linkid, add_nodeid,
                                                        tile_id, add_info_list,
                                                        seq_num)
                    self._add_add_infos(rst_add_infos, sapa_adds)
                    continue
                # ## IC入口/入口
                elif path_type == HWY_PATH_TYPE_IC:
                    ic_adds = self._set_ic_add_info(add_linkid, add_nodeid,
                                                    tile_id, add_info_list,
                                                    seq_num, inout)
                    for add_info in ic_adds:
                        new_linkid = add_info.link_id
                        if new_linkid == add_linkid:
                            self._add_add_infos(rst_add_infos, [add_info])
                        else:  # 附加link变动了(入口附加link被移到本线)
                            if new_linkid not in add_link_changed:
                                add_link_changed[new_linkid] = [add_info]
                            else:
                                enter_add_info = add_link_changed[new_linkid]
                                self._add_add_infos(enter_add_info, [add_info])
                else:
                    self.log.error('Unkown path_type=%s.', path_type)
            # 重设facility_num和seq_num
            facility_num = len(rst_add_infos)
            seq_num = 1
            for add_info in rst_add_infos:
                add_info.facility_num = facility_num
                add_info.seq_num = seq_num
                seq_num += 1
                self._insert_add_info(add_info)
        # 附加link变动了(入口附加link被移到本线)
        for rst_add_infos in add_link_changed.itervalues():
            facility_num = len(rst_add_infos)
            seq_num = 1
            for add_info in rst_add_infos:
                add_info.facility_num = facility_num
                add_info.seq_num = seq_num
                seq_num += 1
                self._insert_add_info(add_info)
        self.pg.commit2()

    def _set_jct_add_info(self, add_linkid, add_nodeid,
                          tile_id, add_info_list, seq_num):
        rst_add_infos = []
        toll_flag = HWY_FALSE
        no_toll_money = HWY_FALSE
        toll_type_num = 0
        inout = HWY_INOUT_TYPE_OUT
        seq_num, facility_num = 1, 1  # 好去掉重叠的
        (updown_list, facility_ids, facilcls_list, facil_names,
         link_lids, node_lids, dir_s_nodes, dir_e_nodes,
         ic_nums, in_linkids, road_codes) = zip(*add_info_list)
        dir_nodes = zip(dir_s_nodes, dir_e_nodes)
        dir_s_node, dir_e_node = self._get_dir_node(add_linkid, dir_nodes)
        if max(ic_nums) > 1:  # 后方多个设施
            for (updown, facility_id, facilcls, facil_name,
                 link_lid, node_lid,  dir_s_node, dir_e_node,
                 ic_nums, in_linkid, road_code) in add_info_list:
                up_down = self._convert_updwon([updown])
                link_list = map(int, link_lid.split(','))
                new_link_lid = [in_linkid]
                new_link_lid += self._cut_link_lid(link_list, add_linkid)
                toll_type = HwyTollType()
                toll_type.set_facil_type(facilcls, inout)  # 设施种别
                add_info = AddInfoDataRDF(add_linkid, add_nodeid,
                                          toll_flag, no_toll_money,
                                          facility_num, up_down,
                                          facility_id, facil_name,
                                          seq_num, toll_type_num,
                                          toll_type, tile_id,
                                          dir_s_node, dir_e_node,
                                          link_lid=new_link_lid)
                self._add_add_infos(rst_add_infos, [add_info])
        else:
            facil_id = min(facility_ids)
            up_down = self._convert_updwon(updown_list)
            facil_name = facil_names[0]
            toll_type = HwyTollType()
            toll_type.set_facil_type(facilcls_list[0], inout)  # 设施种别
            add_info = AddInfoDataRDF(add_linkid, add_nodeid,
                                      toll_flag, no_toll_money,
                                      facility_num, up_down,
                                      facil_id, facil_name,
                                      seq_num, toll_type_num,
                                      toll_type, tile_id,
                                      dir_s_node, dir_e_node)
            self._add_add_infos(rst_add_infos, [add_info])
        return rst_add_infos

    def _set_ic_add_info(self, add_linkid, add_nodeid,
                         tile_id, add_info_list,
                         seq_num, inout):
        '''IC出口/入口'''
        rst_add_infos = []
        toll_flag = HWY_FALSE
        no_toll_money = HWY_FALSE
        toll_type_num = 0
        seq_num, facility_num = 1, 1  # 好去掉重叠的
        (updown_list, facility_ids, facilcls_list, facil_names,
         link_lids, node_lids, dir_s_nodes, dir_e_nodes,
         ic_nums, in_linkids, road_codes) = zip(*add_info_list)
        dir_nodes = zip(dir_s_nodes, dir_e_nodes)
        dir_s_node, dir_e_node = self._get_dir_node(add_linkid, dir_nodes)
        max_ic_num = max(ic_nums)
        all_name_num = len(facil_names)
        set_name_num = len(set(facil_names))
        # 一个设施 or 所有设施名称相同
        if(max_ic_num <= 1 or
           (all_name_num >= max_ic_num and set_name_num <= 1)):
            facil_id = facility_ids[0]
            up_down = self._convert_updwon(updown_list)
            facil_name = facil_names[0]
            toll_type = HwyTollType()
            toll_type.set_facil_type(facilcls_list[0], inout)  # 设施种别
            add_info = AddInfoDataRDF(add_linkid, add_nodeid,
                                      toll_flag, no_toll_money,
                                      facility_num, up_down,
                                      facil_id, facil_name,
                                      seq_num, toll_type_num,
                                      toll_type, tile_id,
                                      dir_s_node, dir_e_node)
            self._add_add_infos(rst_add_infos, [add_info])
        else:
            for (updown, facility_id, facilcls, facil_name,
                 link_lid, node_lid,  dir_s_node, dir_e_node,
                 ic_nums, in_linkid, road_code) in add_info_list:
                up_down = self._convert_updwon([updown])
                link_list = map(int, link_lid.split(','))
                cut_link_lid = self._cut_link_lid(link_list, add_linkid)
                # 入口: link序列是倒序，本线link做成附加link
                if inout == HWY_INOUT_TYPE_IN:
                    # link序要加上旧的add_linkid
                    new_link_lid = cut_link_lid + [add_linkid]
                    new_add_linkid = in_linkid  # 入口时，in_linkid其实是设施的本线退出link
                    node_lid = map(int, node_lid.split(','))
                    new_add_nodeid = node_lid[0]
                    new_dir_s_node = new_add_nodeid
                    new_dir_e_node = self._get_facil_outlink(new_add_nodeid,
                                                             road_code)
                    G = self.data_mng.get_graph()
                    new_tile_id = G.get_tile_id(new_dir_s_node, new_dir_e_node)
                else:  # 出口
                    # link序要加上in_link
                    new_link_lid = [in_linkid] + cut_link_lid
                    new_add_linkid = add_linkid
                    new_add_nodeid = add_nodeid
                    new_dir_s_node = dir_s_node
                    new_dir_e_node = dir_e_node
                    new_tile_id = tile_id
                toll_type = HwyTollType()
                toll_type.set_facil_type(facilcls, inout)  # 设施种别
                add_info = AddInfoDataRDF(new_add_linkid, new_add_nodeid,
                                          toll_flag, no_toll_money,
                                          facility_num, up_down,
                                          facility_id, facil_name,
                                          seq_num, toll_type_num,
                                          toll_type, new_tile_id,
                                          new_dir_s_node, new_dir_e_node,
                                          link_lid=new_link_lid)
                self._add_add_infos(rst_add_infos, [add_info])
        return rst_add_infos

    def _set_sapa_add_info(self, add_linkid, add_nodeid,
                           tile_id, add_info_list, seq_num):
        rst_add_infos = []
        toll_flag = HWY_FALSE
        no_toll_money = HWY_FALSE
        toll_type_num = 0
        inout = HWY_INOUT_TYPE_OUT
        seq_num, facility_num = 1, 1
        (updown_list, facility_ids, facilcls_list, facil_names,
         link_lids, node_lids, dir_s_nodes, dir_e_nodes,
         ic_nums, in_linkids, road_codes) = zip(*add_info_list)
        dir_nodes = zip(dir_s_nodes, dir_e_nodes)
        dir_s_node, dir_e_node = self._get_dir_node(add_linkid, dir_nodes)
        if len(set(facil_names)) > 1:
            self.log.error('SAPA Name > 1. facil_names=%s' % (facil_names,))
        if len(set(facilcls_list)) > 1:
            self.log.error('SAPA Type > 1. add_linkid=%s' % (add_linkid,))
        facil_id = min(facility_ids)
        up_down = self._convert_updwon(updown_list)
        facil_name = facil_names[0]
        toll_type = HwyTollType()
        toll_type.set_facil_type(facilcls_list[0], inout)  # 设施种别
        add_info = AddInfoDataRDF(add_linkid, add_nodeid,
                                  toll_flag, no_toll_money,
                                  facility_num, up_down,
                                  facil_id, facil_name,
                                  seq_num, toll_type_num,
                                  toll_type, tile_id,
                                  dir_s_node, dir_e_node)
        rst_add_infos.append(add_info)
        return rst_add_infos

    def _add_add_infos(self, rst_add_infos, add_infos=[]):
        for new_add_info in add_infos:
            exist_flag = False
            for rst_add_info in rst_add_infos:
                if new_add_info == rst_add_info:
                    exist_flag = True
                    break
            if not exist_flag:
                rst_add_infos.append(new_add_info)

    def _cut_link_lid(self, link_lid, add_link_id):
        cut_link_lid = []
        for link in link_lid:
            if link == add_link_id:
                break
            else:
                cut_link_lid.append(link)
        return cut_link_lid

    def _get_facil_outlink(self, node_id, road_code):
        '''取得设施本线退出link'''
        G = self.data_mng.get_graph()
        outnodes = G.get_main_link(node_id, road_code,
                                   same_code=True, reverse=False)
        if outnodes:
            if len(outnodes) > 1:
                self.log.error('OutLink Number > 1. node=%s' % node_id)
            return outnodes[0]
        else:
            self.log.error('No OutLink. node=%s' % node_id)
        return None

    def _get_min_facility_id(self, facility_ids, facil_names):
        facility_list1 = []  # 有名称
        facility_list2 = []  # 无名称
        for facility_id, name in zip(facility_ids, facil_names):
            if name:
                facility_list1.append(facility_id)
            else:
                facility_list2.append(facility_id)
        if facility_list1:
            return min(facility_list1)
        else:
            return min(facility_list2)

    def _get_dir_node(self, add_link_id, dir_nodes):
        dir_nodes_set = set(dir_nodes)
        if not dir_nodes_set:
            self.log.error('No Dir Node. link_id=%s' % add_link_id)
            return None, None
        if len(dir_nodes_set) > 1:
            self.log.error('Number of Dir Node > 1. link_id=%s' % add_link_id)
            return None, None
        return dir_nodes_set.pop()

    def _get_facil_name(self, facil_names, facility_ids, facility_id):
        for id_idx in range(0, len(facility_ids)):
            if facility_ids[id_idx] == facility_id:
                return facil_names[id_idx]
        return None, None

    def _get_link_tile_id(self, node_id, link_id, tile_id_14):
        if tile_id_14:
            return convert_tile_id(tile_id_14)
        G = self.data_mng.get_graph()
        if node_id not in G:
            return None
        tile_id_14 = G.get_out_link_tile(node_id, link_id)
        if not tile_id_14:
            tile_id_14 = G.get_in_link_tile(node_id, link_id)
        return convert_tile_id(tile_id_14)

    def _is_single_toll(self, add_link, link_lid_array):
        '''单一收费站：经过该收费站的设施是唯一的'''
        G = self.data_mng.get_graph()
        for link_lid in link_lid_array:
            for link_id in link_lid[::-1]:
                if link_id == add_link:
                    break
                toll_facils = self.data_mng.get_toll_facil_by_link(link_id)
                if not toll_facils:
                    continue
                if len(toll_facils) == 1:
                    toll_node = toll_facils[0].node_id
                    if G.get_node_name(toll_node):
                        # 收费站点有名称时，通常收费站名称和设施名称不同，所以设施附加情报不和收费站放一起
                        return False
                else:
                    return False
        return True

    def _get_toll_facil(self, link_id):
        toll_facils_list = self.data_mng.get_toll_facil_by_link(link_id)
        return toll_facils_list

    def _get_out_link(self, node_id):
        out_link_id = None
        G = self.data_mng.get_graph()
        out_linkids = G.get_outlinkids(node_id)
        if out_linkids:
            if len(out_linkids) > 1:
                self.log.error('OutLink Number > 1. node_id=%s'
                               % node_id)
            else:
                out_link_id = out_linkids[0]
        return out_link_id

    def _get_temp_add_info_data(self):
        sqlcmd = """
        SELECT a.ic_no, updown, facility_id,
               facilcls, in_out, link_lid,
               node_lid, add_node_id, add_link_id,
               dir_s_node, dir_e_node, name_kanji,
               pos_type, pos_type_name, toll_flag,
               path_type, in_linkid, ic_num,
               all_link_lid
          FROM mid_hwy_node_add_info as a
          LEFT JOIN (
            SELECT ic_no, array_agg(link_lid) as all_link_lid
              FROM mid_hwy_node_add_info
              where path_type = 'IC'
              group by ic_no
          ) AS b
          ON a.ic_no = b.ic_no
          order by a.ic_no, add_link_id
        """
        return self.get_batch_data(sqlcmd, batch_size=1024)

    def _get_add_info_data(self):
        '''料金情报，不包含料金所'''
        sqlcmd = """
        SELECT add_link_id,
               tile_id,
               array_agg(add_node_id) as add_node_ids,
               array_agg(in_out) in_outs,
               array_agg(updown) updowns,
               array_agg(a.facility_id) facility_ids,
               array_agg(facilcls) facilcls,
               array_agg(facil_name) facil_names,
               array_agg(link_lid) as link_lid_array,
               array_agg(node_lid) as node_lid_array,
               array_agg(dir_s_node) as dir_s_nodes,
               array_agg(dir_e_node) as dir_e_nodes,
               array_agg(ic_num) as ic_nums,
               array_agg(in_linkid) as in_linkids,
               array_agg(road_code) as road_codes,
               array_agg(path_type) as path_types
          FROM mid_hwy_node_add_info_merged AS a
          LEFT JOIN link_tbl as b
          ON add_link_id = link_id
          LEFT JOIN (
            SELECT distinct road_code, facility_id
              FROM mid_hwy_ic_no
          ) as c
          ON a.facility_id = c.facility_id
          group by tile_id, add_link_id
          order by tile_id, add_link_id;
        """
        return self.get_batch_data(sqlcmd)

    def _get_toll_data(self):
        sqlcmd = """
        SELECT node_id, array_agg(dummy_toll_node) dummy_toll_nodes,
               array_agg(up_down) updowns, array_agg(facility_id) FacilityIds,
               array_agg(toll_class) toll_cls, array_agg(class_name) cls_name,
               array_agg(etc_antenna) etc_antenna, array_agg(enter) enter,
               array_agg(exit) exit, array_agg(jct) jct,
               array_agg(sa_pa) sa_pa, array_agg(unopen) unopen,
               array_agg(dummy) dummy, array_agg(non_ticket_gate) non_ticket,
               array_agg(check_gate) check_gate, array_agg(single_gate) single,
               array_agg(cal_gate) cal_gate, array_agg(ticket_gate) ticket,
               array_agg(nest) nest, array_agg(uturn) uturn,
               array_agg(not_guide) not_guide, array_agg(normal_toll) normal,
               array_agg(etc_toll) etc_toll, array_agg(etc_section) etc_sec,
               link_id, tile_id,
               array_agg(facil_name) as facil_names
          from (
                SELECT distinct a.node_id, dummy_toll_node, toll_class,
                       class_name, up_down, facility_id,
                       etc_antenna, enter, exit,
                       jct, sa_pa, unopen,
                       dummy, non_ticket_gate, check_gate,
                       single_gate, cal_gate, ticket_gate,
                       nest, a.uturn, not_guide,
                       normal_toll, etc_toll, etc_section,
                       b.link_id, tile_id, facil_name
                  FROM highway_toll_info AS a
                  LEFT JOIN hwy_tollgate AS b
                  ON a.node_id = b.node_id and
                     a.road_code = b.road_code and
                     a.road_seq = b.road_seq and
                     ((toll_class = 1 and inout_c = 1) or  -- in
                      (toll_class = 2 and inout_c = 2) or
                      (toll_class not in (1, 2)))
                  LEFT JOIN link_tbl as c
                  ON b.link_id = c.link_id
          ) as toll
          group by node_id, link_id, tile_id
          order by node_id, link_id, tile_id;
        """
        return self.get_batch_data(sqlcmd)

    def _insert_add_info(self, add_info):
        if self.file_obj:
            facil_name = add_info.facil_name
            if not facil_name:
                facil_name = ''
            toll_type = add_info.toll_type
            if not add_info.dir_s_node:
                add_info.dir_s_node = ''
            if not add_info.dir_e_node:
                add_info.dir_e_node = ''
            if add_info.link_lid:
                link_lid = ','.join(map(str, add_info.link_lid))
            else:
                link_lid = ''
            params = (add_info.link_id, add_info.node_id,
                      add_info.toll_flag, add_info.no_toll_money,
                      add_info.facility_num, add_info.up_down,
                      add_info.facility_id, add_info.seq_num,
                      add_info.dir_s_node, add_info.dir_e_node,
                      toll_type.etc_antenna, toll_type.enter,
                      toll_type.exit, toll_type.jct,
                      toll_type.sa, toll_type.pa,
                      toll_type.gate, toll_type.unopen,
                      toll_type.dummy_facil, add_info.toll_type_num,
                      toll_type.non_ticket_gate, toll_type.check_gate,
                      toll_type.single_gate, toll_type.cal_gate,
                      toll_type.ticket_gate, toll_type.nest,
                      toll_type.uturn, toll_type.not_guide,
                      toll_type.normal_toll, toll_type.etc_toll,
                      toll_type.etc_section, facil_name,
                      add_info.tile_id, add_info.no_toll_flag,
                      link_lid)
        self.file_obj.write('%d\t%d\t%d\t%d\t'
                            '%d\t%d\t%d\t%d\t'
                            '%s\t%s\t%d\t%d\t'
                            '%d\t%d\t%d\t%d\t'
                            '%d\t%d\t%d\t%d\t'
                            '%d\t%d\t%d\t%d\t'
                            '%d\t%d\t%d\t%d\t'
                            '%d\t%d\t%d\t%s\t'
                            '%d\t%d\t%s\n'
                            % params)
        return

    def _insert_merged_add_info(self, file_obj, params):
        if file_obj:
            # (ic_no, updown, facility_id,
            # facilcls, in_out, link_lid,
            # node_lid, add_node_id, add_link_id,
            # dir_s_node, dir_e_node, name_kanji,
            # pos_type, pos_type_name, toll_flag
            # )
            file_obj.write('%d\t%d\t%d\t'
                           '%d\t%d\t%s\t'
                           '%s\t%d\t%d\t'
                           '%d\t%d\t%s\t'
                           '%d\t%s\t%d\t'
                           '%d\t%d\t%s\n'
                           % params)

    def _store_merged_add_info(self, file_obj):
        if file_obj:
            file_obj.seek(0)
            self.pg.copy_from2(file_obj, 'mid_hwy_node_add_info_merged',
                               columns=['ic_no', 'updown', 'facility_id',
                                        'facilcls', 'in_out', 'link_lid',
                                        'node_lid', 'add_node_id',
                                        'add_link_id', 'dir_s_node',
                                        'dir_e_node', 'facil_name',
                                        'pos_type', 'pos_type_name',
                                        'toll_flag', 'in_linkid',
                                        'ic_num', 'path_type'
                                        ])
            self.pg.commit2()
            cache_file.close(file_obj)
            file_obj = None

    def _store_add_info(self):
        if self.file_obj:
            self.file_obj.seek(0)
            self.pg.copy_from2(self.file_obj,
                               'highway_node_add_info',
                               columns=['link_id', 'node_id',
                                        'toll_flag', 'no_toll_money',
                                        'facility_num', 'up_down',
                                        'facility_id', 'seq_num',
                                        'dir_s_node', 'dir_e_node',
                                        'etc_antenna', 'enter',
                                        'exit', 'jct',
                                        'sa', 'pa',
                                        'gate', 'un_open',
                                        'dummy', 'toll_type_num',
                                        'non_ticket_gate', 'check_gate',
                                        'single_gate', 'cal_gate',
                                        'ticket_gate', 'nest',
                                        'uturn', 'not_guide',
                                        'normal_toll', 'etc_toll',
                                        'etc_section', 'name',
                                        'tile_id', 'no_toll_flag',
                                        'link_lid'])
            self.pg.commit2()
            cache_file.close(self.file_obj)
            self.file_obj = None
        self.CreateIndex2('highway_node_add_info_link_id_idx')
        self.CreateIndex2('highway_node_add_info_node_id_link_id_idx')

    def _check_toll_add_info(self):
        '''check收费站的附加情报有无重复'''
        self.log.info('Check toll add info.')
        # ## 收费站的附加link有重复
        sqlcmd = """
        SELECT count(link_id)
          FROM (
            SELECT link_id
              FROM highway_node_add_info
              where toll_flag = 1
              group by link_id
              having count(*) > 1
          ) AS a
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] != 0:
                self.log.error('Toll Add Link > 1.')
                return False
        # ## 收费站的附加点有重复
        sqlcmd = """
        SELECT count(node_id)
          FROM (
            SELECT node_id
              FROM highway_node_add_info
              where toll_flag = 1
              group by node_id
              having count(*) > 1
          ) AS a
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] != 0:
                self.log.error('Toll Add Node > 1.')
                return False
        return True

    def _check_facility_num(self):
        self.log.info('Check facility number.')
        sqlcmd = """
        SELECT distinct a.link_id
          FROM (
            SELECT distinct link_id, facility_num
              FROM highway_node_add_info
          ) AS a
          LEFT JOIN (
            SELECT distinct link_id, COUNT(link_id) as num
              FROM highway_node_add_info
              group by link_id
          ) AS b
          on a.link_id = b.link_id
          where facility_num <> num
        """
        link_id = None
        for row in self.get_batch_data(sqlcmd):
            link_id = row[0]
            self.log.warning('Check Facility_num Failed. link_id=%s' % link_id)
        if link_id:
            return False
        return True

    def _check_repeat_add_link(self):
        sqlcmd = """
        SELECT link_id, node_id, link_lid,
               array_agg(enter), array_agg(exit),
               array_agg(dir_s_node), array_agg(dir_e_node)
          FROM highway_node_add_info
          GROUP BY link_id, node_id, link_lid
          HAVING COUNT(*) > 1
        """
        for row in self.get_batch_data(sqlcmd):
            # link_id, node_id, link_lid = row[:3]
            enters, exits, dir_s_nodes, dir_e_nodes = row[3:7]
            # 种别必须是入口和出口
            types = zip(enters, exits)
            if set(types) != set([(1, 0), (0, 1)]):
                self.log.error('Repeat Add link.')
                return False
            # s_node, e_node不能重复
            s_e_nodes = zip(dir_s_nodes, dir_e_nodes)
            if len(s_e_nodes) != len(set(s_e_nodes)):
                self.log.error('Repeat Add link.')
                return False
            return True

    def _get_all_ic_add_info_link(self):
        sqlcmd = """
        SELECT add_link_id, s_node, e_node, one_way_code
          FROM (
            SELECT distinct add_link_id
              FROM mid_hwy_node_add_info_merged
              WHERE path_type = 'IC'
          ) AS a
          LEFT JOIN link_tbl
          ON add_link_id = link_id
          ORDER BY add_link_id;
        """
        return self.get_batch_data(sqlcmd)

    def _backup_facility_num_failed_add_info(self):
        '''备分facility_num出错的记录。'''
        self.CreateTable2('highway_node_add_info_bak')
        sqlcmd = """
        INSERT INTO highway_node_add_info_bak(
                      gid, link_id, node_id, toll_flag,
                       no_toll_money, facility_num, up_down, facility_id,
                       seq_num, dir_s_node, dir_e_node, etc_antenna,
                       enter, exit, jct, sa,
                       pa, gate, un_open, dummy,
                       toll_type_num, non_ticket_gate, check_gate, single_gate,
                       cal_gate, ticket_gate, nest, uturn,
                       not_guide, normal_toll, etc_toll, etc_section,
                       name, tile_id, no_toll_flag, link_lid)
        (
        SELECT gid, link_id, node_id, toll_flag,
               no_toll_money, facility_num, up_down, facility_id,
               seq_num, dir_s_node, dir_e_node, etc_antenna,
               enter, exit, jct, sa,
               pa, gate, un_open, dummy,
               toll_type_num, non_ticket_gate, check_gate, single_gate,
               cal_gate, ticket_gate, nest, uturn,
               not_guide, normal_toll, etc_toll, etc_section,
               name, tile_id, no_toll_flag, link_lid
          FROM highway_node_add_info
          WHERE link_id in (
                SELECT distinct a.link_id
                  FROM (
                    SELECT distinct link_id, facility_num
                      FROM highway_node_add_info
                  ) AS a
                  LEFT JOIN (
                    SELECT distinct link_id, COUNT(link_id) as num
                      FROM highway_node_add_info
                      group by link_id
                  ) AS b
                  on a.link_id = b.link_id
                  where facility_num <> num
          )
        )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_facility_num(self):
        '''更新设施番号数目和seq_num'''
        self.log.info('Update facility num of add info.')
        sqlcmd = """
        SELECT array_agg(gid) as gids
          FROM (
            SELECT gid, a.link_id, num
              FROM highway_node_add_info AS a
              inner JOIN (
                SELECT distinct a.link_id, num
                  FROM (
                    SELECT distinct link_id, facility_num
                      FROM highway_node_add_info
                  ) AS a
                  LEFT JOIN (
                    SELECT distinct link_id, COUNT(link_id) as num
                      FROM highway_node_add_info
                      group by link_id
                  ) AS b
                  on a.link_id = b.link_id
                  where facility_num <> num
              ) AS c
              ON a.link_id = c.link_id
              order by link_id, gid
          ) as d
          group by link_id
        """
        sqlcmd2 = """
        UPDATE highway_node_add_info SET facility_num = %s, seq_num = %s
          WHERE gid = %s;
        """
        for (gids,) in self.get_batch_data(sqlcmd):
            seq_num = 1
            facility_num = len(gids)
            for gid in gids:
                self.pg.execute2(sqlcmd2, (facility_num, seq_num, gid))
                seq_num += 1
        self.pg.commit2()


# =============================================================================
# AddInfoData
# =============================================================================
class AddInfoDataRDF():
    def __init__(self, link_id, node_id,
                 toll_flag=HWY_FALSE, no_toll_money=HWY_FALSE,
                 facility_num=HWY_FALSE, up_down=HWY_FALSE,
                 facility_id=HWY_FALSE, facil_name='',
                 seq_num=HWY_FALSE, toll_type_num=HWY_FALSE,
                 toll_type=None, tile_id=None,
                 dir_s_node=None, dir_e_node=None,
                 no_toll_flag=HWY_FALSE, link_lid=[]):
        '''
        Constructor
        '''
        self.link_id = link_id
        self.node_id = node_id
        self.toll_flag = toll_flag
        self.no_toll_money = no_toll_money  # 一般道料金才会设置成“1”
        self.facility_num = facility_num  # 高速施設の数
        self.up_down = up_down
        self.facility_id = facility_id
        self.seq_num = seq_num
        self.toll_type_num = toll_type_num  # 料金所種別の数, 当前最多只会是1
        self.dir_s_node = dir_s_node
        self.dir_e_node = dir_e_node
        if toll_type:
            self.toll_type = toll_type
        else:
            self.toll_type = HwyTollType()
        self.facil_name = facil_name
        self.tile_id = convert_tile_id(tile_id)
        self.no_toll_flag = no_toll_flag
        self.link_lid = link_lid

    def __eq__(self, obj):
        if(self.link_id == obj.link_id and
           self.node_id == obj.node_id and
           self.toll_flag == obj.toll_flag and
           self.no_toll_money == obj.no_toll_money and  # 一般道料金才会设置成“1”
           self.facility_num == obj.facility_num and  # 高速施設の数
           self.up_down == obj.up_down and
           self.facility_id == obj.facility_id and
           self.seq_num == obj.seq_num and
           self.toll_type_num == obj.toll_type_num and
           self.dir_s_node == obj.dir_s_node and
           self.dir_e_node == obj.dir_e_node and
           self.toll_type == obj.toll_type and
           self.facil_name == obj.facil_name and
           self.tile_id == obj.tile_id and
           self.no_toll_flag == obj.no_toll_flag and
           self.link_lid == obj.link_lid):
            return True
        else:
            return False
