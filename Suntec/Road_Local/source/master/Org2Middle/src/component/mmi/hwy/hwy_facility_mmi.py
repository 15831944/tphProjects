# -*- coding: UTF-8 -*-
'''
Created on 2015-4-27

@author: hcz
'''
from component.rdf.hwy.highway_rdf import HwyFacilityRDF


class HwyFacilityMmi(HwyFacilityRDF):
    '''生成设施情报(MMI)
    '''

    def __init__(self, data_mng):
        '''
        Constructor
        '''
        HwyFacilityRDF.__init__(self, data_mng)

    def _get_rest_area_info(self):
        '''取得服'''
        sqlcmd = """
        SELECT DISTINCT road_code, road_seq, c.poi_id,
               1 as rest_area_type, name  -- 1: SA
          FROM (
            SELECT road_code, road_seq, facilcls_c,
                   regexp_split_to_table(link_lid, E'\\,+')::BIGINT as link_id
              FROM mid_temp_hwy_ic_path as a
              where facilcls_c in (1, 2) and   -- 1: sa, 2: pa
                    link_lid <> '' and link_lid is not null
          ) as b
          LEFT JOIN mid_temp_poi_link as c
          ON b.link_id = c.link_id
          LEFT JOIN mid_temp_hwy_sapa_name as d
          ON c.poi_id = d.poi_id
          INNER JOIN org_poi_point as e
          ON c.poi_id = e.uid::BIGINT and e.cat_code = 'TRNRST'
          ORDER BY road_code, road_seq
        """
        return self.get_batch_data(sqlcmd)
