# -*- coding: UTF-8 -*-
'''
Created on 2015-9-29

@author: hcz
'''
import component.component_base
from component.rdf.hwy.hwy_def_rdf import HWY_TRUE


# ==============================================================================
# Hwy_Tollgate(收费站情报)
# ==============================================================================
class HwyTollgateRDF(component.component_base.comp_base):
    '''
    Make Highway Tollgate Facility Info.
    '''

    def __init__(self, ItemName='HwyTollgate'):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, ItemName)

    def _Do(self):
        self.CreateTable2('hwy_tollgate')
        self._make_main_toll()  # 本线上收费站
        # self._make_temp_ic_toll()  # IC上收费站
        # self._del_uturn_toll()
        # self._make_ic_toll()
        # self._update_tollgate_name()
        self.CreateIndex2('hwy_tollgate_node_id_idx')
        self.CreateIndex2('hwy_tollgate_the_geom_idx')

    def _make_main_toll(self):
        '''本线上收费站'''
        self.log.info('Start Make Main Toll.')
        sqlcmd = """
        INSERT INTO hwy_tollgate(road_code, road_seq, updown_c,
                                 facilcls_c, inout_c, node_id,
                                 tollcls_c, facil_name, path_type,
                                 link_id, the_geom)
        (
        SELECT a.road_code, a.road_seq, updown_c,
               facilcls_c, inout_c, a.node_id,
               tollcls_c, facil_name, NULL as path_type,
               (case when c.link_id is not null then c.link_id -- out link
                     else b.link_id -- In link
                end ) as link_id,
               a.the_geom
          FROM hwy_node as a
          LEFT JOIN hwy_link_road_code_info as b  -- out link
          ON a.road_code = b.road_code and
             a.updown_c = b.updown and
             a.node_id = b.node_id
          LEFT JOIN hwy_link_road_code_info as c  -- in link
          ON b.road_code = c.road_code and
             b.updown = c.updown and
             b.seq_nm + 1 = c.seq_nm
          where facilcls_c = 6
          order by a.road_code, updown_c, a.road_seq
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('End Make Main Toll.')

    def _make_temp_ic_toll(self):
        # 同时多个设施经过这个收费站，收费站有就多个设施名
        self.log.info('Make Temp IC Toll Info.')
        self.CreateTable2('mid_temp_hwy_ic_tollgate')
        sqlcmd = """
        INSERT INTO mid_temp_hwy_ic_tollgate(road_code, road_seq, updown_c,
                     facilcls_c, inout_c, node_id,
                     tollcls_c, facil_name, path_type,
                     link_id, prev_link_id, the_geom)
        (
        SELECT distinct road_code, road_seq, updown_c,
                        facilcls_c, inout_c, node_id,
                        0 as tollcls_c, toward_name, path_type,
                        link_id, prev_link_id, the_geom
          FROM (
              SELECT node_id, toll_flag, the_geom
              FROM node_tbl
              where toll_flag = 1
          ) AS toll
          INNER JOIN (
            SELECT road_code, road_seq, facilcls_c, updown_c,
                   node_array[seq_num] as node, path_type, inout_c,
                   (case
                    when inout_c = 2 and link_array[seq_num] is not null
                    then link_array[seq_num] -- out
                    else link_array[seq_num -1]  -- in
                    end) as link_id,
                   (case
                    when inout_c = 1 and link_array[seq_num] is not null
                    then link_array[seq_num]
                    else link_array[seq_num -1]  -- in
                    end) as prev_link_id
             FROM (
                SELECT road_code, road_seq, facilcls_c, updown_c,
                       node_array, path_type, inout_c,
                       generate_series(2, array_upper(node_array,
                                                       1)) as seq_num,
                       array_upper(node_array, 1) as max_seq_num,
                       link_array
                  FROM (
                    SELECT road_code, road_seq, facilcls_c, updown_c,
                           regexp_split_to_array(node_lid,
                              E'\\,+')::bigint[] as node_array,
                           regexp_split_to_array(link_lid,
                              E'\\,+')::bigint[] as link_array,
                           path_type, inout_c
                      FROM mid_temp_hwy_ic_path
                      where ((facilcls_c in (1, 2, 3) and inout_c = 2) or
                             (facilcls_c in (5, 7) and path_type <> 'UTURN') or
                             (facilcls_c <> 10 and path_type = 'UTURN'
                              and inout_c = 2)
                            )
                  ) AS a
              ) AS b
              where (facilcls_c in (1, 2, 3) and seq_num < max_seq_num) or
                     facilcls_c not in (1, 2, 3)
          ) as c
          ON toll.node_id = c.node
          LEFT JOIN towardname_tbl
          ON toll.node_id = towardname_tbl.nodeid and
             guideattr = 7 and namekind = 2
          order by road_code, road_seq, updown_c, inout_c, node_id, path_type
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _del_uturn_toll(self):
        '''Uturn上的收费站，同时也是其他设施的收费站，那么删除Uturn情报
           otherwise 删除收费站的uturn信息。
        '''
        self.log.info('Delete Uturn Toll Info.')
        sqlcmd = """
        DELETE FROM mid_temp_hwy_ic_tollgate
          WHERE path_type = 'UTURN' and node_id in (
            SELECT node_id
              FROM (
                select node_id, array_agg(path_type) AS path_types
                  from mid_temp_hwy_ic_tollgate
                  GROUP BY node_id
              ) as a
              where 'UTURN' = any(path_types) and
                (not 'UTURN' = all(path_types))
          )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_ic_toll(self):
        # ## 出口收费站
        self.log.info('Make IC Toll Info.')
        sqlcmd = """
        INSERT INTO hwy_tollgate(road_code, road_seq, updown_c,
                                 facilcls_c, inout_c, node_id,
                                 tollcls_c, facil_name, path_type,
                                 link_id, the_geom)
        (
        SELECT distinct road_code, road_seq, updown_c,
               facilcls_c, inout_c, node_id,
               tollcls_c, facil_name,path_type,
               link_id, the_geom
          FROM mid_temp_hwy_ic_tollgate
          WHERE inout_c = 2 -- OUT
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        # ## 入口收费站
        # 入口和出口共用收费站时，入口收费站的link_id和出口收费站的link_id一致
        sqlcmd = """
        INSERT INTO hwy_tollgate(road_code, road_seq, updown_c,
                                 facilcls_c, inout_c, node_id,
                                 tollcls_c, facil_name, path_type,
                                 link_id, the_geom)
        (
        SELECT distinct road_code, road_seq, updown_c,
               facilcls_c, inout_c, a.node_id,
               tollcls_c, facil_name, path_type,
               (case when b.link_id is not null then b.link_id
                 else a.link_id end) as link_id,
               the_geom
        FROM (
            SELECT road_code, road_seq, updown_c,
                   facilcls_c, inout_c, prev_link_id,
                   node_id, link_id, tollcls_c,
                   facil_name, path_type, the_geom
              FROM mid_temp_hwy_ic_tollgate
              where inout_c = 1
        ) as a
        LEFT JOIN (
            SELECT distinct prev_link_id, node_id, link_id
              FROM mid_temp_hwy_ic_tollgate
              where inout_c = 2
        ) as b
        ON a.node_id = b.node_id and
           a.link_id = b.prev_link_id and
           a.prev_link_id = b.link_id
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _update_tollgate_name(self):
        # 1. 收费站所在点名称==>收费站站名称
        sqlcmd = """
        UPDATE hwy_tollgate SET facil_name = a.toward_name,
                                toll_name_flg = %s
          FROM (
            SELECT nodeid, toward_name
            FROM towardname_tbl
            where guideattr = 7 and namekind = 2
          ) as a
          where toward_name is not null and
                hwy_tollgate.node_id = a.nodeid
        """
        self.pg.execute2(sqlcmd, (HWY_TRUE, ))
        self.pg.commit2()
        return 0
        # 2. 收费站没有名称，且设施番号唯一时，设施名称==>收费站站名称
        sqlcmd = """
        UPDATE hwy_tollgate SET facil_name = d.facil_name
         FROM (
            SELECT b.node_id, b.link_id, facil_name
              FROM (
                SELECT node_id, link_id,
                       (array_agg(road_code))[1] as road_code,
                       (array_agg(road_seq))[1] as road_seq
                  FROM (
                    SELECT distinct node_id, link_id,
                           road_code, road_seq, inout_c
                      FROM hwy_tollgate
                      WHERE facil_name is null
                  ) as a
                  GROUP BY node_id, link_id
                  having count(*) = 1
              ) AS b
              LEFT JOIN hwy_node as c
              ON b.road_code = c.road_code and
                 b.road_seq = c.road_seq
          ) AS d
          where hwy_tollgate.node_id =  d.node_id and
                hwy_tollgate.link_id = d.link_id and
                d.facil_name is not null;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
