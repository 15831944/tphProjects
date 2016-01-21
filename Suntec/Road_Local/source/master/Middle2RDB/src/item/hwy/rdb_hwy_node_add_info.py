# -*- coding: UTF8 -*-
'''
Created on 2014-6-24

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase


class rdb_hwy_node_add_info(ItemBase):
    '''
    node附加情报类
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_Add_Info')

    def Do(self):
        self._make_highway_node_add_info()
        # 一张表拆成三张
        self._tear_add_info()
        self._check_tear_add_info()

    def _make_highway_node_add_info(self):
        # 注： 距离四舍五入处理
        self.CreateFunction2('rdb_get_start_end_flag')  # 起终点标志
        self.CreateFunction2('rdb_cvt_passlid_linkid')
        self.CreateTable2('rdb_highway_node_add_info')
        self._add_geom_column()
        if self.pg.IsExistTable('highway_node_add_info') != True:
            return 0
        sqlcmd = """
        INSERT INTO rdb_highway_node_add_info(
                    link_id, node_id, start_end_flag,
                    toll_flag, no_toll_money, facility_num,
                    up_down, facility_id, seq_num,
                    etc_antenna, enter, exit,
                    jct, sa, pa, gate,
                    un_open, dummy, toll_type_num,
                    non_ticket_gate, check_gate, single_gate,
                    cal_gate, ticket_gate, nest,
                    uturn, not_guide, normal_toll,
                    etc_toll, etc_section, "name",
                    tile_id, node_geom, link_geom,
                    dir, dir_s_node, dir_e_node,
                    link_lid)
        (
        SELECT tile_link_id, c.tile_node_id,
               rdb_get_start_end_flag(a.link_id, node_id),
               toll_flag, no_toll_money, facility_num,
               up_down, facility_id, a.seq_num,
               etc_antenna, enter, exit,
               jct, sa, pa, gate,
               un_open, dummy, toll_type_num,
               non_ticket_gate, check_gate, single_gate,
               cal_gate, ticket_gate, nest,
               a.uturn, not_guide, normal_toll,
               etc_toll, etc_section, "name",
               a.tile_id, node_geom, link_geom,
               (case
               when dir_s_node = d.s_node and dir_e_node = d.e_node then 0 --正
               when dir_s_node = d.e_node and dir_e_node = d.s_node then 1 --逆
               else NULL end) as dir,
               e.tile_node_id as dir_s_node,
               f.tile_node_id as dir_e_node,
               rdb_cvt_passlid_linkid(link_lid, ',')
          FROM highway_node_add_info as a
          LEFT JOIN rdb_tile_link as b
          ON a.link_id = b.old_link_id
          left join rdb_tile_node as c
          ON a.node_id = c.old_node_id
          LEFT JOIN link_tbl as d
          ON a.link_id = d.link_id
          left join rdb_tile_node as e
          ON a.dir_s_node = e.old_node_id
          left join rdb_tile_node as f
          ON a.dir_e_node = f.old_node_id
          order by a.tile_id, tile_link_id, a.seq_num
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_node_add_info_link_id_idx')
        self.CreateIndex2('rdb_highway_node_add_info_link_geom_idx')
        self.CreateIndex2('rdb_highway_node_add_info_node_geom_idx')
        # 更新link表的高速附加情报Flag
        self._update_hwy_flag()
        # 由于经常有手动更新，所以这里补充check
        if not self._check_hwy_flag():
            self.log.error('Check Hwy Flag failed.')

    def _add_geom_column(self):
        sqlcmd = """
        SELECT AddGeometryColumn('','rdb_highway_node_add_info',
                                 'node_geom','4326','POINT',2);
        SELECT AddGeometryColumn('','rdb_highway_node_add_info',
                                 'link_geom','4326','LINESTRING',2);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_hwy_flag(self):
        '''更新link表的高速附加情报Flag'''
        sqlcmd = """
        UPDATE rdb_link set hwy_flag = 1
          FROM rdb_highway_node_add_info as hwy
          WHERE rdb_link.link_id = hwy.link_id;

        UPDATE rdb_link_with_all_attri_view set hwy_flag = 1
          FROM rdb_highway_node_add_info as hwy
          WHERE rdb_link_with_all_attri_view.link_id = hwy.link_id;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _check_hwy_flag(self):
        sqlcmd = """
        SELECT count(a.link_id)
          FROM rdb_link_with_all_attri_view as a
          LEFT JOIN rdb_highway_node_add_info as b
          ON a.link_id = b.link_id
          where hwy_flag = 1 and b.link_id is null;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0]:
            return False
        sqlcmd = """
        SELECT count(a.link_id)
          FROM rdb_highway_node_add_info as a
          LEFT JOIN rdb_link_with_all_attri_view as b
          ON a.link_id = b.link_id
          where hwy_flag <> 1;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row and row[0]:
            return False
        return True

    def _tear_add_info(self):
        '''一张表拆成三张'''
        self.log.info('Tear Add Info Table.')
        self.CreateTable2('rdb_highway_node_add_info_link')
        self.CreateTable2('rdb_highway_node_add_info_facility')
        self.CreateTable2('rdb_highway_node_add_info_toll')
        add_info_links = []
        add_info_facilitys = []
        add_info_tolls = []
        prev_link_id = None
        add_info_id = 1
        seq_num = 1
        for row in self._get_add_info():
            (link_id, node_id, start_end_flag, toll_flag,
             no_toll_money, facility_num, up_down, facility_id,
             dir, dir_s_node, dir_e_node, etc_antenna,
             enter, exit, jct, sa,
             pa, gate, un_open, dummy,
             name, tile_id, link_lid, toll_type_num
             ) = row[:24]
            add_info_toll = []
            temp_num = 0
            toll_seq_num = 1
            for toll_info in zip(*row[24:-1]):
                temp_num += 0
                if toll_type_num == 0:
                    if set(toll_info) != set([0]):
                        self.log.error('')
                    else:
                        continue
                toll_seq_num += 1
                add_info_tolls.append((add_info_id,) + toll_info +
                                      (toll_seq_num,))
            if toll_type_num == 0 and temp_num > 1:
                self.log.error('toll_type_num == 0 and temp_num > 1.'
                               'link_id=%s' % link_id)
            if toll_type_num != len(add_info_toll):
                self.log.error('toll_type_num != len(add_info_toll).'
                               'link_id=%s' % link_id)
                toll_type_num = len(add_info_toll)
            self._store_add_toll(add_info_tolls)
            if prev_link_id == link_id:
                seq_num += 1
                add_info_links.append((link_id, add_info_id, seq_num, tile_id))
                add_info_facilitys.append((add_info_id, node_id, start_end_flag,
                                           toll_flag, no_toll_money, up_down,
                                           facility_id, dir, dir_s_node,
                                           dir_e_node, etc_antenna, enter,
                                           exit, jct, sa,
                                           pa, gate, un_open,
                                           dummy, toll_type_num, name,
                                           link_lid))
            else:
                # if facility_num != len(add_info_links) and prev_link_id:
                #    self.log.error('facility_num is not equal. link_id=%s'
                #                   % prev_link_id)
                # store
                self._store_add_link(add_info_links)
                self._store_add_facility(add_info_facilitys)
                seq_num = 1
                add_info_links = [(link_id, add_info_id, seq_num, tile_id)]
                add_info_facilitys = [(add_info_id, node_id, start_end_flag,
                                       toll_flag, no_toll_money, up_down,
                                       facility_id, dir, dir_s_node,
                                       dir_e_node, etc_antenna, enter,
                                       exit, jct, sa,
                                       pa, gate, un_open,
                                       dummy, toll_type_num, name,
                                       link_lid)]
                prev_link_id = link_id
            add_info_id += 1
        # store 最后一条
        self._store_add_link(add_info_links)
        self._store_add_facility(add_info_facilitys)
        self.pg.commit2()
        self.CreateIndex2('rdb_highway_node_add_info_link_'
                          'link_id_add_info_id_idx')
        self.CreateIndex2('rdb_highway_node_add_info_facility_'
                          'add_info_id_idx')
        self.CreateIndex2('rdb_highway_node_add_info_toll_'
                          'add_info_id_seq_num_idx')

    def _get_add_info(self):
        sqlcmd = """
        SELECT link_id, node_id, start_end_flag, toll_flag,
               no_toll_money, facility_num, up_down, facility_id,
               dir, dir_s_node, dir_e_node, etc_antenna,
               enter, exit, jct, sa,
               pa, gate, un_open, dummy,
               name, tile_id, link_lid, toll_type_num,
               array_agg(non_ticket_gate) non_ticket_gates,
               array_agg(check_gate) check_gates,
               array_agg(single_gate) single_gates,
               array_agg(cal_gate) cal_gates,
               array_agg(ticket_gate) ticket_gates,
               array_agg(nest) nests,
               array_agg(uturn) uturns,
               array_agg(not_guide) not_guides,
               array_agg(normal_toll) normal_tolls,
               array_agg(etc_toll) etc_tolls,
               array_agg(etc_section) etc_sections,
               min(seq_num) seq_num
        FROM (
            SELECT link_id, node_id, start_end_flag, toll_flag,
                   no_toll_money, facility_num, up_down, facility_id,
                   dir, dir_s_node, dir_e_node, seq_num,
                   etc_antenna, enter, exit, jct,
                   sa, pa, gate, un_open,
                   dummy, name, tile_id, link_lid,
                   toll_type_num, non_ticket_gate, check_gate, single_gate,
                   cal_gate, ticket_gate, nest, uturn,
                   not_guide, normal_toll, etc_toll, etc_section
              FROM rdb_highway_node_add_info
              ORDER BY tile_id, link_id, seq_num
        ) AS a
        GROUP BY link_id, node_id, start_end_flag, toll_flag,
             no_toll_money, facility_num, up_down, facility_id,
             dir, dir_s_node, dir_e_node, --seq_num,
             etc_antenna, enter, exit, jct,
             sa, pa, gate, un_open, dummy, name, tile_id, link_lid,
             toll_type_num
        ORDER BY tile_id, link_id, seq_num
        """
        return self.pg.get_batch_data2(sqlcmd)

    def _store_add_link(self, add_info_links):
        sqlcmd = """
        INSERT INTO rdb_highway_node_add_info_link(
                    link_id, add_info_id, seq_num,
                    tile_id, facility_num)
            VALUES (%s, %s, %s,
                    %s, %s);
        """
        facility_num = len(add_info_links)
        for add_info_link in add_info_links:
            self.pg.execute2(sqlcmd, add_info_link + (facility_num,))

    def _store_add_facility(self, add_info_facilitys):
        sqlcmd = """
        INSERT INTO rdb_highway_node_add_info_facility(
                    add_info_id, node_id, start_end_flag, toll_flag,
                    no_toll_money, up_down, facility_id, dir,
                    dir_s_node, dir_e_node, etc_antenna, enter,
                    exit, jct, sa, pa,
                    gate, un_open, dummy, toll_type_num,
                    name, link_lid)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s);
        """
        for add_info_facility in add_info_facilitys:
            self.pg.execute2(sqlcmd, add_info_facility)

    def _store_add_toll(self, add_info_tolls):
        sqlcmd = """
        INSERT INTO rdb_highway_node_add_info_toll(
                    add_info_id, non_ticket_gate, check_gate, single_gate,
                    cal_gate, ticket_gate, nest, uturn,
                    not_guide, normal_toll, etc_toll, etc_section,
                    seq_num)
            VALUES (%s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s, %s, %s, %s,
                    %s);
        """
        for add_info_toll in add_info_tolls:
            self.pg.execute2(sqlcmd, add_info_toll)

    def _check_tear_add_info(self):
        sqlcmd = """
        SELECT COUNT(*)
          FROM rdb_highway_node_add_info as org
          join (
            SELECT link_id, node_id, start_end_flag, toll_flag,
                   no_toll_money, facility_num, up_down, facility_id,
                   dir, dir_s_node, dir_e_node, a.seq_num,
                   etc_antenna, enter, exit, jct,
                   sa, pa, gate, un_open,
                   dummy, toll_type_num,
                   (case when non_ticket_gate is null then 0
                    else non_ticket_gate end) non_ticket_gate,
                   (case when check_gate is null then 0
                    else check_gate end) check_gate,
                   (case when single_gate is null then 0
                    else single_gate end) single_gate,
                   (case when cal_gate is null then 0
                    else cal_gate end) cal_gate,
                   (case when ticket_gate is null then
                    0 else ticket_gate end) ticket_gate,
                   (case when nest is null then
                    0 else nest end) nest,
                   (case when uturn is null then 0
                    else uturn end) uturn,
                   (case when not_guide is null then
                    0 else not_guide end) not_guide,
                   (case when normal_toll is null then
                    0 else normal_toll end) normal_toll,
                   (case when etc_toll is null then
                    0 else etc_toll end) etc_toll,
                   (case when etc_section is null then
                    0 else etc_section end) etc_section,
                   name, tile_id, link_lid
              FROM rdb_highway_node_add_info_link as a
              LEFT JOIN rdb_highway_node_add_info_facility as b
              ON a.add_info_id = b.add_info_id
              LEFT JOIN rdb_highway_node_add_info_toll AS c
              ON b.add_info_id = c.add_info_id
          ) as tear
          on org.link_id = tear.link_id and org.node_id = tear.node_id and
             org.start_end_flag = tear.start_end_flag and
             org.toll_flag = tear.toll_flag and
             org.no_toll_money = tear.no_toll_money and
             org.facility_num = tear.facility_num and
             org.up_down = tear.up_down and
             org.facility_id = tear.facility_id and
             org.dir = tear.dir and
             org.dir_s_node = tear.dir_s_node and
             org.dir_e_node = tear.dir_e_node and
             org.seq_num = tear.seq_num and
             org.etc_antenna = tear.etc_antenna and
             org.enter = tear.enter and org.exit = tear.exit and
             org.jct = tear.jct and org.sa = tear.sa and
             org.pa = tear.pa and org.gate = tear.gate and
             org.un_open = tear.un_open and org.dummy = tear.dummy  and
             org.toll_type_num = tear.toll_type_num and
             org.non_ticket_gate = tear.non_ticket_gate and
             org.check_gate = tear.check_gate and
             org.single_gate = tear.single_gate and
             org.cal_gate = tear.cal_gate and
             org.ticket_gate = tear.ticket_gate and
             org.nest = tear.nest and org.uturn = tear.uturn and
             org.not_guide = tear.not_guide and
             org.normal_toll = tear.normal_toll and
             org.etc_toll = tear.etc_toll and
             org.etc_section = tear.etc_section and
             (org.name = tear.name or
              (org.name is null and tear.name is null)) and
             org.tile_id = tear.tile_id  and
             (org.link_lid = tear.link_lid or
              (org.link_lid is null and tear.link_lid is null))
          WHERE org.link_id is null or tear.link_id is null;
        """
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row[0] == 0:
            return True
        self.log.error('Check Tear Add Info Failed.')
        return False
