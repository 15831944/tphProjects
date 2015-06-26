# -*- coding: UTF8 -*-
'''
Created on 2014-6-11

@author: hongchenzai
'''
import component.component_base
from component.jdb.hwy.hwy_def import IC_TYPE_TOLL
from component.jdb.hwy.hwy_def import TOLL_CLASS_RAMP
from component.jdb.hwy.hwy_def import TOLL_CLASS_MAIN
from component.jdb.hwy.hwy_def import TOLL_CLASS_BARRIER
from component.jdb.hwy.hwy_def import HWY_UPDOWN_DOWN
from component.jdb.hwy.hwy_def import HWY_UPDOWN_UP
from component.jdb.hwy.hwy_def import HWY_UPDOWN_COMMON
from component.jdb.hwy.hwy_def import IC_TYPE_RAMP
from component.jdb.hwy.hwy_def import IC_TYPE_IC
from component.jdb.hwy.hwy_def import IC_TYPE_SMART_IC
from component.jdb.hwy.hwy_toll_info import IC_DEFAULT_VAL
from component.jdb.hwy.hwy_toll_info import IC_TYPE_TRUE
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_ENTER
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_EXIT
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_JCT
from component.jdb.hwy.hwy_toll_info import TOLL_CLS_PASS
from component.jdb.hwy.hwy_toll_info import HwyTollType
from component.jdb.hwy.hwy_data_mng import HwyDataMng
from component.jdb.hwy.hwy_ic_info import convert_tile_id
from component.jdb.hwy.hwy_ic_info import ADD_NODE_POS_TYPE_BWD
from component.jdb.hwy.hwy_ic_info import ADD_NODE_POS_TYPE_EQUAL
from component.jdb.char_code_convert import strQ2B_jp


