# -*- coding: UTF-8 -*-
'''
Created on 2015-5-27

@author: hcz
'''
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE
from component.rdf.hwy.hwy_def_rdf import HWY_FALSE
from component.rdf.hwy.hwy_facility_rdf import ROAD_SEQ_MARGIN
from component.rdf.hwy.hwy_facility_rdf import IC_PATH_TYPE_DICT
from component.rdf.hwy.hwy_facility_rdf import HwyFacilityRDF
from component.ni.hwy.hwy_poi_category_ni import HwyPoiCategoryNi
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_SA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_PA
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_IC
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_TOLL
from component.rdf.hwy.hwy_def_rdf import HWY_IC_TYPE_JCT
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_IN
from component.rdf.hwy.hwy_def_rdf import HWY_INOUT_TYPE_OUT
from component.rdf.hwy.hwy_def_rdf import HWY_LINK_TYPE_RAMP
from component.rdf.hwy.hwy_graph_rdf import HWY_LINK_TYPE
from component.rdf.hwy.hwy_facility_rdf import HWY_PATH_TYPE_SAPA
from component.jdb.hwy.hwy_graph import get_simple_cycle
from component.rdf.hwy.hwy_def_rdf import ANGLE_45
from component.rdf.hwy.hwy_def_rdf import ANGLE_100
from component.rdf.hwy.hwy_def_rdf import ANGLE_360
SAPA_TYPE_DICT = {'8380': HWY_IC_TYPE_SA,  # 高速停车区
                  '8381': HWY_IC_TYPE_PA,  # 高速停车区
                  '4101': HWY_IC_TYPE_PA,  # 室内停车场
                  '4102': HWY_IC_TYPE_PA,  # 室外停车场
                  }


