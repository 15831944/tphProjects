# -*- coding: UTF-8 -*-
'''
Created on 2015年9月22日

@author: hcz
'''
from common import cache_file
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_RAMP
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.jdb.hwy.hwy_ic_info import ADD_NODE_POS_TYPE_BWD
from component.jdb.hwy.hwy_ic_info import ADD_NODE_POS_TYPE_EQUAL
from component.jdb.hwy.hwy_node_addinfo import HwyTollType
from component.jdb.hwy.hwy_node_addinfo import AddInfoData
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
        self.file_obj = cache_file.open('temp_highway_node_add_info')
        # 暂时不做收费站附加情报
        # self._make_toll_node_add_info()  # 料金
        self._make_ics_node_add_info()  # IC等，除料金外
        self._store_add_info()
        # 一条设施路径上，只能一个附加情报(非收费站)
        self._filter_repeat_add_info()
        # 添加坐标信息(the_geom)——给QGis显示用
        self._add_the_geom()

    def _make_toll_node_add_info(self):
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

    def _filter_repeat_add_info(self):
        '''一条设施路径上，只能一个附加情报(非收费站),
           if 有多个附加， 且前面附加情报的设施号>后面的加情报的设施号，删除前面。
           if 有多个附加， 且前面附加情报的设施号<=后面的加情报的设施号，删除前面
        '''
        for (link_lids, node_lids,
             add_links, add_nodes) in self._get_multi_add_info_facil():
            link_lid_list = [map(int, n_lid.split(','))
                             for n_lid in link_lids]
            node_lid_list = [map(int, n_lid.split(','))
                             for n_lid in node_lids]
            add_link_set = set(add_links)
            for link_lid in link_lid_list:
                inter_links = add_link_set.intersection(link_lid)
                if len(inter_links) <= 1:
                    continue
                else:
                    self.log.warning('Mulit Add Info. link_lid=%s', link_lid)
