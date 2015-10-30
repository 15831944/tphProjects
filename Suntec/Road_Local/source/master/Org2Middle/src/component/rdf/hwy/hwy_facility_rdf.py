# -*- coding: UTF-8 -*-
'''
Created on 2015-2-12

@author: hcz
'''
import json
import common
import component.component_base
from component.rdf.hwy.hwy_graph_rdf import is_cycle_path
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_SAPA
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_UTURN
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SERVICE_ROAD
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
from component.rdf.hwy.hwy_poi_category_rdf import HwyPoiCategory
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_RESTAURANT
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_REST_AREA
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_SHOPPING_CORNER
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_GAS_STATION
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_POST_BOX
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_INFO
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_TOILET
from component.rdf.hwy.hwy_poi_category_rdf import HWY_SR_ATM

ROAD_SEQ_MARGIN = 10
SAPA_TYPE_DICT = {1: HWY_IC_TYPE_SA,  # COMPLETE REST AREA
                  2: HWY_IC_TYPE_PA,  # PARKING AND REST ROOM ONLY
                  3: HWY_IC_TYPE_PA,  # PARKING ONLY
                  4: HWY_IC_TYPE_SA,  # MOTORWAY SERVICE AREA
                  5: None             # SCENIC OVERLOOK
                  }
HWY_PATH_TYPE_IC = 'IC'
HWY_PATH_TYPE_SAPA = 'SAPA'
HWY_PATH_TYPE_JCT = 'JCT'
HWY_PATH_TYPE_UTURN = 'UTURN'
HWY_PATH_TYPE_ICS = 'ICS'  # IC等: JCT/SAPA/UTURN
IC_PATH_TYPE_DICT = {HWY_IC_TYPE_IC: HWY_PATH_TYPE_IC,
                     HWY_IC_TYPE_SA: HWY_PATH_TYPE_SAPA,
                     HWY_IC_TYPE_PA: HWY_PATH_TYPE_SAPA,
                     HWY_IC_TYPE_JCT: HWY_PATH_TYPE_JCT,
                     HWY_IC_TYPE_UTURN: HWY_PATH_TYPE_UTURN,
                     HWY_IC_TYPE_TOLL: '',
                     }


