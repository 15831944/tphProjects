# -*- coding: cp936 -*-
'''
Created on 2014-4-24

@author: zhaojie
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common
class rdb_park(ItemBase):
    def __init__(self):
        ItemBase.__init__(self,'park')
       
    def Do_CreateTable(self):
        self.CreateTable2('rdb_park_link')
        self.CreateTable2('rdb_park_node')
        self.CreateTable2('rdb_park_point')
        self.CreateTable2('rdb_park_region')
        return 0
    
    def Do_CreateFunction(self):
        'Create Function'
        return 0
    
    def Do(self):
        rdb_log.log(self.ItemName,'start make rdb park..','info')
        
        #judge park_link_tbl record
        sqlcmd = '''
                select count(*) from park_link_tbl;
                '''
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone2()
        
        if row[0] > 0 :
            self.__make_rdb_park_link()
            self.__make_rdb_park_node()
            self.__make_rdb_park_region()
            self.__make_park_point()
            pass
        
        rdb_log.log(self.ItemName,'end make rdb park..','info')
        return 0
    
    def __make_rdb_park_link(self):
        rdb_log.log(self.ItemName,'start make rdb_park_link','info')
        
        sqlcmd='''
                insert into rdb_park_link(park_link_id,park_link_oneway,park_s_node_id,
                                        park_e_node_id,park_link_length,park_link_connect_type,
                                        park_floor,park_link_lean,park_link_toll,park_link_add1,
                                        park_link_add2,park_region_id,the_geom)
                select a.link_id as park_link_id,
                    a.one_way as park_link_oneway,
                    a.start_node_id as park_s_node_id,
                    a.end_node_id as park_e_node_id,
                    c.park_link_length as park_link_length,
                    c.park_link_connect_type as park_link_connect_type,
                    c.park_floor as park_floor,
                    c.park_link_lean as park_link_lean,
                    c.park_link_toll as park_link_toll,
                    c.park_link_add1 as park_link_add1,
                    c.park_link_add2 as park_link_add2,
                    c.park_region_id as park_region_id,
                    a.the_geom as the_geom
                from
                (
                    select link_id,start_node_id,end_node_id,one_way,the_geom
                    from rdb_link
                    where (link_id::bit(32))::integer > 65535
                    order by link_id
                )as a
                left join
                rdb_tile_link as b
                on a.link_id = b.tile_link_id
                left join
                park_link_tbl as c
                on b.old_link_id = c.park_link_id;
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log(self.ItemName,'end make rdb_park_link','info')
        return 0
        
    def __make_rdb_park_node(self):
        rdb_log.log(self.ItemName,'start make rdb_park_node','info')
        
        self.CreateFunction2('rdb_get_outlink_park')
        sqlcmd='''
                insert into rdb_park_node(park_node_id,park_node_type,
                                        park_connect_link_num,park_connect_link_array,
                                        park_link_num,park_branches,park_region_id,the_geom)
                select node_id as park_node_id,
                    park_node_type as park_node_type,
                    array_upper(park_out_lib,1) as park_connect_link_num,
                    park_out_lib as park_connect_link_array,
                    link_num as park_link_num,
                    link_lib as park_branches,
                    park_region_id,
                    the_geom
                from
                (
                    select node_id,link_num,link_lib,d.park_node_type,
                        rdb_get_outlink_park(node_id,d.park_node_type,link_lib,
                            s_link_lib,e_link_lib,one_way_lib) as park_out_lib,
                            d.park_region_id,temp_c.the_geom
                    from
                    (
                        select node_id,count(node_id) as link_num,array_agg(link_id) as link_lib,
                            array_agg(start_node_id) as s_link_lib,
                            array_agg(end_node_id) as e_link_lib,
                            array_agg(one_way) as one_way_lib,the_geom
                        from
                        (
                            select node_id,link_id,start_node_id,end_node_id,one_way,angle,the_geom
                            from
                            (
                                select node_id,link_id,start_node_id,end_node_id,one_way,rdb_node.the_geom,
                                    (case when rdb_link.start_node_id = rdb_node.node_id then fazm else tazm end) as angle
                                from rdb_link
                                join
                                rdb_node
                                on rdb_link.start_node_id = rdb_node.node_id or rdb_link.end_node_id = rdb_node.node_id
                                where ((rdb_node.extend_flag::integer)::bit(2))::integer > 1
                            )as temp_a
                            order by node_id,angle
                        )as temp_b
                        group by node_id,the_geom
                    )as temp_c
                    left join
                    rdb_tile_node as c
                    on temp_c.node_id = c.tile_node_id
                    left join
                    park_node_tbl as d
                    on c.old_node_id = d.park_node_id
                 )as a;
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log(self.ItemName,'end make rdb_park_node','info')
        return 0
    
    def __make_rdb_park_region(self):
        rdb_log.log(self.ItemName,'start make rdb_park_region','info')
        
        #create tile
        self.CreateTable2('temp_tile_level_14_park')
        sqlcmd = '''
                insert into temp_tile_level_14_park(tile_id,tile_x, tile_y, the_geom)
                    select ((14 << 28) | (tile_x << 14) | tile_y) as tile_id,
                           tile_x, tile_y,
                           rdb_get_tile_area_byxy(tile_x, tile_y, 14)
                           as the_geom
                    from
                    (
                        select  tile_x,
                                generate_series(y_min, y_max) as tile_y
                        from
                        (
                            select generate_series(x_min, x_max)
                                   as tile_x, y_min, y_max
                            from
                            (
                                select rdb_cal_tilex(point_left_top, 14)
                                       as x_min,
                                       rdb_cal_tilex(point_right_bottom, 14)
                                       as x_max,
                                       rdb_cal_tiley(point_left_top, 14)
                                       as y_min,
                                       rdb_cal_tiley(point_right_bottom, 14)
                                       as y_max
                                from
                                (
                                    select  ST_PointN(polygon_geom, 2)
                                            as point_left_top,
                                            ST_PointN(polygon_geom, 4)
                                            as point_right_bottom
                                    from
                                    (
                                        select st_boundary(st_envelope(
                                               st_collect(the_geom)))
                                               as polygon_geom
                                        from park_polygon_tbl
                                    )as a
                                )as b
                            )as c
                        )as d
                    )as e;
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        #
        sqlcmd = '''
                insert into rdb_park_region(park_region_id,park_region_title,park_region_name,
                                park_type, park_toll, park_map_ID, the_geom)
                    select park_region_id, tile_id, park_name, park_type,
                           park_toll,park_map_ID,
                           (st_dump(st_intersection(tile_geom, park_geom))).geom as the_geom
                    from
                    (
                        select  b.park_region_id,a.tile_id,b.park_name,b.park_type,
                                b.park_toll,b.park_map_ID,
                                a.the_geom as tile_geom,
                                b.the_geom as park_geom
                        from temp_tile_level_14_park as a
                        inner join 
                        park_polygon_tbl as b
                        on st_intersects(a.the_geom, b.the_geom)
                    )as c
                    order by park_region_id, tile_id;
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log(self.ItemName,'end make rdb_park_region','info')
        return 0
    
    def __make_park_point(self):
        rdb_log.log(self.ItemName,'start make rdb_park_point','info')
        
        sqlcmd = '''
                insert into rdb_park_point(park_poi_id,park_region_id,park_name,the_geom)
                select a.park_node_id,a.park_region_id,c.node_name,a.the_geom
                from rdb_park_node as a
                inner join
                rdb_tile_node as b
                on a.park_node_id = b.tile_node_id
                inner join
                park_node_tbl as c
                on b.old_node_id = c.park_node_id
                where a.park_node_type in (1,3) and c.node_name is not null;
                '''
        rdb_log.log(self.ItemName,'end make rdb_park_point','info')