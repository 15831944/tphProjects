# -*- coding: UTF8 -*-
'''
Created on 2014-5-26

@author: hongchenzai
'''
import component.component_base
from component.jdb.hwy.hwy_def import IC_TYPE_TRUE
from component.jdb.hwy.hwy_def import IC_DEFAULT_VAL
# from component.jdb.hwy.hwy_def import convert_updown
# from component.jdb.hwy.hwy_def import TOLL_FUNC_TICKET
# from component.jdb.hwy.hwy_def import TOLL_FUNC_CAL
# from component.jdb.hwy.hwy_def import TOLL_FUNC_SINGLE
# from component.jdb.hwy.hwy_def import TOLL_FUNC_SINGLE_TICKET
# from component.jdb.hwy.hwy_def import TOLL_FUNC_SINGLE_CAL
# from component.jdb.hwy.hwy_def import TOLL_FUNC_UTURN_CHECK
# from component.jdb.hwy.hwy_def import TOLL_FUNC_NO_TICKET
# from component.jdb.hwy.hwy_def import TOLL_FUNC_CAL_TICKET
from component.jdb.hwy.hwy_def import HWY_UPDOWN_DOWN
from component.jdb.hwy.hwy_def import HWY_UPDOWN_UP
from component.jdb.hwy.hwy_def import HWY_UPDOWN_COMMON
from component.jdb.hwy.hwy_data_mng import HwyDataMng
from component.jdb.hwy.hwy_ic_info import convert_tile_id


