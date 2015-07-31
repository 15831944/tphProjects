# -*- coding: UTF-8 -*-
'''
Created on 2014-3-25

@author: hongchenzai
'''
import base
from component.jdb.hwy.hwy_graph import HwyGraph
from component.jdb.hwy.hwy_graph import HwyGraphNoToll
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN  # 入口・合流
from component.jdb.hwy.hwy_def import INOUT_TYPE_OUT  # 出口・分岐
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_UP
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_DOWN
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_COMMON
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import IC_TYPE_TOLL
from component.jdb.hwy.hwy_def import ROAD_ATTR_NONE
from component.jdb.hwy.hwy_def import ROAD_ATTR_UP
from component.jdb.hwy.hwy_def import ROAD_ATTR_DOWN
from component.jdb.hwy.hwy_def import ROAD_ATTR_IN
from component.jdb.hwy.hwy_def import ROAD_ATTR_OUT
from component.jdb.hwy.hwy_def import ROAD_ATTR_WEST
from component.jdb.hwy.hwy_def import ROAD_ATTR_EAST
from component.jdb.hwy.hwy_def import HWY_INVALID_FACIL_ID
from component.jdb.hwy.hwy_def import ROAD_CODE_NO_TOLL
from component.link_graph import load_link_by_box
from component.link_graph import get_node_expand_box
UNITS_TO_EXPAND_NO_TOLL = 0.01


