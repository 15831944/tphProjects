# -*- coding: UTF-8 -*-
'''
Created on 2015-2-12

@author: hcz
'''
import json
import component.component_base
from component.rdf.hwy.hwy_graph_rdf import is_cycle_path
from component.rdf.hwy.hwy_graph_rdf import HWY_ROAD_CODE
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_def_rdf import HWY_UPDOWN_TYPE_UP
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
ROAD_SEQ_MARGIN = 10
SAPA_TYPE_DICT = {1: HWY_IC_TYPE_SA,  # COMPLETE REST AREA
                  2: HWY_IC_TYPE_PA,  # PARKING AND REST ROOM ONLY
                  3: HWY_IC_TYPE_PA,  # PARKING ONLY
                  4: HWY_IC_TYPE_SA,  # MOTORWAY SERVICE AREA
                  5: None             # SCENIC OVERLOOK
                  }


class HwyFacilityRDF(component.component_base.comp_base):
    '''生成设施情报
    '''

    def __init__(self, data_mng):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Highway_Facility')
        self.hwy_data = data_mng
        self.G = None
        if self.hwy_data:
            self.G = self.hwy_data.get_graph()
        self.org_facil_dict = {}  # (org_facil_id, facil_cls): road_seq

    def _Do(self):
        self._make_ic_path()
        self._make_sapa_info()
        self._del_sapa_node()
        self._make_facil_name()  # 设施名称
        self._make_hwy_node()
        self._make_facil_same_info()  # 并设情报
        self._make_hwy_store()  # 店铺 情报
        self._make_hwy_service()  # 服务情报

    def _make_ic_path(self):
        self.log.info('Start make IC Path.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_ic_path')
        for road_code, route_path in self.hwy_data.get_road_code_path():
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
            sapa_node_dict = {}  # SaPa出口:SaPa入口 or SaPa入口:SaPa出口
            facils_list = []
            # ## 取得设施情报
            for node_idx in range(0, end_pos):
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
                facils_list.append((node, all_facils))
                for facilcls, inout_c, path in all_facils:
                    if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
                        node = path[0]
                        to_node = path[-1]
                        if node not in sapa_node_dict:
                            sapa_node_dict[node] = set([to_node])
                        else:
                            sapa_node_dict[node].add(to_node)
            # ## 设置设施序号
            sapa_seq_dict = {}  # Sapa node: road_seq
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
                    self._store_facil_path(road_code, road_seq, facilcls,
                                           inout_c, path)
        self.pg.commit1()
        self.log.info('End make IC Path.')

    def _get_road_seq(self, node, all_facils, road_seq,
                      sapa_node_dict, sapa_seq_dict):
        org_facil_id = self.G.get_org_facil_id(node)
        sorted_facils = self._sort_facils(all_facils)
        road_seq_dict = {}
        next_road_seq = road_seq
        visited = []
        for facil_idx in range(0, len(sorted_facils)):
            facilcls, inout = sorted_facils[facil_idx]
            if facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):
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
                                                              facilcls)
                if temp_seq:
                    road_seq_dict[facilcls, inout] = temp_seq
                else:
                    road_seq_dict[facilcls, inout] = next_road_seq
                    self._store_road_seq(org_facil_id, facilcls, next_road_seq)
                    next_road_seq = next_road_seq + ROAD_SEQ_MARGIN
        return road_seq_dict, next_road_seq

    def _get_road_seq_by_org_facil_id(self, org_facil_id, facilcls):
        if not org_facil_id:
            return None
        key = (org_facil_id, facilcls)
        road_seq = self.org_facil_dict.get((key))
        return road_seq

    def _store_road_seq(self, org_facil_id, facilcls, road_seq):
        if not org_facil_id:
            return
        if (org_facil_id, facilcls) in self.org_facil_dict:
            temp_seq = self.org_facil_dict.get((org_facil_id, facilcls))
            if temp_seq != road_seq:
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
        for facilcls, inout_c, path in all_facils:
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
        SELECT distinct a.road_code, a.road_seq, 1 as updown_c,
               facilcls_c, inout_c, 0 as tollcls_c, -- None
               a.node_id, b.facil_name, the_geom
          FROM mid_temp_hwy_ic_path as a
          LEFT JOIN mid_temp_hwy_facil_name as b
          ON a.road_code = b.road_code and a.road_seq = b.road_seq
          LEFT JOIN node_tbl as c
          ON a.node_id = c.node_id
          order by road_code, road_seq
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

    def _make_sapa_info(self):
        '''SAPA对应的Rest Area POI情报。'''
        self.log.info('Start Make SAPA Info.')
        self.CreateTable2('mid_temp_hwy_sapa_info')
        for data in self._get_rest_area_info():
            road_code, road_seq, poi_id, rest_area_type, name = data
            if rest_area_type:
                facil_cls = SAPA_TYPE_DICT.get(rest_area_type)
                if not facil_cls:
                    self.log.error('Unknown Rest Area type. poi_id=%s,'
                                   'rest_area_type' % (poi_id, rest_area_type))
                    continue
            else:
                facil_cls = HWY_IC_TYPE_PA
            self._store_sapa_info(road_code, road_seq,
                                  facil_cls, poi_id, name)
        self.pg.commit1()
        self._update_sapa_facilcls()
        self.CreateIndex2('mid_temp_hwy_sapa_info_road_code_road_seq_idx')
        self.log.info('End Make SAPA Info.')

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
        self.pg.commit1()

    def _get_rest_area_info(self):
        '''取得服'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq, c.poi_id,
                        rest_area_type, name
          FROM (
            SELECT road_code, road_seq, facilcls_c,
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
          ORDER BY road_code, road_seq
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
            elif(parent_facil.facilcls == facil.facilcls and
                 parent_facil.inout != HWY_INOUT_TYPE_OUT and
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
        org_same_facils = []
        org_facil_id = self.G.get_org_facil_id(curr_facil.node_id)
        if not org_facil_id:
            return []
        facil_idx += 1
        for facil_idx in range(facil_idx, len(facil_list)):
            next_facil = facil_list[facil_idx]
            next_node_id = next_facil.node_id
            next_org_facil_id = self.G.get_org_facil_id(next_node_id)
            if org_facil_id == next_org_facil_id:
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
               array_agg(node_id) as node_lid
          FROM (
            SELECT a.road_code, a.road_seq, updown_c,
                   facilcls_c, inout_c, a.node_id
              FROM hwy_node as a
              LEFT JOIN hwy_link_road_code_info as b
              ON a.road_code = b.road_code and
                 a.node_id = b.node_id
              ORDER BY a.road_code, seq_nm, road_seq
          ) as c
          GROUP BY road_code, updown_c
          ORDER BY road_code;
        """
        data = self.get_batch_data(sqlcmd)
        for roadcode, updown, road_seqs, facilcls_cs, inouts, node_lid in data:
            facil_list = []
            for road_seq, facilcls, inout, node in zip(road_seqs, facilcls_cs,
                                                       inouts, node_lid):
                facil_info = HwyFacilInfo(roadcode, road_seq, facilcls,
                                          updown, node, inout,
                                          '')
                facil_list.append(facil_info)
            if node_lid[0] == node_lid[1]:  # 环
                # 去掉最后一个点(和第一个点重复)的设施
                facil_info = facil_list[-1]
                while facil_info.node_id == node_lid[0]:
                    facil_list.pop()
                    facil_info = facil_list[-1]
            yield facil_list

    def _make_hwy_store(self):
        '''店铺情报'''
        self.log.info('Start Make Facility Stores.')
        self.CreateTable2('hwy_store')
        sqlcmd = """
        INSERT INTO hwy_store(road_code, road_seq, updown_c,
                              store_cat_id, store_chain_id)
        (
        SELECT distinct road_code, road_seq, 1 as updown_c,
                        cat_id, b.chain_id
          FROM mid_temp_hwy_sapa_info as a
          LEFT JOIN mid_temp_sapa_store_info as b
          ON a.poi_id = b.poi_id
          WHERE b.chain_id is not null
          ORDER BY road_code, road_seq, chain_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End Make Facility Stores.')

    def _make_hwy_service(self):
        '''服务情报'''
        self.log.info('Start Make Facility Service.')
        self.CreateTable2('hwy_service')
        sqlcmd = """
        SELECT road_code, road_seq, ARRAY_AGG(cat_id)
          FROM (
            SELECT distinct road_code, road_seq, cat_id
              FROM mid_temp_hwy_sapa_info as a
              LEFT JOIN mid_temp_sapa_store_info as b
              ON a.poi_id = b.poi_id
              WHERE cat_id is not null
              ORDER BY road_code, road_seq, cat_id
          ) AS c
          GROUP BY road_code, road_seq
          ORDER BY road_code, road_seq
        """
        self.pg.connect1()
        for road_code, road_seq, cat_id_list in self.get_batch_data(sqlcmd):
            service_types = self._get_service_types(cat_id_list)
            updown = HWY_UPDOWN_TYPE_UP
            self._store_service_info(road_code, road_seq,
                                     updown, service_types)
        self.pg.commit1()
        self.log.info('End Make Facility Service.')

    def _get_service_types(self, cat_id_list):
        gas_station, information, rest_area = HWY_FALSE, HWY_FALSE, HWY_FALSE
        shopping_corner, postbox, atm = HWY_FALSE, HWY_FALSE, HWY_FALSE
        restaurant, toilet = HWY_FALSE, HWY_FALSE
        for cat_id in cat_id_list:
            if cat_id in SERVICE_GAS_DICT:
                gas_station = HWY_TRUE
            elif cat_id in SERVICE_INFORMATION_DICT:
                information = HWY_TRUE
            elif cat_id in SERVICE_REST_AREA_DICT:
                rest_area = HWY_TRUE
            elif cat_id in SERVICE_SHOPPING_DICT:
                shopping_corner = HWY_TRUE
            elif cat_id in SERVICE_POSTBOX_DICT:
                postbox = HWY_TRUE
            elif cat_id in SERVICE_ATM_DICT:
                atm = HWY_TRUE
            elif cat_id in SERVICE_RESTAURANT_DICT:
                restaurant = HWY_TRUE
            elif cat_id in SERVICE_UNDEFINED_DICT:
                continue
            else:
                self.log.error('Unknown Category ID. cat_id=%s' % cat_id)
        return (gas_station, information, rest_area,
                shopping_corner, postbox, atm,
                restaurant, toilet)

# ==============================================================================
#
# ==============================================================================
    def _store_facil_path(self, road_code, road_seq, facilcls_c,
                          inout_c, path):
        '''保存设施路径'''
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_path(road_code, road_seq, facilcls_c,
                                         inout_c, node_id, to_node_id,
                                         node_lid, link_lid)
           VALUES(%s, %s, %s,
                  %s, %s, %s,
                  %s, %s)
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
                  node_lid, link_lid)
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

    def _store_sapa_info(self, road_code, road_seq,
                         facil_cls, poi_id, name):
        sqlcmd = """
        INSERT INTO mid_temp_hwy_sapa_info(road_code, road_seq, facilcls_c,
                                           poi_id, sapa_name)
          VALUES(%s, %s, %s,
                 %s, %s);
        """
        self.pg.execute1(sqlcmd, (road_code, road_seq, facil_cls,
                                  poi_id, name))

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
            if facil == p_facil:
                continue
            params = (facil.road_code, facil.road_point, facil.updown,
                      facil.inout, facil.node_id, p_node_id,
                      p_road_seq)
            self.pg.execute1(sqlcmd, params)

# ==============================================================================
# 服务情报对应表
# ==============================================================================
# キャッシュコーナー/ATM
SERVICE_ATM_DICT = {3578: None,  # ATM
                    }
# ガソリンスタンド/Gas station
SERVICE_GAS_DICT = {5540: None,  # Gas station
                    }
# 仮眠休憩所/rest_area
SERVICE_REST_AREA_DICT = {7011: None  # Hotel
                          }
# レストラン/Restaurant
SERVICE_RESTAURANT_DICT = {5800: None,  # Restaurant
                           9996: None,  # Coffee Shop
                           }
# ショッピングコーナー/Shopping Corner
SERVICE_SHOPPING_DICT = {9535: None,  # Convenience Store
                         9567: None,  # Specialty Store
                         }
# 郵便ポスト/post_box
SERVICE_POSTBOX_DICT = {}
# インフォメーション
SERVICE_INFORMATION_DICT = {}
# 未定义服务种别
SERVICE_UNDEFINED_DICT = {7538: None,  # Auto Service & Maintenance
                          9992: None,  # Place of Worship
                          }
