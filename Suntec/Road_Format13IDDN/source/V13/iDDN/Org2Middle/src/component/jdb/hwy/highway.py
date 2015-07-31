# -*- coding: UTF-8 -*-
'''
Created on 2014-3-26

@author: hongchenzai
'''
import base
from component.jdb.hwy.hwy_def import IC_TYPE_INVALID
from component.jdb.hwy.hwy_def import IC_TYPE_SA
from component.jdb.hwy.hwy_def import IC_TYPE_PA
from component.jdb.hwy.hwy_def import IC_TYPE_JCT
from component.jdb.hwy.hwy_def import IC_TYPE_RAMP
from component.jdb.hwy.hwy_def import IC_TYPE_IC
from component.jdb.hwy.hwy_def import IC_TYPE_TOLL
from component.jdb.hwy.hwy_def import IC_TYPE_SMART_IC
from component.jdb.hwy.hwy_def import INOUT_TYPE_IN  # 入口・合流
from component.jdb.hwy.hwy_def import INOUT_TYPE_OUT  # 出口・分岐
from component.jdb.hwy.hwy_def import INOUT_TYPE_NONE
from component.jdb.hwy.hwy_def import IC_TYPE_TRUE
from component.jdb.hwy.hwy_def import IC_DEFAULT_VAL
from component.jdb.hwy.hwy_data_mng import HwyDataMng
from component.jdb.hwy.hwy_data_mng import HwyFacilInfo
from component.jdb.hwy.hwy_mainpath import HwyMainPath
from component.jdb.hwy.hwy_def import HWY_INVALID_FACIL_ID
from component.jdb.hwy.hwy_ic_path import HwyFacilPathFactory
from component.jdb.hwy.hwy_ic_info import convert_tile_id
from component.jdb.hwy.hwy_ic_info import HwyICInfo
from component.jdb.hwy.hwy_ic_info import HwyICInfoNoToll
from component.jdb.hwy.hwy_ic_info import HwyBoundaryOutInfo
from component.jdb.hwy.hwy_ic_info import HwyBoundaryInInfo
from component.jdb.hwy.hwy_service_info import HwyServiceInfo
from component.jdb.hwy.hwy_illust import HwyIllust
from component.jdb.hwy.hwy_store import HwyStore
from component.jdb.hwy.hwy_fee import HwyTollFee
from component.jdb.hwy.hwy_node_addinfo import HwyNodeAddInfo
from component.jdb.hwy.hwy_mapping import HwyMapping
from component.jdb.hwy.hwy_facility_name import HwyFacilityName