class HwyDataMng(base.component_base.comp_base):
    '''
    classdocs
    '''
    __instance = None

    @staticmethod
    def instance():
        if HwyDataMng.__instance is None:
            HwyDataMng.__instance = HwyDataMng()
        return HwyDataMng.__instance

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'HwyDataMng')
        self.__graph = HwyGraph()
        self.__graph_no_toll = HwyGraphNoToll()
        self.__hwy_facility_dict = {}
        self.__hwy_facility_pos_dict = {}
        self.__main_path_dict = {}
        self.__road_no_dict = {}
        self.__ic_list = []
        self.__toll_dict = {}
        self.__facility_dict = {}
        self.__link_fwd_bwd_dict = {}  # link的前方设施/后方设施
        self.__sapa_dict = {}
        self.__road_start_end_node = {}  # 道路的起末点，key是(road_code, updown)

    def _Do(self):
        # 高速本线
        self._make_main_links()
        # 高速无料金区间的本线link(road_code > 600000)
        self._make_main_links_no_toll()
        return 0

    def get_graph(self):
        return self.__graph

    def get_graph_no_toll(self):
        '''无料金区间link的graph.'''
        return self.__graph_no_toll

    def load_link_by_expand_box(self, G, s_node, t_node):
        '''加载两点1km里范围的link(无料区间设施使用).'''
        if s_node != t_node:
            expand_box = get_node_expand_box(s_node, t_node,
                                             UNITS_TO_EXPAND_NO_TOLL)
        else:
            expand_box = get_node_expand_box(s_node, t_node,
                                             UNITS_TO_EXPAND_NO_TOLL * 2)
        load_link_by_box(G, expand_box)

    def _make_main_links(self):
        '''高速本线link，不包括高速无料金区间的本线link(road_code > 600000)'''
        self.CreateTable2('mid_hwy_main_link')
        self.CreateFunction2('mid_get_midle_linkid_by_org_linkid')
        self.CreateIndex2('road_link_4326_road_code_idx')
        self.CreateIndex2('road_code_list_road_code_idx')
        sqlcmd = '''
        INSERT INTO mid_hwy_main_link
        (
            select distinct
                   link_array[seq_num] as link_id,
                   s_node,
                   e_node,
                   display_class,
                   link_type,
                   road_type,
                   toll,
                   one_way_code,
                   lineclass_c,
                   road_code,
                   fazm,
                   tazm,
                   (case when lineclass_c < 9 then 1
                        when lineclass_c >=9 then 0
                        else null end) as hwymode_f,
                   tile_id
              from (
                select lineclass_c, road_code, link_array,
                       generate_series(1, array_upper(link_array,1)) as seq_num
                  from (
                    select lineclass_c, road_code,
                           mid_get_midle_linkid_by_org_linkid(objectid::bigint
                           ) as link_array
                      from (
                        select a.lineclass_c, a.road_code, b.objectid
                          from road_code_list as a
                          left join road_link_4326 as b
                          on a.road_code = b.road_code
                          --where a.lineclass_c < 9
                      ) as a
                  ) as b
              ) as c
              left join link_tbl
              on link_array[seq_num] = link_tbl.link_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _make_main_links_no_toll(self):
        '''高速无料金区间的本线link(road_code > 600000)'''
        self.CreateTable2('mid_hwy_main_link_no_toll')
        self.CreateFunction2('mid_get_midle_linkid_by_org_linkid')
        sqlcmd = '''
        INSERT INTO mid_hwy_main_link_no_toll(
                link_id, s_node, e_node, display_class,
                link_type, road_type, toll, one_way_code,
                roadcode, fazm, tazm, hwymode_f, tile_id)
        (
            select distinct
                   link_array[seq_num] as link_id,
                   s_node,
                   e_node,
                   display_class,
                   link_type,
                   road_type,
                   toll,
                   one_way_code,
                   road_code,
                   fazm,
                   tazm,
                   0 as hwymode_f,
                   tile_id
              from (
                select road_code, link_array,
                       generate_series(1, array_upper(link_array,1)) as seq_num
                  from (
                    select road_code,
                           mid_get_midle_linkid_by_org_linkid(objectid::bigint
                              ) as link_array
                      from road_link_4326 as a
                      where road_code > %s
                  ) as b
              ) as c
              left join link_tbl
              on link_array[seq_num] = link_tbl.link_id
        );
        '''
        self.pg.execute2(sqlcmd, (ROAD_CODE_NO_TOLL,))
        self.pg.commit2()
        return 0

    def load_hwy_main_links(self):
        '''HWY本线link到图(不包括无料区间本线link)'''
        self.pg.connect2()
        self._add_main_links_to_graph(self.__graph,
                                      self.__get_hwy_main_links())
        self.__load_facility()

    def load_hwy_no_toll_main_links(self):
        '''加载无料区间本线link到图'''
        self.pg.connect2()
        self._add_main_links_to_graph(self.__graph_no_toll,
                                      self.__get_hwy_no_toll_main_links())

    def _add_main_links_to_graph(self, G, links):
        for info in links:
            link_id = info[0]
            s_node = info[1]
            e_node = info[2]
            one_way = info[7]
            G.add_link(s_node,
                       e_node,
                       one_way,
                       link_id,
                       display_class=info[3],
                       link_type=info[4],
                       road_type=info[5],
                       toll=info[6],
                       road_kind=info[8],
                       road_code=info[9],
                       s_angle=info[10],
                       e_angle=info[11],
                       hwymode_f=info[12]
                       )

    def load_hwy_data(self):
        self.load_hwy_links()
        self.__update_inout_flag(self.__graph)
        self.__update_ic_type(self.__graph)
        self.__update_updown(self.__graph)
        self.__load_facility_position()
        self.__load_facility()
        # 无料区间
        self.load_hwy_no_toll_main_links()
        self.__update_ic_type(self.__graph_no_toll)
        self.__update_updown(self.__graph_no_toll)

    def load_hwy_links(self):
        '''HWY link(包括本线和非本线)'''
        self.pg.connect2()
        links = self.__get_hwy_links()
        for info in links:
            link_id = info[0]
            s_node = info[1]
            e_node = info[2]
            one_way = info[7]
            self.__graph.add_link(s_node,
                                  e_node,
                                  one_way,
                                  link_id,
                                  display_class=info[3],
                                  link_type=info[4],
                                  road_type=info[5],
                                  toll=info[6],
                                  road_kind=info[8],
                                  road_code=info[9],
                                  s_angle=info[10],
                                  e_angle=info[11],
                                  hwymode_f=info[12],
                                  length=info[13],
                                  tile_id=info[14]
                                  )
            self.__graph.add_node(s_node, tile_id=info[15])
            self.__graph.add_node(e_node, tile_id=info[16])

    def __get_hwy_main_links(self):
        '''高速本线link, 不包括无料区间本线link'''
        sqlcmd = '''
        SELECT link_id, s_node, e_node,
               display_class, link_type, road_type,
               toll, one_way_code, lineclass_c,
               roadcode, fazm, tazm,
               hwymode_f, tile_id
          FROM mid_hwy_main_link
          WHERE hwymode_f = 1;
        '''
        self.pg.connect2()
        return self.pg.get_batch_data2(sqlcmd)

    def __get_hwy_no_toll_main_links(self):
        '''无料区间本线link(road_code > 600000)'''
        sqlcmd = '''
        SELECT link_id, s_node, e_node,
               display_class, link_type, road_type,
               toll, one_way_code, -1 as lineclass_c,
               roadcode, fazm, tazm,
               hwymode_f, tile_id
          FROM mid_hwy_main_link_no_toll;
        '''
        self.pg.connect2()
        return self.pg.get_batch_data2(sqlcmd)

    def __get_hwy_links(self):
        sqlcmd = """
        SELECT hwy.link_id,
               hwy.s_node,
               hwy.e_node,
               hwy.display_class,
               hwy.link_type,
               hwy.road_type,
               hwy.toll,
               hwy.one_way_code,
               lineclass_c,
               roadcode,
               hwy.fazm,
               hwy.tazm,
               hwymode_f,
               length,
               hwy.tile_id,
               s.tile_id as s_tile_id,
               e.tile_id as e_tile_id
          FROM (
                SELECT link_id, s_node, e_node, display_class,
                       road_type, link_type, toll, one_way_code,
                       fazm, tazm, length, tile_id
                  FROM link_tbl
                  where road_type in (0, 1) or toll = 1
          ) as hwy
          left join mid_hwy_main_link as m
          ON hwy.link_id = m.link_id
          LEFT JOIN node_tbl as s
          ON hwy.s_node = s.node_id
          LEFT JOIN node_tbl as e
          ON hwy.e_node = e.node_id
        """
        return self.pg.get_batch_data2(sqlcmd)

    def __load_facility_position(self):
        sqlcmd = '''
        SELECT road_code,
               road_seq as road_point,
               array_agg(node_id),
               array_agg(facilclass_c),
               array_agg(inout_c),
               array_agg(name_kanji),
               array_agg(direction_c) as updwown
          FROM mid_road_hwynode_middle_nodeid
          where "tollclass_c" > 0 and "tollfunc_c" = 0
          group by road_code, road_seq
        '''
        data = self.pg.get_batch_data2(sqlcmd)
        for pos_info in data:
            road_code = pos_info[0]
            road_point = pos_info[1]
            node_lid = pos_info[2]
            facilcls_list = pos_info[3]  # 设施种别
            inout_list = pos_info[4]
            fac_names = pos_info[5]
            updowns = pos_info[6]
            pos_list = []
            for facilcls, node_id, facilcls, inout, name, updown in \
                    zip(facilcls_list, node_lid, facilcls_list,
                        inout_list, fac_names, updowns):
                pos = HwyFacilPositionInfo(road_code, road_point, facilcls,
                                           updown, node_id, inout, name)
                pos_list.append(pos)
                # 更新IC设施所在点
                if node_id in self.__graph:
                    self.__graph.add_node(node_id, ic_position=True)
                self.__graph_no_toll.add_node(node_id, ic_position=True)
            key = (road_code, road_point)
            self.__hwy_facility_pos_dict[key] = pos_list

    def __load_facility(self):
        sqlcmd = '''
        SELECT road_code,
               road_seq as road_point,
               direction_c as updwown,
               array_agg(facilclass_c),
               array_agg(node_id),
               array_agg(inout_c),
               array_agg(name_kanji),
               array_agg(name_yomi)
          FROM mid_road_hwynode_middle_nodeid
          where hwymode_f = 1 and
                (("tollclass_c" = 0 and "tollfunc_c" = 0)
                or (facilclass_c = 6 and tollclass_c = 1))
          group by road_code, road_point, direction_c
          order by road_code, direction_c, road_point;
        '''
        data = self.pg.get_batch_data2(sqlcmd)
        for fac_info in data:
            roadcode = fac_info[0]
            roadpoint = fac_info[1]
            updown = fac_info[2]
            facilcls_list = fac_info[3]
            node_lid = fac_info[4]
            inout_list = fac_info[5]
            facil_names = fac_info[6]
            name_yomis = fac_info[7]
            ficilties = []
            for facilcls, nodeid, inout, facil_name, name_yomi in \
                    zip(facilcls_list, node_lid, inout_list,
                        facil_names, name_yomis):
                fac = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                   updown, nodeid, inout,
                                   facil_name, name_yomi)
                ficilties.append(fac)
            if ficilties:
                key = (roadcode, roadpoint, updown)
                self.__hwy_facility_dict[key] = ficilties

    def __update_inout_flag(self, G):
        self.CreateFunction2('mid_check_hwy_in_out')
        sqlcmd = '''
        SELECT node, mid_check_hwy_in_out(node) in_out_flag
          FROM (
                SELECT s_node as node
                  FROM link_tbl
                  where road_type in (0, 1) or toll = 1

                union

                SELECT e_node as node
                  FROM link_tbl
                  where road_type in (0, 1) or toll = 1
          ) as a
        '''
        nodes = self.pg.get_batch_data2(sqlcmd)
        for inout_info in nodes:
            nodeid = inout_info[0]
            inout_flag = inout_info[1]
            #print nodeid, inout_flag
            G.set_inout_flag(nodeid, inout_flag)

    def __update_ic_type(self, G):
        '''设施种别'''
        sqlcmd = """
        SELECT node_id, array_agg(facilclass_c), array_agg(inout_c)
          FROM (
                SELECT node_id, road_code, road_seq,
                       facilclass_c, direction_c as updown_flag, inout_c
                  FROM mid_road_hwynode_middle_nodeid
                  where hwymode_f = 1 and
                       (("tollclass_c" = 0 and "tollfunc_c" = 0)
                          or facilclass_c = 6)
                  order by node_id, facilclass_c, objectid
          ) as a
          group by node_id
          order by node_id
        """
        data = self.pg.get_batch_data2(sqlcmd)
        for type_info in data:
            nodeid = type_info[0]
            types = type_info[1]
            inout = type_info[2]
            ic_types = zip(types, inout)
            if nodeid in G:
                G.set_ic_type(nodeid, ic_types)

    def __update_updown(self, G):
        '''设施种别'''
        sqlcmd = """
        SELECT node_id, array_agg(updown_flag)
          from (
          SELECT distinct node_id, direction_c as updown_flag
            FROM mid_road_hwynode_middle_nodeid
            where hwymode_f = 1 and "tollclass_c" = 0 and "tollfunc_c" = 0
            order by node_id
          ) as a
          group by node_id
        """
        data = self.pg.get_batch_data2(sqlcmd)
        for type_info in data:
            nodeid = type_info[0]
            updown_list = type_info[1]
            if len(set(updown_list)) != 1:
                continue
            if nodeid in G:
                G.set_updown(nodeid, updown_list[0])

    def get_facil_nodeid(self, roadcode, roadpoint, updown):
        node_ids = []
        sqlcmd = """
        SELECT array_agg(node_id)
          FROM mid_road_hwynode_middle_nodeid
          where road_code = %s
                and road_seq = %s
                and direction_c = %s
          group by road_code;
        """
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown))
        data = self.pg.fetchone2()
        if data:
            node_ids = data[0]
        return node_ids

    def get_first_facil(self, roadcode, updown):
        '''路的第一个设施'''
        sqlcmd = """
        SELECT road_code, road_seq, updown_c
          FROM highway_text
          WHERE (road_code, updown_c, order_seq) in (
                SELECT road_code, updown_c, min(order_seq) as min_order_seq
                  FROM highway_text
                  where road_code = %s and updown_c = %s and order_seq <> 0
                  group by road_code, updown_c
          );
        """
        self.pg.execute2(sqlcmd, (roadcode, updown))
        data = self.pg.fetchall2()
        facil_list = []
        for facil_info in data:
            roadcode = facil_info[0]
            roadpoint = facil_info[1]
            updown = facil_info[2]
            facil_list += self.get_hwy_facils(roadcode, roadpoint,
                                              updown, INOUT_TYPE_IN)
            facil_list += self.get_hwy_facils(roadcode, roadpoint,
                                              updown, INOUT_TYPE_OUT)
        return facil_list

    def get_pos_infos(self, roadcode, roadpoint, updown, fac_cls, inout):
        pos_infos = []
        key = (roadcode, roadpoint)
        pos_list = self.__hwy_facility_pos_dict.get(key)
        if pos_list:
            for pos_info in pos_list:
                if (pos_info.match_updown(updown) and
                    pos_info.match_facilty_type(fac_cls) and
                    pos_info.match_inout(inout)):
                    pos_infos.append(pos_info)
        return pos_infos

    def get_hwy_facils(self, roadcode, roadpoint, updown, inout=None):
        '''[HwyFacility]'''
        facils = []
        key = (roadcode, roadpoint, updown)
        fac_list = self.__hwy_facility_dict.get(key)
        if fac_list:
            for fac in fac_list:
                if not inout:
                    facils.append(fac)
                elif fac.match_inout(inout):
                    facils.append(fac)
                else:
                    pass
        return facils

    def get_hwy_facils_by_nodeid(self, node_id, t_facilcls=None):
        '''[HwyFacility]'''
        facils = []
        sqlcmd = """
        SELECT road_code, road_seq,
               direction_c, inout_c,
               facilclass_c, name_kanji,
               name_yomi
          FROM mid_road_hwynode_middle_nodeid
          WHERE node_id = %s
          ORDER BY road_code, road_seq
        """
        self.pg.execute2(sqlcmd, (node_id,))
        data = self.pg.fetchall2()
        for facil_info in data:
            roadcode = facil_info[0]
            roadpoint = facil_info[1]
            updown = facil_info[2]
            inout = facil_info[3]
#             facilcls = facil_info[4]
            facil_name = facil_info[5]
            name_yomi = facil_info[6]
            temp_facils = []
            key = (roadcode, roadpoint, updown)
            fac_list = self.__hwy_facility_dict.get(key)
            if fac_list:
                for fac_info in fac_list:
                    if node_id == fac_info.node_id:
                        if not t_facilcls:
                            temp_facils.append(fac_info)
                        elif t_facilcls == fac_info.facilcls:
                            temp_facils.append(fac_info)
                        else:
                            pass
            # 料金所
            if not temp_facils and t_facilcls == IC_TYPE_TOLL:
                facil_info = HwyFacilInfo(roadcode, roadpoint, t_facilcls,
                                          updown, node_id, inout,
                                          facil_name, name_yomi)
                temp_facils.append(facil_info)
            facils += temp_facils
        return facils

    def get_hwy_in_facils(self, roadcode, roadpoint, updown, inout):
        '''取得入口: [HwyFacility]'''
        facils = []
        key = (roadcode, roadpoint, updown)
        fac_list = self.__hwy_facility_dict.get(key)
        if fac_list:
            for fac in fac_list:
                if fac.match_fac_in(inout):
                    facils.append(fac)
        return facils

    def get_hwy_opposite_in_facils(self, roadcode, roadpoint, updown):
        '''取得对面入口(U-turn用)：[HwyFacility]'''
        updown = (~updown) & 3
        return self.get_hwy_in_facils(roadcode, roadpoint,
                                      updown, INOUT_TYPE_OUT)

    def get_hwy_forward_branches(self, node_id, roadcode,
                                 roadpoint, updown, inout):
        '''取得前方分歧
        [HwyFacility]
        '''
        branches = self.__get_hwy_forward_branches(roadcode, roadpoint, updown)
        if not branches:
            # ## 取得父设施的branches
            parent_facils = self.get_parent_facil(roadcode, roadpoint,
                                                  updown, inout)
            if parent_facils:
                parent = parent_facils[0]
                branches = self.__get_hwy_forward_branches(parent.road_code,
                                                           parent.road_point,
                                                           parent.updown)
        same_facils = self.get_same_facil(roadcode, roadpoint, updown, inout)
        # 去掉合并设施的分歧
        branches = self._delete_same_branch(node_id, branches, same_facils)
        return branches

    def _delete_same_branch(self, node_id, branches, same_facils):
        '''去掉合并设施的分歧'''
        temp_branches = list(branches)
        for same in same_facils:
            if same.node_id != node_id:
                continue
            for branch in branches:
                if (same.facilcls == branch.facilcls and
                    same.facil_name == branch.facil_name):
                    if branch in temp_branches:
                        temp_branches.remove(branch)
        return temp_branches

    def __get_hwy_forward_branches(self, roadcode, roadpoint, updown):
        '''取得分歧
        [HwyFacility]
        '''
        targets = []
        children = []
        sqlcmd = """
                SELECT road_code, road_seq, updown_c
                  FROM highway_text as a
                  where objectid in (
                   SELECT unnest((array[fore1_id, fore2_id, fore3_id,
                                  fore4_id])[1:fore_count]) as objectid
                     FROM highway_text as a
                      where road_code = %s
                            and road_seq = %s
                            and updown_c = %s
                            and  fore_count > 0
                  )
        """
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown))
        data = self.pg.fetchall2()
        for facil_info in data:
            target_roadcode = facil_info[0]
            target_roadpoint = facil_info[1]
            target_updown = facil_info[2]
            targets += self.get_hwy_facils(target_roadcode, target_roadpoint,
                                           target_updown, INOUT_TYPE_IN)
            children += self.get_children_facil(target_roadcode,
                                                target_roadpoint,
                                                target_updown,
                                                INOUT_TYPE_IN)
        branches = self.__delete_repeat_facil(targets, children)
        return branches

    def __delete_repeat_facil(self, targets, children):
        branches = []
        removed_targets = list(targets)
        removed_children = list(children)
        for child in children:
            for target in targets:
                if child.node_id == target.node_id:
                    if (child.facilcls == IC_TYPE_JCT and
                        target.facilcls == IC_TYPE_JCT):
                        # self.log.warning('parent and child repeat. node=%s'
                        #                % (child.node_id))
                        pass
                    elif (child.facilcls != IC_TYPE_JCT and
                          target.facilcls == IC_TYPE_JCT):
                        if child in removed_children:
                            removed_children.remove(child)
                    elif (child.facilcls == IC_TYPE_JCT and
                          target.facilcls != IC_TYPE_JCT):
                        if target in removed_targets:
                            removed_targets.remove(target)
                    else:
                        # self.log.warning('parent and child repeat. node=%s'
                        #                % (child.node_id))
                        pass
        children = list(removed_children)
        for child in children:
            if child.facilcls != IC_TYPE_JCT:
                for child2 in children:
                    if child != child2 and child.node_id == child2.node_id:
                        if child2.facilcls == IC_TYPE_JCT:
                            removed_children.remove(child)
                            break
                        else:
                            # self.log.warning('Children repeat. node=%s'
                            #                 % (child.node_id))
                            pass
        branches += removed_targets
        branches += removed_children
        return branches

    def __get_hwy_same_forward_branches(self, nodeid):
        '''取得并合设施的分歧
        '''
        branches = []
        sqlcmd = """
        SELECT road_code, road_seq, direction_c as updown
          FROM mid_road_hwynode_middle_nodeid
          WHERE node_id = %s
        """
        self.pg.execute2(sqlcmd, (nodeid,))
        data = self.pg.fetchall2()
        for same_facil in data:
            roadcode = same_facil[0]
            roadpoint = same_facil[1]
            updown = same_facil[2]
            branches += self.__get_hwy_forward_branches(roadcode,
                                                        roadpoint,
                                                        updown)
        return branches

    def get_parent_facil(self, roadcode, roadpoint, updown, inout=None):
        '''取得父设施: [HwyFacility]'''
        parents = []
        sqlcmd = '''
        SELECT road_code, road_seq, updown_c, inout_c
          FROM highway_text as a
          where objectid in (
                SELECT parent_id
                  FROM highway_text as a
                  where road_code = %s
                        and road_seq = %s
                        and updown_c = %s
            )
        '''
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown))
        data = self.pg.fetchall2()
        for facil_info in data:
            p_roadcode = facil_info[0]
            p_roadpoint = facil_info[1]
            p_updown = facil_info[2]
            parents += self.get_hwy_facils(p_roadcode, p_roadpoint,
                                           p_updown, inout)
        # 出口没有分歧信息，只有入口才有
        if not parents:
            if inout:
                inout = (~inout) & 3
            # case: road_code = 207010 and road_seq = 40 and direction_c = 1
            for facil_info in data:
                p_roadcode = facil_info[0]
                p_roadpoint = facil_info[1]
                p_updown = facil_info[2]
                parents += self.get_hwy_facils(p_roadcode, p_roadpoint,
                                               p_updown, inout)
        return parents

    def is_parent_facil(self, roadcode, roadpoint, updown):
        '''是父设施'''
        sqlcmd = '''
        SELECT parent_id
          FROM highway_text as a
          where road_code = %s
          and road_seq = %s
          and updown_c = %s
        '''
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown))
        row = self.pg.fetchone2()
        if row:
            parent_id = row[0]
            if not parent_id:  # 为0，是父设施
                return True
        return False

    def get_children_facil(self, roadcode, roadpoint, updown, inout=None):
        '''取得子设施: [HwyFacility]'''
        children = []
        sqlcmd = '''
          SELECT road_code, road_seq, updown_c, inout_c
             FROM highway_text as a
                  where parent_id in (
                        SELECT objectid
                          FROM highway_text as a
                          where road_code = %s
                                and road_seq = %s
                                and updown_c = %s
                                and inout_c in (0, %s)
             )
        '''
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown, inout))
        data = self.pg.fetchall2()
        for facil_info in data:
            c_roadcode = facil_info[0]
            c_roadpoint = facil_info[1]
            c_updown = facil_info[2]
            children += self.get_hwy_facils(c_roadcode, c_roadpoint,
                                            c_updown, inout)
        return children

    def get_same_facil(self, roadcode, roadpoint, updown, inout=None):
        '''取得并列设施，不包括自己: [HwyFacility]'''
        # ## 它的子设施
        sqlcmd = """
        select road_code, road_seq, updown_c
          from highway_text
          where parent_id in (
                SELECT objectid
                 FROM highway_text as a
                 where road_code = %s
                       and road_seq = %s
                       and updown_c = %s
           );
        """
        same_facils = []
        self.pg.execute2(sqlcmd, (roadcode, roadpoint, updown))
        data = self.pg.fetchall2()
        for facil_info in data:
            c_roadcode = facil_info[0]
            c_roadpoint = facil_info[1]
            c_updown = facil_info[2]
            same_facils += self.get_hwy_facils(c_roadcode, c_roadpoint,
                                               c_updown, None)
        if same_facils:
            return same_facils
        # ## 父设施的并列设施
        ###################################################################
        parents = self.get_parent_facil(roadcode, roadpoint, updown, inout)
        if not parents:
            return []
        same_facils = list(parents)
        p = parents[0]
        parent_same_facils = self.get_same_facil(p.road_code,
                                                 p.road_point,
                                                 p.updown)
        for same_facil in parent_same_facils:
            if (same_facil.road_code == roadcode and
                same_facil.road_point == roadpoint and
                same_facil.updown == updown and
                same_facil.inout == inout):
                pass
            else:
                same_facils.append(same_facil)
        return same_facils

    def is_same_facil(self, s_facil, t_facil):
        '''判断两个设施是并列设施'''
        if not s_facil or not t_facil:
            return False
        same_facils = self.get_same_facil(s_facil.road_code,
                                          s_facil.road_point,
                                          s_facil.updown,
                                          None)
        for same in same_facils:
            if (same.road_code == t_facil.road_code and
                same.road_point == t_facil.road_point and
                same.updown == t_facil.updown and
                same.node_id == t_facil.node_id):
                return True
        return False

    def get_main_path(self, road_code, updown):
        key = (road_code, updown)
        if not self.__main_path_dict:
            self.__load_main_path()
        if self.__main_path_dict:
            return self.__main_path_dict.get(key)
        return None

    def __load_main_path(self):
        sqlcmd = """
        SELECT road_code, updown_flag, array_agg(node_id)
          FROM (
                SELECT gid, road_code, updown_flag, node_id
                  FROM mid_hwy_main_link_path
                  order by road_code, updown_flag, gid
          ) AS A
          group by road_code, updown_flag;
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for main_path in data:
            road_code = main_path[0]
            updown = main_path[1]
            path = main_path[2]
            key = (road_code, updown)
            self.__main_path_dict[key] = path

    def get_ic_no(self, facil):
        sqlcmd = """
        SELECT ic_no
          FROM mid_hwy_ic_no
          where node_id = %s and road_code = %s
                and road_point = %s and updown = %s;
        """
        self.pg.execute2(sqlcmd, (facil.node_id, facil.road_code,
                                  facil.road_point, facil.updown))
        row = self.pg.fetchone2()
        if row:
            return row[0]
        return None

    def get_road_no(self, roadcode):
        if not self.__road_no_dict:
            self._load_road_no()
        road_info = self.__road_no_dict.get(roadcode)
        if road_info:
            roadno = road_info[0]
            return roadno
        else:
            return None

    def get_road_attrs(self, roadcode):
        if not self.__road_no_dict:
            self._load_road_no()
        road_info = self.__road_no_dict.get(roadcode)
        if road_info:
            attr_list = road_info[1]
            return attr_list
        else:
            return []

    def get_road_name(self, roadcode):
        '''(name_kanji, name_yomi)'''
        if not self.__road_no_dict:
            self._load_road_no()
        road_info = self.__road_no_dict.get(roadcode)
        if road_info:
            return road_info[2]
        else:
            return None, None

    def get_road_kind(self, roadcode, updown):
        attr_list = self.get_road_attrs(roadcode)
        attr_dict = {UPDOWN_TYPE_UP: (ROAD_ATTR_UP,
                                      ROAD_ATTR_IN,
                                      ROAD_ATTR_WEST),
                     UPDOWN_TYPE_DOWN: (ROAD_ATTR_DOWN,
                                        ROAD_ATTR_OUT,
                                        ROAD_ATTR_EAST)
                     }
        attr_map = attr_dict.get(updown)
        for road_kind in attr_list:
            if road_kind in attr_map:
                return road_kind
        return ROAD_ATTR_NONE

    def _load_road_no(self):
        sqlcmd = """
        SELECT a.road_no, roadcode,
               a.lineclass_c, road_type,
               road_attr, name_kanji,
               name_yomi
         FROM mid_hwy_road_no as a
         LEFT JOIN (
                 SELECT road_no,
                        array_agg(road_attr) as road_attr
                  FROM highway_road_info
                  group by road_no
         ) as b
         on a.road_no = b.road_no
         LEFT JOIN road_code_list AS l
         ON a.roadcode = l.road_code;
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for road in data:
            roadno = road[0]
            roadcode = road[1]
            road_attrs = road[4]
            name = road[5:7]
            self.__road_no_dict[roadcode] = (roadno, road_attrs, name)

    def get_ic_list(self):
        if not self.__ic_list:
            self._load_ic()
        return self.__ic_list

    def get_ic(self, ic_no):
        if not self.__ic_list:
            self._load_ic()
        if self.__ic_list and ic_no > 0 and ic_no <= len(self.__ic_list):
            return self.__ic_list[ic_no - 1]
        else:
            return None, None, None

    def _load_ic(self):
        sqlcmd = """
        SELECT ic_no,
               array_agg(node_id),
               road_code,
               road_point,
               updown,
               facilclass_c,
               array_agg(inout_c),
               facility_id,
               name_kanji,
               name_yomi,
               tollclass_c,
               dummytoll_f
          from (
                SELECT ic_no, a.node_id, a.road_code,
                       road_point, updown, facilclass_c,
                       inout_c, facility_id, a.name_kanji,
                       b.name_yomi, tollclass_c, dummytoll_f
                  FROM mid_hwy_ic_no as a
                  LEFT JOIN (
                        SELECT distinct node_id, road_code, road_seq,
                               name_kanji, name_yomi, tollclass_c,
                               dummytoll_f
                         FROM mid_road_hwynode_middle_nodeid
                  ) as b
                  ON a.node_id = b.node_id and
                     a.road_code = b.road_code
                     and road_point = b.road_seq
                  order by ic_no, gid
          ) as c
          group by ic_no, facility_id, road_code,
                   road_point, updown, facilclass_c,
                   name_kanji, name_yomi, tollclass_c,
                   dummytoll_f
          order by ic_no
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for ic in data:
            ic_no = ic[0]
            nodeids = ic[1]
            roadcode = ic[2]
            roadpoint = ic[3]
            updown = ic[4]
            facilcls = ic[5]
            inouts = ic[6]
            facility_id = ic[7]
            name_kanji = ic[8]
            name_yomi = ic[9]
            tollclass = ic[10]
            dummytoll = ic[11]
            facil_list = []
            for nodeid, inout in zip(nodeids, inouts):
                # get facil info
                facil = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                     updown, nodeid, inout,
                                     name_kanji, name_yomi, facility_id,
                                     tollclass, dummytoll)
                facil_list.append(facil)
            self.__ic_list.append((ic_no, facility_id, facil_list))

#     def get_path_point(self, facil):
#         path_points = set()
#         sqlcmd = """
#         SELECT node_lid, link_lid
#           FROM mid_hwy_ic_path
#         """
#         if (facil.facilcls in (IC_TYPE_JCT, IC_TYPE_SA, IC_TYPE_PA) and
#             facil.inout == INOUT_TYPE_IN):
#             in_flag = True
#             condition = """
#                 WHERE t_road_code = %s and t_road_point= %s
#                   and t_updown = %s and t_node_id = %s
#                   and t_facilcls = %s and t_inout_c = %s
#                   ORDER BY s_road_code, s_road_point, s_updown, s_node_id
#                 """
#         else:
#             in_flag = False
#             condition = """
#                 WHERE s_road_code = %s and s_road_point= %s
#                   and s_updown = %s and s_node_id = %s
#                   and s_facilcls = %s and s_inout_c = %s
#                   ORDER BY t_road_code, t_road_point, t_updown, t_node_id
#                 """
#         sqlcmd = sqlcmd + condition
#         self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
#                                   facil.updown, facil.node_id,
#                                   facil.facilcls, facil.inout))
#         data = self.pg.fetchall2()
#         for point_info in data:
#             node_lid = eval(point_info[0])
#             link_str = point_info[1].replace('{', '(').replace('}', ',)')
#             link_lid = eval(link_str)
#             if not in_flag:
#                 node_id = node_lid[1]  # 取得第二个点，第一个点是HwyNode
#                 link_id = link_lid[0]
#             else:  # JCT/SA/PA IN
#                 node_id = node_lid[-2]
#                 link_id = link_lid[-1]
#             path_points.add((node_id, link_id))
#         return list(path_points)

    def get_position_path_point(self, facil):
        if facil.facilcls == IC_TYPE_JCT:
            return []
        pos_points = set()
        if facil.facilcls in (IC_TYPE_SA, IC_TYPE_PA):
            sqlcmd = """
                select node_id
                  from mid_access_point_middle_nodeid
                  where road_code = %s
                        and road_seq = %s
                        and direction_c = %s
                  ORDER BY node_id;
                """
            self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
                                      facil.updown))
        else:
            sqlcmd = """
                SELECT t_node_id
                  FROM mid_hwy_ic_path
                  WHERE s_road_code = %s and s_road_point= %s
                        and s_updown = %s and s_node_id = %s
                        and s_facilcls = %s and s_inout_c = %s
                  order by t_node_id;
                """
            self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
                                      facil.updown, facil.node_id,
                                      facil.facilcls, facil.inout))
        data = self.pg.fetchall2()
        for point_info in data:
            node_id = point_info[0]
            pos_points.add(node_id)
        return list(pos_points)

    def get_conn_infos(self, facil):
        '''取得接续情报, 返回(设施信息, path, path_type)'''
        conn_infos = []
        if facil.inout == INOUT_TYPE_OUT:
            sqlcmd = """
            SELECT t_road_code, t_road_point,
                   t_updown, t_node_id,
                   t_facilcls, t_inout_c,
                   node_lid, path_type
              FROM mid_hwy_ic_path
              WHERE s_road_code = %s and s_road_point = %s
                    and s_updown = %s and s_node_id = %s
                    and s_facilcls = %s and s_inout_c = %s
                    and path_type in ('JCT', 'UTURN')
              order by t_road_code, t_road_point,
                       t_updown DESC, t_node_id;
            """
        else:
            sqlcmd = """
            SELECT s_road_code, s_road_point,
                   s_updown, s_node_id,
                   s_facilcls, s_inout_c,
                   node_lid, path_type
              FROM mid_hwy_ic_path
              WHERE t_road_code = %s and t_road_point = %s
                    and t_updown = %s and t_node_id = %s
                    and t_facilcls = %s and t_inout_c = %s
                    and path_type in ('JCT', 'UTURN')
              order by s_road_code, s_road_point,
                       s_updown DESC, s_node_id;
            """
        self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
                                         facil.updown, facil.node_id,
                                         facil.facilcls, facil.inout))
        data = self.pg.fetchall2()
        for info in data:
            roadcode, roadpoint = info[0:2]
            updown, nodeid = info[2:4]
            facilcls, inout = info[4:6]
            path, path_type = info[6:8]
            path = eval(path)
            conn_facil = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                      updown, nodeid, inout, '')
            conn_infos.append((conn_facil, path, path_type))
        return conn_infos

    def get_ic_path_info(self, facil):
        '''取得设施的所有路径. 返回(设施信息, link_lid, node_lid, path_type)'''
        path_infos = []
#         if (facil.facilcls in (IC_TYPE_JCT, IC_TYPE_SA, IC_TYPE_PA) and
#             facil.inout == INOUT_TYPE_IN):
        sqlcmd = """
        SELECT s_road_code as road_code, s_road_point as road_point,
               s_updown as updown, s_node_id as node_id,
               s_facilcls as facilcls, s_inout_c as inout,
               link_lid, node_lid,
               path_type, length(node_lid) as len
          FROM mid_hwy_ic_path
          WHERE t_road_code = %s and t_road_point = %s
                and t_updown = %s and t_node_id = %s
                and t_facilcls = %s and t_inout_c = %s
        union
        SELECT t_road_code as road_code, t_road_point as road_point,
               t_updown as updown, t_node_id as node_id,
               t_facilcls as facilcls, t_inout_c as inout,
               link_lid, node_lid,
               path_type, length(node_lid) as len
          FROM mid_hwy_ic_path
          WHERE s_road_code = %s and s_road_point = %s
                and s_updown = %s and s_node_id = %s
                and s_facilcls = %s and s_inout_c = %s
        order by path_type, road_code, road_point, node_id, len
        """
        self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
                                  facil.updown, facil.node_id,
                                  facil.facilcls, facil.inout,
                                  facil.road_code, facil.road_point,
                                  facil.updown, facil.node_id,
                                  facil.facilcls, facil.inout))
        data = self.pg.fetchall2()
        for info in data:
            roadcode, roadpoint = info[0:2]
            updown, nodeid = info[2:4]
            facilcls, inout = info[4:6]
            link_str, node_lid = info[6:8]
            path_type = info[8]
            link_str = link_str.replace('{', '(').replace('}', ',)')
            link_lid = eval(link_str)
            node_lid = eval(node_lid)
            t_facil = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                   updown, nodeid, inout, '')
            path_infos.append((t_facil, link_lid, node_lid, path_type))
        return path_infos

    def get_next_road_point(self, facil):
        '''前方设施号'''
        sqlcmd = '''
        SELECT road_code, road_seq, order_seq,
               updown_c, inout_c, name_kanji
          FROM highway_text
          where objectid in (
                SELECT next_id
                  FROM highway_text
                  where road_code = %s and road_seq = %s and updown_c = %s
                        and next_id <> 0
                union
                SELECT next_id
                  FROM highway_text
                  where objectid in (
                        SELECT parent_id
                          FROM highway_text
                          where road_code = %s and road_seq = %s
                                and updown_c = %s
                  ) and next_id <> 0
          );
        '''
        self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
                                  facil.updown, facil.road_code,
                                  facil.road_point, facil.updown))
        row = self.pg.fetchone2()
        if row:
            return row[1]
        else:
            return 0

    def get_prev_road_point(self, facil):
        '''前方设施号'''
        sqlcmd = '''
        SELECT road_code, road_seq, order_seq,
               updown_c, inout_c, name_kanji
          FROM highway_text
          where objectid in (
                SELECT prev_id
                  FROM highway_text
                  where road_code = %s and road_seq = %s and updown_c = %s
                        and next_id <> 0
                union
                SELECT prev_id
                  FROM highway_text
                  where objectid in (
                        SELECT parent_id
                          FROM highway_text
                          where road_code = %s and road_seq = %s
                                and updown_c = %s
                  ) and next_id <> 0
          );
        '''
        self.pg.execute2(sqlcmd, (facil.road_code, facil.road_point,
                                  facil.updown, facil.road_code,
                                  facil.road_point, facil.updown))
        row = self.pg.fetchone2()
        if row:
            return row[1]
        else:
            return 0

    def is_toll_node(self, nodeid):
        if not self.__toll_dict:
            self._load_toll_facil()
        return nodeid in self.__toll_dict

    def get_toll_facil(self, nodeid):
        if not self.__toll_dict:
            self._load_toll_facil()
        return self.__toll_dict[nodeid]

    def _load_toll_facil(self):
        '''加载料金信息。'''
        sqlcmd = """
        SELECT node_id,
               array_agg(road_code), array_agg(road_seq),
               array_agg(direction_c), array_agg(inout_c),
               array_agg(tollclass_c),array_agg(tollfunc_c),
               array_agg(dummytoll_f), array_agg(guide_f),
               array_agg(tollgate_lane), array_agg(name_kanji),
               array_agg(name_yomi), array_agg(facilclass_c),
               array_agg(dummyfacil_f)
          from (
                SELECT node_id, road_code, road_seq, direction_c,
                       inout_c, tollclass_c, tollfunc_c,
                       dummytoll_f, guide_f, tollgate_lane,
                       name_kanji, name_yomi,
                       facilclass_c, dummyfacil_f
                  FROM mid_road_hwynode_middle_nodeid
                  WHERE hwymode_f = 1 and  tollclass_c in (1, 2, 3)
                  order by node_id, road_code, road_seq, direction_c
          ) as a
          group by node_id
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for info in data:
            nodeid = info[0]
            toll_info_list = []
            for (roadcode, roadpoint, updown, inout,
                 tollclass, tollfunc_c, dummytoll, guide_f,
                 tollgate_lane, facil_name, name_yomi, facilcls,
                 dummyfacil) in \
                 zip(info[1], info[2], info[3], info[4],
                     info[5], info[6], info[7], info[8],
                     info[9], info[10], info[11], info[12],
                     info[13]):
                toll_info = TollFacilInfo(roadcode, roadpoint, facilcls,
                                  updown, nodeid, inout,
                                  facil_name, name_yomi, tollclass,
                                  dummytoll, tollfunc_c, guide_f,
                                  tollgate_lane, dummyfacil)
                toll_info_list.append(toll_info)
            self.__toll_dict[nodeid] = toll_info_list

    def get_facility_id(self, roadcode, roadpoint):
        if not self.__facility_dict:
            self._load_facility_dict()
        return self.__facility_dict.get((roadcode, roadpoint))

    def _load_facility_dict(self):
        sqlcmd = """
        SELECT facility_id, road_code, road_point
          FROM mid_hwy_facility_id;
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for info in data:
            facility_id, roadcode, roadpoint = info
            self.__facility_dict[(roadcode, roadpoint)] = facility_id

    def get_link_fwd_bwd_facil(self, link_id):
        '''取得link的前后设施情报。'''
        if not self.__link_fwd_bwd_dict:
            self._load_link_fwd_bwd_facil()
        bwd_fwd_facil = self.__link_fwd_bwd_dict.get(link_id)
        if bwd_fwd_facil:
            bwd_list = bwd_fwd_facil[0]
            fwd_list = bwd_fwd_facil[1]
            return bwd_list, fwd_list
        else:
            return [], []

    def get_link_type(self, link_id):
        if not self.__link_fwd_bwd_dict:
            self._load_link_fwd_bwd_facil()
        bwd_fwd_facil = self.__link_fwd_bwd_dict.get(link_id)
        if bwd_fwd_facil:
            link_type = bwd_fwd_facil[2]
            return link_type
        else:
            return None

    def _load_link_fwd_bwd_facil(self):
        sqlcmd = """
        select a.link_id,
               array_agg(backward_node_id) as bwd_node_ids,
               array_agg(bwd_facility_id) as bwd_facility_ids,
               array_agg(forward_node_id) as fwd_node_ids,
               array_agg(fwd_facility_id) as fwd_facility_ids,
               array_agg(path_type) as path_types,
               link_type
          from mid_hwy_ic_link_temp_mapping as a
          LEFT JOIN link_tbl
          ON a.link_id = link_tbl.link_id
          group by a.link_id, link_type
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for info in data:
            link_id = info[0]
            bwd_node_ids, bwd_ic_nos = info[1:3]
            fwd_node_ids, fwd_ic_nos = info[3:5]
            path_types, link_type = info[5:7]
            # 后方设施及料金
            bwd_list = []
            fwd_list = []
            for (bwd_node_id, bwd_facility_id, fwd_node_id,
                 fwd_facility_id, path_type) in \
                zip(bwd_node_ids, bwd_ic_nos, fwd_node_ids,
                    fwd_ic_nos, path_types):
                bwd = {'node_id': bwd_node_id,
                       'facility_id': bwd_facility_id,
                       'path_type': path_type}
                if bwd not in bwd_list:
                    bwd_list.append(bwd)
                fwd = {'node_id': fwd_node_id,
                       'facility_id': fwd_facility_id,
                       'path_type': path_type}
                if fwd not in fwd_list:
                    fwd_list.append(fwd)
            self.__link_fwd_bwd_dict[link_id] = [bwd_list, fwd_list, link_type]

    def is_sapa_pos_node(self, nodeid):
        '''判断nodeid是不是SAPA设施所在点'''
        if not self.__sapa_dict:
            self.__load_sapa_postion()
        if nodeid in self.__sapa_dict:
            return True
        else:
            return False

    def get_sapa_pos_facil(self, nodeid):
        if not self.__sapa_dict:
            self.__load_sapa_postion()
        return self.__sapa_dict.get(nodeid)

    def __load_sapa_postion(self):
        '''SAPA设施所在位置信息'''
        sqlcmd = """
        SELECT node_id,
               array_agg(road_code) as road_codes,
               array_agg(road_seq) as road_seqs,
               array_agg(direction_c) as updowns,
               array_agg(inout_c) as inout_cs,
               array_agg(facilclass_c) as facilclass_cs,
               array_agg(name_kanji) as name_kanjis,
               array_agg(name_yomi) as name_yomis
          FROM (
                SELECT distinct node_id, a.road_code, a.road_seq, direction_c,
                       inout_c, facilclass_c, b.name_kanji, name_yomi
                  FROM mid_access_point_middle_nodeid as a
                  LEFT JOIN facil_info_point_4326 as b
                  ON a.road_code = b.road_code and a.road_seq = b.road_seq
                  WHERE hwymode_f = 1 and facilclass_c in (1, 2)
          ) AS c
          group by node_id;
        """
        self.pg.execute2(sqlcmd)
        data = self.pg.fetchall2()
        for info in data:
            nodeid = info[0]
            facil_list = []
            for (roadcode, roadpoint, updown, inout, facilcls, name_kanji,
                name_yomi) in zip(info[1], info[2], info[3], info[4],
                                  info[5], info[6], info[7]):
                facil = HwyFacilPositionInfo(roadcode, roadpoint, facilcls,
                                             updown, nodeid, inout,
                                             name_kanji, name_yomi)
                facil_list.append(facil)
            self.__sapa_dict[nodeid] = facil_list

    def get_no_toll_facil_infos(self):
        sqlcmd = """
        select distinct s_node_id, s_road_code,
               s_road_point, s_updown,
               s_facilcls, s_inout_c,
               facility_id, name_kanji,
               name_yomi
          from mid_hwy_ic_path AS a
          LEFT JOIN (
                SELECT distinct node_id, road_code, road_seq,
                       name_kanji, name_yomi, tollclass_c,
                       dummytoll_f
                 FROM mid_road_hwynode_middle_nodeid
          ) as b
          ON s_node_id = b.node_id
             and s_road_code = b.road_code
             and s_road_point = b.road_seq
          LEFT JOIN mid_hwy_facility_id as c
          ON s_road_code = c.road_code and s_road_point = c.road_point
          where toll_flag = 0
        """
        for info in self.get_batch_data(sqlcmd):
            nodeid = info[0]
            roadcode = info[1]
            roadpoint = info[2]
            updown = info[3]
            facilcls = info[4]
            inout = info[5]
            facility_id = info[6]
            facil_name = info[7]
            name_yomi = info[8]
            facil_info = HwyFacilInfo(roadcode, roadpoint, facilcls,
                                      updown, nodeid, inout,
                                      facil_name, name_yomi, facility_id)
            yield facility_id, facil_info

    def is_road_start_end_node(self, road_code, updown, node_id):
        if not node_id:
            return False
        self.__load_road_start_end_node()
        key = (road_code, updown)
        s_e_nodes = self.__road_start_end_node.get(key)
        if s_e_nodes and node_id in s_e_nodes:
            return True
        return False

    def __load_road_start_end_node(self):
        if self.__road_start_end_node:
            return
        sqlcmd = """
        SELECT road_code, updown_flag,
               node_array[1] AS start_node,
               node_array[array_upper(node_array, 1)] AS end_node
          from (
            SELECT road_code, updown_flag,
                   array_agg(node_id) as node_array
              FROM (
                SELECT road_code, updown_flag, node_id
                  FROM mid_hwy_main_link_path
                  order by road_code, updown_flag, seqnum
              ) AS a
              group by road_code, updown_flag
          ) as b
        """
        self.pg.execute2(sqlcmd)
        for row in self.pg.fetchall2():
            road_code, updown = row[0:2]
            start_node, end_node = row[2:4]
            key = (road_code, updown)
            self.__road_start_end_node[key] = (start_node, end_node)


#==============================================================================
# HwyFacilityPosition
#==============================================================================
class NodeInfo(object):
    def __init__(self):
        '''
        Constructor
        '''
        self.node_id = None
        self.in_out_flag = False
        self.facility_position_flg = False
        self.hwy_node_flg = False
        self.hwy_facility = []
        self.facility_position = []


#==============================================================================
# HwyFacility
#==============================================================================
class HwyFacilInfo(object):
    def __init__(self, roadcode, roadpoint, facilcls,
                 updown, nodeid, inout,
                 facil_name, name_yomi='', facility_id=HWY_INVALID_FACIL_ID,
                 tollclass=0, dummytoll=0):
        '''
        Constructor
        '''
        self.road_code = roadcode
        self.road_point = roadpoint
        self.facilcls = facilcls
        self.updown = updown
        self.node_id = nodeid
        #self.same_facilities = None  # 并列设施
        #self.is_same_node = False  # 并列设施在同个点上
        #self.same_section_facilities = None  # 介于前后设施之间的并列设施
        self.inout = inout
        self.facil_name = facil_name
        self.name_yomi = name_yomi
        self.facility_id = facility_id
        self.tollclass = tollclass
        self.dummytoll = dummytoll
        self.boundary_flag = 0

    def get_nodeid(self):
        return self.node_id

    def match_fac_in(self, inout):
        if inout == INOUT_TYPE_OUT and self.inout == INOUT_TYPE_IN:
            return True
        return False

    def match_inout(self, inout):
        if inout == self.inout:
            return True
        return False


#==============================================================================
# HwyFacilityPosition
#==============================================================================
class HwyFacilPositionInfo(HwyFacilInfo):
    def __init__(self, roadcode, roadpoint, facilcls,
                 updown, nodeid, inout, facil_name, name_yomi=''):
        '''
        Constructor
        '''
        HwyFacilInfo.__init__(self, roadcode, roadpoint, facilcls,
                              updown, nodeid, inout,
                              facil_name, name_yomi)

    def match_updown(self, fac_updown):
        '''比较HwyNode的上下行和RoadPoint的位置'''
        if (fac_updown == UPDOWN_TYPE_UP and
            self.updown in (UPDOWN_TYPE_UP, UPDOWN_TYPE_COMMON)):
            return True
        if (fac_updown == UPDOWN_TYPE_DOWN and
            self.updown in (UPDOWN_TYPE_DOWN, UPDOWN_TYPE_COMMON)):
            return True
        return False

    def match_facilty_type(self, facilcls):
        '比较施设种别'
        return self.facilcls == facilcls

    def match_inout(self, fac_inout):
        return self.inout == fac_inout


#==============================================================================
# TollFacilInfo(料金信息)
#==============================================================================
class TollFacilInfo(HwyFacilInfo):
    def __init__(self, roadcode, roadpoint, facilcls,
                 updown, nodeid, inout,
                 facil_name, name_yomi, tollclass,
                 dummytoll, tollfunc_c, guide_f,
                 tollgate_lane, dummyfacil):
        '''
        Constructor
        '''
        HwyFacilInfo.__init__(self, roadcode, roadpoint, facilcls,
                              updown, nodeid, inout,
                              facil_name, name_yomi, HWY_INVALID_FACIL_ID,
                              tollclass, dummytoll)
        self.tollfunc = tollfunc_c
        self.guide_f = guide_f
        self.tollgate_lane = tollgate_lane
        self.dummyfacil = dummyfacil  # 架空