class HwyFacilityNiPro(HwyFacilityRDF):
    '''生成设施情报(中国四维)
    '''

    def __init__(self, data_mng, ItemName='HwyFacilityNi'):
        '''
        Constructor
        '''
        HwyFacilityRDF.__init__(self, data_mng, ItemName)
        self.poi_cate = HwyPoiCategoryNi()

    def _update_undefined_dict(self):
        self.undefined_dict.update(SERVICE_UNDEFINED_DICT)
        return 0

    def _make_road_seq(self, road_code, updown, facils_list,
                       sapa_node_dict, next_seq, temp_file_obj):
        sapa_seq_dict = {}  # SAPA node: road_seq
        for node, all_facils in facils_list:
            temp_all_facils = []
            for facilcls, inout, path in all_facils:
                facil_name = None
                # 入口
                if(facilcls == HWY_IC_TYPE_IC and
                   inout == HWY_INOUT_TYPE_IN):
                    facil_name = self._get_ic_in_name(path)
                    facil_path = (facilcls, inout, path, facil_name)
                    temp_all_facils.insert(0, facil_path)
                else:
                    facil_path = (facilcls, inout, path, facil_name)
                    temp_all_facils.append(facil_path)
            road_seq_dict, next_seq = self._get_road_seq(node,
                                                         temp_all_facils,
                                                         next_seq,
                                                         sapa_node_dict,
                                                         sapa_seq_dict,
                                                         )
            for facilcls, inout_c, path, facil_name in temp_all_facils:
                road_seq = road_seq_dict.get((facilcls, inout_c, facil_name))
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
                                              path_type, temp_file_obj,
                                              facil_name)

    def _sort_facils(self, all_facils):
        '''出口: 按IC, JCT, SAPA; 入口: SAPA, JCT, IC.'''
        toll_facil_types = set()
        in_facil_types = set()
        out_facil_types = set()
        sorted_facils = []
        for facil_path_info in all_facils:
            facilcls, inout_c = facil_path_info[0], facil_path_info[1]
            facil_name = facil_path_info[3]
            if inout_c == HWY_INOUT_TYPE_OUT:
                out_facil_types.add((facilcls, inout_c, facil_name))
            elif inout_c == HWY_INOUT_TYPE_IN:
                in_facil_types.add((facilcls, inout_c, facil_name))
            else:
                toll_facil_types.add((facilcls, inout_c, facil_name))
        # 入口按种别和设施名称除序排:大到小
        temp_list = list(in_facil_types)
        temp_list.sort(key=lambda l: (l[0], l[2]), reverse=True)
        sorted_facils += temp_list
        # 无出入口种别
        sorted_facils += list(toll_facil_types)
        # 出口设施按种别倒序排: 小到大
        temp_list = list(out_facil_types)
        temp_list.sort(key=lambda l: (l[0], l[2]), reverse=False)
        sorted_facils += temp_list
        return sorted_facils

    def _get_road_seq(self, node, all_facils, road_seq,
                      sapa_node_dict, sapa_seq_dict):
        # org_facil_id = self.G.get_org_facil_id(node)
        sorted_facils = self._sort_facils(all_facils)
        road_seq_dict = {}
        next_road_seq = road_seq
        visited = []
        for facil_idx in range(0, len(sorted_facils)):
            facilcls, inout, facil_name = sorted_facils[facil_idx]
            key = facilcls, inout, facil_name
            if(facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA) and
               node in sapa_node_dict):
                sapa_road_seq = self._get_sapa_road_seq(node,
                                                        facilcls,
                                                        sapa_node_dict,
                                                        sapa_seq_dict,
                                                        visited)
                if sapa_road_seq:
                    road_seq_dict[key] = sapa_road_seq
                    sapa_seq_dict[node] = sapa_road_seq
                else:
                    road_seq_dict[key] = next_road_seq
                    sapa_seq_dict[node] = next_road_seq
                    next_road_seq = next_road_seq + ROAD_SEQ_MARGIN
            else:
                road_seq_dict[key] = next_road_seq
                # self._store_road_seq(org_facil_id, facilcls,
                #                      next_road_seq, inout,
                #                      facil_name)
                next_road_seq = next_road_seq + ROAD_SEQ_MARGIN
        return road_seq_dict, next_road_seq

    def _store_road_seq(self, org_facil_id, facilcls, road_seq,
                        inout, facil_name):
        if not org_facil_id:
            return
        if (org_facil_id, facilcls, inout, facil_name) in self.org_facil_dict:
            temp_seq = self.org_facil_dict.get((org_facil_id, facilcls,
                                                facil_name))
            if(temp_seq != road_seq and
               not (facilcls == HWY_IC_TYPE_IC and
                    inout == HWY_INOUT_TYPE_OUT)):
                self.log.error('RoadSeq is changed. org_facil_id=%s,'
                               'facilcls=%s' % (org_facil_id, facilcls))
        else:
            self.org_facil_dict[(org_facil_id, facilcls,
                                 facil_name)] = road_seq

    def _cat_id_map(self, category_id):
        cat_id = category_id
        return cat_id

    def _get_sapa_type(self, sapa_kind):
        facil_cls = None
        if sapa_kind:
            facil_cls = SAPA_TYPE_DICT.get(sapa_kind)
        else:
            facil_cls = HWY_IC_TYPE_PA
        return facil_cls

    def _get_rest_area_info(self):
        '''取得服务情报'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq, c.poi_id,
                        kind, name, updown_c
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
          LEFT JOIN org_poi as e  -- org_p: 11year, org_poi: 14year
          ON c.poi_id = e.poi_id::bigint
          where e.kind IN ('8380', '8381', '4101', '4102')
          ORDER BY road_code, road_seq;
        """
        return self.get_batch_data(sqlcmd)

    def get_paths_2_inout(self, facilcls, path, inout):
        '''第一个和一般道连接的点之后，还有Ramp(中国使用)'''
        return [path]
        if facilcls != HWY_IC_TYPE_IC:
            return []
        if inout == HWY_INOUT_TYPE_IN:  # 入口
            reverse = True
        else:
            reverse = False
        pathes = list(self.G.all_path_2_inout(path, reverse=reverse))
        if pathes:
            return pathes
        return [path]

    def _make_inout_join_node(self):
        '''出入口的汇合点'''
        self.log.info('Make In/out join node.')
        self.CreateTable2('mid_temp_hwy_inout_join_node')
        join_node_set = set()
        inout_c = HWY_INOUT_TYPE_OUT
        for out_node_lids, in_node_lids in self._get_inout_join_path():
            f_node_lids = [map(int, n_lid.split(','))
                           for n_lid in out_node_lids]
            t_node_lids = [map(int, n_lid.split(','))
                           for n_lid in in_node_lids]
            # 求出入口路径的重叠点
            overlay_nodes = self._get_overlay_node(f_node_lids, t_node_lids, 1)
            if not overlay_nodes:  # 没有重叠的点
                if not self._get_overlay_node(f_node_lids, t_node_lids, 0):
                    self.log.error('No overlay_nodes. f_node_lids=%s, '
                                   't_node_lids=%s'
                                   % (f_node_lids, t_node_lids))
                continue
            join_nodes = self._get_join_nodes(overlay_nodes,
                                              f_node_lids,
                                              t_node_lids,
                                              inout_c)
            join_node_set = join_node_set.union(join_nodes)
        self._store_inout_join_node(join_node_set)

    def _get_inout_join_path(self):
        '''出入口交汇的路径'''
        sqlcmd = """
        SELECT distinct out_node_lids, in_node_lids
          FROM (
            SELECT distinct
                   a.road_code as out_road_code,
                   a.road_seq as out_road_seq,
                   a.inout_c as out_inout_c,
                   a.node_id as out_node_id,
                   b.road_code as in_road_code,
                   b.road_seq as in_road_seq,
                   b.inout_c as in_inout_c,
                   b.node_id as in_node_id
              FROM (
                -- IC OUT
                SELECT road_code, road_seq,
                       inout_c, node_id, pass_node_id
                  FROM mid_temp_hwy_ic_path_expand_node
                  WHERE facilcls_c = 5 AND inout_c = 2 AND
                        path_type = 'IC'
              ) AS a
              INNER JOIN (
                -- IC IN
                SELECT road_code, road_seq,
                       inout_c, node_id, pass_node_id
                  FROM mid_temp_hwy_ic_path_expand_node
                  WHERE facilcls_c = 5 AND inout_c = 1 AND
                        path_type = 'IC'
              ) as b
              ON a.pass_node_id = b.pass_node_id
          ) as c
          INNER JOIN (
            -- IC OUT
            SELECT road_code, road_seq, inout_c,
                   node_id, array_agg(node_lid) as out_node_lids
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c = 5 AND inout_c = 2 AND
                path_type = 'IC'
              group by road_code, road_seq, inout_c, node_id
          ) AS d
          ON out_road_code = d.road_code and
             out_road_seq = d.road_seq and
             out_inout_c = d.inout_c and
             out_node_id =  d.node_id
          INNER JOIN (
            -- IC OUT
            SELECT road_code, road_seq, inout_c,
                   node_id, array_agg(node_lid) as in_node_lids
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c = 5 AND inout_c = 1 AND
                path_type = 'IC'
              group by road_code, road_seq, inout_c, node_id
          ) AS e
          ON in_road_code = e.road_code and
             in_road_seq = e.road_seq and
             in_inout_c = e.inout_c and
             in_node_id =  e.node_id
        """
        return self.get_batch_data(sqlcmd)

    def _filter_JCT_UTurn_SAPA(self):
        '''过滤假JCT/UTurn/SAPA:下了高速、到了出口又转弯回来的径路'''
        self.log.info('Start Filtering JCT/UTurn Path.')
        self.pg.connect1()
        self.pg.connect2()
        self.CreateTable2('mid_temp_hwy_jct_uturn_del')
        del_pathes = set()
        for jct_uturn in self._get_jct_uturn():
            gid_array, join_node, path_type = jct_uturn[0:3]
            node_lids, inout = jct_uturn[3:5]
            node_lids = [map(int, n_lid.split(','))
                         for n_lid in node_lids]
            for gid, node_lid in zip(gid_array, node_lids):
                if join_node in node_lid:
                    # 所有SAPA路径都经过该汇合点
                    if(path_type == HWY_PATH_TYPE_SAPA and
                       self._all_path_pass_join_node(node_lids, join_node) and
                       not self._pass_inout(node_lid, inout, join_node)):
                        continue
                    del_pathes.add(gid)
        ic_pathes = self._get_filter_ic_by_join_node()
        del_pathes = del_pathes.union(ic_pathes)
        # 删除错误的JCT/UTurn/SAPA
        self._store_del_jct_uturn(del_pathes)
        self.log.info('End Filtering JCT/UTurn Path.')

    def _all_path_pass_join_node(self, node_lids, join_node):
        for node_lid in node_lids:
            if join_node not in node_lid:
                return False
        return True

    def _pass_inout(self, node_lid, inout, join_node):
        # ##角度小于45度
        count = node_lid.count(join_node)
        start_idx = 0
        while count >= 1:
            count -= 1
            join_idx = node_lid.index(join_node, start_idx)
            start_idx = join_idx + 1
            if join_idx <= 0:
                continue
            if join_idx >= len(node_lid) - 1:
                break
            if inout == HWY_INOUT_TYPE_IN:
                reverse = True
            else:
                reverse = False
            if self.G.is_hwy_inout(node_lid[:join_idx+1], reverse):
                return True
        return False

    def _get_filter_ic_by_join_node(self):
        '''过滤出入口:到达出入口汇合点，又转变回去，再转一圈才出来'''
        sqlcmd = """
        SELECT gids, a.node_id as join_node, node_lids, b.inout_c
          FROM mid_temp_hwy_inout_join_node AS a
          INNER JOIN mid_temp_hwy_ic_path_expand_node as b
          on a.node_id = b.pass_node_id
          INNER JOIN (
            SELECT array_agg(gid) as gids, road_code, road_seq, path_type,
                   node_id, inout_c, array_agg(node_lid) as node_lids
              FROM mid_temp_hwy_ic_path
              WHERE  path_type in ('IC')
              group by road_code, road_seq, node_id, inout_c, path_type
          ) as c
          ON b.road_code = c.road_code and
             b.road_seq = c.road_seq and
             b.node_id = c.node_id and
             b.inout_c = c.inout_c
        """
        del_pathes = set()
        for gids, join_node, node_lids, inout in self.get_batch_data(sqlcmd):
            for gid, node_lid in zip(gids, node_lids):
                node_lid = map(int, node_lid.split(','))
                count = node_lid.count(join_node)
                if count > 1:
                    index = node_lid.index(join_node)
                    cycles = get_simple_cycle(node_lid[index:])
                    for cycle in cycles:
                        if join_node in cycle:
                            del_pathes.add(gid)
                            break
                # ##角度小于45度
                start_idx = 0
                while count >= 1:
                    count -= 1
                    join_idx = node_lid.index(join_node, start_idx)
                    start_idx = join_idx + 1
                    if join_idx <= 0:
                        continue
                    if join_idx >= len(node_lid) - 1:
                        break
                    prev_node = node_lid[join_idx - 1]
                    next_node = node_lid[join_idx + 1]
                    if inout == HWY_INOUT_TYPE_IN:
                        in_edge = (next_node, join_node)
                        out_edge = (join_node, prev_node)
                        reverse = True
                    else:
                        in_edge = (prev_node, join_node)
                        out_edge = (join_node, next_node)
                        reverse = False
                    angle = self.G.get_angle(in_edge, out_edge)
                    if angle <= ANGLE_45 or angle >= (ANGLE_360 - ANGLE_45):
                        del_pathes.add(gid)
                        break
                    if self.G.is_hwy_inout(node_lid[:join_idx+1], reverse):
                        if(angle < ANGLE_100 or
                           angle > (ANGLE_360 - ANGLE_100)):
                            del_pathes.add(gid)
                            break
        return del_pathes

    def _filter_service_road(self):
        '''过滤辅路、类辅路， 从辅路路径选出SAPA路径(路径两头都有SAPA设施)'''
        self.CreateTable2('mid_temp_hwy_service_road_path2')
        # 1. 删除经过 出入口汇合点的 类辅路
        self._filter_service_road_by_join_node()
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
        self.CreateTable2('mid_temp_hwy_service_road_path_del')
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
                if f_inout == HWY_INOUT_TYPE_OUT:
                    data = self.G[u][v]
                else:
                    data = self.G[v][u]
                # ramp link
                link_type = data.get(HWY_LINK_TYPE)
                if link_type != HWY_LINK_TYPE_RAMP:
                    break
                # ramp link
                link_type = data.get(HWY_LINK_TYPE)
                if link_type != HWY_LINK_TYPE_RAMP:
                    break
                if v in curr_overlay_nodes:
                    overlay_sec.append(v)
                    has_overlay = True
                else:
                    if has_overlay:  # 重叠点必须连续
                        break
                node_idx -= 1
            # 判断X型
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

    def _get_overlay_node(self, f_node_lids, t_node_lids, s_idx=1):
        '''重叠点'''
        f_set = set()
        t_set = set()
        for node_lid in f_node_lids:
            f_set = f_set.union(node_lid[s_idx:])
        for node_lid in t_node_lids:
            t_set = t_set.union(node_lid[s_idx:])
        return f_set.intersection(t_set)

    def _get_toll_index(self, path):
        node_idx = len(path) - 1
        while node_idx >= 0:
            if self.G.is_tollgate(path[node_idx]):
                return node_idx
            node_idx -= 1
        return node_idx

    def _get_service_types(self, cat_id_list):
        gas_station, information, rest_area = HWY_FALSE, HWY_FALSE, HWY_FALSE
        shopping_corner, postbox, atm = HWY_FALSE, HWY_FALSE, HWY_FALSE
        restaurant, toilet = HWY_FALSE, HWY_FALSE
        if self.poi_cate:
            for cat_id in cat_id_list:
                if cat_id in self.gas_station_dict:
                    gas_station = HWY_TRUE
                elif cat_id in self.info_dict:
                    information = HWY_TRUE
                elif cat_id in self.rest_area_dict:
                    rest_area = HWY_TRUE
                elif cat_id in self.shopping_corner_dict:
                    shopping_corner = HWY_TRUE
                elif cat_id in self.post_box_dict:
                    postbox = HWY_TRUE
                elif cat_id in self.atm_dict:
                    atm = HWY_TRUE
                elif cat_id in self.restaurant_dict:
                    restaurant = HWY_TRUE
                elif cat_id in self.toilet_dict:
                    toilet = HWY_TRUE
                elif cat_id in self.undefined_dict:
                    continue
                else:
                    self.log.error('Unknown Category ID. cat_id=%s' % cat_id)
        return (gas_station, information, rest_area,
                shopping_corner, postbox, atm,
                restaurant, toilet)

    def _get_sapa_link(self):
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
          LEFT JOIN org_poi as d
          on c.poi_id = d.poi_id::bigint
          where d.kind NOT IN ('8380', '8381', '4101', '4102')
          GROUP BY b.road_code, b.road_seq, b.updown_c,
                   b.node_id, b.first_node_id, c.poi_id,
                   b.link_lid, b.node_lid
          ORDER BY road_code, road_seq
        '''
        return self.get_batch_data(sqlcmd, batch_size=1024)

    def _load_poi_category(self):
        HwyFacilityRDF._load_poi_category(self)
        self.gas_station_dict = SERVICE_GAS_DICT

    def _get_jct_uturn(self):
        '''取得经过in/out汇合点的路径'''
        sqlcmd = """
        SELECT gids, a.node_id as join_node, c.path_type,
               node_lids, b.inout_c
          FROM mid_temp_hwy_inout_join_node AS a
          INNER JOIN mid_temp_hwy_ic_path_expand_node as b
          on a.node_id = b.pass_node_id
          INNER JOIN (
            SELECT array_agg(gid) as gids, road_code, road_seq, path_type,
                   node_id, inout_c, array_agg(node_lid) as node_lids
              FROM mid_temp_hwy_ic_path
              WHERE  path_type in ('JCT', 'UTURN', 'SAPA')
              group by road_code, road_seq, node_id, inout_c, path_type
          ) as c
          ON b.road_code = c.road_code and
             b.road_seq = c.road_seq and
             b.node_id = c.node_id and
             b.inout_c = c.inout_c;
        """
        return self.get_batch_data(sqlcmd, batch_size=1024)

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

    def _get_ic_out_name(self, path):
        '''出口设施名称'''
        pass

    def _get_ic_in_name(self, path):
        '''入口设施名称'''
        node_cnt = len(path) - 1
        while node_cnt >= 0:
            last_node = path[node_cnt]
            enter_name = self.G.get_enter_poi_name(last_node)
            if enter_name:
                return enter_name
            node_cnt -= 1
        return ''

    def _make_facil_name(self):
        '''制作设施名称'''
        self.log.info('Start Make Facil Name.')
        self.pg.connect1()
        self.CreateTable2('mid_temp_hwy_facil_name')
        sqlcmd = """
        SELECT road_code, road_seq, facilcls_c,
               array_agg(node_id), array_agg(facil_name)
          FROM (
            SELECT distinct road_code, road_seq,
                   facilcls_c, inout_c, node_id, facil_name
              FROM mid_temp_hwy_ic_path
              WHERE facilcls_c <> 10  -- Not U-turn
              ORDER BY road_code, road_seq,
                       facilcls_c, inout_c
          ) AS a
          group by road_code, road_seq, facilcls_c
          order by road_code, road_seq
        """
        for data in self.pg.get_batch_data2(sqlcmd):
            road_code, road_seq, facilcls, node_id_list, facil_names = data[:]
            name_set = set(facil_names)
            if None in name_set:
                name_set.remove(None)
            if name_set:
                if len(name_set) == 1:
                    self._store_facil_name(road_code, road_seq, name_set.pop())
                else:
                    self.log.error('Multi Facil Name. roadcode=%s, roadseq=%s'
                                   % (road_code, road_seq))
            elif facilcls in (HWY_IC_TYPE_SA, HWY_IC_TYPE_PA):  # SAPA
                facil_name = self._get_sapa_facil_name(road_code, road_seq)
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
            elif facilcls in (HWY_IC_TYPE_TOLL,):
                facil_name = self.G.get_node_name(node_id_list[0])
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
            elif facilcls in (HWY_IC_TYPE_JCT,):  # JCT, 名称不做
                pass
            else:  # IC
                facil_name = self._get_exit_name(node_id_list)
                if facil_name:
                    self._store_facil_name(road_code, road_seq, facil_name)
        self.pg.commit1()
        self.CreateIndex2('mid_temp_hwy_facil_name_road_code_road_seq_idx')
        self.log.info('End Make Facility Name.')

    def _store_inout_join_node(self, join_nodes):
        sqlcmd = """INSERT INTO mid_temp_hwy_inout_join_node values(%s)"""
        for node in join_nodes:
            self.pg.execute2(sqlcmd, (node,))
        self.pg.commit2()

SERVICE_GAS_DICT = {'4080': 1,  # 加油站
                    '4081': 1,  # 中国石油
                    '4082': 1,  # 中国石化
                    '4083': 1,  # 中海油
                    '4086': 1,  # 壳牌
                    '4087': 1,  # 美孚
                    '4094': 1,  # BP(港澳仅有)
                    '4095': 1,  # 埃索(港澳仅有)
                    '4096': 1,  # 加德士(港澳仅有)
                    '4098': 1,  # 华润(港澳仅有)
                    '40A0': 1,  # 加气站
                    '40A1': 1,  # 加油加气站
                    }

SERVICE_UNDEFINED_DICT = {}
