# -*- coding: UTF8 -*-
'''
Created on 2014-7-7

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
import networkx as nx


HWY_UPDOWN_DOWN = 0  # 下り
HWY_UPDOWN_UP = 1  # 上り
HWY_UPDOWN_COMMON = 3  # 上下線共有
HWY_FLAG_FALSE = 0
HWY_FLAG_TRUE = 1
TEMP_TOLL_CSL_ABOUT = 1  # 概算料金
TEMP_TOLL_CSL_ABOVE = 2  # 最小料金
MONEY_IDX_INVALID = 0


class rdb_hwy_fee(ItemBase):
    '''料金金额
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Fee')
        self._toll_tile_dict = {}  # 料金的tile
        self._ic_toll_tile_dict = {}  # 设施对应的料金所在tile
        self._ic_tile_dict = {}  # 设施的tile(找不到料金所时使用)
        self._alone_updown_dict = {}
        self._temp_toll_dict = {}

    def Do(self):
        self._make_fee_money()  # 金额
        self._make_toll_info()  # 料金情报
        self._make_free_toll()  # 无券料金
        self._make_alone_toll()  # 单独料金
        self._make_ticket_toll()  # 区间(券)料金
        self._make_same_facility()  # 同一设施对象

    def _make_fee_money(self):
        '''金额(钱)的引索表'''
        self.CreateTable2('rdb_highway_fee_money')
        if (not self.pg.IsExistTable('highway_fee_alone_toll') or
            not self.pg.IsExistTable('highway_fee_ticket_toll')):
            return
        sqlcmd = """
        SELECT money
          FROM (
                -----alone------------
                SELECT k_money as money
                  FROM highway_fee_alone_toll
                  where etc_discount_c = -1
                union
                SELECT s_money as money
                  FROM highway_fee_alone_toll
                  where etc_discount_c = -1
                union
                SELECT m_money as money
                  FROM highway_fee_alone_toll
                  where etc_discount_c = -1
                union
                SELECT l_money as money
                  FROM highway_fee_alone_toll
                  where etc_discount_c = -1
                union
                SELECT vl_money as money
                  FROM highway_fee_alone_toll
                  where etc_discount_c = -1
                ------ticket-----------
                union
                SELECT k_money as money
                  FROM highway_fee_ticket_toll
                  where etc_discount_c = -1
                union
                SELECT s_money as money
                  FROM highway_fee_ticket_toll
                  where etc_discount_c = -1
                union
                SELECT m_money as money
                  FROM highway_fee_ticket_toll
                  where etc_discount_c = -1
                union
                SELECT l_money as money
                  FROM highway_fee_ticket_toll
                  where etc_discount_c = -1
                union
                SELECT vl_money as money
                  FROM highway_fee_ticket_toll
                  where etc_discount_c = -1
          ) AS A
          order by money;
        """
        money_idx = 1
        for info in self.pg.get_batch_data2(sqlcmd):
            money = info[0]
            sqlcmd = """ INSERT INTO rdb_highway_fee_money(money_idx, money)
                           VALUES (%s, %s);
                     """
            if money in (0, -1):
                self.pg.execute2(sqlcmd, (MONEY_IDX_INVALID, money))
            else:
                self.pg.execute2(sqlcmd, (money_idx, money))
                money_idx += 1
        self.pg.commit2()

    def _make_toll_info(self):
        '''料金情报'''
        self.CreateTable2('rdb_highway_fee_toll_info')
        if not self.pg.IsExistTable('highway_fee_toll_info'):
            return
        self._load_toll_tile_id()  # 料金设施Tile
        self._load_ic_toll_tile_id()  # 设施的料金所在Tile
        self._load_ic_tile_id()  # 设施所在Tile
        self._load_alone_toll_updown()  # 单独料金上下行情报
        self._load_temp_toll()  # 暂时收费(大概，以上)
        sqlcmd = """
        SELECT facility_id, up_down,
               ticket_flag, alone_flag,
               free_flag
         FROM highway_fee_toll_info
         ORDER BY facility_id, up_down
        """
        for info in self.pg.get_batch_data2(sqlcmd):
            # print info[0]
            facility_id = info[0]
            up_down = info[1]
            ticket_flag, alone_flag, free_flag = info[2:5]
            if (set([ticket_flag, alone_flag, free_flag]) ==
                set([HWY_FLAG_FALSE])):
                self.log.error('无料金情报。facil_id=%s', facility_id)
                continue
            # 上下行单独料金金额相同
            if up_down == HWY_UPDOWN_COMMON:
                min_tile_id = self._get_tile_id(facility_id, None)
                if not min_tile_id:
                    self.log.error("No tile id.facility_id=%s" % facility_id)
                    continue
                up_down = HWY_UPDOWN_DOWN  # 上下金额相同时，做成下行
            else:  # 上下行单独料金金额不相同
                min_tile_id = self._get_tile_id(facility_id, up_down)
                if not min_tile_id:
                    self.log.warning("No tile id.facility_id=%s"
                                     % facility_id)
                    continue
            # 取得大约、以上Flag
            about_flag, above_flag = self._get_temp_flag(facility_id)
            # 保存
            param = (up_down, facility_id, about_flag,
                     above_flag, ticket_flag, alone_flag,
                     free_flag, min_tile_id)
            self._insert_toll_info(param)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_fee_toll_info_facility_id_up_down_idx')

    def _make_alone_toll(self):
        '''单独料金'''
        self.CreateTable2('rdb_highway_fee_alone_toll')
        if not self.pg.IsExistTable('highway_fee_alone_toll'):
            return
        sqlcmd = """
        INSERT INTO rdb_highway_fee_alone_toll(
                                up_down, facility_id, k_money_idx,
                                s_money_idx, m_money_idx, l_money_idx,
                                vl_money_idx, tile_id)
        (
        SELECT info.up_down, a.facility_id, k.money_idx,
               s.money_idx, m.money_idx, l.money_idx,
               vl.money_idx, tile_id
          FROM highway_fee_alone_toll as a
          LEFT JOIN rdb_highway_fee_money as k
          on k_money = k.money
          LEFT JOIN rdb_highway_fee_money as s
          on s_money = s.money
          LEFT JOIN rdb_highway_fee_money as m
          on m_money = m.money
          LEFT JOIN rdb_highway_fee_money as l
          on l_money = l.money
          LEFT JOIN rdb_highway_fee_money as vl
          on vl_money = vl.money
          LEFT JOIN rdb_highway_fee_toll_info as info
          ON a.facility_id = info.facility_id
            and (a.up_down = info.up_down or
                 (a.up_down = 3 and info.up_down = 0)
                )
          where etc_discount_c = -1
          ORDER BY tile_id, facility_id, info.up_down
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_fee_alone_toll_facility_id_up_down_idx')

    def _make_ticket_toll(self):
        '''区间(券)料金'''
        self.CreateTable2('rdb_highway_fee_ticket_toll')
        if not self.pg.IsExistTable('highway_fee_ticket_toll'):
            return
        sqlcmd = """
        INSERT INTO rdb_highway_fee_ticket_toll(
                    from_facility_id, to_facility_id,
                    k_money_idx, s_money_idx,
                    m_money_idx, l_money_idx,
                    vl_money_idx, from_tile_id)
        (
        SELECT facility_id, to_facility_id,
               k.money_idx, s.money_idx,
               m.money_idx, l.money_idx,
               vl.money_idx, from_tile_id
          FROM (
                -- one direction
                SELECT a.facility_id, to_facility_id,
                       k_money, s_money,
                       b.m_money, l_money,
                       b.vl_money, tile_id as from_tile_id
                  FROM rdb_highway_fee_toll_info AS a
                  INNER JOIN (
                        -- money for one direction
                        SELECT from_facility_id, to_facility_id,
                               k_money, s_money,
                               m_money, l_money,
                               vl_money
                          FROM highway_fee_ticket_toll
                          WHERE (from_facility_id, to_facility_id) IN (
                                SELECT to_facility_id, from_facility_id
                                  FROM highway_fee_ticket_toll
                                ) and etc_discount_c = -1
                  ) as b
                  ON a.facility_id = b.from_facility_id
                  WHERE ticket_flag = 1
                ---------------------------------------------------------
                union
                ---------------------------------------------------------
                -- both direction
                SELECT facility_id,
                         (case when facility_id = from_facility_id
                             then to_facility_id
                             else from_facility_id end) as to_facility_id,
                         k_money, s_money,
                         m_money, l_money,
                         vl_money, tile_id as from_tile_id
                  FROM rdb_highway_fee_toll_info AS c
                  INNER JOIN (
                        -- money for both direction
                        SELECT from_facility_id, to_facility_id,
                               k_money, s_money,
                               m_money, l_money,
                               vl_money
                          FROM highway_fee_ticket_toll
                          WHERE (from_facility_id, to_facility_id) NOT IN (
                                SELECT to_facility_id, from_facility_id
                                  FROM highway_fee_ticket_toll
                          ) and etc_discount_c = -1
                  ) as d
                  ON c.facility_id = d.from_facility_id
                     OR c.facility_id = d.to_facility_id
                  WHERE ticket_flag = 1
          ) as e
          LEFT JOIN rdb_highway_fee_money as k
          on k_money = k.money
          LEFT JOIN rdb_highway_fee_money as s
          on s_money = s.money
          LEFT JOIN rdb_highway_fee_money as m
          on m_money = m.money
          LEFT JOIN rdb_highway_fee_money as l
          on l_money = l.money
          LEFT JOIN rdb_highway_fee_money as vl
          on vl_money = vl.money
          order by from_tile_id, facility_id, to_facility_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_fee_ticket_toll_'
                          'from_facility_id_to_facility_id_idx')

    def _make_free_toll(self):
        '''无券料金'''
        self.CreateTable2('rdb_highway_fee_free_toll')
        if not self.pg.IsExistTable('highway_fee_free_toll'):
            return
        sqlcmd = """
        INSERT INTO rdb_highway_fee_free_toll(
                    from_facility_id, to_facility_id, from_tile_id)
        (
        SELECT distinct from_facility_id, to_facility_id, tile_id
          FROM highway_fee_free_toll
          LEFT JOIN rdb_highway_fee_toll_info
          ON from_facility_id = facility_id
          order by tile_id, from_facility_id, to_facility_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_fee_free_toll_'
                          'from_facility_id_to_facility_id_idx')

    def _insert_toll_info(self, param):
        sqlcmd = """
        INSERT INTO rdb_highway_fee_toll_info(
                    up_down, facility_id, about_flag,
                    over_flag, ticket_flag, alone_flag,
                    free_flag, tile_id)
            VALUES (%s, %s, %s,
                    %s, %s, %s,
                    %s, %s);
        """
        self.pg.execute2(sqlcmd, param)

    def _get_temp_flag(self, facility_id):
        '''暫定区分'''
        about_flag = HWY_FLAG_FALSE
        above_flag = HWY_FLAG_FALSE
        temp_toll_cls = self._temp_toll_dict.get(facility_id)
        if temp_toll_cls == TEMP_TOLL_CSL_ABOUT:  # 概算料金
            about_flag = HWY_FLAG_TRUE
        elif temp_toll_cls == TEMP_TOLL_CSL_ABOVE:  # 最小料金, 以上
            above_flag = HWY_FLAG_TRUE
        else:
            pass
        return about_flag, above_flag

    def _get_tile_id(self, facility_id, updown=None):
        '''取得料金设施所在tiles的最小tile_id.'''
        # ## 1. 先从料金tile里取最小的tile_id
        min_tile_id = self._get_min_tile_id(self._toll_tile_dict,
                                            facility_id,
                                            updown)
        if min_tile_id:
            return min_tile_id
        # 当前方向没有找到，那么取对面方向的料金tile_id。
        # 只有一个方向的料金所，但是有双方向的金额
        if updown is not None:
            opposite_updown = updown ^ 1
            min_tile_id = self._get_min_tile_id(self._toll_tile_dict,
                                                facility_id,
                                                opposite_updown)
            if min_tile_id:
                return min_tile_id
        #self.log.warning('No Toll tile id. facility_id=%s' % facility_id)
        # ## 2. 从设施对应的料金tile里取最小的tile_id
        min_tile_id = self._get_min_tile_id(self._ic_toll_tile_dict,
                                            facility_id,
                                            updown)
        if min_tile_id:
            return min_tile_id
        #self.log.warning('No IC Toll tile id. facility_id=%s' % facility_id)
        # ## 3. 取设施所在最小tile (设施没有料金所，但却有金额数据。)
        min_tile_id = self._get_min_tile_id(self._ic_tile_dict,
                                            facility_id,
                                            updown)
        return min_tile_id

    def _get_min_tile_id(self, tile_dict, facility_id, updown=None):
        '''取得料金设施所在tiles的最小tile_id.'''
        min_tile_id = None
        tile_info = tile_dict.get(facility_id)
        if not tile_info:
            return min_tile_id
        for (temp_updown, tile_id) in tile_info:
            if updown is None:
                min_tile_id = self._get_min_num(min_tile_id, tile_id)
            else:
                if (temp_updown == HWY_UPDOWN_COMMON or
                    updown == HWY_UPDOWN_COMMON):
                    min_tile_id = self._get_min_num(min_tile_id, tile_id)
                elif temp_updown == updown:
                    min_tile_id = self._get_min_num(min_tile_id, tile_id)
                else:
                    pass
        return min_tile_id

    def _get_min_num(self, s, t):
        if s is None:
            return t
        elif t is None:
            return s
        else:
            return min((s, t))

    def _load_toll_tile_id(self):
        '''料金设施的tile_id'''
        sqlcmd = """
        SELECT facility_id,
               ARRAY_AGG(up_down) AS up_downs,
               ARRAY_AGG(tile_id) AS tile_ids
          FROM (
                SELECT up_down, facility_id, min(tile_id) as tile_id
                  FROM rdb_highway_node_add_info
                  where toll_type_num = 1
                  group by facility_id, up_down
                  order by facility_id, up_down
          ) AS a
          group by facility_id
          order by facility_id;
        """
        for info in self.pg.get_batch_data2(sqlcmd):
            facility_id = info[0]
            up_downs = info[1]
            tile_ids = info[2]
            self._toll_tile_dict[facility_id] = zip(up_downs, tile_ids)

    def _load_ic_toll_tile_id(self):
        '''设施对应的料金所在的tile_id.(通常是设施的facilityid和料金的facilityid不相等)'''
        sqlcmd = """
        SELECT facility_id,
               array_agg(up_down) as up_downs,
               array_agg(tile_id) as tile_ids
          FROM (
                SELECT toll.up_down,
                       ic.facility_id,
                       min(add_info.tile_id) as tile_id
                  FROM highway_toll_info as toll
                  LEFT JOIN highway_ic_info as ic
                  ON toll.ic_no = ic.ic_no
                  LEFT JOIN highway_node_add_info as add_info
                  ON toll.node_id = add_info.node_id
                  where toll.facility_id <> ic.facility_id
                  group by ic.facility_id, toll.up_down
                  order by facility_id, up_down
           ) AS A
           GROUP BY facility_id
           ORDER BY facility_id
        """
        for info in self.pg.get_batch_data2(sqlcmd):
            facility_id = info[0]
            up_downs = info[1]
            tile_ids = info[2]
            self._ic_toll_tile_dict[facility_id] = zip(up_downs, tile_ids)

    def _load_ic_tile_id(self):
        '''设施对的tile_id.(设施料金情报，但是有料金金额时，使用。)'''
        sqlcmd = """
        SELECT facility_id,
               ARRAY_AGG(up_down) AS up_downs,
               array_agg(tile_id) AS tile_ids
          FROM (
                SELECT up_down, facility_id, min(tile_id) as tile_id
                  FROM highway_ic_info
                  group by facility_id, up_down
                  ORDER BY facility_id, up_down
          ) AS A
          GROUP BY facility_id
          ORDER BY facility_id;
        """
        for info in self.pg.get_batch_data2(sqlcmd):
            facility_id = info[0]
            up_downs = info[1]
            tile_ids = info[2]
            self._ic_tile_dict[facility_id] = zip(up_downs, tile_ids)

    def _load_alone_toll_updown(self):
        '''单独料金的上下行情报'''
        sqlcmd = """
        SELECT facility_id, array_agg(up_down)
          FROM (
                SELECT distinct facility_id, up_down
                  FROM highway_fee_alone_toll
                  where etc_discount_c = -1  -- -1: not etc discount
          ) as a
          group by facility_id
          ORDER BY facility_id;
        """
        for info in self.pg.get_batch_data2(sqlcmd):
            facility_id = info[0]
            up_downs = info[1]
            self._alone_updown_dict[facility_id] = up_downs

    def _load_temp_toll(self):
        '''加载临时收费情报'''
        sqlcmd = """
        SELECT facility_id, temptollclass_c
          FROM highway_fee_temp_toll
          where etc_discount_c = -1;
        """
        for info in self.pg.get_batch_data2(sqlcmd):
            facility_id = info[0]
            temptollcls = info[1]
            self._temp_toll_dict[facility_id] = temptollcls

    def _make_block(self):
        '''制作券(发券和结算)料金分区块。'''
        G = self._load_ticket_toll_to_graph()
        # 取得子图的所有node点的list. 注：子图内任意顶点，和其他子图不相连
        conn_comps = nx.connected_components(G)
        # 对子图排序：按子图里最小的设施番号，从小到大
        conn_comps.sort(cmp=lambda x, y: cmp(min(x), min(y)))
        # 对子图里的设施排序：设施番号从小到大
        for comp in conn_comps:
            comp.sort()
        for block_no in range(1, len(conn_comps) + 1):  # block_no从1到N
            print block_no, len(conn_comps[block_no - 1])

    def _load_ticket_toll_to_graph(self):
        '''加载券料金，到图'''
        sqlcmd = """
        SELECT from_facility_id, to_facility_id
          FROM highway_money_ticket_toll;
        """
        G = nx.Graph()
        for toll in self.pg.get_batch_data2(sqlcmd):
            from_facility_id = toll[0]
            to_facility_id = toll[1]
            G.add_edge(from_facility_id, to_facility_id)
        return G

    def _make_same_facility(self):
        '''同一设施料金对象'''
        self.CreateTable2('rdb_highway_fee_same_facility')
        if not self.pg.IsExistTable('highway_fee_same_facility'):
            return
        sqlcmd = """
        INSERT INTO rdb_highway_fee_same_facility(from_up_down,
                                                  from_facility_id,
                                                  to_up_down,
                                                  to_facility_id)
        (
          SELECT from_up_down, from_facility_id,
                 to_up_down, to_facility_id
            FROM highway_fee_same_facility
            order by gid
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_fee_same_facility_'
                          'from_facility_id_to_facility_id_idx')
