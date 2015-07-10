# -*- coding: UTF8 -*-
'''
Created on 2015��5��27��

@author: PC_ZH
'''
from base.rdb_ItemBase import ItemBase


class rdb_highway_statis(ItemBase):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Highway_statis')

    def Do(self):

        return 0

    def _get_hwy_ic_info_sqlcmd(self):
        '''get hwy ic info sqlcmd '''
        sqlcmd = '''
        CREATE OR REPLACE VIEW rdb_highway_ic_statis_view
        AS(
            SELECT road_no, ic_no, name, ic_type, iso_country_code
            FROM (
                SELECT road_no, ic_no, name,
                       ic_type, link_id
                FROM (
                     SELECT view.road_no, name,
                        view.ic_no, ic_type,
                       CASE
                     WHEN center_flag = 1 THEN center_the_geom
                     ELSE main_the_geom
                       END AS the_geom ,
                       CASE
                     WHEN center_flag = 1 THEN center_link_id
                     ELSE  main_link_id
                       END AS link_id
                     FROM rdb_highway_ic_info_view AS view
                     LEFT JOIN rdb_highway_path_point_view AS path_point_view
                     ON view.ic_no = path_point_view.ic_no
                    )AS a
                ORDER BY ic_no
                )AS ic
            LEFT JOIN rdb_link
            ON ic.link_id = rdb_link.link_id
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _CreateView_hwy_ic_info(self):
        ''' '''
        sqlcmd = '''
        CREATE OR REPLACE VIEW rdb_highway_ic_info_view
        AS
        (
        SELECT  road_no, ic_no,
          (CASE
           WHEN tollgate = 1 then 'TOLL'
           WHEN jct = 1 then 'JCT'
           WHEN pa = 1 then 'PA'
           WHEN sa = 1 then 'SA'
           WHEN ic = 1 then 'IC'
           WHEN ramp = 1 then 'RAMP'
           END
           ) AS ic_type,  "name"
        FROM rdb_highway_ic_info
        WHERE boundary <> 1
        ORDER BY ic_no
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0

    def _CreateView_hwy_path_point(self):
        ''' '''
        print (' create hwy path point view')
        sqlcmd = '''
        CREATE OR REPLACE VIEW rdb_highway_path_point_view
        AS (
        SELECT ic.ic_no, main_flag, center_flag,
               main.link_id as main_link_id,
               center.link_id as center_link_id,
               main.the_geom as main_the_geom,
               center.the_geom as center_the_geom
        FROM rdb_highway_ic_info as ic
        LEFT JOIN (
            SELECT ic_no, link_id, main_flag, the_geom
            FROM rdb_highway_path_point
            WHERE main_flag = 1
        ) AS main
        ON ic.ic_no = main.ic_no
        LEFT JOIN (
            SELECT ic_no, link_id, center_flag, the_geom
            FROM rdb_highway_path_point
            WHERE center_flag = 1
        )AS center
        ON ic.ic_no = center.ic_no
        order by ic.ic_no
        );
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