class HwyTollFee(component.component_base.comp_base):
    '''
    Toll Money(料金金额情报)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'HighwayTollFee')

    def _Do(self):
        self.CreateFunction2('mid_convert_road_code')
        self.CreateFunction2('mid_convert_road_seq')
        # 単独料金無効券発券型
        self._make_free_toll()
        # 单独料金金额
        self._make_alone_toll_money()
        # 区间(券)料金金额
        self._make_ticket_toll_money()
        # 临时料金金额(1:大约、2:以上)
        self._make_temp_toll_money()
        # 金额施設情報
        self._make_toll_info_money()
        # 同一设施
        self._make_toll_same_facility()
        return 0

    def _make_free_toll(self):
        '''単独料金無効券発券型'''
        self.log.info('Making Free Toll.')
        self.CreateTable2('highway_fee_free_toll')
        sqlcmd = """
        INSERT INTO highway_fee_free_toll(from_facility_id, to_facility_id)
        (
        SELECT b.facility_id as from_facility_id,
               c.facility_id as to_facility_id
          FROM (
                SELECT mid_convert_road_code(code1) AS road_code1,
                       mid_convert_road_seq(code1) as road_seq1,
                       mid_convert_road_code(code2) AS road_code2,
                       mid_convert_road_seq(code2) as road_seq2
                  FROM toll_free_table
          ) AS a
          LEFT JOIN mid_hwy_facility_id as b
          ON a.road_code1 = b.road_code and a.road_seq1 = b.road_point
          LEFT JOIN mid_hwy_facility_id as c
          ON a.road_code2 = c.road_code and a.road_seq2 = c.road_point
          ORDER BY from_facility_id, to_facility_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_alone_toll_money(self):
        '''单独料金'''
        self.log.info('Making Alone Toll.')
        self.CreateTable2('highway_fee_alone_toll')
        sqlcmd = """
        INSERT INTO highway_fee_alone_toll(
                      up_down, facility_id,
                      etc_discount_c, k_money,
                      s_money, m_money,
                      l_money, vl_money)
                VALUES(%s, %s, %s, %s,
                       %s, %s, %s, %s);
        """
        for toll in self._get_alone_toll_money():
            facility_id = toll[0]
            all_car_moneys = toll[2:]
            # 判断上下行金额是否相同
            if self._check_all_car_moneys(all_car_moneys):
                # 上下料金金额相同，上下行共用金额情报
                up_down = HWY_UPDOWN_COMMON
            else:
                #print facility_id
                up_down = None
            for (etc_discount, k_moneys, s_moneys, \
                 m_moneys, l_moneys, vl_moneys) in zip(*toll[1:]):
                k_money_up, k_money_down = eval('(' + k_moneys + ')')
                s_money_up, s_money_down = eval('(' + s_moneys + ')')
                m_money_up, m_money_down = eval('(' + m_moneys + ')')
                l_money_up, l_money_down = eval('(' + l_moneys + ')')
                vl_money_up, vl_money_down = eval('(' + vl_moneys + ')')
                if up_down == HWY_UPDOWN_COMMON:  # 上下料金金额相同
                    self.pg.execute2(sqlcmd, (up_down, facility_id,
                                              etc_discount, k_money_up,
                                              s_money_up, m_money_up,
                                              l_money_up, vl_money_up))
                else:
                    # 下行
                    self.pg.execute2(sqlcmd, (HWY_UPDOWN_DOWN, facility_id,
                                              etc_discount, k_money_down,
                                              s_money_down, m_money_down,
                                              l_money_down, vl_money_down))
                    # 上行
                    self.pg.execute2(sqlcmd, (HWY_UPDOWN_UP, facility_id,
                                              etc_discount, k_money_up,
                                              s_money_up, m_money_up,
                                              l_money_up, vl_money_up))
        self.pg.commit2()

    def _check_all_car_moneys(self, all_car_moneys):
        '''判断上下行金额是否相同。'''
        for car_moneys in all_car_moneys:
            for etc_discount_toll in car_moneys:   # 同种打折类型
                up_toll, down_toll = etc_discount_toll.split(',')
                if up_toll != down_toll:
                    # print all_car_moneys
                    return False
        return True

    def _get_alone_toll_money(self):
        sqlcmd = """
SELECT facility_id,
       array_agg(etc_discount_c) etc_discount_cs,
       array_agg(k_toll) as k_tolls,
       array_agg(s_toll) as s_tolls,
       array_agg(m_toll) as m_tolls,
       array_agg(l_toll) as l_tolls,
       array_agg(vl_toll) as vl_tolls
  FROM (
    SELECT facility_id, etc_discount_c,
           k_toll, s_toll,
           m_toll, l_toll,
           vl_toll
      FROM (
        SELECT mid_convert_road_code(code) as road_code,
               mid_convert_road_seq(code) as road_seq,
               etc_discount_c,
               k_toll_up::varchar || ',' || k_toll_down::varchar as k_toll,
               s_toll_up::varchar || ',' || s_toll_down::varchar as s_toll,
               m_toll_up::varchar || ',' || m_toll_down::varchar as m_toll,
               l_toll_up::varchar || ',' || l_toll_down::varchar as l_toll,
               vl_toll_up::varchar || ',' || vl_toll_down::varchar as vl_toll
          FROM toll_alone_table_ex
      ) AS a
      INNER JOIN mid_hwy_facility_id as b
      ON a.road_code = b.road_code and a.road_seq = b.road_point
      ORDER BY facility_id, etc_discount_c
  ) AS c
  group by facility_id
  order by facility_id
"""
        return self.get_batch_data(sqlcmd)

    def _make_ticket_toll_money(self):
        '''区间收费'''
        self.log.info('Making Ticket Toll.')
        self.CreateIndex2('toll_from_to_table_ex_code1_code2_idx')
        self.CreateTable2('highway_fee_ticket_toll')
        sqlcmd = """
        INSERT INTO highway_fee_ticket_toll(
                            from_facility_id, to_facility_id,
                            etc_discount_c, k_money,
                            s_money, m_money,
                            l_money, vl_money)
        (
        SELECT b.facility_id as from_facility_id,
               c.facility_id as to_facility_id,
               etc_discount_c, k_toll,
               s_toll, m_toll,
               l_toll, vl_toll
          from (
                SELECT mid_convert_road_code(code1) as from_road_code,
                       mid_convert_road_seq(code1) as from_road_seq,
                       mid_convert_road_code(code2) as to_road_code,
                       mid_convert_road_seq(code2) as to_road_seq,
                       etc_discount_c, k_toll,
                       s_toll, m_toll,
                       l_toll, vl_toll
                  FROM toll_from_to_table_ex
           ) as a
          INNER JOIN mid_hwy_facility_id as b
          ON a.from_road_code = b.road_code
             and a.from_road_seq = b.road_point
          INNER JOIN mid_hwy_facility_id as c
          ON a.to_road_code = c.road_code
          and a.to_road_seq = c.road_point
          order by from_facility_id, to_facility_id, etc_discount_c
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('highway_fee_ticket_toll_from_'
                          'facility_id_to_facility_id_idx')
        self.CreateIndex2('highway_fee_ticket_toll_from_'
                          'facility_id_to_facility_id_etc_idx')

    def _make_temp_toll_money(self):
        '''临时料金金额(大约、以上)'''
        self.log.info('Making Temp Toll.')
        self.CreateTable2('highway_fee_temp_toll')
        sqlcmd = """
        INSERT INTO highway_fee_temp_toll(
                           facility_id, temptollclass_c, etc_discount_c)
        (
        SELECT facility_id, temptollclass_c, etc_discount_c
          FROM toll_temp_facil_table_ex AS a
          LEFT JOIN mid_hwy_facility_id as b
          ON a.road_code = b.road_code
             and a.road_seq = b.road_point
          ORDER BY facility_id, etc_discount_c
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_toll_info_money(self):
        '''金额施設情報'''
        self.log.info('Making Toll Info.')
        self.CreateTable2('highway_fee_toll_info')
        # 上下行与料金记录条目
        # 如果单独料金上下金额不同, 就做成两条记录,上下行做成"1"和"0"
        # 如果单独料金上下金额相同, 只做一条记录, 上下行做成"3"
        # 如果单独料金无，只做一条记录, 上下行做成"3"
        sqlcmd = """
        INSERT INTO highway_fee_toll_info(up_down, facility_id,
                                          ticket_flag, alone_flag,
                                          free_flag)
        (
        SELECT (CASE WHEN alone.up_down is null then 3
                     else alone.up_down end) as up_down,
               a.facility_id,
               (ticket.facility_id is not null)::integer as ticket_flag,
               (alone.facility_id is not null)::integer as alone_flag,
               (free.facility_id is not null)::integer as free_flag
          FROM (
                SELECT facility_id
                  FROM highway_fee_alone_toll
                UNION
                SELECT from_facility_id
                  FROM highway_fee_ticket_toll
                UNION
                SELECT to_facility_id
                  FROM highway_fee_ticket_toll
                UNION
                SELECT from_facility_id
                  FROM highway_fee_free_toll
          ) AS a
          LEFT JOIN (
                SELECT distinct up_down, facility_id
                  FROM highway_fee_alone_toll
          ) as alone
          ON a.facility_id = alone.facility_id
          LEFT JOIN (
                SELECT from_facility_id AS facility_id
                  FROM highway_fee_ticket_toll
                UNION
                SELECT to_facility_id  AS facility_id
                  FROM highway_fee_ticket_toll
          ) AS ticket
          ON a.facility_id = ticket.facility_id
          LEFT JOIN (
                SELECT distinct from_facility_id AS facility_id
                  FROM highway_fee_free_toll
          ) as free
          ON a.facility_id = free.facility_id
          ORDER BY facility_id, up_down
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _insert_toll_info(self, params):
        sqlcmd = """
        INSERT INTO highway_fee_toll_info(up_down, facility_id,
                                          ticket_flag, alone_flag,
                                          free_flag, tile_lid)
                VALUES(%s, %s, %s, %s, %s, %s)
        """
        self.pg.execute2(sqlcmd, params)

    def _get_toll_tile_id(self, toll_nodes):
        tile_ids = set()
        hwy_data = HwyDataMng.instance()
        G = hwy_data.get_graph()
        for nodeid in toll_nodes:
            out_tiles = G.get_out_tile(nodeid)
            if len(out_tiles) > 1:
                self.log.warning('Out Tile > 1. toll_node_id=%s' % nodeid)
            for tile_id_14 in out_tiles:
                tile_id = convert_tile_id(tile_id_14)
                tile_ids.add(tile_id)
        tile_ids = list(tile_ids)
        tile_ids.sort()
        return tile_ids

    def _get_toll_function(self, tollfunc_list):
        '''收费功能：单独，区间(券)'''
        ticket_flag = IC_DEFAULT_VAL
        alone_flag = IC_DEFAULT_VAL
        free_flag = IC_DEFAULT_VAL
        for non_ticket, check, alone, cal, ticket in zip(*tollfunc_list):
            if (check == IC_TYPE_TRUE or
                cal == IC_TYPE_TRUE or
                ticket == IC_TYPE_TRUE):
                ticket_flag = IC_TYPE_TRUE
            if alone == IC_TYPE_TRUE:
                alone_flag = IC_TYPE_TRUE
            if non_ticket == IC_TYPE_TRUE:
                free_flag = IC_TYPE_TRUE
        return ticket_flag, alone_flag, free_flag
#         for tollfunc in tollfunc_list:
#             if tollfunc in (TOLL_FUNC_TICKET, TOLL_FUNC_CAL,
#                             TOLL_FUNC_SINGLE_TICKET, TOLL_FUNC_SINGLE_CAL,
#                             TOLL_FUNC_CAL_TICKET, TOLL_FUNC_UTURN_CHECK):
#                 ticket_flag = IC_TYPE_TRUE
#             if tollfunc in (TOLL_FUNC_SINGLE,
#                             TOLL_FUNC_SINGLE_TICKET, TOLL_FUNC_SINGLE_CAL):
#                 alone_flag = IC_TYPE_TRUE
#             if tollfunc == TOLL_FUNC_NO_TICKET:  # 単独料金無効発券機能
#                 free_flag = IC_TYPE_TRUE
#         return ticket_flag, alone_flag, free_flag

    def _get_toll_info(self):
        sqlcmd = """
        SELECT facility_id, up_down,
               array_agg(non_ticket_gate) as non_ticket_gates,
               array_agg(check_gate) as check_gates,
               array_agg(single_gate) as single_gates,
               array_agg(cal_gate) as cal_gates,
               array_agg(ticket_gate) as ticket_gates,
               array_agg(node_id) as node_ids
          from (
                SELECT distinct t.up_down, ic.facility_id,
                       non_ticket_gate, check_gate,
                       single_gate, cal_gate,
                       ticket_gate, node_id
                  FROM highway_toll_info as t
                  LEFT JOIN highway_ic_info as ic
                  ON t.ic_no = ic.ic_no
                  where toll_class <> 4   -- Not JCT Toll
                --------------------------------------------
                union
                --------------------------------------------
                SELECT distinct t.up_down, facility_id,
                       non_ticket_gate, check_gate,
                       single_gate, cal_gate,
                       ticket_gate, node_id
                  FROM highway_toll_info as t
                  where toll_class = 4    -- JCT Toll
                  order by facility_id, up_down
          ) as a
          group by facility_id, up_down;
        """
        return self.get_batch_data(sqlcmd)
        ###################################################
#         sqlcmd = """
#         SELECT facility_id, direction_c, tollfunc_c_list
#           FROM (
#                 SELECT road_code, road_seq,
#                        direction_c, array_agg(tollfunc_c) as tollfunc_c_list
#                   FROM (
#                         SELECT distinct road_code, road_seq,
#                                         direction_c, tollfunc_c
#                           FROM mid_road_hwynode_middle_nodeid
#                           WHERE tollclass_c in (1, 2, 3)
#                                 and hwymode_f = 1
#                   ) AS a
#                   group by road_code, road_seq, direction_c
#           ) AS a
#           LEFT JOIN mid_hwy_facility_id as b
#           ON a.road_code = b.road_code
#               and a.road_seq = b.road_point
#           ORDER BY facility_id, direction_c desc;
#         """
#         return self.get_batch_data(sqlcmd)

    def _make_toll_same_facility(self):
        '''同一设施料金对象。'''
        # from_up_down和to_up_down，都填0, 即不区分上下行.
        self.log.info('Making Toll Same Facility.')
        self.CreateTable2('highway_fee_same_facility')
        for nest_toll_info in self._get_nest_tolls():
            ic_no, conn_ic_no = nest_toll_info[0:2]
            nest_up_down, nest_facility_id = nest_toll_info[2:4]
            if not ic_no:
                self.log.error('ic_no is null of nest_toll')
                return
            if not conn_ic_no:  # 入れ子料金所在JCT之间， 所以conn_ic_no不能为空
                self.log.error('conn_ic_no is null of nest_toll')
                return
            road_no = self._get_road_no(ic_no)
            conn_road_no = self._get_road_no(conn_ic_no)
            nest_road_no = self._get_nest_toll_road_no(nest_facility_id)
            if road_no != nest_road_no and conn_road_no != nest_road_no:
                # 入れ子料金所的road_no和两关JCT的道路番号都不一样
                self.log.error('No Same RoadNo. toll_facility_id=%s'
                               % nest_facility_id)
                return
            if road_no != nest_road_no:
                diff_ic_no = ic_no
            elif conn_road_no != nest_road_no:
                diff_ic_no = conn_ic_no
            else:
                return
            same_ic_list = self._get_same_facility(diff_ic_no)
            if len(same_ic_list) > 1:  # 当前只有一个的
                self.log.warning('number of same IC > 1.')
            if not same_ic_list:
                self.log.error('No Same Facility for Facility(ic_no=%s)'
                               % ic_no)
            for same_ic_no in same_ic_list:
                toll_facils = self._get_toll_facility_id(same_ic_no)
                if not toll_facils:
                    self.log.error('No Toll Same Facility for Toll'
                                   '(toll_facility_id=%s)' % nest_facility_id)
                for toll in toll_facils:
                    up_down, facility_id = toll[0:2]
                    # 保存同一设施对象
                    self._insert_toll_same_facility(nest_up_down,
                                                    nest_facility_id,
                                                    up_down,
                                                    facility_id)
        self.CreateIndex2('highway_fee_same_facility_'
                          'from_facility_id_to_facility_id_idx')

    def _get_nest_tolls(self):
        '''入れ子料金所'''
        sqlcmd = """
        SELECT ic_no, conn_ic_no, up_down, facility_id
          FROM highway_toll_info
          where nest = 1
          order by facility_id;
        """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        return rows

    def _get_road_no(self, ic_no):
        '''Get road_no of ic_no'''
        sqlcmd = """
        SELECT road_no
          from highway_ic_info as ic
          where ic_no = %s
        """
        self.pg.execute2(sqlcmd, (ic_no,))
        row = self.pg.fetchone2()
        if row:
            return row[0]
        return None

    def _get_nest_toll_road_no(self, nest_facility_id):
        sqlcmd = """
        SELECT distinct road_no
          FROM mid_hwy_facility_id as a
          left join mid_hwy_road_no as b
          on a.road_code = b.roadcode
          where facility_id = %s
        """
        self.pg.execute2(sqlcmd, (nest_facility_id,))
        row = self.pg.fetchone2()
        if row:
            return row[0]
        return None

    def _get_toll_facility_id(self, ic_no):
        '''取得料金所的设施番号'''
        sqlcmd = """
        SELECT ic_no, conn_ic_no, up_down, facility_id
          FROM highway_toll_info
          where ic_no = %s
        """
        toll_facils = set()
        self.pg.execute2(sqlcmd, (ic_no,))
        rows = self.pg.fetchall2()
        for row in rows:
            up_down, facility_id = row[2:4]
            toll_facils.add((up_down, facility_id))
        return toll_facils

    def _get_same_facility(self, ic_no):
        '''取得并设, 注ic_no必须是连续的'''
        if not ic_no:
            return []
        # 前并设
        fwd_same_facil = self._get_fwd_same_facilty(ic_no)
        # 后并设
        bwd_same_facil = self._get_bwd_same_facilty(ic_no)
        return bwd_same_facil + fwd_same_facil

    def _get_fwd_same_facilty(self, ic_no):
        '''前并设'''
        same_facil = []
        sqlcmd = """
        SELECT gid, ic_no, forward_flag, reverse_flag
          FROM highway_ic_info
          where ic_no = %s;
        """
        self.pg.execute2(sqlcmd, (ic_no,))
        row = self.pg.fetchone2()
        if row:
            forward_flag = row[2]
            if forward_flag:
                same_facil.append(ic_no + 1)
                same_facil += self._get_fwd_same_facilty(ic_no + 1)
        return same_facil

    def _get_bwd_same_facilty(self, ic_no):
        '''后并设'''
        same_facil = []
        sqlcmd = """
        SELECT gid, ic_no, forward_flag, reverse_flag
          FROM highway_ic_info
          where ic_no = %s;
        """
        self.pg.execute2(sqlcmd, (ic_no,))
        row = self.pg.fetchone2()
        if row:
            reverse_flag = row[3]
            if reverse_flag:
                same_facil.append(ic_no - 1)
                same_facil += self._get_bwd_same_facilty(ic_no - 1)
        return same_facil

    def _insert_toll_same_facility(self, nest_up_down, nest_facility_id,
                                   up_down, facility_id):
        sqlcmd = """
        INSERT INTO highway_fee_same_facility(
                    from_up_down, from_facility_id,
                    to_up_down, to_facility_id)
            VALUES (%s, %s, %s, %s);
        """
        # nest_up_down 和 up_down，都填成[0]，即不考虑上下行
        nest_up_down = 0
        up_down = 0
        params = (nest_up_down, nest_facility_id,
                  up_down, facility_id)
        self.pg.execute2(sqlcmd, params)
        self.pg.commit2()
