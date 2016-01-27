# -*- coding: UTF-8 -*-
'''
Created on 2015-3-14

@author: hcz
'''
from component.rdf.hwy.hwy_store_rdf import HwyStoreRDF


class HwyStoreTa(HwyStoreRDF):
    ''' highway store (For SA/PA)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        HwyStoreRDF.__init__(self)

    def _make_store_info(self):
        self.log.info('Make SAPA Store Info.')
        sqlcmd = """
        INSERT INTO highway_store_info(ic_no, bis_time_flag, bis_time_num,
                                       store_kind, open_hour, open_min,
                                       close_hour, close_min, holiday,
                                       goldenweek, newyear, yearend,
                                       bonfestival, sunday, saturday,
                                       friday, thursday, wednesday,
                                       tuesday, monday, seq_nm,
                                       priority, service_kind
                                       )
        (
        SELECT distinct ic_no, 0 as bis_time_flag, 0 as bis_time_num,
               a.per_code as store_kind, 0 as open_hour, 0 as open_min,
               0 as close_hour, 0 as close_min, 0 as holiday,
               0 as goldenweek, 0 as newyear, 0 as yearend,
               0 as bonfestival,0 as sunday, 0 as saturday,
               0 as friday, 0 as thursday, 0 as wednesday,
               0 as tuesday, 0 as monday, 1 as seq_nm,
               a.priority, a.service_kind
          FROM hwy_store as a
          LEFT JOIN mid_hwy_ic_no as b
          ON a.road_code = b.road_code and
             a.road_seq = b.road_seq and
             a.updown_c = b.updown
          left join hwy_chain_name as c
          on a.per_code = c.u_code
          WHERE a.per_code is not null and
                a.service_kind is not null and
                c.chain_name is not null
          order by ic_no, service_kind, priority, store_kind
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_ic_store_info(self):
        self.log.info('Make IC Store Info.')
        sqlcmd = """
        INSERT INTO highway_store_info(ic_no, bis_time_flag, bis_time_num,
                                       store_kind, open_hour, open_min,
                                       close_hour, close_min, holiday,
                                       goldenweek, newyear, yearend,
                                       bonfestival, sunday, saturday,
                                       friday, thursday, wednesday,
                                       tuesday, monday, seq_nm,
                                       priority, service_kind
                                       )
        (
        SELECT distinct ic_no, 0 as bis_time_flag, 0 as bis_time_num,
               a.per_code as store_kind, 0 as open_hour, 0 as open_min,
               0 as close_hour, 0 as close_min, 0 as holiday,
               0 as goldenweek, 0 as newyear, 0 as yearend,
               0 as bonfestival,0 as sunday, 0 as saturday,
               0 as friday, 0 as thursday, 0 as wednesday,
               0 as tuesday, 0 as monday, 1 as seq_nm,
               a.priority, a.service_kind
          FROM hwy_ic_store as a
          LEFT JOIN mid_hwy_ic_no as b
          ON a.road_code = b.road_code and
             a.road_seq = b.road_seq and
             a.updown_c = b.updown and
             a.node_id = b.node_id
          left join hwy_chain_name as c
          on a.per_code = c.u_code
          WHERE a.per_code IS not NULL and
                a.service_kind is not null and
                c.chain_name is not null
          order by ic_no, service_kind, priority, store_kind
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