class HwyNodeAddInfo(component.component_base.comp_base):
    '''
    Node附加情报
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HwyNodeAddInfo')

    def _Do(self):
        # 料金所的附加情报
        self.CreateTable2('highway_node_add_info')
        self._make_toll_node_add_info()  # 料金
        self._make_ic_node_add_info()  # IC，除料金外
        self.CreateIndex2('highway_node_add_info_link_id_idx')
        self.CreateIndex2('highway_node_add_info_node_id_link_id_idx')
        # 添加坐标信息(the_geom)——给QGis显示用
        self._add_the_geom()

    def _make_toll_node_add_info(self):
        toll_data = self._get_toll_data()
        prev_node_id = None
        for toll in toll_data:
            node_id, dummy_toll_node = toll[0:2]
            up_down, facility_id = toll[2:4]
            toll_classes, class_names, etc_antennas, enters = toll[4:8]
            exits, jcts, sa_pas = toll[8:11]
            unopen = toll[11][0]
            dummy = toll[12][0]
            road_code = toll[24][0]
            road_seq = toll[25][0]
            out_link_id = self._get_out_link(node_id)
            if not out_link_id:
                self.log.error('Not Exist OutLink for toll. node_id=%s'
                               % node_id)
                continue
            toll_facil = self._get_toll_facil(node_id, road_code, road_seq)
            if not toll_facil:
                self.log.error('No Toll Facil Info. node_id=%s' % node_id)
                continue
            no_toll_money = IC_DEFAULT_VAL
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
            self._set_toll_class(toll_classes,
                                 toll_facil.tollclass,
                                 toll_facil.facilcls,
                                 toll_type)
            toll_type.unopen = unopen
            # 料金所名称
            name_kanji = toll_facil.facil_name
            name_yomi = toll_facil.name_yomi
            addinfo_list = []
            if dummy_toll_node:  # 有dummy收费站(料金所上有多个番号)，这时收费站信息做在dummy点.
                dummy_out_link = self._get_out_link(dummy_toll_node)
                if not dummy_out_link:
                    self.log.error('Not Exist OutLink for dummy. node_id=%s'
                                   % node_id)
                    continue
                toll_type.not_guide = IC_TYPE_TRUE
                # 设置 架空Flag
                self._set_dummy_flag(toll_classes, toll_type, dummy)
                toll_flag = self._get_toll_flag(toll_type)
                dummy_addinfo = AddInfoData(dummy_out_link, dummy_toll_node,
                                            toll_flag, no_toll_money,
                                            facility_num, up_down,
                                            facility_id, name_kanji,
                                            name_yomi, seq_num,
                                            toll_type_num, toll_type)
                if prev_node_id != node_id:
                    # toll_type没有指定，里面的字段默认都设为"0"
                    toll_flag = IC_DEFAULT_VAL
                    toll_addinfo = AddInfoData(out_link_id, node_id,
                                               toll_flag, no_toll_money)
                    addinfo_list.append(toll_addinfo)
                addinfo_list.append(dummy_addinfo)
            else:  # 没dummy收费站，这时收费站信息做在料金点
                # 设置 架空Flag
                self._set_dummy_flag(toll_classes, toll_type, dummy)
                toll_flag = self._get_toll_flag(toll_type)
                toll_addinfo = AddInfoData(out_link_id, node_id,
                                           toll_flag, no_toll_money,
                                           facility_num, up_down,
                                           facility_id, name_kanji,
                                           name_yomi, seq_num,
                                           toll_type_num, toll_type)
                addinfo_list.append(toll_addinfo)
            for addinfo in addinfo_list:
                self.__insert_add_info(addinfo)
            prev_node_id = node_id
        self.pg.commit2()

    def _set_toll_class(self, toll_classes, tollclass_c,
                        facilclass, hwy_toll_type):
        '''料金种别'''
        # 高速道路入口
        if tollclass_c == TOLL_CLASS_MAIN:  # 本線上料金所
            hwy_toll_type.gate = IC_TYPE_TRUE  # Barrier/Gate
        elif tollclass_c == TOLL_CLASS_BARRIER:  # バリア(检札所)
            hwy_toll_type.gate = IC_TYPE_TRUE  # Barrier/Gate
        elif tollclass_c == TOLL_CLASS_RAMP:  # ランプ上料金所
            if facilclass == IC_TYPE_TOLL:  # 料金所
                pass  # 什么种别都不做
            elif set(toll_classes) == set([TOLL_CLS_ENTER]):  # 入口
                hwy_toll_type.enter = IC_TYPE_TRUE
            elif set(toll_classes) == set([TOLL_CLS_EXIT]):  # 出口
                hwy_toll_type.exit = IC_TYPE_TRUE
            elif set(toll_classes) == set([TOLL_CLS_JCT]):  # JCT/UTURN
                pass   # 什么种别都不做
            else:
                self.log.warning('Unkown toll class. toll_classes=%s'
                                 % toll_classes)

    def _set_dummy_flag(self, toll_classes, hwy_toll_type, toll_info_dummy):
        '''架空Flag'''
        if(toll_classes[0] == TOLL_CLS_PASS and  # 本线通过料金所
           not toll_info_dummy):  # 料金情报的架空flag为False
            hwy_toll_type.dummy_facil = toll_info_dummy
        else:
            hwy_toll_type.dummy_facil = hwy_toll_type.not_guide

    def _get_toll_flag(self, toll_type=None):
        # "架空", 且"非案内"，料金所案内Flag设为IC_DEFAULT_VAL
        if toll_type.dummy_facil and toll_type.not_guide:
            return IC_DEFAULT_VAL
        else:
            return IC_TYPE_TRUE

    def _get_toll_facil(self, node_id, road_code, road_seq):
        hwy_data = HwyDataMng.instance()
        toll_facils = hwy_data.get_toll_facil(node_id)
        for toll_facil in toll_facils:
            if(toll_facil.road_code == road_code and
               toll_facil.road_point == road_seq):
                return toll_facil

    def _get_out_link(self, node_id):
        out_link_id = None
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        out_linkids = G.get_outlinkids(node_id)
        if out_linkids:
            if len(out_linkids) > 1:
                self.log.error('OutLink Number > 1. node_id=%s'
                               % node_id)
            else:
                out_link_id = out_linkids[0]
        return out_link_id

    def _add_the_geom(self):
        '''添加坐标信息'''
        # ## node坐标
        sqlcmd = """
        SELECT AddGeometryColumn('','highway_node_add_info',
                                 'node_geom','4326','POINT',2);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
        UPDATE highway_node_add_info as a
           SET node_geom = b.the_geom
         from node_tbl as b
         WHERE a.node_id = b.node_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('highway_node_add_info_node_geom_idx')
        # ## link坐标
        sqlcmd = """
        SELECT AddGeometryColumn('','highway_node_add_info',
                                 'link_geom','4326','LINESTRING',2);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        sqlcmd = """
        UPDATE highway_node_add_info as a
           SET link_geom = b.the_geom
         from link_tbl as b
         WHERE a.link_id = b.link_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('highway_node_add_info_link_geom_idx')

    def _get_toll_data(self):
        sqlcmd = """
        SELECT node_id, dummy_toll_node,
               up_down, facility_id,
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
               array_agg(road_code) road_code, array_agg(road_seq) road_seq
          from (
                SELECT distinct node_id, dummy_toll_node, toll_class,
                       class_name, up_down, facility_id,
                       etc_antenna, enter, exit,
                       jct, sa_pa, unopen,
                       dummy, non_ticket_gate, check_gate,
                       single_gate, cal_gate, ticket_gate,
                       nest, uturn, not_guide,
                       normal_toll, etc_toll, etc_section,
                       road_code, road_seq
                  FROM highway_toll_info
          ) as toll
          group by node_id, dummy_toll_node, facility_id, up_down
          order by node_id, up_down, facility_id, dummy_toll_node;
        """
        return self.get_batch_data(sqlcmd)

    def __insert_add_info(self, add_info):
        sqlcmd = """
        INSERT INTO highway_node_add_info(
                    link_id, node_id,
                    toll_flag, no_toll_money,
                    facility_num, up_down,
                    facility_id, seq_num,
                    etc_antenna, enter,
                    exit, jct,
                    sapa, gate,
                    un_open, dummy,
                    toll_type_num, non_ticket_gate,
                    check_gate, single_gate,
                    cal_gate, ticket_gate,
                    nest, uturn,
                    not_guide, normal_toll,
                    etc_toll, etc_section,
                    "name", tile_id,
                    no_toll_flag)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s, %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    mid_get_json_string_for_japan_names(array[%s], array[%s]),
                    %s, %s);
        """
        toll_type = add_info.toll_type
        self.pg.execute2(sqlcmd, (add_info.link_id, add_info.node_id,
                                  add_info.toll_flag, add_info.no_toll_money,
                                  add_info.facility_num, add_info.up_down,
                                  add_info.facility_id, add_info.seq_num,
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
                                  add_info.name_kanji, add_info.name_yomi,
                                  add_info.tile_id, add_info.no_toll_flag)
                         )

    def _make_ic_node_add_info(self):
        """设施附加情报"""
        # 已知的多个设施名称
        MULTI_NAMES = (set(['海老名ＪＣＴ', '厚木ＩＣ']),
                       set(['三郷ランプ', '三郷ＩＣ']))
        data = self._get_add_info_data()
        for info in data:
            link_id, node_id = info[0:2]
            updowns, facility_ids, facilcls_list, in_outs = info[2:6]
            name_kanjis, name_yomis, pos_types, pos_type_names = info[6:10]
            tile_id = info[10]
            no_toll_flags = info[11]  # 无料区间flag. 1: 无料，0: 非无料
            if len(set(no_toll_flags)) == 1:
                no_toll_flag = no_toll_flags[0]
            else:
                self.log.error('存在多种"无料区间标志". no_toll_flags=%s'
                               % set(no_toll_flags))
                continue
            facility_id = min(facility_ids)
            updown = self._merge_updwon(updowns, facility_ids,
                                        facility_id, facilcls_list)  # 上下行合并
            toll_type = HwyTollType()
            # 判断设施种类唯一
            if len(set(facilcls_list)) == 1 and len(set(in_outs)) == 1:
                toll_type.set_facil_type(facilcls_list[0], in_outs[0])
            else:
                if set(name_kanjis) not in MULTI_NAMES:
                    self.log.warning('多个设施种别. link_id=%s, facil_class=%s,'
                                     'facil_name=%s' %
                                     (link_id, facilcls_list,
                                      ','.join(set(name_kanjis))))
                facility_idx = facility_ids.index(facility_id)
                toll_type.set_facil_type(facilcls_list[facility_idx],
                                         in_outs[facility_idx])
            # 当设施种别为IC时，附加情报点在料金所后方的不收录
            if facilcls_list[0] in (IC_TYPE_RAMP, IC_TYPE_IC,
                                    IC_TYPE_SMART_IC):
                if len(set(pos_types)) == 1:
                    if pos_types[0] == ADD_NODE_POS_TYPE_BWD:  # 在料金所后方
                        continue  # 在料金所附加情报点，不收录
                    elif pos_types[0] == ADD_NODE_POS_TYPE_EQUAL:  # 重叠
                        self.log.warning("点和料金重叠. link_id=%s, node_id=%s"
                                         % (link_id, node_id))
                else:
                    self.log.error('多个位置类别. position_type=%s'
                                   % pos_type_names)
                    continue
            # 判断设施名称唯一
            if len(set(name_kanjis)) == 1:
                name_kanji = name_kanjis[0]
                name_yomi = name_yomis[0]
            else:
                if set(name_kanjis) not in MULTI_NAMES:
                    self.log.warning('多个设施名称:link_id=%s, facil_name=%s'
                                     % (link_id, ','.join(set(name_kanjis))))
                name_kanji, name_yomi = self._get_facil_name(name_kanjis,
                                                             name_yomis,
                                                             facility_ids,
                                                             facility_id)
            toll_flag = IC_DEFAULT_VAL
            no_toll_money = IC_DEFAULT_VAL
            facility_num = 1
            toll_type_num = 0
            seq_num = 1
            add_info = AddInfoData(link_id, node_id,
                                   toll_flag, no_toll_money,
                                   facility_num, updown,
                                   facility_id, name_kanji,
                                   name_yomi, seq_num,
                                   toll_type_num, toll_type,
                                   tile_id, no_toll_flag)
            self.__insert_add_info(add_info)
        self.pg.commit2()

    def _get_all_facil_name(self, name_kanjis):
        return ','.join(set(name_kanjis))

    def _get_facil_name(self, name_kanjis, name_yomis,
                        facility_ids, facility_id):
        for id_idx in range(0, len(facility_ids)):
            if facility_ids[id_idx] == facility_id:
                return name_kanjis[id_idx], name_yomis[id_idx]
        return None, None

    def _merge_updwon(self, updown_list, facility_ids,
                      facil_id, facilcls_list):
        '''相同设施种别的上下行合并，不同种别的不合并'''
        temp_updown_list = []
        if len(set(facilcls_list)) == 1:
            temp_updown_list = updown_list
        else:
            for id_idx in range(0, len(facility_ids)):
                if facility_ids[id_idx] == facil_id:
                    temp_updown_list.append(updown_list[id_idx])
        if HWY_UPDOWN_COMMON in temp_updown_list:
            return HWY_UPDOWN_COMMON
        if(HWY_UPDOWN_DOWN in temp_updown_list and
           HWY_UPDOWN_UP in temp_updown_list):
            return HWY_UPDOWN_COMMON
        if HWY_UPDOWN_DOWN in temp_updown_list:
            return HWY_UPDOWN_DOWN
        if HWY_UPDOWN_UP in temp_updown_list:
            return HWY_UPDOWN_UP
        return None

    def _get_add_info_data(self):
        '''料金情报，不包含料金所'''
        sqlcmd = """
        SELECT add_link_id,
               add_node_id,
               array_agg(updown) updown,
               array_agg(facility_id) facility_ids,
               array_agg(facilcls) facilcls,
               array_agg(in_out) in_outs,
               array_agg(name_kanji) name_kanjis,
               array_agg(name_yomi) name_yomis,
               array_agg(pos_type) pos_types,
               array_agg(pos_type_name) pos_type_names,
               tile_id,
               array_agg(toll_flag#1) as no_toll_flags
          FROM mid_hwy_node_add_info AS a
          LEFT JOIN link_tbl as b
          ON add_link_id = link_id
          group by tile_id, add_link_id, add_node_id
          order by tile_id, add_link_id, add_node_id
        """
        return self.get_batch_data(sqlcmd)


# =============================================================================
# AddInfoData
# =============================================================================
class AddInfoData():
    def __init__(self, link_id, node_id,
                 toll_flag=IC_DEFAULT_VAL, no_toll_money=IC_DEFAULT_VAL,
                 facility_num=IC_DEFAULT_VAL, up_down=IC_DEFAULT_VAL,
                 facility_id=IC_DEFAULT_VAL, name_kanji='',
                 name_yomi='', seq_num=IC_DEFAULT_VAL,
                 toll_type_num=IC_DEFAULT_VAL, toll_type=None,
                 tile_id=None, no_toll_flag=IC_DEFAULT_VAL):
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
        if toll_type:
            self.toll_type = toll_type
        else:
            self.toll_type = HwyTollType()
        self.name_kanji = strQ2B_jp(name_kanji)
        self.name_yomi = strQ2B_jp(name_yomi)
        self.tile_id = self._get_link_tile_id(self.node_id,
                                              self.link_id,
                                              tile_id)
        self.no_toll_flag = no_toll_flag

    def _get_link_tile_id(self, node_id, link_id, tile_id_14):
        if tile_id_14:
            return convert_tile_id(tile_id_14)
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        if node_id not in G:
            return None
        tile_id_14 = G.get_out_link_tile(node_id, link_id)
        if not tile_id_14:
            tile_id_14 = G.get_in_link_tile(node_id, link_id)
        return convert_tile_id(tile_id_14)