#                 all_add_infos = []
#                 for link_id in link_lid:
#                     if link_id in add_link_set:
#                         add_infos = self.data_mng.get_add_info(link_id)
#                         all_add_infos.append(add_infos)

    def _get_multi_add_info_facil(self):
        '''取得有多个附加情报的设施'''
        sqlcmd = """
        SELECT link_lids, node_lids,
               add_links, add_nodes
        FROM (
            SELECT road_code, road_seq,
                   facilcls_c, inout_c, node_id,
                   array_agg(add_link) AS add_links,
                   array_agg(add_node) AS add_nodes
             FROM (
                SELECT distinct b.road_code, b.road_seq,
                       b.facilcls_c, b.inout_c, b.node_id,
                       a.link_id as add_link,
                       a.node_id as add_node
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
         order by d.road_code, d.road_seq, d.facilcls_c, d.inout_c, d.node_id
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
        self.data_mng.load_add_info_link()
        for (ic_no, updown, facility_id,
             facilcls, in_out, link_lid,
             node_lid, add_node_id, add_link_id,
             dir_s_node, dir_e_node, facil_name,
             pos_type, pos_type_name, toll_flag
             ) in self._get_temp_add_info_data():
            link_list = map(int, link_lid.split(','))
            node_list = map(int, node_lid.split(','))
            last_add_idx = -1
            curr_add_idx = len(link_list)
            link_idx = len(link_list) - 1
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
                add_link_id = link_list[last_add_idx]
                add_node_id = node_list[last_add_idx]
                if in_out == HWY_INOUT_TYPE_IN:
                    dir_s_node = node_list[last_add_idx+1]
                    dir_e_node = node_list[last_add_idx]
                else:
                    dir_s_node = node_list[last_add_idx]
                    dir_e_node = node_list[last_add_idx+1]
            if not facil_name:
                facil_name = ''
            params = (ic_no, updown, facility_id,
                      facilcls, in_out, link_lid,
                      node_lid, add_node_id, add_link_id,
                      dir_s_node, dir_e_node, facil_name,
                      pos_type, pos_type_name, toll_flag
                      )
            self._insert_merged_add_info(file_obj, params)
        self._store_merged_add_info(file_obj)

    def _make_ics_node_add_info(self):
        """设施附加情报"""
        for info in self._get_add_info_data():
            add_link_id, tile_id, add_node_ids, inout_list = info[0:4]
            if len(set(add_node_ids)) > 1:
                self.log.warning('Number of add node > 1. link_id=%s'
                                 % add_link_id)
            add_node_id = add_node_ids[0]
            # updowns, facility_ids, facilcls, faicl_names= info[4:8]
            # link_lid_array, node_lid_array = info[8:10]
            add_info_dict = {}
            add_info_list = zip(*info[4:12])
            # ## 按inout分类
            for i in range(0, len(inout_list)):
                inout = inout_list[i]
                if inout in add_info_dict:
                    add_info_dict[inout].append(add_info_list[i])
                else:
                    add_info_dict[inout] = [add_info_list[i]]
            inout_set = set(inout_list)
            facility_num = len(inout_set)
            seq_num = 1
            for inout, add_info_list in add_info_dict.items():
                (updown_list, facility_ids, facilcls_list, facil_names,
                 link_lids, node_lids,
                 dir_s_nodes, dir_e_nodes) = zip(*add_info_list)
                dir_nodes = zip(dir_s_nodes, dir_e_nodes)
                link_lids = [map(int, n_lid.split(','))
                             for n_lid in link_lids]
                node_lids = [map(int, n_lid.split(','))
                             for n_lid in node_lids]
                # 取最小设施号
                min_facil_id = min(facility_ids)
                up_down = self._merge_updwon(updown_list, facility_ids,
                                             min_facil_id, facilcls_list)
                # 上下行合并
                dir_s_node, dir_e_node = self._get_dir_node(add_link_id,
                                                            dir_nodes)
                toll_type = HwyTollType()
                facility_idx = facility_ids.index(min_facil_id)
                toll_type.set_facil_type(facilcls_list[facility_idx],
                                         inout)
                # 判断设施名称唯一
                if len(set(facil_names)) == 1:
                    facil_name = facil_names[0]
                else:
                    facil_name = self._get_facil_name(facil_names,
                                                      facility_ids,
                                                      min_facil_id)
                toll_flag = HWY_FALSE
                no_toll_money = HWY_FALSE
                toll_type_num = 0
                add_info = AddInfoDataRDF(add_link_id, add_node_id,
                                          toll_flag, no_toll_money,
                                          facility_num, up_down,
                                          min_facil_id, facil_name,
                                          seq_num, toll_type_num,
                                          toll_type, tile_id,
                                          dir_s_node, dir_e_node)
                seq_num += 1
                self._insert_add_info(add_info)
        self.pg.commit2()

    def _get_dir_node(self, add_link_id, dir_nodes):
        dir_nodes_set = set(dir_nodes)
        if not dir_nodes_set:
            self.log.error('No Dir Node. link_id=%s'
                           % add_link_id)
            return None, None
        if len(dir_nodes_set) > 1:
            self.log.error('Number of Dir Node > 1. link_id=%s')
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
        SELECT ic_no, updown, facility_id,
               facilcls, in_out, link_lid,
               node_lid, add_node_id, add_link_id,
               dir_s_node, dir_e_node, name_kanji,
               pos_type, pos_type_name, toll_flag
          FROM mid_hwy_node_add_info
          order by ic_no, add_link_id
        """
        return self.get_batch_data(sqlcmd)

    def _get_add_info_data(self):
        '''料金情报，不包含料金所'''
        sqlcmd = """
        SELECT add_link_id,
               tile_id,
               array_agg(add_node_id) as add_node_ids,
               array_agg(in_out) in_outs,
               array_agg(updown) updowns,
               array_agg(facility_id) facility_ids,
               array_agg(facilcls) facilcls,
               array_agg(facil_name) facil_names,
               array_agg(link_lid) as link_lid_array,
               array_agg(node_lid) as node_lid_array,
               array_agg(dir_s_node) as dir_s_nodes,
               array_agg(dir_e_node) as dir_e_nodes
          FROM mid_hwy_node_add_info_merged AS a
          LEFT JOIN link_tbl as b
          ON add_link_id = link_id
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
            params = (add_info.link_id, add_info.node_id,
                      add_info.toll_flag, add_info.no_toll_money,
                      add_info.facility_num, add_info.up_down,
                      add_info.facility_id, add_info.seq_num,
                      add_info.dir_s_node, add_info.dir_e_node,
                      toll_type.etc_antenna, toll_type.enter,
                      toll_type.exit, toll_type.jct,
                      toll_type.sa_pa, toll_type.gate,
                      toll_type.unopen, toll_type.dummy_facil,
                      add_info.toll_type_num, toll_type.non_ticket_gate,
                      toll_type.check_gate, toll_type.single_gate,
                      toll_type.cal_gate, toll_type.ticket_gate,
                      toll_type.nest, toll_type.uturn,
                      toll_type.not_guide, toll_type.normal_toll,
                      toll_type.etc_toll, toll_type.etc_section,
                      facil_name, add_info.tile_id,
                      add_info.no_toll_flag)
            self.file_obj.write('%d\t%d\t%d\t%d\t'
                                '%d\t%d\t%d\t%d\t'
                                '%d\t%d\t%d\t%d\t'
                                '%d\t%d\t%d\t%d\t'
                                '%d\t%d\t%d\t%d\t'
                                '%d\t%d\t%d\t%d\t'
                                '%d\t%d\t%d\t%d\t'
                                '%d\t%d\t%s\t%d\t'
                                '%d\n'
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
                           '%d\t%s\t%d\n'
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
                                        'toll_flag'
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
                                        'sapa', 'gate',
                                        'un_open', 'dummy',
                                        'toll_type_num', 'non_ticket_gate',
                                        'check_gate', 'single_gate',
                                        'cal_gate', 'ticket_gate',
                                        'nest', 'uturn',
                                        'not_guide', 'normal_toll',
                                        'etc_toll', 'etc_section',
                                        'name', 'tile_id',
                                        'no_toll_flag'])
            self.pg.commit2()
            cache_file.close(self.file_obj)
            self.file_obj = None
        self.CreateIndex2('highway_node_add_info_link_id_idx')
        self.CreateIndex2('highway_node_add_info_node_id_link_id_idx')


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
                 no_toll_flag=HWY_FALSE):
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
