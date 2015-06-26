# -*- coding: UTF-8 -*-
'''
Created on 2014-4-18

@author: hongchenzai
'''
import component.component_base
from component.jdb.hwy.hwy_def import UPDOWN_TYPE_NONE
from component.jdb.hwy.hwy_def import INVALID_ROAD_NO
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import IC_TYPE_RAMP
from component.jdb.hwy.hwy_def import IC_TYPE_IC
from component.jdb.hwy.hwy_def import ROAD_ATTR_NONE
from component.jdb.hwy.hwy_def import IC_TYPE_SMART_IC
from component.jdb.hwy.hwy_def import INVALID_IC_NO
from component.jdb.hwy.hwy_graph import LINK_TYPE_SR
from component.jdb.hwy.hwy_data_mng import HwyDataMng
from component.jdb.hwy.hwy_ic_info import convert_tile_id
from component.jdb.hwy.hwy_graph import ONE_WAY_POSITIVE
from component.jdb.hwy.hwy_graph import ONE_WAY_RERVERSE
PATH_TYPE_MAIN = 'MAIN'
PATH_TYPE_IC = 'IC'
PATH_TYPE_SR = 'SrvRoad'  # 辅路


class HwyMapping(component.component_base.comp_base):
    '''
    Main Link Mapping(本线link的Mapping)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HwyMapping')
        self.link_id = 0
        self.link_type = 0

    def _Do(self):
        self.CreateTable2('highway_mapping')
        self._make_main_link_mapping()
        self._make_ic_link_mapping()
        # 高速/收费 辅路(link_type=6, road_code无)
        self._make_service_road_mapping()
        self.CreateIndex2('highway_mapping_forward_ic_no_idx')
        self.CreateIndex2('highway_mapping_backward_ic_no_idx')
        self.CreateIndex2('highway_mapping_link_id_forward_ic_no_'
                          'backward_ic_no_idx')
        self._check_link()
        return

    def _make_main_link_mapping(self):
        hwy_data = HwyDataMng.instance()
        data = self.__get_main_path()
        self.pg.connect1()
        for info in data:
            road_code = info[0]
            updown = info[1]
            node_lid = info[2]
            link_lid = info[3]
            dispcls = info[4]
            min_ic_nos = info[5]  # 点上的最小设施号
            max_ic_nos = info[6]  # 点上的最大设施号
            cycle_flag = info[7]
            tile_ids = info[8]
            road_kind = hwy_data.get_road_kind(road_code, updown)
            road_no = hwy_data.get_road_no(road_code)
            backward_ic = None  # 后方
            forward_ic = None  # 前方
            section_maps = []  # 上个设施到下个设施之间的link
            road_maps = []  # 整条道路的Link
            for node, link, displayclass, min_ic, max_ic, tile_id in \
            zip(node_lid, link_lid, dispcls, min_ic_nos, max_ic_nos, tile_ids):
                tile_id = convert_tile_id(tile_id)  # 14层tile转成高层tile
                if min_ic and section_maps and not backward_ic:
                    self.log.error('在第一个设施后方有本线link. roadcode=%s' % road_code)
                if min_ic and backward_ic:
                    # 环形的最后一个设施(JCT OUT)到第一个设施(JCT IN), 之间的Link, 不收录
                    # 因为，后面的IC link会收录。
                    if cycle_flag and node == node_lid[0]:
                        # print road_code, len(section_maps)
                        section_maps = []
                        break
                    forward_ic = min_ic
                    link_map = HwyLinkMapping(road_kind, 1, road_no,
                                              displayclass, link,
                                              None, backward_ic,
                                              PATH_TYPE_MAIN, tile_id)
                    section_maps.append(link_map)
                    for link_map in section_maps:
                        link_map.forward_ic_no = forward_ic
                        road_maps.append(link_map)
                    section_maps = []
                    if max_ic:
                        backward_ic = max_ic
                    else:
                        print 'Error'
                    continue
                if max_ic:
                    backward_ic = max_ic
                if link:
                    link_map = HwyLinkMapping(road_kind, 1, road_no,
                                              displayclass, link,
                                              None, backward_ic,
                                              PATH_TYPE_MAIN, tile_id)
                    section_maps.append(link_map)
            self._insert_mapping(road_maps)
        self.pg.commit1()

    def _make_ic_link_mapping(self):
        '''Ramp, SAPA, JCT Link的mapping'''
        self.log.info('Start Make IC Link Mapping.')
        self.__make_ic_link_temp_mapping()
        self.pg.connect1()
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        data = self.__get_forward_backward_ic()
        for fb_info in data:
            link_id = fb_info[0]
            bwd_ic_nos = fb_info[1]
            fwd_ic_nos = fb_info[2]
            bwd_node_ids = fb_info[3]
            fwd_node_ids = fb_info[4]
            displayclass = fb_info[5]
            tile_id = convert_tile_id(fb_info[6])
            self.link_type = fb_info[7]
            self.link_id = link_id
            if not bwd_ic_nos and not fwd_ic_nos:
                #self.log.error('1. No Backward/Forward IC for link=%s'
                #               % link_id)
                continue
            #print link_id
            bwd_ic_list, bwd_facil_list = self._get_ic_facils(bwd_ic_nos,
                                                              bwd_node_ids)
            fwd_ic_list, fwd_facil_list = self._get_ic_facils(fwd_ic_nos,
                                                              fwd_node_ids)
            road_code, updown = self.__get_ic_link_road_code(bwd_facil_list,
                                                             fwd_facil_list)
            if road_code:
                road_no = hwy_data.get_road_no(road_code)
                road_kind = hwy_data.get_road_kind(road_code, updown)
            else:
                road_no = INVALID_ROAD_NO
                road_kind = ROAD_ATTR_NONE
            bwd_ics, fwd_ics = self.__get_ic_link_fb_ic(bwd_ic_list,
                                                        fwd_ic_list)
            if not bwd_ics and not fwd_ics:
                self.log.error('2. No Backward/Forward IC for link=%s' % link_id)
                continue
            link_maps = []
            for bwd_ic, fwd_ic in zip(bwd_ics, fwd_ics):
                # 前后设施都为空——通常是无料区间
                if not bwd_ic and not fwd_ic:
                    self.log.error('3. No Backward/Forward IC for link=%s'
                                   % link_id)
                    continue
                link_map = HwyLinkMapping(road_kind, len(bwd_ics), road_no,
                                          displayclass, link_id,
                                          fwd_ic, bwd_ic,
                                          PATH_TYPE_IC, tile_id)
                link_maps.append(link_map)
            self._insert_mapping(link_maps)
        self.pg.commit1()

    def _get_ic_facils(self, ic_nos, node_ids):
        '''取得IC，以及设施情报。'''
        rst_facils = []
        rst_ic_no = []
        if not ic_nos:
            return rst_ic_no, rst_facils
        for one_node_ics, node_id in zip(ic_nos, node_ids):
            if not one_node_ics:
                continue
            one_node_ics = eval(one_node_ics.replace(')', ',)'))  # 同个点上，有多个设施
            # 当前几个设施中(同个点上)，选择一个优先级最高的
            ic_no, facil = self._get_max_priority_facil(node_id,
                                                        one_node_ics)
            if ic_no and facil:
                rst_ic_no.append(ic_no)
                rst_facils.append(facil)
            else:
                print 'nodeid=%s, ic_nos=%s' % (node_id, one_node_ics)
        return rst_ic_no, rst_facils

    def _get_max_priority_facil(self, node_id, ic_nos):
        '''当前几个设施中(同个点上)，选择一个优先级最高的'''
        # SAPA > JCT > IC; 另外, 同等级时, 父 > 子
        ic_list, facils = self._get_facils(node_id, ic_nos)
        max_facil = facils[0]
        # 取得等级最高的一个设施及ic_no
        for facil in facils[1:]:
            if facil.facilcls in (IC_TYPE_SA, IC_TYPE_PA):
                if (max_facil.facilcls not in (IC_TYPE_SA, IC_TYPE_PA)):
                    max_facil = facil
                else:
                    if max_facil.facilcls == facil.facilcls:
                        # 取得二者中的父设施
                        parent = self._get_parent_facil(max_facil, facil)
                        if parent:
                            max_facil = parent
                        else:
                            self.log.warning('No Parent. link=%s,node=%s'
                                             % (self.link_id, node_id))
                    else:
                        self.log.error('Two SAPA: link=%s, node=%s,'
                                       'facilcls=%s, facilcls=%s'
                                       % (self.link_id, node_id,
                                          max_facil.facilcls, facil.facilcls))
            elif facil.facilcls == IC_TYPE_JCT:
                if (max_facil.facilcls not in (IC_TYPE_SA, IC_TYPE_PA,
                                               IC_TYPE_JCT)):
                    max_facil = facil
                else:
                    if max_facil.facilcls == facil.facilcls:
                        # 取得二者中的父设施
                        parent = self._get_parent_facil(max_facil, facil)
                        if parent:
                            max_facil = parent
                        else:
                            self.log.warning('No Parent. link=%s,node=%s'
                                             % (self.link_id, node_id))
            elif facil.facilcls in (IC_TYPE_RAMP, IC_TYPE_IC,
                                    IC_TYPE_SMART_IC):
                if (max_facil.facilcls in (IC_TYPE_SA, IC_TYPE_PA,
                                           IC_TYPE_JCT)):
                    continue
                else:
                    if max_facil.facilcls == facil.facilcls:
                        # 取得二者中的父设施
                        parent = self._get_parent_facil(max_facil, facil)
#                         max_ic = ic_list[facils.index(max_facil)]
#                         temp_ic = ic_list[facils.index(facil)]
#                         if (max_facil.inout == INOUT_TYPE_OUT and
#                             max_ic < temp_ic):
#                             print max_ic, temp_ic
#                         if (max_facil.inout == INOUT_TYPE_IN and
#                             max_ic > temp_ic):
#                             print max_ic, temp_ic
                        if parent:
                            max_facil = parent
                        else:
                            self.log.warning('No Parent. link=%s,node=%s'
                                             % (self.link_id, node_id))
                    else:
                        self.log.error('IC Type: link=%s, node=%s,'
                                       'facilcls=%s, facilcls=%s'
                                       % (self.link_id, node_id,
                                          max_facil.facilcls, facil.facilcls))
            else:
                self.log.error('Error facil_class. node=%s, facil_cls=%s'
                               % (node_id, facil.facilcls))
        max_proi_ic = ic_list[facils.index(max_facil)]
        sapa_ic, sapa_facil = self._get_same_sapa_for_jct(max_proi_ic,
                                                          max_facil)
        if sapa_ic and sapa_facil:
            return sapa_ic, sapa_facil
        return max_proi_ic, max_facil

    def _get_facils(self, node_id, ic_nos):
        facils = []
        ic_list = []
        hwy_data = HwyDataMng.instance()
        for ic_no in sorted(set(ic_nos)):
            ic_no, facility_id, facil_list = hwy_data.get_ic(ic_no)
            for facil in facil_list:
                if facil.node_id == node_id:
                    # 按设施各别排序: SA > PA > JCT > Ramp > IC > SmartIC
                    ic_idx = 0
                    while ic_idx < len(facils):
                        if facil.facilcls < facils[ic_idx].facilcls:
                            break
                        elif facil.facilcls == facils[ic_idx].facilcls:
                            # 取得二者中的父设施
                            parent = self._get_parent_facil(facils[ic_idx],
                                                            facil)
                            if parent == facil:
                                break
                        else:
                            pass
                        ic_idx += 1
                    facils.insert(ic_idx, facil)
                    ic_list.insert(ic_idx, ic_no)
        return ic_list, facils

    def _get_same_sapa_for_jct(self, ic_no, facil):
        # 取得JCT设施点上的SAPA
        if facil.facilcls != IC_TYPE_JCT:
            return None, None
        sapa_ic, sapa_facil = self._get_same_sapa(ic_no, facil.node_id)
#         if sapa_ic and sapa_facil:
#             if facil.inout == INOUT_TYPE_IN and sapa_ic > ic_no:
#                 print ic_no
#             if facil.inout == INOUT_TYPE_OUT and sapa_ic < ic_no: # 草津ＰＡ
#                 print ic_no
        return sapa_ic, sapa_facil

    def _get_same_sapa(self, ic_no, node_id):
        '''取得并列，且同点的SAPA。'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        if not G.is_sapa(node_id):
            return None, None
        # backward
        bwd_ic_no = ic_no - 1
        while bwd_ic_no:
            bwd_ic_no, facility_id, facil_list = hwy_data.get_ic(bwd_ic_no)
            same_node_flg = False
            for facil in facil_list:
                if facil.node_id == node_id:
                    same_node_flg = True
                    if facil.facilcls in (IC_TYPE_SA, IC_TYPE_PA):
                        return bwd_ic_no, facil
            if not same_node_flg:
                break
            bwd_ic_no -= 1
        # forward
        fwd_ic_no = ic_no + 1
        while True:
            fwd_ic_no, facility_id, facil_list = hwy_data.get_ic(fwd_ic_no)
            if not fwd_ic_no:
                break
            same_node_flg = False
            for facil in facil_list:
                if facil.node_id == node_id:
                    same_node_flg = True
                    if facil.facilcls in (IC_TYPE_SA, IC_TYPE_PA):
                        return fwd_ic_no, facil
            if not same_node_flg:
                break
            fwd_ic_no += 1
        return None, None

    def _get_parent_facil(self, facil_1, facil_2):
        '''判断二者谁是父设施，并返回。如果都不是，返回None。'''
        hwy_data = HwyDataMng.instance()
        parents = hwy_data.get_parent_facil(facil_1.road_code,
                                            facil_1.road_point,
                                            facil_1.updown,
                                            facil_1.inout)
        if not parents:
            parents = hwy_data.get_parent_facil(facil_2.road_code,
                                                facil_2.road_point,
                                                facil_2.updown,
                                                facil_2.inout)
            if not parents:
                return None
        parent = parents[0]
        if facil_2.road_point == parent.road_point:
            return facil_2
        elif facil_1.road_point == parent.road_point:
            return facil_1
        else:
            return None

    def __get_ic_link_fb_ic(self, bwd_ic_nos, fwd_ic_nos):
        '''取得前后设施'''
        # 前后各只有一个设施    前后刚好做成一条记录
        # 后一条以上，前无。    后有几条，做成几条记录，其中所有前都填0
        # 后一条，前大于一条。    只记录后的一条，前填0
        # 前一条以上，后无。    前有几条，做成几条记不，其中所有后都填0
        # 前一条，后大于一条。    只记录前的一条，后填0
        # 前后都是多条的情况    几个后设施做成多条记录，前都填成0
        bwd_len = 0
        fwd_len = 0
        if bwd_ic_nos:
            bwd_len = len(bwd_ic_nos)
        if fwd_ic_nos:
            fwd_len = len(fwd_ic_nos)
        if bwd_len == 1 and fwd_len == 1:
            return bwd_ic_nos, fwd_ic_nos
        if bwd_len > 0 and fwd_len == 0:
            return bwd_ic_nos, [INVALID_IC_NO] * bwd_len
        if bwd_len == 1 and fwd_len > 1:
            return bwd_ic_nos, [INVALID_IC_NO]
        if bwd_len == 0 and fwd_len > 0:
            return [INVALID_IC_NO] * fwd_len, fwd_ic_nos
        if bwd_len > 1 and fwd_len == 1:
            return [INVALID_IC_NO] * fwd_len, fwd_ic_nos
        if bwd_len > 1 and fwd_len > 1:
            return bwd_ic_nos, [INVALID_IC_NO] * bwd_len
        return [], []

    def __get_ic_link_road_code(self, bwd_facls_list, fwd_facls_list):
        # 只有前设施，无后设施 ,且前设施番号，上下行相同==> 前设施道路番号
        # 无前设施，只有后设施,且后设施番号，上下行相同==> 后施道路番号
        # 前后设施所在道路番号相同，且上下行相同==>设施所在道路番号
        # 其他 ==> INVALID_ROAD_NO
        # ## 后设施
        bwd_road_code = None
        bwd_updown = None
        for facil in bwd_facls_list:
            if bwd_road_code and bwd_road_code != facil.road_code:
                return None, UPDOWN_TYPE_NONE
            if bwd_updown and bwd_updown != facil.updown:
                return None, UPDOWN_TYPE_NONE
            bwd_road_code = facil.road_code
            bwd_updown = facil.updown
        # ## 前设施
        fwd_road_code = None
        fwd_updown = None
        for facil in fwd_facls_list:
            if fwd_road_code and fwd_road_code != facil.road_code:
                return None, UPDOWN_TYPE_NONE
            if fwd_updown and fwd_updown != facil.updown:
                return None, UPDOWN_TYPE_NONE
            fwd_road_code = facil.road_code
            fwd_updown = facil.updown
        if bwd_road_code and not fwd_road_code:
            return bwd_road_code, bwd_updown
        if not bwd_road_code and fwd_road_code:
            return fwd_road_code, fwd_updown
        if bwd_road_code == fwd_road_code and bwd_updown == fwd_updown:
            return bwd_road_code, bwd_updown
        return None, UPDOWN_TYPE_NONE

    def __get_forward_backward_ic(self):
        sqlcmd = """
        SELECT a.link_id,
               backward_ic_nos,
               forward_ic_nos,
               backward_node_ids,
               forward_node_ids,
               display_class,
               tile_id,
               link_type
          from (
                SELECT distinct link_id
                  FROM mid_hwy_ic_link_temp_mapping
          ) as a
          LEFT JOIN (
                -- Get backward ic_no
                SELECT link_id,
                       array_agg(backward_node_id) as backward_node_ids,
                       array_agg(bwd_ic_for_one_node) as backward_ic_nos
                  FROM (
                        SELECT link_id, backward_node_id,
                               '(' ||
                                  array_to_string(array_agg(backward_ic_no),
                                  ',') || ')' as bwd_ic_for_one_node
                          FROM (
                                select distinct link_id, backward_node_id,
                                       backward_ic_no
                                  from mid_hwy_ic_link_temp_mapping
                                  where backward_ic_no is not null
                          ) as a
                          group by link_id, backward_node_id
                          --ORDER BY link_id
                  ) AS a
                  group by link_id
          ) as bwd
          ON a.link_id = bwd.link_id
          LEFT JOIN (
                -- Get forward min ic_no
                SELECT link_id,
                       array_agg(forward_node_id) as forward_node_ids,
                       array_agg(fwd_ic_for_one_node) as forward_ic_nos
                  FROM (
                        SELECT link_id, forward_node_id,
                               '(' ||
                                 array_to_string(array_agg(forward_ic_no),
                                  ',') || ')' as fwd_ic_for_one_node
                          FROM (
                                select distinct link_id, forward_node_id,
                                      forward_ic_no
                                  from mid_hwy_ic_link_temp_mapping
                                  where forward_ic_no is not null
                          ) as a
                          group by link_id, forward_node_id
                          --ORDER BY link_id
                  ) AS a
                  group by link_id
           ) as fwd
           ON a.link_id = fwd.link_id
           LEFT JOIN link_tbl
           ON a.link_id::bigint = link_tbl.link_id
           order by link_id;
        """
        return self.pg.get_batch_data2(sqlcmd)

    def __make_ic_link_temp_mapping(self):
        '''做成每条IC Link的每个前后设施。'''
        self.CreateTable2('mid_hwy_ic_link_temp_mapping')
        sqlcmd = """
        INSERT INTO mid_hwy_ic_link_temp_mapping(
                    backward_node_id, backward_ic_no,
                    bwd_facility_id, forward_node_id,
                    forward_ic_no, fwd_facility_id,
                    link_id, path_type,
                    toll_flag)
        (
        SELECT distinct s_node_id as backward_node_id,
               back_ic.ic_no as backward_ic_no,
               bwd_facil.facility_id as bwd_facility_id,
               t_node_id as forward_node_id,
               forward_ic.ic_no as forward_ic_no,
               fwd_facil.facility_id as fwd_facility_id,
               link_id::bigint, path_type,
               toll_flag
          FROM (
                -- Ramp/ic/smartic IN
                SELECT NULL as s_road_code, NULL as s_road_point,
                       NULL as s_updown, NULL as s_node_id,
                       NULL as s_facilcls, NULL as s_inout_c,
                       s_road_code as t_road_code,
                       s_road_point as t_road_point,
                       s_updown as t_updown,
                       s_node_id as t_node_id,
                       s_facilcls as t_facilcls,
                       s_inout_c as t_inout_c,
                       regexp_split_to_table(substr(link_lid, 2,
                          length(link_lid) - 2), E'\\,+') as link_id,
                       path_type, toll_flag
                  FROM mid_hwy_ic_path
                  where path_type = 'IC' and s_inout_c = 1

                union
                -- Ramp/ic/smartic OUT
                SELECT s_road_code, s_road_point, s_updown,
                       s_node_id, s_facilcls, s_inout_c,
                       null as t_road_code, NULL as t_road_point,
                       NULL as t_updown, NULL as t_node_id,
                       NULL as t_facilcls, NULL as t_inout_c,
                       regexp_split_to_table(substr(link_lid, 2,
                          length(link_lid) - 2), E'\\,+') as link_id,
                       path_type, toll_flag
                  FROM mid_hwy_ic_path
                  --where s_facilcls in (4, 5, 7) and s_inout_c = 2
                  where path_type = 'IC' and s_inout_c = 2

                union
                -- SAPA, JCT, U-Turn
                SELECT s_road_code, s_road_point, s_updown,
                       s_node_id, s_facilcls, s_inout_c,
                       t_road_code, t_road_point, t_updown,
                       t_node_id, t_facilcls, t_inout_c,
                       regexp_split_to_table(substr(link_lid, 2,
                          length(link_lid) - 2), E'\\,+') as link_id,
                       path_type, toll_flag
                  FROM mid_hwy_ic_path
                  where path_type <> 'IC'
          ) AS a
          LEFT JOIN mid_hwy_ic_no as back_ic
          ON a.s_node_id = back_ic.node_id and
             a.s_road_code = back_ic.road_code and
             a.s_road_point = back_ic.road_point and
             a.s_inout_c = back_ic.inout_c
          LEFT JOIN mid_hwy_facility_id AS bwd_facil
          ON a.s_road_code = bwd_facil.road_code and
             a.s_road_point = bwd_facil.road_point 
          LEFT JOIN mid_hwy_ic_no as forward_ic
          ON a.t_node_id = forward_ic.node_id and
             a.t_road_code = forward_ic.road_code and
             a.t_road_point = forward_ic.road_point and
             a.t_inout_c = forward_ic.inout_c
          LEFT JOIN mid_hwy_facility_id AS fwd_facil
          ON a.t_road_code = fwd_facil.road_code and
             a.t_road_point = fwd_facil.road_point
          ORDER BY link_id, backward_ic_no, bwd_facility_id,
                   forward_ic_no, fwd_facility_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def __get_main_path(self):
        sqlcmd = """
        SELECT road_code,
               updown_flag,
               array_agg(node_id) as node_lid,
               array_agg(link_id) as link_lid,
               array_agg(display_class) as dispcls,
               array_agg(min_ic_no) as min_ic_nos,
               array_agg(max_ic_no) as max_ic_nos,
               cycle_flag,
               array_agg(tile_id) as tile_ids
          FROM (
                SELECT road_code, updown_flag,
                       p.node_id, p.link_id,
                       display_class, cycle_flag,
                       min_ic_no, max_ic_no,
                       tile_id
                  FROM mid_hwy_main_link_path as p
                  LEFT JOIN link_tbl
                  ON p.link_id = link_tbl.link_id
                  LEFT JOIN (
                        SELECT a.node_id, min_ic_no, max_ic_no
                         from (
                                SELECT node_id, min(ic_no) as min_ic_no
                                  FROM mid_hwy_ic_no
                                  group by node_id
                         ) as a
                         left join (
                                SELECT node_id, max(ic_no) as max_ic_no
                                  FROM mid_hwy_ic_no
                                  group by node_id
                         ) as b
                         ON a.node_id = b.node_id
                  ) as ic
                  ON p.node_id = ic.node_id
                  order by road_code, updown_flag desc, seqnum
          ) as a
          group by road_code, updown_flag, cycle_flag
          order by road_code, updown_flag desc;
        """
        return self.pg.get_batch_data2(sqlcmd)

    def _insert_mapping(self, link_maps):
        sqlcmd = """
        INSERT INTO highway_mapping(
                    road_kind, ic_count,
                    road_no, display_class,
                    link_id, forward_ic_no,
                    backward_ic_no, path_type,
                    tile_id)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s);
        """
        for link_map in link_maps:
            if not link_map.link_id:
                print link_map.forward_ic_no, link_map.backward_ic_no
                continue
            self.pg.execute1(sqlcmd, (link_map.road_kind,
                                      link_map.ic_count,
                                      link_map.road_no,
                                      link_map.display_class,
                                      link_map.link_id,
                                      link_map.forward_ic_no,
                                      link_map.backward_ic_no,
                                      link_map.path_type,
                                      link_map.tile_id)
                             )

    def __delete_mapping(self, link_id):
        sqlcmd = """
        delete from highway_mapping
          where link_id = %s;
        """
        self.pg.execute1(sqlcmd, (link_id,))
        self.pg.commit1()

    def _check_link(self):
        if not self._check_main_link():
            self.log.error('No all main link are included.')
            return False
        if not self._check_ic_link():
            self.log.error('No all IC link are included.')
            return False
        return True

    def _check_main_link(self):
        '''判断本线link都被收录。'''
        sqlcmd = '''
        SELECT count(link_id)
          FROM mid_hwy_main_link
          where hwymode_f = 1 and link_id not in (
                SELECT link_id
                  FROM highway_mapping
          );
        '''
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] == 0:
            return True
        return False

    def _check_ic_link(self):
        '''判断IC link(Ramp,JCT,SAPA)都被收录。'''
        sqlcmd = '''
        SELECT count(link_id)
          FROM mid_hwy_ic_link_temp_mapping
          where link_id not in (
                SELECT link_id
                  FROM highway_mapping)
                and toll_flag = 1;
        '''
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0] == 0:
            return True
        return False

    def _make_service_road_mapping(self):
        '''高速/收费 辅路(link_type=6, road_code无)'''
        road_no = INVALID_ROAD_NO
        road_kind = ROAD_ATTR_NONE
        for link_info in self._get_service_road():
            link_id, s_node, e_node = link_info[0:3]
            one_way, display_class, tile_id = link_info[3:6]
            tile_id = convert_tile_id(tile_id)  # 14层tile转成高层tile
            bwd_ics, fwd_ics = self._get_sv_link_bwd_fwd_ic(link_id, s_node,
                                                            e_node, one_way)
            # 取得道路情报
            road_kind, road_no = self._get_sv_link_road_info(link_id, s_node,
                                                             e_node, one_way)
            path_type = PATH_TYPE_SR   # 辅路
            ic_count = len(bwd_ics)
            # 删除旧的记录
            if self._is_in_hwy_mapping(link_id):
                self.__delete_mapping(link_id)
            link_maps = []
            for bwd_ic, fwd_ic in zip(bwd_ics, fwd_ics):
                if not bwd_ic and not fwd_ic:
                    self.log.error('No Backward/Forward IC for Service link=%s'
                                   % link_id)
                    continue
                # print link_id, road_kind, road_no, fwd_ic, bwd_ic
                link_map = HwyLinkMapping(road_kind, ic_count,
                                          road_no, display_class,
                                          link_id, fwd_ic,
                                          bwd_ic, path_type,
                                          tile_id)
                link_maps.append(link_map)
            self._insert_mapping(link_maps)
        self.pg.commit1()

    def _is_in_hwy_mapping(self, link_id):
        hwy_data = HwyDataMng.instance()
        bwd_list, fwd_list = hwy_data.get_link_fwd_bwd_facil(link_id)
        if bwd_list or fwd_list:
            return True
        return False

    def _get_service_road(self):
        sqlcmd = """
        SELECT link_id, s_node, e_node,
               one_way_code, display_class, tile_id
          FROM link_tbl
          where (road_type in (0, 1) or toll = 1)
                and link_type = 6;
        """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        links = []
        for row in rows:
            links.append(tuple(row))
        return links

    def _get_sv_link_bwd_fwd_ic(self, link_id, s_node, e_node, one_way):
        '''辅路的前、后方设施'''
        bwd_ic_nos = set()
        fwd_ic_nos = set()
        if one_way == ONE_WAY_POSITIVE:
            bwd_ic_nos = self._get_sv_link_bwd_ic_no(s_node)
            fwd_ic_nos = self._get_sv_link_fwd_ic_no(e_node)
        elif one_way == ONE_WAY_RERVERSE:
            bwd_ic_nos = self._get_sv_link_bwd_ic_no(e_node)
            fwd_ic_nos = self._get_sv_link_fwd_ic_no(s_node)
        else:
            self.log.error('No Forward/Backward IC No for service Road. '
                           'link_id=%s.' % link_id)
            return [], []
        if self._is_in_hwy_mapping(link_id):
            # 取得已有前后方设施
            hwy_data = HwyDataMng.instance()
            old_bwd_nos = hwy_data.get_mapping_bwd_ic(link_id)
            old_fwd_nos = hwy_data.get_mapping_fwd_ic(link_id)
            if old_bwd_nos:
                bwd_ic_nos = bwd_ic_nos.union(old_bwd_nos)
                fwd_ic_nos = set()
            elif old_fwd_nos:
                bwd_ic_nos = set()
                fwd_ic_nos = fwd_ic_nos.union(old_fwd_nos)
            else:
                pass

        return self.__get_ic_link_fb_ic(list(bwd_ic_nos),
                                        list(fwd_ic_nos))

    def _get_sv_link_bwd_ic_no(self, node):
        '''辅路的后方设施'''
        bwd_ic_nos = set()
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        for prant, node, data in G.in_edges_iter(node, True):
            link_id = data.get('link_id')
            if not self._is_in_hwy_mapping(link_id):
                temp_bwd_nos = self._get_sv_link_bwd_ic_no(prant)
                bwd_ic_nos = bwd_ic_nos.union(temp_bwd_nos)
            else:
                temp_bwd_nos = set(hwy_data.get_mapping_bwd_ic(link_id))
                bwd_ic_nos = bwd_ic_nos.union(temp_bwd_nos)
                link_type = hwy_data.get_link_type(link_id)
                # 辅路继续找后方ic
                if link_type is None or link_type == LINK_TYPE_SR:
                    temp_bwd_nos = self._get_sv_link_bwd_ic_no(prant)
                bwd_ic_nos = bwd_ic_nos.union(temp_bwd_nos)
        return bwd_ic_nos

    def _get_sv_link_fwd_ic_no(self, node):
        '''辅路的前方设施'''
        fwd_ic_nos = set()
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        for node, child, data in G.out_edges_iter(node, True):
            link_id = data.get('link_id')
            if not self._is_in_hwy_mapping(link_id):
                temp_fwd_nos = self._get_sv_link_fwd_ic_no(child)
                fwd_ic_nos = fwd_ic_nos.union(temp_fwd_nos)
            else:
                temp_fwd_nos = set(hwy_data.get_mapping_fwd_ic(link_id))
                fwd_ic_nos = fwd_ic_nos.union(temp_fwd_nos)
                link_type = hwy_data.get_link_type(link_id)
                # 辅路继续找前方道路情报
                if link_type is None or link_type == LINK_TYPE_SR:
                    temp_fwd_nos = self._get_sv_link_fwd_ic_no(child)
                    fwd_ic_nos = fwd_ic_nos.union(temp_fwd_nos)
        return fwd_ic_nos

    def _get_sv_link_road_info(self, link_id, s_node, e_node, one_way):
        '''辅路的前、后方设施'''
        if self._is_in_hwy_mapping(link_id):
            # 已经在Mapping表, 用已有的道路属性（通常是无效值）
            hwy_data = HwyDataMng.instance()
            road_kind, road_no = hwy_data.get_mapping_road_info(link_id)
            return road_kind, road_no
        bwd_road_infos = set()
        fwd_road_infos = set()
        if one_way == ONE_WAY_POSITIVE:
            bwd_road_infos = self._get_sv_link_bwd_road_info(s_node)
            fwd_road_infos = self._get_sv_link_fwd_road_info(e_node)
        elif one_way == ONE_WAY_RERVERSE:
            bwd_road_infos = self._get_sv_link_bwd_road_info(e_node)
            fwd_road_infos = self._get_sv_link_fwd_road_info(s_node)
        else:
            self.log.error('无道路情报')
            return (ROAD_ATTR_NONE, INVALID_ROAD_NO)
        # 道路情报大于1
        if len(bwd_road_infos) > 1 or len(fwd_road_infos) > 1:
            return (ROAD_ATTR_NONE, INVALID_ROAD_NO)
        # 前后道路情报不同
        if bwd_road_infos != fwd_road_infos:
            return (ROAD_ATTR_NONE, INVALID_ROAD_NO)
        else:
            if not bwd_road_infos:  # 空
                return (ROAD_ATTR_NONE, INVALID_ROAD_NO)
            else:
                return bwd_road_infos.pop()

    def _get_sv_link_bwd_road_info(self, node):
        '''辐路的后方道路情报(road_attr, road_no)'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        road_infos = set()
        for prant, node, data in G.in_edges_iter(node, True):
            link_id = data.get('link_id')
            bwd_ic_nos = hwy_data.get_mapping_bwd_ic(link_id)
            road_kind, road_no = hwy_data.get_mapping_road_info(link_id)
            # 有后方设施，有道路番号
            if bwd_ic_nos and road_no != INVALID_ROAD_NO:
                road_infos = road_infos.union(set([(road_kind, road_no)]))
            else:
                link_type = hwy_data.get_link_type(link_id)
                # 辅疆继续找后方道路情报
                if link_type is None or link_type == LINK_TYPE_SR:
                    temp_road_infos = self._get_sv_link_bwd_road_info(prant)
                    road_infos = road_infos.union(temp_road_infos)
        return road_infos

    def _get_sv_link_fwd_road_info(self, node):
        '''辐路的前方道路情报(road_attr, road_no)'''
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        road_infos = set()
        for node, child, data in G.out_edges_iter(node, True):
            link_id = data.get('link_id')
            bwd_ic_nos = hwy_data.get_mapping_fwd_ic(link_id)
            road_kind, road_no = hwy_data.get_mapping_road_info(link_id)
            # 有前方设施，有道路番号
            if bwd_ic_nos and road_no != INVALID_ROAD_NO:
                road_infos = road_infos.union(set([(road_kind, road_no)]))
            else:
                link_type = hwy_data.get_link_type(link_id)
                # 辅疆继续找前方道路情报
                if link_type is None or link_type == LINK_TYPE_SR:
                    temp_road_infos = self._get_sv_link_fwd_road_info(child)
                    road_infos = road_infos.union(temp_road_infos)
        return road_infos


#==============================================================================
# HwyLinkMapping
#==============================================================================
class HwyLinkMapping(object):
    '''
    '''

    def __init__(self, road_kind, ic_count,
                 road_no, display_class, link_id,
                 forward_ic_no,  backward_ic_no,
                 path_type, tile_id):
        self.road_kind = road_kind
        self.ic_count = ic_count
        self.road_no = road_no
        self.display_class = display_class
        self.link_id = link_id
        self.forward_ic_no = forward_ic_no
        self.backward_ic_no = backward_ic_no
        self.path_type = path_type   # MAIN:本线link, IC:设施link(Ramp,JCT, SAPA)
        self.tile_id = tile_id