# ===============================================================================
# HwyFacilityRDF
# ===============================================================================
class HwyFacilityRDF(component.component_base.comp_base):
    '''生成设施情报
    '''

    def __init__(self, data_mng, ItemName='HwyFacilityRDF'):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, ItemName)
        self.hwy_data = data_mng
        self.G = None
        if self.hwy_data:
            self.G = self.hwy_data.get_graph()
        self.org_facil_dict = {}  # (org_facil_id, facil_cls): road_seq
        self.poi_cate = HwyPoiCategory()
        self.restaurant_dict, self.gas_station_dict = {}, {}
        self.rest_area_dict, self.shopping_corner_dict = {}, {}
        self.post_box_dict, self.info_dict = {}, {}
        self.toilet_dict, self.atm_dict = {}, {}
        self.undefined_dict = {}

    def _Do(self):
        self.poi_cate.Make()
        self.pg.connect2()
        self._make_ic_path()
        self._expand_ics_path_node()  # 把设施路径点展开
        self._make_inout_join_node()
        # 过滤假JCT/UTurn:下了高速又转弯回来的径路
        self._filter_JCT_UTurn()
        # 过滤SAPA: 其实只是JCT
        self._filter_sapa()
        # 过滤双向SAPA: SAPA Link是双向的，SAPA出口同时也是入口，SAPA的入口同时也是出口
        # 把SAPA出口处的SAPA入口删除，同理把SAPA入口处的SAPA出口删除
        self._filter_both_dir_sapa()
        # 过滤辅路、类辅路， 从辅路路径选出SAPA路径(路径两头都有SAPA设施)
        self._filter_service_road()
        # 过滤掉高速和一般直接相连的出入口设施
        self._filter_ic()
        # SAPA对应的Rest Area POI情报
        self._make_sapa_first_node()
        self._make_sapa_info()
        self._make_sapa_link()
        self._deal_with_uturn()  # 处理U-Turn
        # 148.11217,-35.00432
        # self._del_sapa_node()
        self._make_facil_name()  # 设施名称
        self._make_hwy_node()
        self._make_facil_same_info()  # 并设情报
        self._load_poi_category()
        self._make_hwy_store()  # 店铺情报
        self._make_hwy_service()  # 服务情报
        # self._make_facil_postion()
        self._expand_ics_path_link()  # 把设施路径link展开

    def _make_ic_path(self):
        self.log.info('Start make IC Path.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_ic_path')
        self.CreateTable2('mid_temp_hwy_service_road_path1')
        temp_file_obj = common.cache_file.open('mid_temp_hwy_ic_path')
        for route in self.hwy_data.get_road_code_path():
            road_code, updown, route_path = route
            self.org_facil_dict = {}  # 清空
            next_seq = ROAD_SEQ_MARGIN
            end_pos = len(route_path)
            if is_cycle_path(route_path):
                if not self.adjust_path_for_cycle(route_path):
                    self.log.error('No indicate start node for Cycle Route.'
                                   'road_code=%s' % road_code)
                    continue
                else:
                    self._store_moved_path(road_code, route_path)
                end_pos = len(route_path) - 1
            path = route_path[:end_pos]
            # 取得线路上所有设施
            all_facils = self._get_facils_for_path(road_code, path)
            facils_list, sapa_node_dict, service_road_list = all_facils
            # ## 设置设施序号
            self._make_road_seq(road_code, updown, facils_list,
                                sapa_node_dict, next_seq, temp_file_obj)
            # 辅路、类辅路设施
            for sr_facil_info in service_road_list:
                inout_c, path = sr_facil_info[1], sr_facil_info[2]
                # 保存到辅路到数库
                self._store_service_road_facil_path(inout_c, path,
                                                    road_code, updown)
        # ## 辅路提交到数据库
        self.pg.commit1()
        # ## 保存设施路径到数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj,
                           'mid_temp_hwy_ic_path',
                           columns=('road_code', 'road_seq', 'facilcls_c',
                                    'inout_c', 'updown_c', 'node_id',
                                    'to_node_id', 'node_lid', 'link_lid',
                                    'path_type', 'facil_name'),
                           )
        self.pg.commit2()
        common.cache_file.close(temp_file_obj, True)
        # 创建索引
        self.CreateIndex2('mid_temp_hwy_ic_path_path_type_idx')
        self.CreateIndex2('mid_temp_hwy_ic_path_road_code_'
                          'road_seq_node_id_inout_c_fac_idx')
        self.log.info('End make IC Path.')

    def _make_road_seq(self, road_code, updown, facils_list,
                       sapa_node_dict, next_seq, temp_file_obj):
        sapa_seq_dict = {}  # SAPA node: road_seq
        for node, all_facils in facils_list:
            road_seq_dict, next_seq = self._get_road_seq(node,
                                                         all_facils,
                                                         next_seq,
                                                         sapa_node_dict,
                                                         sapa_seq_dict,
                                                         )
            for facilcls, inout_c, path in all_facils:
                road_seq = road_seq_dict.get((facilcls, inout_c))
                if not road_seq:
                    self.log.error('No Road Seq.')
                    continue
                if not path:
                    self.log.error('No Path. node=%s' % node)
                    continue
                path_type = IC_PATH_TYPE_DICT.get(facilcls)
                self._store_facil_path_2_file(road_code, road_seq,
                                              facilcls, inout_c,
                                              updown, path,
                                              path_type, temp_file_obj)

    def _get_facils_for_path(self, road_code, route_path):
        '''取得线路上所有设施'''
        sapa_node_dict = {}  # SaPa出口:SaPa入口 or SaPa入口:SaPa出口
        facils_list = []
        service_road_list = []
        # ## 取得设施情报
        for node_idx in range(0, len(route_path)):
            node = route_path[node_idx]
            all_facils = self.G.get_all_facil(node, road_code,
                                              HWY_ROAD_CODE)
            if not all_facils:
                if node_idx == 0:  # 断头高速, 强行加个IC入口
                    all_facils = []
                    facilcls = HWY_IC_TYPE_IC
                    inout = HWY_INOUT_TYPE_IN
                    path = [node]
                    all_facils.append((facilcls, inout, path))
                else:
                    continue
            temp_facils = []
            for facil_info in all_facils:
                facilcls, path = facil_info[0], facil_info[2]
                if node != path[0]:
                    self.log.error('Error Path for node=%s' % node)
                    continue
                # 辅路、类辅路/U-Turn，不要折返(掉头)
                if facilcls in (HWY_IC_TYPE_SERVICE_ROAD,
                                HWY_IC_TYPE_UTURN,
                                HWY_IC_TYPE_JCT):
                    inout = facil_info[1]
                    reverse = False
                    if inout == HWY_INOUT_TYPE_IN:
                        reverse = True
                    # 所有link都是inner link
                    if self.G.all_is_inner_link(path, reverse):
                        continue
                    # 有一般道Inner Link
                    if self.G.exit_nwy_inner_link(path, reverse):
                        # print path
                        continue
                    # 某一条link折返(掉头)
                    if self.check_turn_back(path):
                        if facilcls in (HWY_IC_TYPE_SERVICE_ROAD,):
                            self.log.info('Turn Back--SR Road. %s' % path)
                        elif facilcls in (HWY_IC_TYPE_JCT,):
                            self.log.info('Turn Back--JCT. %s' % path)
                        continue
                if facilcls == HWY_IC_TYPE_SERVICE_ROAD:  # 辅路、类辅路设施
                    service_road_list.append(facil_info)
                elif facilcls == HWY_IC_TYPE_IC:
                    inout = facil_info[1]
                    pathes = self.get_paths_2_inout(facilcls, path, inout)
                    for path in pathes:
                        temp_facil_info = (facilcls, inout, path)
                        temp_facils.append(temp_facil_info)
                else:
                    temp_facils.append(facil_info)
                    if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                        node = path[0]
                        to_node = path[-1]
                        if to_node in route_path:  # 同一线路
                            if node not in sapa_node_dict:
                                sapa_node_dict[node] = set([to_node])
                            else:
                                sapa_node_dict[node].add(to_node)
                        else:
                            # print to_node
                            pass
            if temp_facils:
                facils_list.append((node, temp_facils))
        return facils_list, sapa_node_dict, service_road_list

    def _get_sapa_from_jct_uturn(self, all_facils, road_code):
        sapa_facil_list = []
        jct_uturn_list = []
        for facil_info in all_facils:
            facilcls, path = facil_info[0], facil_info[2]
            if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                return []
            if facilcls in (HWY_IC_TYPE_UTURN, HWY_IC_TYPE_JCT):
                jct_uturn_list.append(facil_info)
        for facil_info in jct_uturn_list:
            facilcls, inout, path = facil_info[0], facil_info[2]
            if inout == HWY_INOUT_TYPE_IN:
                reverse = True
            else:
                reverse = False
            if self.G.is_sapa_path(path, road_code,
                                   HWY_ROAD_CODE, reverse):
                sapa_facil = (HWY_IC_TYPE_PA, inout, path)
                sapa_facil_list.append(sapa_facil)
        return sapa_facil_list

    def get_paths_2_inout(self, facilcls, path, inout):
        '''第一个和一般道连接的点之后，还有Ramp(中国使用)'''
        return [path]

    def _get_road_seq(self, node, all_facils, road_seq,
                      sapa_node_dict, sapa_seq_dict):
        org_facil_id = self.G.get_org_facil_id(node)
        sorted_facils = self._sort_facils(all_facils)
        road_seq_dict = {}
        next_road_seq = road_seq
        visited = []
        for facil_idx in range(0, len(sorted_facils)):
            facilcls, inout = sorted_facils[facil_idx]
            if(facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA) and
               node in sapa_node_dict):
                sapa_road_seq = self._get_sapa_road_seq(node,
                                                        facilcls,
                                                        sapa_node_dict,
                                                        sapa_seq_dict,
                                                        visited)
                if sapa_road_seq:
                    road_seq_dict[facilcls, inout] = sapa_road_seq
                    sapa_seq_dict[node] = sapa_road_seq
                else:
                    road_seq_dict[facilcls, inout] = next_road_seq
                    sapa_seq_dict[node] = next_road_seq
                    next_road_seq = next_road_seq + ROAD_SEQ_MARGIN
            else:
                temp_seq = self._get_road_seq_by_org_facil_id(org_facil_id,
                                                              facilcls,
                                                              inout)
                if temp_seq:
                    road_seq_dict[facilcls, inout] = temp_seq
                else:
                    road_seq_dict[facilcls, inout] = next_road_seq
                    self._store_road_seq(org_facil_id, facilcls,
                                         next_road_seq, inout)
                    next_road_seq = next_road_seq + ROAD_SEQ_MARGIN
        return road_seq_dict, next_road_seq

    def _get_road_seq_by_org_facil_id(self, org_facil_id, facilcls, inout):
        if not org_facil_id:
            return None
        # IC出口的方面名称或Next Street可能不同，所以才用不同的road_seq
        if facilcls == HWY_IC_TYPE_IC and inout == HWY_INOUT_TYPE_OUT:
            return None
        key = (org_facil_id, facilcls)
        road_seq = self.org_facil_dict.get((key))
        return road_seq

    def _store_road_seq(self, org_facil_id, facilcls, road_seq, inout):
        if not org_facil_id:
            return
        if (org_facil_id, facilcls) in self.org_facil_dict:
            temp_seq = self.org_facil_dict.get((org_facil_id, facilcls))
            if(temp_seq != road_seq and
               not (facilcls == HWY_IC_TYPE_IC and inout == HWY_INOUT_TYPE_OUT)
               ):
                self.log.error('RoadSeq is changed. org_facil_id=%s,'
                               'facilcls=%s' % (org_facil_id, facilcls))
        else:
            self.org_facil_dict[(org_facil_id, facilcls)] = road_seq

    def _get_sapa_road_seq(self, node, facilcls,
                           sapa_node_dict, sapa_seq_dict, visited):
        # self.log.info('SAPA node %s' % node)
        if facilcls not in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
            return None
        if node in sapa_seq_dict:
            return sapa_seq_dict.get(node)
        visited.append(node)
        # 取关联SAPA Node的序号
        if node not in sapa_node_dict:
            self.log.warning('Not in sapa_node_dict. node=%s' % node)
            return None
        to_nodes = list(sapa_node_dict.get(node))
        for to_node in to_nodes:
            if to_node in visited:
                continue
            to_sapa_seq = self._get_sapa_road_seq(to_node,
                                                  facilcls,
                                                  sapa_node_dict,
                                                  sapa_seq_dict,
                                                  visited)
            if to_sapa_seq:
                return to_sapa_seq
        return None

    def _sort_facils(self, all_facils):
        '''出口: 按IC, JCT, SAPA; 入口: SAPA, JCT, IC.'''
        toll_facil_types = set()
        in_facil_types = set()
        out_facil_types = set()
        sorted_facils = []
        for facil_path_info in all_facils:
            facilcls, inout_c = facil_path_info[0], facil_path_info[1]
            if inout_c == HWY_INOUT_TYPE_OUT:
                out_facil_types.add((facilcls, inout_c))
            elif inout_c == HWY_INOUT_TYPE_IN:
                in_facil_types.add((facilcls, inout_c))
            else:
                toll_facil_types.add((facilcls, inout_c))
        # 入口按种别升序排
        temp_list = list(in_facil_types)
        temp_list.sort(cmp=lambda x, y: cmp(x[0], y[0]), reverse=False)
        sorted_facils += temp_list
        # 无出入口种别
        sorted_facils += list(toll_facil_types)
        # 出口设施按种别倒序排
        temp_list = list(out_facil_types)
        temp_list.sort(cmp=lambda x, y: cmp(x[0], y[0]), reverse=True)
        sorted_facils += temp_list
        return sorted_facils

    def _make_hwy_node(self):
        self.log.info('Start Make Highway Node')
        self.CreateTable2('hwy_node')
        sqlcmd = """
        INSERT INTO hwy_node(road_code, road_seq, updown_c,
                             facilcls_c, inout_c, tollcls_c,
                             node_id, facil_name, the_geom)
        (
        SELECT distinct a.road_code, a.road_seq, a.updown_c,
               facilcls_c, inout_c, 0 as tollcls_c, -- None
               a.node_id, b.facil_name, the_geom
          FROM mid_temp_hwy_ic_path as a
          LEFT JOIN mid_temp_hwy_facil_name as b
          ON a.road_code = b.road_code and a.road_seq = b.road_seq
          LEFT JOIN node_tbl as c
          ON a.node_id = c.node_id
          WHERE facilcls_c <> 10 -- Not U-turn
          ORDER BY road_code, road_seq
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('hwy_node_the_geom_idx')
        self.CreateIndex2('hwy_node_road_code_road_seq'
                          '_facilcls_c_inout_c_node_id_idx')
        self.log.info('End Make Highway Node')

    def adjust_path_for_cycle(self,  path):
        route_start_end = self.hwy_data.get_cycle_route_start_end()
        for start_end in route_start_end:
            s = start_end[0]
            if s in path:
                # 调速顺序
                self._move_start_facil_to_first(s, path)
                return True
        return False

    def _move_start_facil_to_first(self, s, path):
        s_idx = path.index(s)
        path.pop(-1)
        node_idx = 0
        while node_idx < s_idx:
            node = path.pop(0)
            path.append(node)
            node_idx += 1
        path.append(s)

    def _store_moved_path(self, road_code, path):
        sqlcmd = """
        INSERT INTO hwy_link_road_code_info (road_code, node_id,
                                             link_id, seq_nm,
                                             the_geom)
        (
        SELECT a.road_code, a.node_id, a.link_id, a.seq_nm, the_geom
         FROM (
            SELECT %s as road_code, %s as node_id,
                   cast(%s as bigint) as link_id,  %s AS seq_nm
         ) AS a
         LEFT JOIN link_tbl
         ON a.link_id = link_tbl.link_id
        );
        """
        self._delete_old_cycle_path(road_code)
        linkids = self.G.get_linkids(path)
        linkids.insert(0, None)
        seq_nm = 0
        for node_id, link_id in zip(path, linkids):
            self.pg.execute1(sqlcmd, (road_code, node_id,
                                      link_id, seq_nm)
                             )
            seq_nm += 1
        self.pg.commit1()

    def _delete_old_cycle_path(self, road_code):
        sqlcmd = """
        delete from hwy_link_road_code_info
          where road_code = %s;
        """
        self.pg.connect1()
        self.pg.execute1(sqlcmd, (road_code,))
        self.pg.commit1()

    def _insert_into_first_node(self, parm):
        sqlcmd = '''
        INSERT INTO temp_hwy_sapa_first_node(road_code, road_seq,
                                             updown_c, node_id)
        VALUES(%s, %s, %s, %s)
        '''
        self.pg.execute2(sqlcmd, parm)

    def _make_sapa_first_node(self):
        '''road_code,road_seq,node,seq_nm '''
        self.log.info('Start make sapa first node mapping')
        self.CreateTable2('temp_hwy_sapa_first_node')
        sqlcmd = '''
        select distinct a.road_code, a.road_seq, updown_c,
              array_agg(a.node_id) as node, array_agg(seq_nm)as seq
        from mid_temp_hwy_ic_path as a
        left join hwy_link_road_code_info as b
        on a.road_code = b.road_code and
           a.updown_c = b.updown and
           a.node_id = b.node_id
        where facilcls_c in (1, 2) --and
              --inout_c = 2   --inout:  1 入口&合流; 2 出口&分歧
        group by a.road_code, a.road_seq, updown_c
        order by a.road_code, a.road_seq, updown_c
        '''
        for row in self.get_batch_data(sqlcmd):
            road_code = row[0]
            road_seq = row[1]
            updown_c = row[2]
            list_node = row[3]
            list_seq = row[4]
            min_node_id = None
            min_seq_nm = None
            for (node_id, seq_nm) in zip(list_node, list_seq):
                if ((min_seq_nm is None) or
                    (seq_nm < min_seq_nm)):
                    min_node_id = node_id
                    min_seq_nm = seq_nm
                else:
                    continue
            self._insert_into_first_node((road_code, road_seq,
                                          updown_c, min_node_id))
        self.pg.commit2()
        self.log.info('End make sapa first node mapping')

    def _get_temp_path(self):
        sqlcmd = '''
        select road_code, updown,
               array_agg(node_id)as path
        from (
             select road_code, updown, node_id, link_id, seq_nm
              from hwy_link_road_code_info
              order by road_code, updown, seq_nm
        )as a
        group by road_code, updown
        order by road_code, updown
        '''
        path_dict = dict()
        for row in self.pg.get_batch_data2(sqlcmd):
            (road_code, updown, path) = row
            if not road_code:
                continue
            if (road_code, updown) not in path_dict.keys():
                path_dict[(road_code, updown)] = path
        return path_dict

    def _get_first_node_to_node(self, temp_path, first_node, node):
        distance = 0
        first_node_index = temp_path.index(first_node)
        node_index = temp_path.index(node)
        node_path = temp_path[first_node_index:node_index + 1]
        for temp_index in xrange(0, len(node_path) - 1):
            u = node_path[temp_index]
            v = node_path[temp_index + 1]
            distance += self.G.get_length(u, v)
        return distance

    def  _get_sapa_link(self):
        sqlcmd = '''
        SELECT DISTINCT b.road_code, b.road_seq, b.updown_c,
               b.node_id, b.first_node_id, c.poi_id,
               array_agg(b.link_id), b.link_lid, b.node_lid
          FROM (
            SELECT a.road_code, a.road_seq, a.updown_c, link_lid, node_lid,
                   a.node_id as node_id, first_node.node_id as first_node_id,
                   regexp_split_to_table(link_lid, E'\\,+')::bigint as link_id
              FROM mid_temp_hwy_ic_path as a
              LEFT JOIN temp_hwy_sapa_first_node as first_node
                ON a.road_code = first_node.road_code and
                   a.road_seq = first_node.road_seq and
                   a.updown_c = first_node.updown_c
              where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
                    link_lid <> '' and
                    link_lid is not null and
                    inout_c = 2
          ) as b
          LEFT JOIN mid_temp_poi_link as c
          ON b.link_id = c.link_id
          --LEFT JOIN rdf_poi_rest_area as d
         -- ON c.poi_id = d.poi_id
          --where d.poi_id is not null
          where not exists(select * from rdf_poi_rest_area as d
                           where d.poi_id = c.poi_id )
          GROUP BY b.road_code, b.road_seq, b.updown_c,
                   b.node_id, b.first_node_id, c.poi_id,
                   b.link_lid, b.node_lid
          ORDER BY road_code, road_seq
        '''
        return self.get_batch_data(sqlcmd, batch_size=1024)

    def _make_sapa_link(self):
        '''SAPA中poi绑定link情报'''
        self.log.info('Start Make SAPA Link.')
        self.CreateTable2('mid_temp_hwy_sapa_link')
        self.pg.connect1()
#         sqlcmd = '''
#         SELECT DISTINCT b.road_code, b.road_seq, b.updown_c,
#                b.node_id, b.first_node_id, c.poi_id,
#                array_agg(b.link_id), b.link_lid, b.node_lid
#           FROM (
#             SELECT a.road_code, a.road_seq, a.updown_c, link_lid, node_lid,
#                    a.node_id as node_id, first_node.node_id as first_node_id,
#                    regexp_split_to_table(link_lid, E'\\,+')::bigint as link_id
#               FROM mid_temp_hwy_ic_path as a
#               LEFT JOIN temp_hwy_sapa_first_node as first_node
#                 ON a.road_code = first_node.road_code and
#                    a.road_seq = first_node.road_seq and
#                    a.updown_c = first_node.updown_c
#               where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
#                     link_lid <> '' and
#                     link_lid is not null and
#                     inout_c = 2
#           ) as b
#           LEFT JOIN mid_temp_poi_link as c
#           ON b.link_id = c.link_id
#           GROUP BY b.road_code, b.road_seq, b.updown_c,
#                    b.node_id, b.first_node_id, c.poi_id,
#                    b.link_lid, b.node_lid
#           ORDER BY road_code, road_seq
#         '''
        temp_path_dict = self._get_temp_path()
#         for data in self.pg.get_batch_data2(sqlcmd):
        for data in self._get_sapa_link():
            (road_code, road_seq, updown, node, first_node,
             poi_id, link_ids, link_lid, node_lid) = data
            if poi_id is None:
                continue
            distance = 0
            node_list = map(int, node_lid.split(','))
            link_list = map(int, link_lid.split(','))
            if node != first_node:
                self.log.warning('no first node. road_code= %s, road_seq = %s'
                                 % (road_code, road_seq))
                # 取first_node到node的path,算出这段距离
                temp_path = temp_path_dict[(road_code, updown)]
                distance = self._get_first_node_to_node(temp_path,
                                                        first_node,
                                                        node)
            else:
                distance = 0.0
            for index in xrange(0, len(link_list)):
                u = node_list[index]
                v = node_list[index + 1]
                if link_list[index] not in link_ids:
                    distance += self.G.get_length(u, v)
                else:
                    break
            self._store_sapa_link(road_code, road_seq, updown,
                                  link_ids, link_lid, poi_id, distance)
        # 处理双向SAPA
        self.pg.commit1()
        self._update_sapa_link()
        self.log.info('End Make SAPA Link.')

    def _update_sapa_link(self):
        sqlcmd = '''
        select distinct c.to_road_code, c.updown_c, c.to_road_seq,
               sapa_link.link_ids, sapa_link.link_lid,
               sapa_link.poi_id, sapa_link.distance
        from (  -- sapa跨road_code，出/入口点不在同一road_code。
            select b.road_code as to_road_code, b.updown_c,
                   b.road_seq as to_road_seq, b.node_id as to_node_id,
                   a.road_code, a.road_seq
            from (
                  select distinct road_code, updown_c, road_seq, to_node_id
                  from mid_temp_hwy_ic_path
                  where facilcls_c in (1, 2) and
                    inout_c = 2  --2:出口/分歧
                )as a
            left join(
                  select distinct road_code, updown_c, road_seq, node_id
                  from mid_temp_hwy_ic_path
                  where facilcls_c in (1, 2) and
                    inout_c = 1  --1:入口/合流
                )as b
            on  a.to_node_id = b.node_id
            where a.road_code <> b.road_code
            order by b.road_code, b.updown_c, b.road_seq
        )as c
        --过滤一个点属于两个road_code的情况
        left join temp_hwy_sapa_first_node as first_node
        on c.to_road_code = first_node.road_code and
           c.updown_c = first_node.updown_c and
           c.to_road_seq = first_node.road_seq
        --copy  SAPA出口sapa_link信息
        left join mid_temp_hwy_sapa_link as sapa_link
        on c.road_code = sapa_link.road_code and
           c.road_seq = sapa_link.road_seq
        where to_node_id = first_node.node_id
        order by c.to_road_code, c.updown_c, c.to_road_seq,
                 poi_id, distance
        '''
        for row in self.pg.get_batch_data2(sqlcmd):
            (to_road_code, updown_c, to_road_seq, link_ids,
             link_lid, poi_id, distance) = row
            self._store_sapa_link(to_road_code, to_road_seq, updown_c,
                                  link_ids, link_lid, poi_id, distance)
        self.pg.commit1()

    def _make_sapa_info(self):
        '''SAPA对应的Rest Area POI情报。'''
        self.log.info('Start Make SAPA Info.')
        self.CreateTable2('mid_temp_hwy_sapa_info')
        self.pg.connect1()
        prev_road_code, prev_road_seq = None, None
        for data in self._get_rest_area_info():
            road_code, road_seq, poi_id, rest_area_type, name, updown = data
            # SAPA有些路径经过SAPA POI, 有些没有经过时, 过滤掉没有经过SAPA POI的情
            if(prev_road_code == road_code and
               prev_road_seq == road_seq and
               not poi_id):
                continue
            prev_road_code, prev_road_seq = road_code, road_seq
            facil_cls = self._get_sapa_type(rest_area_type)
            if not facil_cls:
                self.log.error('Unknown Rest Area type. poi_id=%s,'
                               'rest_area_type=%s' % (poi_id, rest_area_type))
                continue
            self._store_sapa_info(road_code, road_seq,
                                  facil_cls, poi_id, name,
                                  updown)
        self.pg.commit1()
        self._update_sapa_facilcls()
        self.CreateIndex2('mid_temp_hwy_sapa_info_road_code'
                          '_road_seq_updown_c_idx')
        self.log.info('End Make SAPA Info.')

    def _get_sapa_type(self, rest_area_type):
        facil_cls = None
        if rest_area_type:
            facil_cls = SAPA_TYPE_DICT.get(rest_area_type)
        else:
            facil_cls = HWY_IC_TYPE_PA
        return facil_cls

    def _deal_with_uturn(self):
        '''处理U-turn
        1. 有其他设施，且两边是配对的，那么U-turn情报做到其他设施里
        2. 只有U-turn设施，U-turn做成JCT设施
        3. 一边有其他设施，一边没有其他设施，U-turn做JCT
        4. 两边都有其他设施，但是两边设施类型不一样，U-turn做成JCT
        '''
        self.log.info('Deal with U-Turn.')
        self.pg.connect1()
        for (u_roadcode, u_roadseq, u_inout,
             path, f_facils, t_facils,
             updown) in self._get_uturn_fb_sames():
            # ## 取得起点并设和终点并设中设施种别相同的设施
            f_same_facils = []
            jct_facils = []
            for f_facil in f_facils:
                for t_facil in t_facils:
                    if f_facil.facilcls == t_facil.facilcls:
                        f_same_facils.append(f_facil)
                if f_facil.facilcls == HWY_IC_TYPE_JCT:
                    jct_facils.append(f_facil)
            path_type = HWY_PATH_TYPE_UTURN
            if f_same_facils:  # 有是配对的
                for f in f_same_facils:
                    self._store_facil_path(f.road_code, f.road_point,
                                           f.facilcls, f.inout,
                                           path, path_type,
                                           updown)
            else:  # U-turn做成JCT
                if jct_facils:
                    if len(jct_facils) > 1:
                        self.log.warning('JCT facil > 1. node=%s' % path[0])
                    jct = jct_facils[0]
                    self._store_facil_path(jct.road_code, jct.road_point,
                                           jct.facilcls, jct.inout,
                                           path, path_type,
                                           updown)
                else:
                    self._store_facil_path(u_roadcode, u_roadseq,
                                           HWY_IC_TYPE_JCT, u_inout,
                                           path, path_type,
                                           updown)
        self.pg.commit1()

    def check_turn_back(self, path):
        '''某一条link折返(掉头)'''
        for node, third_node in zip(path[0:-2], path[2:]):
            if node == third_node:
                return True
        return False

    def _update_sapa_facilcls(self):
        # Update mid_temp_hwy_ic_path表的SAPA facicl_class code
        sqlcmd = """
        UPDATE mid_temp_hwy_ic_path as a set facilcls_c = b.facilcls_c
          from mid_temp_hwy_sapa_info as b
          where a.road_code = b.road_code and
                a.road_seq = b.road_seq and
                a.facilcls_c <> b.facilcls_c;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _get_rest_area_info(self):
        '''取得服'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq, c.poi_id,
                        rest_area_type, name, updown_c
          FROM (
            SELECT road_code, road_seq, facilcls_c, updown_c,
                   regexp_split_to_table(link_lid, E'\\,+')::bigint as link_id
              FROM mid_temp_hwy_ic_path as a
              where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
                    link_lid <> '' and link_lid is not null
          ) as b
          LEFT JOIN mid_temp_poi_link as c
          ON b.link_id = c.link_id
          LEFT JOIN mid_temp_hwy_sapa_name as d
          ON c.poi_id = d.poi_id
          LEFT JOIN rdf_poi_rest_area as e
          ON c.poi_id = e.poi_id
          ORDER BY road_code, road_seq, c.poi_id
        """
        return self.get_batch_data(sqlcmd)

    def _del_sapa_node(self):
        '''删除SAPA出口和入口（同个设施）内部的SAPA设施点'''
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE (road_code, road_seq, facilcls_c, node_id) in (
            -- Get Delete SAPA Node: between SAPA OUT and SAPA IN
            SELECT road_code, road_seq, facilcls_c,
                   unnest(del_nodes) as del_sapa_nod
              FROM (
                SELECT road_code, road_seq, facilcls_c,
                       node_lid[2:array_upper(node_lid, 1) -1] as del_nodes
                  FROM (
                    SELECT road_code, road_seq,
                           facilcls_c, array_agg(node_id) as node_lid
                      FROM (
                        SELECT distinct ic.road_code, ic.road_seq,
                             facilcls_c, ic.node_id, seq_nm
                          FROM mid_temp_hwy_ic_path as ic
                          LEFT JOIN hwy_link_road_code_info as road
                          ON ic.road_code = road.road_code and
                             ic.node_id = road.node_id
                          where facilcls_c in (1, 2)
                          ORDER BY seq_nm
                      ) AS a
                      group by road_code, road_seq, facilcls_c
                  ) AS A
                  where array_upper(node_lid, 1) > 2
              ) AS b
         );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_facil_name(self):
        '''制作设施名称'''
        self.log.info('Start Make Facil Name.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_facil_name')
        sqlcmd = """
        SELECT road_code, road_seq, facilcls_c, array_agg(node_id)
          FROM (
            SELECT distinct road_code, road_seq,
                   facilcls_c, inout_c, node_id
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c <> 10  -- Not U-turn
              ORDER BY road_code, road_seq,
                       facilcls_c, inout_c
          ) AS a
          group by road_code, road_seq, facilcls_c
          order by road_code, road_seq
        """
        for data in self.pg.get_batch_data2(sqlcmd):
            road_code, road_seq, facilcls, node_id_list = data[:]
            facil_name = None
            # SAPA
            if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                facil_name = self._get_sapa_facil_name(road_code, road_seq)
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
            elif facilcls in (HWY_IC_TYPE_TOLL,):
                facil_name = self.G.get_node_name(node_id_list[0])
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
            else:  # JCT/IC
                facil_name = self._get_exit_name(node_id_list)
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
        self.pg.commit1()
        self.CreateIndex2('mid_temp_hwy_facil_name_road_code_road_seq_idx')
        self.log.info('End Make Facility Name.')

    def _get_exit_name(self, node_list):
        name_set = set()
        for node in node_list:
            facil_name = self.G.get_exit_poi_name(node)  # HWY Exit POI
            if facil_name:
                name_set.add(facil_name)
                continue
            facil_name = self.G.get_exit_name(node)  # 出口番号/名称
            if facil_name:
                name_set.add(facil_name)
                continue
            else:
                facil_name = self.G.get_junction_name(node)  # 交叉点名称
                if facil_name:
                    name_set.add(facil_name)
                    continue
            name_set.add(None)
        if len(name_set) > 1:
            self.log.error('Number of facil_name > 1. node_list=%s'
                           % node_list)
            return None
        return name_set.pop()

    def _get_sapa_facil_name(self, road_code, road_seq):
        sapa_name = self.hwy_data.get_sapa_name(road_code, road_seq)
        return sapa_name

    def _make_facil_same_info(self):
        '''并列设施情报'''
        self.CreateTable2('hwy_same_info')
        self.pg.connect1()
        for facil_list in self._get_facil_list():
            facil_idx = 0
            while facil_idx < len(facil_list):
                # print (facil_idx, facil_list[facil_idx].road_code,
                #       facil_list[facil_idx].node_id)
                facil_info = facil_list[facil_idx]
                same_facils = [facil_info]
                same_facils += self._get_same_facils(facil_list,
                                                     facil_idx,
                                                     facil_info)
                facil_idx += len(same_facils)
                if len(same_facils) == 2:
                    # 同个设施出口和入口
                    if(same_facils[0].road_point == same_facils[1].road_point
                       and same_facils[0].inout == HWY_INOUT_TYPE_OUT
                       and same_facils[1].inout == HWY_INOUT_TYPE_IN):
                        continue
                if len(same_facils) > 1:
                    # 取得父设施(首点等级最高的)
                    p_facil = self._get_parent_facil(same_facils)
                    if not p_facil:
                        self.log.error('No Parent Facil.road_code=%s,node=%s'
                                       % (facil_info.road_code,
                                          facil_info.node_id))
                    self._store_same_info(same_facils, p_facil)
        self.pg.commit1()
        self.CreateIndex2('hwy_same_info_road_code_'
                          'road_seq_updown_c_node_id_idx')
        self.CreateIndex2('hwy_same_info_road_code_'
                          'p_road_seq_updown_c_p_node_id_idx')

    def _get_same_facils(self, facil_list, facil_idx, curr_facil):
        same_facils = []
        # 取对应的入口设施
        in_idx = self._get_facil_in_idx(facil_list, facil_idx)
        if in_idx:  # 出口到入口之间的所有设施都是并设
            same_facils = facil_list[(facil_idx + 1):(in_idx + 1)]
        else:
            # 同点的并设
            same_facils = self._get_same_node_facil(facil_list,
                                                    facil_idx,
                                                    curr_facil)
        if in_idx:
            in_facil_info = facil_list[in_idx]
            same_facils += self._get_same_node_facil(facil_list,
                                                     in_idx,
                                                     in_facil_info)
        org_same_facils = self._get_org_same_facil(facil_list,
                                                   facil_idx,
                                                   curr_facil)
        for facil in org_same_facils:
            if facil not in same_facils:
                same_facils.append(facil)
        # ## 求并设的并设
        for same_idx in range(0, len(same_facils)):
            same_facil = same_facils[same_idx]
            same_facil_idx = facil_idx + same_idx + 1
            c_same_facils = self._get_same_facils(facil_list,
                                                  same_facil_idx,
                                                  same_facil)
            for facil in c_same_facils:
                if facil not in same_facils:
                    same_facils.append(facil)
        return same_facils

    def _get_parent_facil(self, same_facils):
        if not same_facils:
            return None
        parent_facil = same_facils[0]
        for facil in same_facils[1:]:
            # 1:SA, 2:PA, 3:JCT, 5: IC, 6: 收费站
            if parent_facil.node_id != facil.node_id:
                break
            if parent_facil.facilcls > facil.facilcls:
                parent_facil = facil
            elif parent_facil.facilcls == facil.facilcls:
                # 有名称的设施优先
                if not parent_facil.facil_name and facil.facil_name:
                    parent_facil = facil
                    continue
                # 出口设施优先
                if(parent_facil.inout != HWY_INOUT_TYPE_OUT and
                   facil.inout == HWY_INOUT_TYPE_OUT):
                    parent_facil = facil
            else:
                pass
        return parent_facil

    def _get_same_node_facil(self, facil_list, facil_idx, curr_facil):
        same_node_facils = []
        facil_idx += 1
        while facil_idx < len(facil_list):
            facil_info = facil_list[facil_idx]
            if curr_facil.node_id == facil_info.node_id:
                same_node_facils.append(facil_info)
            else:
                break
            facil_idx += 1
        return same_node_facils

    def _get_org_same_facil(self, facil_list, facil_idx, curr_facil):
        '''元数据番号相同(POI_ID、IS_ID)的并设'''
        MAX_INTERVAL_IDX = 10
        org_same_facils = []
        org_facil_id = self.G.get_org_facil_id(curr_facil.node_id)
        if not org_facil_id:
            return []
        facil_idx += 1
        for temp_facil_idx in range(facil_idx + 1, len(facil_list)):
            next_facil = facil_list[temp_facil_idx]
            next_node_id = next_facil.node_id
            next_org_facil_id = self.G.get_org_facil_id(next_node_id)
            if org_facil_id == next_org_facil_id:
                interval_idx = temp_facil_idx - facil_idx
                if interval_idx > 3:
                    node_id = curr_facil.node_id
                    t_node_id = facil_list[temp_facil_idx].node_id
                    self.log.warning('Interval index = %s, node=%s, t_node=%s'
                                     % (interval_idx, node_id, t_node_id))
                if interval_idx > MAX_INTERVAL_IDX:  # 27.96229,-26.26458
                    break
                org_same_facils.append(next_facil)
        return org_same_facils

    def _get_facil_in_idx(self, facil_list, facil_idx):
        '''入口设施'''
        facil_info = facil_list[facil_idx]
        if facil_info.inout != HWY_INOUT_TYPE_OUT:
            return None
        facil_idx += 1
        for facil_idx in range(facil_idx, len(facil_list)):
            next_facil = facil_list[facil_idx]
            if(next_facil.road_code == facil_info.road_code and
               next_facil.road_point == facil_info.road_point):
                if next_facil.inout == facil_info.inout:
                    break
                if(next_facil.facilcls == facil_info.facilcls and
                   next_facil.inout != HWY_INOUT_TYPE_OUT):
                    return facil_idx
        return None

    def _get_facil_list(self):
        sqlcmd = """
        SELECT road_code, updown_c,
               array_agg(road_seq) as road_seqs,
               array_agg(facilcls_c) as facilcls_cs,
               array_agg(inout_c) as inout_cs,
               array_agg(node_id) as node_lid,
               array_agg(facil_name) as facil_names
          FROM (
            SELECT a.road_code, a.road_seq, a.updown_c,
                   facilcls_c, inout_c, a.node_id, facil_name
              FROM hwy_node as a
              LEFT JOIN hwy_link_road_code_info as b
              ON a.road_code = b.road_code and
                 a.node_id = b.node_id and
                 a.updown_c = b.updown
              ORDER BY a.road_code, seq_nm, road_seq
          ) as c
          GROUP BY road_code, updown_c
          ORDER BY road_code;
        """
        data = self.get_batch_data(sqlcmd)
        for (roadcode, updown, road_seqs, facilcls_cs,
             inouts, node_lid, facil_names) in data:
            facil_list = []
            for(road_seq, facilcls, inout, node,
                facil_name) in zip(road_seqs, facilcls_cs, inouts, node_lid,
                                   facil_names):
                facil_info = HwyFacilInfo(roadcode, road_seq, facilcls,
                                          updown, node, inout,
                                          facil_name)
                facil_list.append(facil_info)
            if node_lid[0] == node_lid[-1]:  # 环
                # 去掉最后一个点(和第一个点重复)的设施
                facil_info = facil_list[-1]
                while facil_info.node_id == node_lid[0]:
                    facil_list.pop()
                    facil_info = facil_list[-1]
            yield facil_list

    def _load_poi_category(self):
        (self.restaurant_dict, self.gas_station_dict,
         self.rest_area_dict, self.shopping_corner_dict,
         self.post_box_dict, self.info_dict,
         self.toilet_dict, self.atm_dict,
         self.undefined_dict) = self.poi_cate.get_all_service_dict()
        self._update_undefined_dict()

    def _update_undefined_dict(self):
        self.undefined_dict.update(SERVICE_UNDEFINED_DICT)
        return 0

    def _get_service_kind(self, cat_id):
        if self.poi_cate:
            if cat_id in self.restaurant_dict:
                service_kind = HWY_SR_RESTAURANT
            elif cat_id in self.gas_station_dict:
                service_kind = HWY_SR_GAS_STATION
            elif cat_id in self.rest_area_dict:
                service_kind = HWY_SR_REST_AREA
            elif cat_id in self.shopping_corner_dict:
                service_kind = HWY_SR_SHOPPING_CORNER
            elif cat_id in self.post_box_dict:
                service_kind = HWY_SR_POST_BOX
            elif cat_id in self.info_dict:
                service_kind = HWY_SR_INFO
            elif cat_id in self.toilet_dict:
                service_kind = HWY_SR_TOILET
            elif cat_id in self.atm_dict:
                service_kind = HWY_SR_ATM
            else:
                service_kind = None
        return service_kind

    def _insert_into_store_info(self, parm):
        sqlcmd = '''
        INSERT INTO hwy_store(road_code, road_seq, updown_c,
                              store_cat_id, store_chain_id,
                              priority, service_kind)
        VALUES(%s, %s, %s, %s,
               %s, %s, %s)
        '''
        self.pg.execute1(sqlcmd, parm)

    def _cat_id_map(self, category_id):
        cat_id = int(category_id)
        return cat_id

    def _make_hwy_store(self):
        '''店铺情报'''
        self.log.info('Start Make Facility Stores.')
        self.CreateTable2('hwy_store')
        sqlcmd = '''
         select distinct road_code, road_seq, updown_c,
                cat_id, chain_id,  array_agg(distance)
         from mid_temp_hwy_sapa_link as a
         left join mid_temp_sapa_store_info as b
         on a.poi_id = b.child_poi_id
         where b.chain_id is not null and b.chain_id <> ''
         group by road_code, updown_c, road_seq,
                  cat_id, chain_id
        '''
        self.pg.connect1()
        for row in self.pg.get_batch_data2(sqlcmd):
            (road_code, road_seq, updown_c,
             cat_id, chain_id, distance) = row
            cat_id = self._cat_id_map(cat_id)
            service_kind = self._get_service_kind(cat_id)
            distance = min(distance)
            self._insert_into_store_info((road_code, road_seq,
                                          updown_c, cat_id,
                                          chain_id, distance, service_kind))
        self.pg.commit1()
        self.log.info('End Make Facility Stores.')

    def _make_hwy_service(self):
        '''服务情报'''
        self.log.info('Start Make Facility Service.')
        self.CreateTable2('hwy_service')
        sqlcmd = """
        SELECT road_code, road_seq, updown_c, ARRAY_AGG(cat_id)
          FROM (
            SELECT distinct road_code, road_seq, updown_c, cat_id
              FROM mid_temp_hwy_sapa_info as a
              LEFT JOIN mid_temp_sapa_store_info as b
              ON a.poi_id = b.poi_id
              WHERE cat_id is not null
              ORDER BY road_code, road_seq, updown_c, cat_id
          ) AS c
          GROUP BY road_code, road_seq, updown_c
          ORDER BY road_code, road_seq, updown_c
        """
        self.pg.connect1()
        for service_info in self.get_batch_data(sqlcmd):
            road_code, road_seq, updown, cat_id_list = service_info
            service_types = self._get_service_types(cat_id_list)
            # 服务标志都为HWY_FALSE时，不收录
            if set(service_types) == set([HWY_FALSE]):
                continue
            self._store_service_info(road_code, road_seq,
                                     updown, service_types)
        self.pg.commit1()
        self.log.info('End Make Facility Service.')

    def _get_service_types(self, cat_id_list):
        gas_station, information, rest_area = HWY_FALSE, HWY_FALSE, HWY_FALSE
        shopping_corner, postbox, atm = HWY_FALSE, HWY_FALSE, HWY_FALSE
        restaurant, toilet = HWY_FALSE, HWY_FALSE
        if self.poi_cate:
            for cat_id in cat_id_list:
                if int(cat_id) in self.gas_station_dict:
                    gas_station = HWY_TRUE
                elif int(cat_id) in self.info_dict:
                    information = HWY_TRUE
                elif int(cat_id) in self.rest_area_dict:
                    rest_area = HWY_TRUE
                elif int(cat_id) in self.shopping_corner_dict:
                    shopping_corner = HWY_TRUE
                elif int(cat_id) in self.post_box_dict:
                    postbox = HWY_TRUE
                elif int(cat_id) in self.atm_dict:
                    atm = HWY_TRUE
                elif int(cat_id) in self.restaurant_dict:
                    restaurant = HWY_TRUE
                elif int(cat_id) in self.toilet_dict:
                    toilet = HWY_TRUE
                elif int(cat_id) in self.undefined_dict:
                    continue
                else:
                    self.log.error('Unknown Category ID. cat_id=%s' % cat_id)
        return (gas_station, information, rest_area,
                shopping_corner, postbox, atm,
                restaurant, toilet)

    def _make_facil_postion(self):
        self.CreateTable2('hwy_facil_position')
        sqlcmd = """
        select addgeometrycolumn('hwy_facil_position', 'the_geom',
                                 4326, 'POINT', 2);
        select addgeometrycolumn('hwy_facil_position', 'link_geom',
                                 4326, 'LINESTRING', 2);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
        INSERT INTO hwy_facil_position(road_code, road_seq, updown_c,
                                       facilcls_c, link_id, node_id,
                                       the_geom, link_geom)
        (
        SELECT road_code, road_seq, updown_c,
               facilcls_c, b.link_id,
               (case when s_length < e_length then s_node
                else e_node end ) as node_id,
               (case when s_length < e_length then ST_StartPoint(the_geom)
                else ST_EndPoint(the_geom) end ) as node_geom,
               the_geom as link_geom
          FROM mid_temp_hwy_sapa_info as a
          INNER JOIN mid_temp_poi_closest_link as b
          ON a.poi_id = b.poi_id
          LEFT JOIN link_tbl
          ON b.link_id = link_tbl.link_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _expand_ics_path_node(self):
        self.log.info('Expand ICs path nodes.')
        self.CreateTable2('mid_temp_hwy_ic_path_expand_node')
        sqlcmd = """
        insert into mid_temp_hwy_ic_path_expand_node(
                               road_code, road_seq, facilcls_c, inout_c,
                               node_id, pass_node_id, path_type
                               )
        (
        SELECT distinct road_code, road_seq, facilcls_c, inout_c,
               node_id,
               regexp_split_to_table(node_lid, ',')::bigint as pass_node_id,
               path_type
          FROM mid_temp_hwy_ic_path
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_ic_path_expand_node_pass_link_id_idx')

    def _expand_ics_path_link(self):
        '''把设施路径link展开'''
        self.log.info('Expand ICs path links.')
        self.CreateTable2('mid_temp_hwy_ic_path_expand_link')
        sqlcmd = """
        insert into mid_temp_hwy_ic_path_expand_link(
                               road_code, road_seq, facilcls_c, inout_c,
                               node_id, pass_link_id, path_type
                               )
        (
        SELECT distinct road_code, road_seq, facilcls_c, inout_c,
               node_id,
               regexp_split_to_table(link_lid, ',')::bigint as pass_link_id,
               path_type
          FROM mid_temp_hwy_ic_path
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_temp_hwy_ic_path_expand_link_pass_link_id_idx')

    def _get_uturn_fb_sames(self):
        '''取得Uturn的起点和终点的并设'''
        sqlcmd = """
        SELECT road_code, road_seq, inout_c,
               uturn.node_id, to_node_id, node_lid,
               f_road_codes, f_road_seqs, f_facilcls_cs,
               t_road_codes, t_road_seqs, t_facilcls_cs,
               t_inout_c, updown_c
          FROM mid_temp_hwy_ic_path as uturn
          LEFT JOIN (
            SELECT array_agg(road_code) as f_road_codes,
                   array_agg(road_seq) as f_road_seqs,
                   array_agg(facilcls_c) as f_facilcls_cs,
                   inout_c as f_inout_c,
                   node_id
              FROM (
                SELECT DISTINCT road_code, road_seq, facilcls_c,
                       inout_c, node_id
                  FROM mid_temp_hwy_ic_path
                  WHERE facilcls_c <> 10   -- Not U-turn
              ) AS b
              GROUP BY node_id, f_inout_c
          ) as c
          ON uturn.node_id = c.node_id and uturn.inout_c = c.f_inout_c
          LEFT JOIN (
            SELECT array_agg(road_code) as t_road_codes,
                   array_agg(road_seq) as t_road_seqs,
                   array_agg(facilcls_c) as t_facilcls_cs,
                   inout_c as t_inout_c,
                   node_id
              FROM (
                SELECT DISTINCT road_code, road_seq, facilcls_c,
                       inout_c, node_id
                  FROM mid_temp_hwy_ic_path
                  WHERE facilcls_c <> 10  -- Not U-turn
              ) AS b
              GROUP BY node_id, inout_c
          ) as d
          ON uturn.to_node_id = d.node_id and
             ((uturn.inout_c = 2 and d.t_inout_c = 1) or
              (uturn.inout_c = 1 and d.t_inout_c = 2))
          where uturn.facilcls_c = 10   -- U-turn
          ORDER BY gid
        """
        updown = HWY_UPDOWN_TYPE_UP
        facil_name = ''
        for uturn in self.get_batch_data(sqlcmd):
            u_roadcode, u_roadseq, u_inout = uturn[0:3]
            u_node, u_to_node, node_lid = uturn[3:6]
            f_roadcodes, f_roadseqs, f_facilclss = uturn[6:9]
            t_roadcodes, t_roadseqs, t_facilclss = uturn[9:12]
            t_inout = uturn[12]
            updown = uturn[13]
            f_facils = []
            t_facils = []
            # 终点的并设
            if f_roadcodes and f_roadseqs and f_facilclss:
                for roadcode, roadseq, facilcls in zip(f_roadcodes, f_roadseqs,
                                                       f_facilclss):
                    facil_info = HwyFacilInfo(roadcode, roadseq, facilcls,
                                              updown, u_node, u_inout,
                                              facil_name)
                    f_facils.append(facil_info)
            # 终点的并设
            if t_roadcodes and t_roadseqs and t_facilclss:
                for roadcode, roadseq, facilcls in zip(t_roadcodes, t_roadseqs,
                                                       t_facilclss):
                    facil_info = HwyFacilInfo(roadcode, roadseq, facilcls,
                                              updown, u_to_node, t_inout,
                                              facil_name)
                    t_facils.append(facil_info)
            path = [int(n) for n in node_lid.split(',')]
            yield (u_roadcode, u_roadseq, u_inout,
                   path, f_facils, t_facils,
                   updown)

    def _make_inout_join_node(self):
        '''出入口的合流点'''
        pass

    def _filter_JCT_UTurn(self):
        '''过滤假JCT/UTurn:下了高速又转弯回来的径路'''
        pass

    def _get_join_nodes(self, overlay_nodes, f_node_lids,
                        t_node_lids, f_inout):
        '''出入口路径的汇合点，像 Y/X字路的汇合点。'''
        join_nodes = set()
        if len(overlay_nodes) <= 1:
            join_nodes = set(overlay_nodes)
            return join_nodes
        for f_node_lid in f_node_lids:
            # 取当前路径的重叠点
            curr_overlay_nodes = overlay_nodes.intersection(f_node_lid)
            # 路径里没有重叠点
            if not curr_overlay_nodes:
                continue
            has_overlay = False
            overlay_sec = []
            node_idx = len(f_node_lid) - 1
            # ## 取最尾部那段重叠路径的第一个点
            while node_idx > 0:
                v = f_node_lid[node_idx]
                u = f_node_lid[node_idx - 1]
                if v in curr_overlay_nodes:
                    overlay_sec.append(v)
                    has_overlay = True
#                     if(node_idx - 1 == 0 and
#                        u in curr_overlay_nodes):
#                         join_nodes.add(u)
                else:
                    if has_overlay:  # 重叠点必须连续
                        break
                if f_inout == HWY_INOUT_TYPE_OUT:
                    data = self.G[u][v]
                else:
                    data = self.G[v][u]
                # SAPA link
                link_type = data.get(HWY_LINK_TYPE)
                if link_type == HWY_LINK_TYPE_SAPA:
                    break
                # Roundabout
                node_idx -= 1
            # 求最大重合段
            max_overlay = self._max_overlay_node(overlay_nodes,
                                                 overlay_sec,
                                                 t_node_lids)
            join_nodes = join_nodes.union(max_overlay)
        return join_nodes

    def _max_overlay_node(self, overlay_nodes, overlay_sec, t_node_lids):
        max_overlay = set()
        if not overlay_sec:
            return max_overlay
        for node_lid in t_node_lids:
            node_cnt = len(node_lid) - 1
            while node_cnt >= 0:
                node = node_lid[node_cnt]
                if node in overlay_nodes:
                    if node == overlay_sec[0]:
                        max_overlay.add(node)
                        i = 1
                        while i < len(overlay_sec) and node_cnt - i >= 0:
                            node = node_lid[node_cnt - i]
                            if overlay_sec[i] == node:
                                max_overlay.add(node)
                            else:
                                break
                            i += 1
                    break
                node_cnt -= 1
        return max_overlay

    def _get_jct_uturn(self):
        sqlcmd = """
        SELECT gid, path_type,
               a.road_code, a.inout_c,
               a.node_lid, a.link_lid,
               f_node_lids, f_link_lids,
               t_node_lids, t_link_lids
          FROM (
            -- get JCT/UTurn
            SELECT gid, road_code, road_seq, facilcls_c,
                   inout_c, node_id, to_node_id,
                   node_lid, link_lid, path_type
              from mid_temp_hwy_ic_path
              where path_type in ('JCT', 'UTURN') -- 'JCT', 'UTURN'
          ) as a
          INNER JOIN (
            SELECT node_id, road_code, inout_c,
                   array_agg(node_lid) as f_node_lids,
                   array_agg(link_lid) as f_link_lids
              FROM (
                SELECT DISTINCT node_id, road_code, inout_c,
                       node_lid, link_lid
                  FROM mid_temp_hwy_ic_path
                  where facilcls_c in (5 ) and path_type <> 'UTURN'
                  ORDER BY  node_id, road_code
              ) AS f
              group by node_id, road_code, inout_c
          ) as b
          ON a.node_id = b.node_id and
             a.road_code = b.road_code and
             a.inout_c = b.inout_c
          INNER JOIN (
            SELECT node_id, road_code, inout_c,
                   array_agg(node_lid) as t_node_lids,
                   array_agg(link_lid) as t_link_lids
              FROM (
                SELECT DISTINCT node_id, road_code, inout_c,
                       node_lid, link_lid
                  FROM mid_temp_hwy_ic_path
                  where facilcls_c in (5) and path_type <> 'UTURN'
                  ORDER BY  node_id, road_code
              ) AS t
              group by node_id, road_code, inout_c
          ) as c
          ON a.to_node_id = c.node_id and
             --a.road_code = c.road_code and
             (a.inout_c = 1 and c.inout_c = 2 or
              a.inout_c = 2 and c.inout_c = 1)
          ORDER BY gid;
        """
        return self.get_batch_data(sqlcmd, batch_size=1024)

    def _get_overlay_node(self, f_node_lids, t_node_lids):
        '''重叠点'''
        f_set = set()
        t_set = set()
        for node_lid in f_node_lids:
            # 取收费站之前的点
            s_idx = self._get_toll_index(node_lid)
            if s_idx < 0:
                s_idx = 0
            f_set = f_set.union(node_lid[s_idx:])
        for node_lid in t_node_lids:
            # 取收费站之前的点
            s_idx = self._get_toll_index(node_lid)
            if s_idx < 0:
                s_idx = 0
            t_set = t_set.union(node_lid[s_idx:])
        return f_set.intersection(t_set)

    def _get_toll_index(self, path):
        node_idx = len(path) - 1
        while node_idx >= 0:
            if self.G.is_tollgate(path[node_idx]):
                return node_idx
            node_idx -= 1
        return node_idx

    def _filter_service_road(self):
        '''过滤辅路、类辅路， 从辅路路径选出SAPA路径(路径两头都有SAPA设施)'''
        self.CreateTable2('mid_temp_hwy_service_road_path2')
        # 1. 删除经过 出入口汇合点的 类辅路
        # self._filter_service_road_by_join_node()
        self.pg.connect1()
        for sr_info in self._get_service_road():
            road_code, inout = sr_info[0:2]
            f_road_seqs, f_facilcls_cs = sr_info[2:4]
            t_road_seqs, t_facilcls_cs = sr_info[4:6]
            node_lid, link_lid = sr_info[6:8]
            updown = sr_info[8]
            node_lid = eval(node_lid + ',')
            link_lid = eval(link_lid + ',')
            node_id = node_lid[0]
            to_node_id = node_lid[-1]
            comm_seqs = self._get_comm_road_seq(f_road_seqs, f_facilcls_cs,
                                                t_road_seqs, t_facilcls_cs)
            # 两头都有IC设施
            if HWY_IC_TYPE_IC in comm_seqs:
                # ## 辅路、类辅路其实只是出入口(辅路路径link是出入口路径link的合集子集)
                link_set = set(link_lid)
                # 取得起点，IC路径link集合
                f_link_set = self._get_ic_path_link_set(road_code, node_id,
                                                        inout, f_road_seqs,
                                                        f_facilcls_cs, updown)
                if not f_link_set:
                    self.log.error('No From Link Set. node=%,to_node=%s'
                                   % (node_id, to_node_id))
                # 取得终点，IC路径link集合
                t_inout = HWY_INOUT_TYPE_IN
                if inout == HWY_INOUT_TYPE_IN:
                    t_inout = HWY_INOUT_TYPE_OUT
                t_link_set = self._get_ic_path_link_set(road_code, to_node_id,
                                                        t_inout, t_road_seqs,
                                                        t_facilcls_cs, updown)
                if not t_link_set:
                    self.log.error('No To Link Set. node=%,to_node=%s'
                                   % (node_id, to_node_id))
                # 辅路路径link是出入口路径link的合集子集
                if link_set.issubset(f_link_set.union(t_link_set)):
                    # print link_lid
                    continue
            # 如果两头有SAPA设施，那么该路径就是SAPA路径
            if(HWY_IC_TYPE_SA in comm_seqs or HWY_IC_TYPE_PA in comm_seqs):
                if HWY_IC_TYPE_SA in comm_seqs:
                    f_seq_list, t_seq_list = comm_seqs[HWY_IC_TYPE_SA]
                    facilcls_c = HWY_IC_TYPE_SA
                else:
                    f_seq_list, t_seq_list = comm_seqs[HWY_IC_TYPE_PA]
                    facilcls_c = HWY_IC_TYPE_PA
                if len(f_seq_list) != 1 and len(t_seq_list) != 1:
                    self.log.error('SAPA road seq > 1. node=%s,to_node=%s'
                                   % (node_id, to_node_id))
                elif f_seq_list[0] != t_seq_list[0]:  # 非同一个SAPA设施
                    self.log.warning('SAPA f_road_seq != t_road_seq.'
                                     'node_id=%s, to_node_id=%s'
                                     % (node_id, to_node_id))
                else:
                    path_type = IC_PATH_TYPE_DICT.get(facilcls_c)
                    self._store_facil_path(road_code, f_seq_list[0],
                                           facilcls_c, inout,
                                           node_lid, path_type,
                                           updown)
                    continue
            # Service
            self._store_service_road_facil_path2(inout, node_lid,
                                                 road_code, updown)
        self.pg.commit1()

    def _filter_service_road_by_join_node(self):
        '''删除经过 出入口汇合点的 类辅路。'''
        # 1.先备份
        sqlcmd = """
        INSERT INTO mid_temp_hwy_service_road_path_del(gid, node_lid)
        (
            SELECT distinct gid, node_lid
              FROM mid_temp_hwy_inout_join_node AS a
              INNER JOIN (
               SELECT gid, regexp_split_to_table(node_lid,
                                           ',')::bigint as pass_node_id,
                      node_lid
             FROM mid_temp_hwy_service_road_path1
              ) as b
              ON a.node_id = b.pass_node_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # 再删除
        sqlcmd = """
        DELETE FROM mid_temp_hwy_service_road_path1 as a
          where a.gid in (
              SELECT gid
               from mid_temp_hwy_service_road_path_del as b
          );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _filter_sapa(self):
        '''过滤SAPA: '''
        self.log.info('Start Filter SAPA.')
        self.CreateTable2('mid_temp_hwy_sapa_del')
        self.pg.connect1()
        del_sapa_list = []
        for info in self._get_filter_sapa():
            array_gid, road_code, node_id = info[0:3]
            to_node_id = info[3]
            # node_lids, link_lids = info[4:6]
            inout_c, path_type, updown = info[6:9]
            # 同侧有配对SAPA HWY Node
            if self._match_sapa_node(node_id, road_code, updown, inout_c):
                continue  # 保留SAPA设施
            else:
                # 出口，路的终点
                if(inout_c == HWY_INOUT_TYPE_OUT and
                   self.hwy_data.is_road_end_node(road_code, updown,
                                                  node_id)):
                    if path_type == HWY_PATH_TYPE_UTURN:
                        continue  # 保留SAPA设施
                # 入口，路的起点
                elif(inout_c == HWY_INOUT_TYPE_IN and
                     self.hwy_data.is_road_start_node(road_code, updown,
                                                      node_id)):
                    if path_type == HWY_PATH_TYPE_UTURN:
                        continue  # 保留SAPA设施
                # ## to_node_id同侧是否有配对SAPA HWY Node
                t_sapa_flag = False
                t_inout_c = HWY_INOUT_TYPE_OUT
                if inout_c == HWY_INOUT_TYPE_OUT:
                    t_inout_c = HWY_INOUT_TYPE_IN
                for t_roadinfo in self._get_roadcode_by_node(to_node_id,
                                                             t_inout_c):
                    t_roadcode, t_updwon = t_roadinfo[0:2]
                    if self._match_sapa_node(to_node_id, t_roadcode,
                                             t_updwon, t_inout_c):
                        t_sapa_flag = True
                        break
                # to_node_id同侧有配对SAPA HWY Node
                if t_sapa_flag:
                    del_sapa_list += array_gid
                else:
                    # 两边都没有 配对SAPA HWY Node
                    if path_type == HWY_PATH_TYPE_JCT:
                        self.log.info('No SAPA HWY Node in Both Side.'
                                      'node=%s,to_node=%s' %
                                      (node_id, to_node_id))
        self._store_del_sapa(del_sapa_list)
        self.log.info('End Filter SAPA.')

    def _filter_both_dir_sapa(self):
        '''过滤双向SAPA: SAPA Link是双向的，所以SAPA出口同时也是入口，SAPA的入口同时也是出口
           SAPA出口处的SAPA入口删除，同理把SAPA入口处的SAPA出口也删除
        '''
        self.log.info('Start Filter Both Direction SAPA.')
        self.pg.connect1()
        sqlcmd = """
        SELECT road_code, road_seq,
               array_agg(inout_c) as inout_cs,
               array_agg(node_id) as node_ids,
               array_agg(to_node_id) as to_node_ids
          FROM (
            SELECT distinct a.road_code, a.road_seq, facilcls_c,
                   inout_c, a.node_id, to_node_id, seq_nm
              FROM mid_temp_hwy_ic_path AS a
              LEFT JOIN hwy_link_road_code_info as b
              ON a.road_code = b.road_code and
                 a.updown_c = b.updown and
                 a.node_id = b.node_id
              WHERE facilcls_c in (1, 2) and path_type = 'SAPA'
              ORDER BY road_code, road_seq, seq_nm, inout_c
          ) AS a
          GROUP BY road_code, road_seq
          having count(*) > 2
          ORDER BY road_code, road_seq
        """
        for both_sapa in self.pg.get_batch_data2(sqlcmd):
            road_code, road_seq = both_sapa[0:2]
            inout_cs, node_ids, to_node_ids = both_sapa[2:5]
            sapa_info = zip(inout_cs, node_ids, to_node_ids)
            # 取出第一个点SAPA情报
            first_node = None
            first_inout_set, first_to_node_set = set(), set()
            for inout, node, to_node in sapa_info:
                if not first_node or first_node == node:
                    first_inout_set.add(inout)
                    first_to_node_set.add(to_node)
                    first_node = node
                else:
                    break
            # SAPA出口, 同时又有SAPA入口
            if first_inout_set == set([HWY_INOUT_TYPE_IN,
                                       HWY_INOUT_TYPE_OUT]):
                inout = HWY_INOUT_TYPE_OUT
                for node in first_to_node_set:  # 其他点出，第一个点入
                    self._del_both_dir_sapa(road_code, road_seq,
                                            inout, node, first_node)
                # 删除第一个点入口情报
                inout = HWY_INOUT_TYPE_IN
                self._del_both_dir_sapa(road_code, road_seq,
                                        inout, first_node)
            # 取出最后一个点SAPA情报
            last_node = None
            last_inout_set, last_to_node_set = set(), set()
            for inout, node, to_node in sapa_info[::-1]:
                if not last_node or last_node == node:
                    last_inout_set.add(inout)
                    last_to_node_set.add(to_node)
                    last_node = node
                else:
                    break
            # SAPA入口，同时又有SAPA出口
            if last_inout_set == set([HWY_INOUT_TYPE_IN,
                                      HWY_INOUT_TYPE_OUT]):
                inout = HWY_INOUT_TYPE_IN
                for node in last_inout_set:  # 其他点入，最后一个点出
                    self._del_both_dir_sapa(road_code, road_seq,
                                            inout, node, last_node)
                # 删除最后一个点出口情报
                inout = HWY_INOUT_TYPE_OUT
                self._del_both_dir_sapa(road_code, road_seq,
                                        inout, last_node)
        self.pg.commit1()

    def _match_sapa_node(self, node_id, road_code, updown, inout):
        '''有无同侧(相同的road_code)的配对SAPA HWY Node.'''
        sqlcmd = """
        SELECT DISTINCT a.to_node_id
               --,a.road_code, b.road_seq, a.node_id
          FROM (
            SELECT road_code, road_seq,
                   node_id, to_node_id, updown_c
              FROM mid_temp_hwy_ic_path
              where node_id = %s and
                    road_code = %s and
                    updown_c = %s and
                    path_type = 'SAPA' and
                    facilcls_c in (1, 2)
          ) AS a
          INNER JOIN mid_temp_hwy_ic_path AS b
          ON a.to_node_id = b.node_id and
             a.road_code = b.road_code and
             a.updown_c = b.updown_c;
        """
        self.pg.execute1(sqlcmd, (node_id, road_code, updown))
        row = self.pg.fetchone()
        if row and row[0]:
            return True
        # 本线直接相连(road_code不同)的配对SAPA HWY Node
        conn_flg = self._match_sapa_node_path_conn(node_id, road_code,
                                                   updown, inout)
        return conn_flg

    def _match_sapa_node_path_conn(self, node_id, road_code, updown, inout):
        '''本线直接相连(road_code不同)的配对SAPA HWY Node'''
        # 取得配对SAPA HWY Node(road_code不同--不同线路)
        sqlcmd = """
        SELECT DISTINCT b.road_code, b.updown_c
          FROM (
            SELECT road_code, road_seq,
                   node_id, to_node_id, updown_c
              FROM mid_temp_hwy_ic_path
              where node_id = %s and
                    road_code = %s and
                    updown_c = %s and
                    path_type = 'SAPA' and
                    facilcls_c in (1, 2)
          ) AS a
          INNER JOIN mid_temp_hwy_ic_path AS b
          ON a.to_node_id = b.node_id and
             a.road_code <> b.road_code;
        """
        path_id = self.hwy_data.get_path_id(road_code)
        path = self.hwy_data.get_path_by_pathid(path_id, updown)
        self.pg.execute1(sqlcmd, (node_id, road_code, updown))
        for row in self.pg.fetchall():
            t_road_code, t_updwon = row[0:2]
            t_path_id = self.hwy_data.get_path_id(t_road_code)
            t_path = self.hwy_data.get_path_by_pathid(t_path_id, t_updwon)
            # 本线是头尾相连
            # 注：这里只能使用path_id对应的路径，不能使用road_code对应的路径，
            # 因为后者的路径被截短了
            if inout == HWY_INOUT_TYPE_OUT:
                # 注：当前上下行直接相连
                if path[-1] == t_path[0]:
                    return True
            elif inout == HWY_INOUT_TYPE_IN:
                if path[0] == t_path[-1]:
                    return True
            else:
                self.log.error('Error inout_c.')
                return False
        return False

    def _get_roadcode_by_node(self, node_id, inout_c):
        sqlcmd = """
        SELECT distinct road_code, updown_c
          FROM mid_temp_hwy_ic_path
          where node_id = %s and inout_c = %s and
                facilcls_c in (1, 2)
          ORDER BY road_code, updown_c
        """
        self.pg.execute1(sqlcmd, (node_id, inout_c))
        for row in self.pg.fetchall():
            road_code, updown_c = row[0:2]
            yield road_code, updown_c

    def _get_min_dist_2_sapa_link(self, node_lids, inout_c):
        min_dist1, min_dist2 = -1, -1
        s_reverse = False
        e_reverse = True
        if inout_c == HWY_INOUT_TYPE_IN:  # 入口
            s_reverse = True
            e_reverse = False
        for node_lid in node_lids:
            node_lid = map(int, node_lid.split(','))
            # ## 起点到SAPA link的最小距离
            dist1 = self._get_dist_2_sapa_link(node_lid, s_reverse)
            if min_dist1 > dist1 or min_dist1 == -1:
                min_dist1 = dist1
            # ## 终点到SAPA link的最小距离
            dist2 = self._get_dist_2_sapa_link(node_lid[::-1], e_reverse)
            if min_dist2 > dist2 or min_dist2 == -1:
                min_dist2 = dist2
        return min_dist1, min_dist2

    def _get_dist_2_sapa_link(self, node_lid, reverse=False):
        dist = 0.0
        link_list = zip(node_lid[:-1], node_lid[1:])
        # ## 起点到SAPA link的最小距离
        for u, v in link_list:
            if reverse:  # 逆
                u, v = v, u
            if not self.G.is_sapa_link(u, v):
                dist += self.G.get_length(u, v)
            else:
                break
        return dist

    def _filter_ic(self):
        '''过滤掉高速和一般直接相连的出入口设施。'''
        self.CreateTable2('mid_temp_hwy_ic_del')
        sqlcmd = """
        SELECT gid, road_code, updown_c, inout_c, node_id, node_lid
          FROM mid_temp_hwy_ic_path
          where facilcls_c = 5;
        """
        del_list = []
        for row in self.get_batch_data(sqlcmd):
            gid, road_code, updown, inout, node_id, node_lid = row
            reverse = False
            if inout == HWY_INOUT_TYPE_IN:
                reverse = True
            path = map(int, node_lid.split(','))
            # 没有Ramp/JCT，且没有
            if(not self.G.exist_ic_link(path, reverse) and
               not self.G.exist_hwy_main_link(path, reverse) and
               not self.hwy_data.is_road_start_node(road_code,
                                                    updown,
                                                    node_id) and
               not self.hwy_data.is_road_end_node(road_code,
                                                  updown,
                                                  node_id)):
                del_list.append(gid)
                continue
        self._store_del_ic(del_list)

    def _get_filter_sapa(self):
        sqlcmd = """
        SELECT array_agg(gid) as array_gid, road_code,
               node_id, to_node_id,
               array_agg(node_lid) as node_lids,
               array_agg(link_lid) as link_lids,
               inout_c, path_type,
               updown_c
          FROM (
            SELECT gid, a.road_code,
                   a.node_id, a.to_node_id,
                   a.node_lid, a.link_lid,
                   a.inout_c, b.path_type,
                   a.updown_c
              FROM mid_temp_hwy_ic_path as a
              INNER JOIN (
                SELECT distinct road_code, updown_c,
                       node_id, to_node_id, path_type
                  FROM mid_temp_hwy_ic_path
                  where path_type in ('JCT', 'UTURN')
              ) AS b
              ON a.road_code = b.road_code
                 and a.node_id = b.node_id
                 and a.to_node_id = b.to_node_id
              where a.path_type in ('SAPA')
              ORDER BY gid
          ) AS c
          GROUP BY road_code, node_id, to_node_id,
                   path_type, inout_c, updown_c
          ORDER BY road_code, node_id, to_node_id;
        """
        return self.get_batch_data(sqlcmd)

    def _get_ic_path_link_set(self, roadcode, node, inout,
                              road_seqs, facilcls_cs, updown):
        facil_name = ''
        link_set = set()
        for road_seq, facilcls in zip(road_seqs, facilcls_cs):
            if facilcls != HWY_IC_TYPE_IC:
                continue
            facil = HwyFacilInfo(roadcode, road_seq, facilcls,
                                 updown, node, inout, facil_name)
            path_infos = self.hwy_data.get_ic_path_info(facil)
            for path_info in path_infos:
                path_type = path_info[2]
                if path_type == HWY_PATH_TYPE_IC:
                    link_lid = path_info[1]
                    link_set = link_set.union(set(link_lid))
        return link_set

    def _get_comm_road_seq(self, f_road_seqs, f_facilcls_cs,
                           t_road_seqs, t_facilcls_cs):
        comm_seqs = {}
        if not f_facilcls_cs or not t_facilcls_cs:
            return comm_seqs
        comm_facilcls_list = set(f_facilcls_cs).intersection(set(t_facilcls_cs)
                                                             )
        for comm_facilcls in comm_facilcls_list:
            f_seq_list = []
            for road_seq, facilcls in zip(f_road_seqs, f_facilcls_cs):
                if comm_facilcls == facilcls:
                    f_seq_list.append(road_seq)
            t_seq_list = []
            for road_seq, facilcls in zip(t_road_seqs, t_facilcls_cs):
                if comm_facilcls == facilcls:
                    t_seq_list.append(road_seq)
            comm_seqs[comm_facilcls] = (f_seq_list, t_seq_list)
        return comm_seqs

    def _get_service_road(self):
        sqlcmd = '''
        SELECT a.road_code, a.inout_c,
               f_road_seqs, f_facilcls_cs,
               t_road_seqs, t_facilcls_cs,
               a.node_lid, a.link_lid,
               a.updown_c
          FROM mid_temp_hwy_service_road_path1 as a
          LEFT JOIN (
            SELECT node_id, road_code, inout_c, updown_c,
                   array_agg(road_seq) as f_road_seqs,
                   array_agg(facilcls_c) as f_facilcls_cs
              FROM (
                SELECT DISTINCT node_id, road_code, inout_c,
                       road_seq, facilcls_c, updown_c
                  FROM mid_temp_hwy_ic_path
                  ORDER BY  node_id, road_code, road_seq, facilcls_c
              ) AS f
              where facilcls_c not in (10) -- Not u-turn
              group by node_id, road_code, updown_c, inout_c
          ) as b
          ON a.node_id = b.node_id and
             a.road_code = b.road_code and
             a.updown_c = b.updown_c and
             a.inout_c = b.inout_c
          left join (
            SELECT node_id, road_code, inout_c, updown_c,
                   array_agg(road_seq) as t_road_seqs,
                   array_agg(facilcls_c) as t_facilcls_cs
              FROM (
                SELECT DISTINCT node_id, road_code, inout_c,
                       road_seq, facilcls_c, updown_c
                  FROM mid_temp_hwy_ic_path
                  ORDER BY  node_id, road_code, road_seq, facilcls_c
              ) AS t
              where facilcls_c not in (10) -- Not u-turn
              group by node_id, road_code, updown_c, inout_c
          ) as c
          ON a.to_node_id = c.node_id and
             a.road_code = c.road_code and
             a.updown_c = c.updown_c and
             (a.inout_c = 1 and c.inout_c = 2 or
              a.inout_c = 2 and c.inout_c = 1)
        '''
        return self.get_batch_data(sqlcmd)

# ==============================================================================
#
# ==============================================================================
    def _store_facil_path(self, road_code, road_seq, facilcls_c,
                          inout_c, path, path_type,
                          updwon_c):
        '''保存设施路径'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_path(road_code, road_seq, facilcls_c,
                                         inout_c, node_id, to_node_id,
                                         node_lid, link_lid, path_type,
                                         updown_c)
           VALUES(%s, %s, %s,
                  %s, %s, %s,
                  %s, %s, %s,
                  %s)
        """
        node_id = path[0]
        to_node_id = path[-1]
        node_lid = ','.join([str(node) for node in path])
        if inout_c == HWY_INOUT_TYPE_OUT:
            link_list = self.G.get_linkids(path)
        elif inout_c == HWY_INOUT_TYPE_IN:
            link_list = self.G.get_linkids(path, reverse=True)
        else:
            link_list = []
        link_lid = ','.join([str(link) for link in link_list])
        params = (road_code, road_seq, facilcls_c,
                  inout_c, node_id, to_node_id,
                  node_lid, link_lid, path_type,
                  updwon_c)
        self.pg.execute1(sqlcmd, params)

    def _store_facil_path_2_file(self, road_code, road_seq, facilcls_c,
                                 inout_c, updown_c, path,
                                 path_type, file_obj, facil_name=''):
        '''保存设施路径到文件'''
        node_id = path[0]
        to_node_id = path[-1]
        node_lid = ','.join([str(node) for node in path])
        if inout_c == HWY_INOUT_TYPE_OUT:
            link_list = self.G.get_linkids(path)
        elif inout_c == HWY_INOUT_TYPE_IN:
            link_list = self.G.get_linkids(path, reverse=True)
        else:
            link_list = []
        link_lid = ','.join([str(link) for link in link_list])
        if not link_lid:
            link_lid = ''
        if not facil_name:
            facil_name = ''
        params = (road_code, road_seq, facilcls_c,
                  inout_c, updown_c, node_id,
                  to_node_id, node_lid, link_lid,
                  path_type, facil_name)
        file_obj.write('%d\t%d\t%d\t'
                       '%d\t%d\t%d\t'
                       '%d\t%s\t%s\t'
                       '%s\t%s\n' % params)

    def _store_service_road_facil_path(self, inout_c, path,
                                       road_code, updown_c):
        '''辅路、类辅路设施'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_service_road_path1(
                                              inout_c, node_id, to_node_id,
                                              node_lid, link_lid, road_code,
                                              updown_c)
           VALUES(%s, %s, %s,
                  %s, %s, %s,
                  %s)
        """
        node_id = path[0]
        to_node_id = path[-1]
        node_lid = ','.join([str(node) for node in path])
        if inout_c == HWY_INOUT_TYPE_OUT:
            link_list = self.G.get_linkids(path)
        elif inout_c == HWY_INOUT_TYPE_IN:
            link_list = self.G.get_linkids(path, reverse=True)
        else:
            link_list = []
        link_lid = ','.join([str(link) for link in link_list])
        params = (inout_c, node_id, to_node_id,
                  node_lid, link_lid, road_code,
                  updown_c)
        self.pg.execute1(sqlcmd, params)

    def _store_service_road_facil_path2(self, inout_c, path,
                                        road_code, updown_c):
        '''辅路、类辅路设施'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_service_road_path2(
                                              inout_c, node_id, to_node_id,
                                              node_lid, link_lid, road_code,
                                              updown_c)
           VALUES(%s, %s, %s,
                  %s, %s, %s,
                  %s)
        """
        node_id = path[0]
        to_node_id = path[-1]
        node_lid = ','.join([str(node) for node in path])
        if inout_c == HWY_INOUT_TYPE_OUT:
            link_list = self.G.get_linkids(path)
        elif inout_c == HWY_INOUT_TYPE_IN:
            link_list = self.G.get_linkids(path, reverse=True)
        else:
            link_list = []
        link_lid = ','.join([str(link) for link in link_list])
        params = (inout_c, node_id, to_node_id,
                  node_lid, link_lid, road_code,
                  updown_c)
        self.pg.execute1(sqlcmd, params)

    def _store_facil_name(self, road_code, road_seq, facil_name):
        '''保存设施名称'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_facil_name(road_code, road_seq, facil_name)
          VALUES(%s, %s, %s)
        """
        if facil_name:
            if type(facil_name) is list:
                json_name = json.dumps(facil_name, ensure_ascii=False,
                                       encoding='utf8', sort_keys=True)
            elif type(facil_name) in (str, unicode):
                json_name = facil_name
            else:
                self.log.error('Unknown type. road_code=%s, road_seq=%s' %
                               (road_code, road_seq))
                return
        else:
            json_name = None
        self.pg.execute1(sqlcmd, (road_code, road_seq, json_name))

    def _store_sapa_link(self, road_code, road_seq, updown,
                         link_ids, link_lid, poi_id, distance):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_sapa_link(road_code, road_seq, updown_c,
                                           link_ids, link_lid, poi_id,
                                           distance)
          VALUES(%s, %s, %s, %s, %s, %s, %s);
        """
        self.pg.execute1(sqlcmd, (road_code, road_seq, updown,
                                  link_ids, link_lid, poi_id, distance))

    def _store_sapa_info(self, road_code, road_seq,
                         facil_cls, poi_id, name,
                         updown):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_sapa_info(road_code, road_seq, facilcls_c,
                                           poi_id, sapa_name, updown_c)
          VALUES(%s, %s, %s,
                 %s, %s, %s);
        """
        self.pg.execute1(sqlcmd, (road_code, road_seq, facil_cls,
                                  poi_id, name, updown))

    def _store_service_info(self, road_code, road_seq,
                            updown, service_types):
        sqlcmd = """
        INSERT INTO hwy_service(road_code, road_seq, updown_c,
                               gas_station, information, rest_area,
                               shopping_corner, postbox, atm,
                               restaurant, toilet)
          VALUES (%s, %s, %s,
                  %s, %s, %s,
                  %s, %s, %s,
                  %s, %s);
        """
        params = (road_code, road_seq, updown) + service_types
        self.pg.execute1(sqlcmd, params)

    def _store_same_info(self, same_facils, p_facil):
        sqlcmd = """
        INSERT INTO hwy_same_info(road_code, road_seq, updown_c,
                                  inout_c, node_id, p_node_id,
                                  p_road_seq)
          VALUES(%s, %s, %s,
                 %s, %s, %s,
                 %s);
        """
        p_node_id = p_facil.node_id
        p_road_seq = p_facil.road_point
        for facil in same_facils:
            if(facil.road_code == p_facil.road_code and
               facil.road_point == p_facil.road_point):
                continue
            params = (facil.road_code, facil.road_point, facil.updown,
                      facil.inout, facil.node_id, p_node_id,
                      p_road_seq)
            self.pg.execute1(sqlcmd, params)

    def _store_del_sapa(self, del_sapas):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_sapa_del(gid)
           VALUES(%s);
        """
        for gid in del_sapas:
            self.pg.execute1(sqlcmd, (gid,))
        self.pg.commit1()
        # ## 备份路径情报
        sqlcmd = """
        UPDATE mid_temp_hwy_sapa_del AS a
           SET road_code=b.road_code, road_seq = b.road_seq,
               facilcls_c = b.facilcls_c, inout_c = b.inout_c,
               node_id = b.node_id, to_node_id = b.to_node_id,
               node_lid = b.node_lid, link_lid = b.link_lid,
               path_type = b.path_type
          FROM mid_temp_hwy_ic_path as b
          where a.gid = b.gid;
        """
        self.pg.execute1(sqlcmd)
        self.pg.commit1()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_sapa_del
          )
        """
        self.pg.execute1(sqlcmd)
        self.pg.commit1()

    def _store_del_jct_uturn(self, del_pathes):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_jct_uturn_del(gid)
           VALUES(%s);
        """
        for gid in del_pathes:
            self.pg.execute2(sqlcmd, (gid,))
        self.pg.commit2()
        # ## 备份路径情报
        sqlcmd = """
        UPDATE mid_temp_hwy_jct_uturn_del AS a
           SET road_code=b.road_code, road_seq = b.road_seq,
               facilcls_c = b.facilcls_c, inout_c = b.inout_c,
               node_id = b.node_id, to_node_id = b.to_node_id,
               node_lid = b.node_lid, link_lid = b.link_lid,
               path_type = b.path_type
          FROM mid_temp_hwy_ic_path as b
          where a.gid = b.gid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_jct_uturn_del
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _store_del_ic(self, del_pathes):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_del(gid)
           VALUES(%s);
        """
        for gid in del_pathes:
            self.pg.execute2(sqlcmd, (gid,))
        self.pg.commit2()
        # ## 备份路径情报
        sqlcmd = """
        UPDATE mid_temp_hwy_ic_del AS a
           SET road_code=b.road_code, road_seq = b.road_seq,
               facilcls_c = b.facilcls_c, inout_c = b.inout_c,
               node_id = b.node_id, to_node_id = b.to_node_id,
               node_lid = b.node_lid, link_lid = b.link_lid,
               path_type = b.path_type
          FROM mid_temp_hwy_ic_path as b
          where a.gid = b.gid;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## 路径从mid_temp_hwy_ic_path表中删去
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_path
          WHERE gid in (
          SELECT gid
            FROM mid_temp_hwy_ic_del
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _del_both_dir_sapa(self, road_code, road_seq,
                           inout, node_id, to_node_id=None):
        if to_node_id:
            sqlcmd = """
            DELETE FROM mid_temp_hwy_ic_path
              WHERE road_code = %s and road_seq = %s
                    and inout_c = %s and node_id = %s
                    and to_node_id = %s;
            """
            self.pg.execute1(sqlcmd, (road_code, road_seq,
                                      inout, node_id, to_node_id))
        else:
            sqlcmd = """
            DELETE FROM mid_temp_hwy_ic_path
              WHERE road_code = %s and road_seq = %s
                    and inout_c = %s and node_id = %s;
            """
            self.pg.execute1(sqlcmd, (road_code, road_seq,
                                      inout, node_id))


# ===============================================================================
# HwyFacilityRDFMea:中东
# ===============================================================================
class HwyFacilityRDFMea(HwyFacilityRDF):
    '''生成设施情报(中东)
    '''

    def __init__(self, data_mng, ItemName='HwyFacilityRDFMea'):
        '''
        Constructor
        '''
        HwyFacilityRDF.__init__(self, data_mng, ItemName)

    def _make_hwy_store(self):
        '''店铺情报--中东BrandIcon未授权'''
        self.log.info('Start Make Facility Stores.')
        self.CreateTable2('hwy_store')
        self.log.info('End Make Facility Stores.')

# ==============================================================================
# 服务情报对应表
# ==============================================================================
# 未定义服务种别
SERVICE_UNDEFINED_DICT = {5000: None,  # Business Facility
                          7538: None,  # Auto Service & Maintenance
                          9992: None,  # Place of Worship
                          9537: None,  # Clothing Store
                          9987: None,  # Consumer Electronics Store
                          }