class Highway(base.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Highway')

    def _Do(self):
        hwy_data = HwyDataMng.instance()
        hwy_data.Make()
        # 加载调整本线
        hwy_data.load_hwy_main_links()
        self.cal_main_path()  # 计算本线路径
        # 加载高速道和收费道(包括Ramp,JCT)，及设施信息
        hwy_data.load_hwy_data()
        self.cal_ic_path()  # 计算设施路径
        # 道路
        self.make_road_no()  # 道路番号(上下共用)
        self.make_road_info()  # 道路情报
        # 设施
        self.make_ic_no()   # 生成ic_no(上下设施番号不共同)
        self.make_facility_id()  # 生成设施番号(上下设施番号共同)
        # Mapping
        hwy_mapping = HwyMapping()
        hwy_mapping.Make()
        # 设施情报
        self._make_ic_info()
        # 无料区间设施node附加情报(临时)
        self._make_temp_add_info_no_toll()
        # 服务情报(For SAPA)
        service_info = HwyServiceInfo()
        service_info.Make()
        # Highway Illust(For SAPA)
        Illust = HwyIllust()
        Illust.Make()
        # 店铺情报(For SAPA)
        store = HwyStore()
        store.Make()
        # 金额(费用)情报
        toll_fee = HwyTollFee()
        toll_fee.Make()
        # 附加情报(最终)
        node_addinfo = HwyNodeAddInfo()
        node_addinfo.Make()
        # 设施/地点名称(TowardName使用, 主要针对设施分歧)
        hwy_fac_name = HwyFacilityName()
        hwy_fac_name.Make()

    def cal_main_path(self):
        main_path = HwyMainPath()
        main_path.Make()

    def cal_ic_path(self):
        self.log.info('Calculating IC Path.')
        self.CreateTable2('mid_hwy_ic_path')
        sqlcmd = '''
        SELECT node_id, road_code, road_seq,
               facilclass_c, direction_c as updown, inout_c,
               name_kanji
          FROM mid_road_hwynode_middle_nodeid
          where "tollclass_c" = 0 and "tollfunc_c" = 0 and hwymode_f = 1
          order by road_code, direction_c, road_seq, objectid
        '''
        self.pg.connect1()
        data = self.pg.get_batch_data2(sqlcmd)
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        no_toll_graph = hwy_data.get_graph_no_toll()
        for hwynode_info in data:
            node_id = hwynode_info[0]
            road_code = hwynode_info[1]
            road_point = hwynode_info[2]
            facil_cls = hwynode_info[3]
            updown = hwynode_info[4]
            inout = hwynode_info[5]
            facil_name = hwynode_info[6]
            if (node_id not in G or
                not G.in_edges(node_id) or
                not G.out_edges(node_id)):
                if node_id in no_toll_graph:
                    # print 'No toll:', node_id
                    toll_flag = IC_DEFAULT_VAL
                else:
                    self.log.warning('Not in No_Toll HWY. node_id=%s'
                                     % node_id)
                    continue
            else:
                # print 'Toll:', node_id
                toll_flag = IC_TYPE_TRUE

            ic_path = None
            ic_path = HwyFacilPathFactory.create_facil_path(self, toll_flag,
                                                            node_id, road_code,
                                                            road_point, updown,
                                                            facil_cls, inout,
                                                            facil_name)
            if ic_path:
                ic_path.cal_path()
                ic_path.cal_uturn_path()
                self.__insert_ic_path(ic_path)
                if not ic_path.pathes_dict and facil_cls != IC_TYPE_IC:
                    if facil_cls in (IC_TYPE_PA, IC_TYPE_SA):
                        pass
                    else:
                        name = ic_path.facil_name
                        self.log.warning('No Path for node=%s,type=%s,name=%s'
                                         % (node_id, facil_cls, name))
            else:
                pass
        self.pg.commit1()
        self.CreateIndex2('mid_hwy_ic_path_s_road_code_'
                          's_road_point_s_updown_s_node_id_idx')
        self.CreateIndex2('mid_hwy_ic_path_t_road_code_'
                          't_road_point_t_updown_t_node_id_idx')

    def _check_facil_type(self, s_facilcls, s_inout, t_facilcls, t_inout):
        return t_facilcls in s_facilcls and s_inout == t_inout

    def is_ic_out(self, facil_cls, inout):
        s_facilcls = [IC_TYPE_IC, IC_TYPE_SMART_IC, IC_TYPE_RAMP]
        return self._check_facil_type(s_facilcls, INOUT_TYPE_OUT,
                                      facil_cls, inout)

    def is_ic_in(self, facil_cls, inout):
        s_facilcls = [IC_TYPE_IC, IC_TYPE_SMART_IC, IC_TYPE_RAMP]
        return self._check_facil_type(s_facilcls, INOUT_TYPE_IN,
                                      facil_cls, inout)

    def is_sapa_out(self, facil_cls, inout):
        s_facilcls = [IC_TYPE_SA, IC_TYPE_PA]
        return self._check_facil_type(s_facilcls, INOUT_TYPE_OUT,
                                      facil_cls, inout)

    def is_sapa_in(self, facil_cls, inout):
        s_facilcls = [IC_TYPE_SA, IC_TYPE_PA]
        return self._check_facil_type(s_facilcls, INOUT_TYPE_IN,
                                      facil_cls, inout)

    def is_jct_out(self, facil_cls, inout):
        s_facilcls = [IC_TYPE_JCT]
        return self._check_facil_type(s_facilcls, INOUT_TYPE_OUT,
                                      facil_cls, inout)

    def is_jct_in(self, facil_cls, inout):
        s_facilcls = [IC_TYPE_JCT]
        return self._check_facil_type(s_facilcls, INOUT_TYPE_IN,
                                      facil_cls, inout)

    def __insert_ic_path(self, ic_path):
        sqlcmd = """
        INSERT INTO mid_hwy_ic_path(
                    s_road_code, s_road_point,
                    s_updown, s_node_id,
                    s_facilcls, s_inout_c,
                    t_road_code, t_road_point,
                    t_updown, t_node_id,
                    t_facilcls, t_inout_c,
                    inlinkid, outlinkid,
                    link_lid, node_lid,
                    path_type, toll_flag)
            VALUES (%s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s);
        """
        if not ic_path.pathes_dict:
            return
        inlinkid = ic_path.get_inlinkid()
        for path, val in ic_path.pathes_dict.iteritems():
            (facil_info, path_type) = val
            outlinkid = ic_path.get_outlinkid(path[-1])
            link_lid = ic_path.get_path_linkids(path)
            if facil_info:
                params = (ic_path.road_code, ic_path.road_point,
                          ic_path.updown, ic_path.node_id,
                          ic_path.facilcls, ic_path.inout,
                          facil_info.road_code, facil_info.road_point,
                          facil_info.updown, facil_info.node_id,
                          facil_info.facilcls, facil_info.inout,
                          inlinkid, outlinkid,
                          link_lid, path,
                          path_type, ic_path.toll_flag
                          )
                self.pg.execute1(sqlcmd, params)
            else:
                params = (ic_path.road_code, ic_path.road_point,
                          ic_path.updown, ic_path.node_id,
                          ic_path.facilcls, ic_path.inout,
                          None, None,
                          None, None,
                          None, None,
                          inlinkid, outlinkid,
                          link_lid, path,
                          path_type, ic_path.toll_flag
                          )
                self.pg.execute1(sqlcmd, params)

    def make_road_no(self):
        '''道路番号(上下共用)'''
        self.pg.connect2()
        self.CreateTable2('mid_hwy_road_no')
        # 当一条道路有多种road_type时，取link数据最多的
        sqlcmd = '''
        insert into mid_hwy_road_no(roadcode, lineclass_c,
                                    road_type, linedirtype_c)
        (
        SELECT roadcode, lineclass_c,
               (array_agg(road_type))[1] as road_type, linedirtype_c
          from (
                SELECT  roadcode, a.lineclass_c,
                        road_type, count(link_id) link_num,
                        linedirtype_c
                  FROM mid_hwy_main_link AS a
                  LEFT JOIN road_code_list
                  ON roadcode = road_code
                  where a.lineclass_c < 9
                  group by roadcode, a.lineclass_c, road_type, linedirtype_c
                  order by lineclass_c, roadcode, link_num desc
          ) as a
          group by lineclass_c, roadcode, linedirtype_c
          order by lineclass_c, roadcode
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('mid_hwy_road_no_roadcode_idx')

    def make_ic_no(self):
        self.log.info('Making IC_NO.')
        self.pg.connect1()
        self.pg.connect2()
        self.CreateTable2('mid_hwy_ic_no')
        ic_no = 1
        data = self.__get_all_hwy_nodes()
        for hwy in data:
            road_code = hwy[0]
            updown = hwy[1]
            node_list = hwy[2]
            boundary_flag_list = hwy[3]
            cycle_flag = hwy[4]
            if cycle_flag:
                node_list.pop()
            ic_list = self.get_ic_list(road_code, updown,
                                       node_list, boundary_flag_list,
                                       cycle_flag)
            curr_idx = 0
            while curr_idx < len(ic_list):
                curr_facil = ic_list[curr_idx]
                next_idx = curr_idx + 1
                if next_idx == len(ic_list):
                    pass
                if curr_facil.boundary_flag and not curr_facil.road_point:
                    # 边界点
                    pass
                elif next_idx < len(ic_list):
                    while next_idx < len(ic_list):
                        next_facil = ic_list[next_idx]
                        if(curr_facil.road_code == next_facil.road_code and
                           curr_facil.road_point == next_facil.road_point and
                           curr_facil.facilcls == next_facil.facilcls):
                            if(curr_facil.inout == INOUT_TYPE_OUT and
                               next_facil.inout == INOUT_TYPE_IN):
                                next_idx += 1
                            else:
                                pass
#                                 if curr_facil.inout != next_facil.inout:
#                                 self.log.warning('current facil:%s,'
#                                                  'facilcsl=%s,'
#                                                  'inout=%s;'
#                                                  'next facil:%s,'
#                                                  'facilcsl=%s,'
#                                                  'inout=%s'
#                                                  % (curr_facil.node_id,
#                                                     curr_facil.facilcls,
#                                                     curr_facil.inout,
#                                                     next_facil.node_id,
#                                                     next_facil.facilcls,
#                                                     next_facil.inout))
#                     if next_idx - curr_idx > 2:
#                         self.log.warning('More than one. current facil:%s,'
#                                          'last facil:%s'
#                                          % (curr_facil.node_id,
#                                             ic_list[next_idx - 1].node_id))
                        break
                else:  # 最后一个设施
                    pass
                self.__insert_ic_id(ic_no, ic_list[curr_idx:next_idx])
                ic_no += 1
                curr_idx = next_idx
        self.pg.commit1()
        self.CreateIndex2('mid_hwy_ic_no_road_code_road_point_idx')
        self.CreateIndex2('mid_hwy_ic_no_node_id_road_code'
                          '_road_point_updown_idx')

    def __get_all_hwy_nodes(self):
        sqlcmd = """
        SELECT road_code,
               updown_flag,
               array_agg(node_id),
               array_agg(tile_boundary_flag),
               cycle_flag
         FROM (
                SELECT road_code, updown_flag, a.node_id,
                       link_id, seqnum, cycle_flag, lineclass_c,
                       tile_boundary_flag
                  FROM mid_hwy_main_link_path as a
                  LEFT JOIN node_tbl as b
                  ON a.node_id = b.node_id
                  ORDER BY lineclass_c, road_code, updown_flag, a.gid
         ) AS c
         GROUP BY road_code, updown_flag, cycle_flag
         ORDER BY road_code, updown_flag desc;
        """
        return self.pg.get_batch_data2(sqlcmd)

    def __insert_ic_id(self, ic_no, facil_info_list):
        sqlcmd = """
        INSERT INTO mid_hwy_ic_no(
                    ic_no, node_id,
                    road_code, road_point,
                    updown, facilclass_c,
                    inout_c, name_kanji)
            VALUES (%s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s);
        """
        for facil in facil_info_list:
            self.pg.execute1(sqlcmd, (ic_no, facil.node_id,
                                      facil.road_code, facil.road_point,
                                      facil.updown, facil.facilcls,
                                      facil.inout, facil.facil_name))

    def get_ic_list(self, road_code, updown, node_list,
                    boundary_flag_list, cycle_flag):
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        ic_group = []  # 一个点上的所有设施做为一个元素
        for node_idx in range(0, len(node_list)):
            node_id = node_list[node_idx]
            if(not cycle_flag and
               (node_idx == 0 or node_idx == len(node_list))):
                # 非环形的起末点，不做边界点
                boundary_flag = False
            else:
                b_flag = boundary_flag_list[node_idx]
                boundary_flag = self.is_hwy_tile_boundary(node_id,
                                                          road_code,
                                                          b_flag)
            if boundary_flag:
                boundary_facil_info = HwyFacilInfo(road_code,
                                                   HWY_INVALID_FACIL_ID,
                                                   IC_TYPE_INVALID,
                                                   updown,
                                                   node_id,
                                                   INOUT_TYPE_NONE,
                                                   '(Tile境界)')
                boundary_facil_info.boundary_flag = boundary_flag
            toll_flag = G.is_toll(node_id)
            if G.is_hwy_node(node_id) or toll_flag:
                in_out = G.get_inout_type(node_id)
                # 出口时，边界点放在施设前面
                if (in_out == INOUT_TYPE_OUT) and boundary_flag:
                    ic_group.append([boundary_facil_info])
                # get facilities of this node
                facil_infos = hwy_data.get_hwy_facils_by_nodeid(node_id)
                # 对当前点的多个设施进行排序
                facil_infos = self._sort_facils(facil_infos)
                ic_group.append(facil_infos)
                # 入口时，边界点放在施设后面
                if (in_out == INOUT_TYPE_IN or toll_flag) and boundary_flag:
                    ic_group.append([boundary_facil_info])
            elif boundary_flag:  # 边界
                ic_group.append([boundary_facil_info])
        ic_list = []
        # ## 重排头尾设施
        # 第一个设施(同个点上可能有多个)
        ic_list += self._sort_start_facils(ic_group[0], cycle_flag)
        # 最后一个设施(同个点上可能有多个)
        ic_group[-1] = self._sort_end_facils(ic_group[-1], cycle_flag)
        for ic_group_idx in range(1, len(ic_group) - 1):
            # 依据前后设施给同个点上的设施再排序(就近原则)
            ic_list += self._sort_facils_by_fb(ic_group, ic_list, ic_group_idx)
        ic_list += ic_group[-1]
        return ic_list

    def _sort_facils_by_fb(self, ic_group, ic_list, ic_group_idx):
        '''根据前后方设施，对当前点的多个设施进行排序。'''
        curr_facil_infos = ic_group[ic_group_idx]
        if len(curr_facil_infos) <= 1:  # 只一个设施
            return curr_facil_infos
        # 前方相同设施, 及相隔了多少个设施
        fwd_facils = self._sort_facils_forward(curr_facil_infos,
                                               ic_group,
                                               ic_group_idx)
        forward_list, fwd_ic_counts = fwd_facils
        # 后方相同设施, 及相隔了多少个设施
        bwk_facils = self._sort_facils_backward(curr_facil_infos, ic_list)
        backward_list, bwd_ic_counts = bwk_facils
        # 前后都有相同的设施(交集)
        forward_set = set(forward_list)
        backward_set = set(backward_list)
        inter_set = forward_set & backward_set
        if inter_set:
            self.log.warning('Exist same IC in forward and backward. node=%s' %
                             curr_facil_infos[0].node_id)
            for facil in inter_set:
                # 和后方相同设施之间隔之几个设施
                bwd_idx = backward_list.index(facil)
                bwd_ic_count = bwd_ic_counts[bwd_idx] + bwd_idx
                # 和前方设施之间隔之几个设施
                fwd_idx = forward_list.index(facil)
                fwd_ic_count = fwd_ic_counts[fwd_idx]
                fwd_ic_count += len(forward_list) - fwd_idx - 1
                if fwd_ic_count > bwd_ic_count:
                    forward_list.pop(bwd_idx)
                    forward_list.pop(bwd_idx)
                else:
                    backward_list.pop(bwd_idx)
                    bwd_ic_counts.pop(bwd_idx)
        if forward_list and backward_list:
            self.log.warning('Exist forward List and backward List. node=%s' %
                             curr_facil_infos[0].node_id)
        most_bwd = backward_list[0:1]  # 最靠后设施
        most_fwd = forward_list[-1::]  # 最靠前设施
        middle_facils = []
        for facil in curr_facil_infos:
            if facil not in most_bwd and facil not in most_fwd:
                middle_facils.append(facil)
        middle_list = self._sort_facils(middle_facils)
        sorted_facil_list = most_bwd + middle_list + most_fwd
        return sorted_facil_list

    def _sort_facils_forward(self, curr_facil_infos, ic_group, ic_group_idx):
        # 前方有相同设施，尽量排后面
        hwy_data = HwyDataMng.instance()
        next_road_point = hwy_data.get_next_road_point(curr_facil_infos[0])
        rst_ic_list = []
        fwd_ic_counts = []  # 两个相同设施之间有几个设施
        forward_ic_list = []
        # ## 取得当前点到下个设施之间的所有设施
        for facil_infos in ic_group[ic_group_idx + 1:]:
            if self._arrive_next_facil(next_road_point, facil_infos):
                break
            forward_ic_list += facil_infos
        ic_count = -1
        temp_facil_infos = list(curr_facil_infos)
        for fwd_facil in forward_ic_list:
            ic_count += 1
            for facil in temp_facil_infos:
                if fwd_facil.road_point == facil.road_point:
                    fwd_ic_counts.insert(0, ic_count)
                    rst_ic_list.insert(0, facil)
                    temp_facil_infos.remove(facil)
                    break
        return rst_ic_list, fwd_ic_counts

    def _sort_facils_backward(self, curr_facil_infos, ic_list):
        '''后方有相同设施，尽量排前面'''
        if len(ic_list) == 0:
            return [], []
        hwy_data = HwyDataMng.instance()
        prev_road_point = hwy_data.get_prev_road_point(curr_facil_infos[0])
        ic_idx = len(ic_list) - 1
        while ic_idx >= 0:
            if ic_list[ic_idx].road_point == prev_road_point:
                break
            ic_idx -= 1
        rst_ic_list = []
        bwd_ic_counts = []  # 两个相同设施之间有几个设施
        backward_ic_list = ic_list[ic_idx + 1:]
        temp_facil_infos = list(curr_facil_infos)
        ic_count = -1
        while backward_ic_list:
            last_facil = backward_ic_list.pop()
            ic_count += 1
            for facil in temp_facil_infos:
                if last_facil.road_point == facil.road_point:
                    bwd_ic_counts.append(ic_count)
                    rst_ic_list.append(facil)
                    temp_facil_infos.remove(facil)
                    break
        return rst_ic_list, bwd_ic_counts

    def _sort_facils(self, facil_infos):
        '''对同个点的设施进行排序。按设施种别排序'''
        ic_list = []
        if not facil_infos:
            return []
        # 按road_seq排序
        facil_infos = self._sort_facils_by_seq(facil_infos)
        # ## 出口
        ic_outs = self.get_ic_out(facil_infos)
        for ic_out in ic_outs:
            ic_list.append(ic_out)
        jct_outs = self.get_jct_out(facil_infos)
        for jct_out in jct_outs:
            ic_list.append(jct_out)
        sapa_outs = self.get_sapa_out(facil_infos)
        for sapa_out in sapa_outs:
            ic_list.append(sapa_out)
        # ## 料金所
        tolls = self.get_tolls(facil_infos)
        for toll in tolls:
            ic_list.append(toll)
        # ## 入口
        sapa_outs = self.get_sapa_in(facil_infos)
        for sapa_in in sapa_outs:
            ic_list.append(sapa_in)
        jct_ins = self.get_jct_in(facil_infos)
        for jct_in in jct_ins:
            ic_list.append(jct_in)
        ic_ins = self.get_ic_in(facil_infos)
        for ic_in in ic_ins:
            ic_list.append(ic_in)
        return ic_list

    def _sort_start_facils(self, facil_infos, cycle_flag):
        '''对路的起始设施(同个点)进行排序：IC入口、JCT入口、SAPA入口'''
        ic_list = []
        if not facil_infos:
            return []
        sapa_ins = self.get_sapa_in(facil_infos)
        for sapa_in in sapa_ins:
            ic_list.append(sapa_in)
        # ## 出口
        ic_outs = self.get_ic_out(facil_infos)
        if ic_outs:
            self.log.error('IC出口在起始设施。')
        jct_outs = self.get_jct_out(facil_infos)
        if jct_outs:
            self.log.error('JCT出口在起始设施。')
        sapa_outs = self.get_sapa_out(facil_infos)
        if sapa_outs:
            self.log.error('SAPA出口在起始设施。')
        # ## 料金所
        tolls = self.get_tolls(facil_infos)
        if tolls:
            self.log.error('有收费站在起始设施。')
        if cycle_flag:  # 环线
            # 按设施种别排序
            return self._sort_facils(facil_infos)
        else:  # 非环线
            ic_list = self._sort_facils_by_seq(facil_infos)
            if ic_list != facil_infos:
                pass
        return ic_list

    def _sort_end_facils(self, facil_infos, cycle_flag):
        '''对路的结尾设施(同个点)进行排序：IC入口、JCT入口、SAPA入口'''
        ic_list = []
        if not facil_infos:
            return []
        sapa_outs = self.get_sapa_out(facil_infos)
        for sapa_out in sapa_outs:
            ic_list.append(sapa_out)
        # ## 入口
        ic_ins = self.get_ic_in(facil_infos)
        if ic_ins:
            self.log.error('IC入口有收费站在结尾设施。')
        jct_ins = self.get_jct_in(facil_infos)
        if jct_ins:
            self.log.error('JCT入口有收费站在结尾设施。')
        sapa_ins = self.get_sapa_in(facil_infos)
        if sapa_ins:
            self.log.error('SAPA入口站在结尾设施。')
        # ## 料金所
        tolls = self.get_tolls(facil_infos)
        if tolls:
            self.log.error('有收费站在结尾设施。')
        if cycle_flag:  # 环线
            # 按设施种别排序
            return self._sort_facils(facil_infos)
        else:  # 非环线
            ic_list = self._sort_facils_by_seq(facil_infos)
            if ic_list != facil_infos:
                pass
        return ic_list

    def _sort_facils_by_seq(self, facil_infos):
        '''同个点的多个设施，按road_seq排序。'''
        return sorted(facil_infos,
                      cmp=lambda x, y: cmp(x.road_point, y.road_point)
                      )

    def _arrive_next_facil(self, next_road_point, facil_infos):
        for facil in facil_infos:
            if facil.road_point == next_road_point:
                return True
        return False

    def get_ic_out(self, facil_infos):
        ic_out = []
        for facil in facil_infos:
            if self.is_ic_out(facil.facilcls, facil.inout):
                ic_out.append(facil)
        return ic_out

    def get_ic_in(self, facil_infos):
        ic_out = []
        for facil in facil_infos:
            if self.is_ic_in(facil.facilcls, facil.inout):
                ic_out.append(facil)
        return ic_out

    def get_jct_out(self, facil_infos):
        jct_out = []
        for facil in facil_infos:
            if self.is_jct_out(facil.facilcls, facil.inout):
                jct_out.append(facil)
        return jct_out

    def get_jct_in(self, facil_infos):
        jct_out = []
        for facil in facil_infos:
            if self.is_jct_in(facil.facilcls, facil.inout):
                jct_out.append(facil)
        return jct_out

    def get_sapa_out(self, facil_infos):
        jct_out = []
        for facil in facil_infos:
            if self.is_sapa_out(facil.facilcls, facil.inout):
                jct_out.append(facil)
        return jct_out

    def get_sapa_in(self, facil_infos):
        jct_out = []
        for facil in facil_infos:
            if self.is_sapa_in(facil.facilcls, facil.inout):
                jct_out.append(facil)
        return jct_out

    def get_tolls(self, facil_infos):
        tolls = []
        for facil in facil_infos:
            if facil.facilcls == IC_TYPE_TOLL:
                tolls.append(facil)
        return tolls

    def is_hwy_tile_boundary(self, node_id, roadcode, boundary_flag):
        if not boundary_flag:
            return False
        # 判断是不是第十层边界
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        in_tile, out_tile = None, None
        in_tiles = G.get_in_tile(node_id, roadcode)
        if in_tiles:
            if len(in_tiles) > 1:
                self.log.warning('Main InTile > 1,boundary_node=%s' % node_id)
            in_tile = convert_tile_id(in_tiles[0])
        out_tiles = G.get_out_tile(node_id, roadcode)
        if out_tiles:
            if len(out_tiles) > 1:
                self.log.warning('Main OutTile > 1,boundary_node=%s' % node_id)
            out_tile = convert_tile_id(out_tiles[0])
        if not in_tile and not out_tile:
            self.log.warning('No InTile and OutTile. boundary_node=%s'
                             % node_id)
            return False
        return in_tile != out_tile

    def make_facility_id(self):
        '''生成1到N的设施番号，边界点没有设施号'''
        self.CreateTable2('mid_hwy_facility_id')
        sqlcmd = """
        INSERT INTO mid_hwy_facility_id(road_code, road_point)
        (
        SELECT road_code, road_seq
          FROM (
                SELECT distinct lineclass_c, a.road_code, road_seq
                  FROM mid_road_hwynode_middle_nodeid AS a
                  LEFT JOIN road_code_list as b
                  ON a.road_code = b.road_code
                  where hwymode_f = 1
          ) AS A
          ORDER BY lineclass_c, road_code, road_seq
        );
        """
        self.pg.execute2(sqlcmd, (HWY_INVALID_FACIL_ID,))
        self.pg.commit2()
        self.CreateIndex2('mid_hwy_facility_id_road_code_road_point_idx')

        # 更新ic_no表的设施番号
        sqlcmd = """
        UPDATE mid_hwy_ic_no as ic set facility_id = facil.facility_id
          FROM mid_hwy_facility_id as facil
          WHERE ic.road_code = facil.road_code
                and ic.road_point = facil.road_point;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # 更新边界点的设施番号
        sqlcmd = """
        UPDATE mid_hwy_ic_no set facility_id = %s
          WHERE road_point = %s;
        """
        self.pg.execute2(sqlcmd, (HWY_INVALID_FACIL_ID, HWY_INVALID_FACIL_ID))
        self.pg.commit2()

    def _make_ic_info(self):
        self.log.info('Making IC Info.')
        self.pg.connect1()
        self.pg.connect2()
        self.CreateTable2('highway_ic_info')
        self.CreateTable2('highway_path_point')
        self.CreateTable2('highway_conn_info')
        self.CreateTable2('highway_toll_info')
        self.CreateTable2('mid_hwy_node_add_info')
        hwy_data = HwyDataMng.instance()
        for ic_no, facility_id, facil_list in hwy_data.get_ic_list():
            if facility_id != HWY_INVALID_FACIL_ID:  # 非边界点
                ic_info = HwyICInfo(ic_no, facility_id, facil_list)
                ic_info.set_ic_info()  # 设置料金情报
                self.__insert_ic_info(ic_info)
                self.__insert_path_point(ic_info)
                self.__insert_conn_info(ic_info)
                self.__insert_toll_info(ic_info)
                self.__insert_temp_add_info(ic_info)
            else:
                # 边界出口点（离开当前Tile）
                ic_info = HwyBoundaryOutInfo(ic_no, facility_id, facil_list)
                ic_info.set_ic_info()
                self.__insert_ic_info(ic_info)
                self.__insert_path_point(ic_info)
                # 边界进入点（进入Tile的点）
                ic_info = HwyBoundaryInInfo(ic_no, facility_id, facil_list)
                ic_info.set_ic_info()
                self.__insert_ic_info(ic_info)
                self.__insert_path_point(ic_info)
        self.pg.commit1()
        self.CreateIndex2('highway_ic_info_ic_no_idx')
        self.CreateIndex2('highway_toll_info_ic_no_conn_ic_no_node_id_idx')
        self.CreateIndex2('highway_conn_info_ic_no_conn'
                          '_ic_no_conn_direction_idx')
        self._add_geom_column()

    def _make_temp_add_info_no_toll(self):
        self.pg.connect1()
        hwy_data = HwyDataMng.instance()
        for facility_id, facil_info in hwy_data.get_no_toll_facil_infos():
            ic_info_no_toll = HwyICInfoNoToll(facility_id, facil_info)
            ic_info_no_toll.get_path_info()
            ic_info_no_toll.set_node_add_info()
            self.__insert_temp_add_info(ic_info_no_toll, IC_DEFAULT_VAL)
        self.pg.commit1()

    def __insert_ic_info(self, ic):
        sqlcmd = """
        INSERT INTO highway_ic_info(
                    ic_no, up_down, facility_id,
                    between_distance, inside_distance, enter,
                    exit, tollgate, jct,
                    pa, sa, ic,
                    ramp, smart_ic, barrier,
                    dummy, boundary, "new",
                    unopen, forward_flag, reverse_flag,
                    toll_count, enter_direction, path_count,
                    vics_count, conn_count, illust_count,
                    store_count, servise_flag, road_no,
                    "name",
                    conn_tile_id, tile_id)
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    %s, %s, %s,
                    mid_get_json_string_for_japan_names(array[%s], array[%s]),
                    %s, %s);
        """
        params = (ic.ic_no, ic.updown, ic.facility_id,
                  ic.between_distance, ic.inside_distance, ic.enter,
                  ic.exit, ic.tollgate, ic.jct,
                  ic.pa, ic.sa, ic.ic,
                  ic.ramp, ic.smart_ic, ic.barrier,
                  ic.dummy, ic.boundary, ic.new,
                  ic.unopen, ic.forward_flag, ic.reverse_flag,
                  ic.toll_count, ic.enter_direction, ic.path_count,
                  ic.vics_count, ic.conn_count, ic.illust_count,
                  ic.store_count, ic.servise_flag, ic.road_no,
                  ic.name, ic.name_yomi,
                  ic.conn_tile_id, ic.tile_id)
        self.pg.execute1(sqlcmd, params)

    def __insert_path_point(self, ic):
        sqlcmd = """
        INSERT INTO highway_path_point(
                    ic_no, enter_flag,
                    exit_flag, main_flag,
                    center_flag, new_flag,
                    unopen_flag, pos_flag,
                    link_id, node_id,
                    tile_id)
            VALUES (%s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s, %s,
                    %s);
        """
        for point in ic.path_points:
            self.pg.execute1(sqlcmd, (point.ic_no, point.enter_flag,
                                      point.exit_flag, point.main_flag,
                                      point.center_flag, point.new_flag,
                                      point.unopen_flag, point.pos_flag,
                                      point.link_id, point.node_id,
                                      point.tile_id))
        # self.pg.commit1()

    def __insert_conn_info(self, ic_info):
        sqlcmd = """
        INSERT INTO highway_conn_info(
                    ic_no, road_attr,
                    conn_direction, same_road_flag,
                    tile_change_flag, uturn_flag,
                    new_flag, unopen_flag,
                    vics_flag, toll_flag,
                    conn_road_no, conn_ic_no,
                    conn_link_length, conn_tile_id,
                    toll_count, vics_count,
                    tile_id, ic_name,
                    conn_ic_name)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s);
        """
        for conn in ic_info.get_conn_info():
            self.pg.execute1(sqlcmd, (conn.ic_no, conn.road_attr,
                                      conn.conn_direction, conn.same_road_flag,
                                      conn.tile_change_flag, conn.uturn_flag,
                                      conn.new_flag, conn.unopen_flag,
                                      conn.vics_flag, conn.toll_flag,
                                      conn.conn_road_no, conn.conn_ic_no,
                                      conn.conn_length, conn.conn_tile_id,
                                      conn.toll_count, conn.vics_count,
                                      conn.tile_id, conn.ic_name,
                                      conn.conn_ic_name)
                             )
        # self.pg.commit1()

    def __insert_toll_info(self, ic_info):
        '''料金情报。'''
        sqlcmd = """
        INSERT INTO highway_toll_info(
                    ic_no, conn_ic_no,
                    toll_class, class_name,
                    up_down, facility_id,
                    tollgate_count, etc_antenna,
                    enter, exit,
                    jct, sa_pa,
                    gate, unopen,
                    dummy, non_ticket_gate,
                    check_gate, single_gate,
                    cal_gate, ticket_gate,
                    nest, uturn,
                    not_guide, normal_toll,
                    etc_toll, etc_section,
                    node_id, road_code,
                    road_seq, "name",
                    dummy_toll_node)
          values(%s, %s, %s, %s, %s, %s,
                 %s, %s, %s, %s, %s, %s,
                 %s, %s, %s, %s, %s, %s,
                 %s, %s, %s, %s, %s, %s,
                 %s, %s, %s, %s, %s, %s,
                 %s);
        """
        for toll in ic_info.get_all_toll_info():
            toll_type = toll.toll_type
            param = (toll.ic_no, toll.conn_ic_no,
                     toll.toll_class, toll.class_name,
                     toll.updown, toll.facility_id,
                     toll.tollgate_count, toll_type.etc_antenna,
                     toll_type.enter, toll_type.exit,
                     toll_type.jct, toll_type.sa_pa,
                     toll_type.gate, toll_type.unopen,
                     toll_type.dummy_facil, toll_type.non_ticket_gate,
                     toll_type.check_gate, toll_type.single_gate,
                     toll_type.cal_gate, toll_type.ticket_gate,
                     toll_type.nest, toll_type.uturn,
                     toll_type.not_guide, toll_type.normal_toll,
                     toll_type.etc_toll, toll_type.etc_section,
                     toll.node_id, toll.road_code,
                     toll.road_seq, toll.name_kanji,
                     toll.dummy_toll_node
                    )
            self.pg.execute1(sqlcmd, param)

    def __insert_temp_add_info(self, ic_info, toll_flag=IC_TYPE_TRUE):
        '''临时附加情报'''
        sqlcmd = """
        INSERT INTO mid_hwy_node_add_info(ic_no, updown,
                                          facility_id, facilcls,
                                          in_out, link_lid,
                                          node_lid, add_node_id,
                                          add_link_id, pos_type,
                                          pos_type_name, name_kanji,
                                          name_yomi, toll_flag
                                         )
          VALUES(%s, %s, %s, %s,
                 %s, %s, %s, %s,
                 %s, %s, %s, %s,
                 %s, %s)
        """
        for add_info in ic_info.get_add_info():
            param = (ic_info.ic_no, ic_info.updown,
                     ic_info.facility_id, add_info.get('facilcls'),
                     add_info.get('in_out'), add_info.get('link_lid'),
                     add_info.get('node_lid'), add_info.get('add_node'),
                     add_info.get('add_link'), add_info.get('pos_type'),
                     add_info.get('pos_type_name'), ic_info.name,
                     ic_info.name_yomi, toll_flag)
            self.pg.execute1(sqlcmd, param)

    def _add_geom_column(self):
        """给临时附加情报(mid_hwy_node_add_info)添加geom信息——QGis显示用"""
        sqlcmd = """
        SELECT AddGeometryColumn('', 'mid_hwy_node_add_info', 'add_node_geom',
                                 '4326', 'POINT', 2);
        SELECT AddGeometryColumn('', 'mid_hwy_node_add_info', 'add_link_geom',
                                 '4326', 'LINESTRING', 2);
        UPDATE  mid_hwy_node_add_info SET add_node_geom = the_geom
          from node_tbl
          where add_node_id = node_id;

        UPDATE  mid_hwy_node_add_info SET add_link_geom = the_geom
          from link_tbl
          where add_link_id = link_id;

        CREATE INDEX mid_hwy_node_add_info_add_link_geom_idx
          ON mid_hwy_node_add_info
          USING gist
          (add_link_geom);

        CREATE INDEX mid_hwy_node_add_info_add_node_geom_idx
          ON mid_hwy_node_add_info
          USING gist
          (add_node_geom);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def make_road_info(self):
        '''道路情报'''
        self.pg.connect1()
        from component.jdb.hwy.hwy_road_info import HwyRoadInfo
        self.CreateTable2('highway_road_info')
        sqlcmd = """
        INSERT INTO highway_road_info(
                    road_no, iddn_road_kind,
                    road_kind, road_attr,
                    loop, "new",
                    un_open, name,
                    up_down)
            VALUES (%s, %s,
                    %s, %s,
                    %s, %s,
                    %s,
                    mid_get_json_string_for_japan_names(array[%s], array[%s]),
                    %s);
        """
        data = self.__get_road_info()
        for info in data:
            road_no, lineclass_c, road_type = info[0:3]
            linedir_type, updown, cycle_flag = info[3:6]
            name_kanji, name_yomi = info[6:]
            road = HwyRoadInfo(road_no, lineclass_c, road_type,
                               linedir_type, updown, cycle_flag,
                               name_kanji, name_yomi)
            self.pg.execute1(sqlcmd, (road.road_no, road.iddn_road_kind,
                                      road.road_kind, road.road_attr,
                                      road.loop, road.new,
                                      road.un_open,
                                      road.name_kanji, road.name_yomi,
                                      road.updown))
        self.pg.commit1()

    def __get_road_info(self):
        sqlcmd = """
        SELECT road_no, road.lineclass_c,
               road_type, road.linedirtype_c,
               updown_flag, cycle_flag,
               name_kanji, name_yomi
          FROM mid_hwy_road_no as road
          LEFT JOIN (
                SELECT distinct road_code, updown_flag, cycle_flag
                     FROM mid_hwy_main_link_path
          ) as path
          ON roadcode = road_code
          LEFT JOIN road_code_list AS l
          ON roadcode = l.road_code
          order by road_no, updown_flag desc;
        """
        return self.pg.get_batch_data2(sqlcmd)
