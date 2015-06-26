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
            self.__make_rdb_update_linkAndnode()
            self.__make_rdb_park_region()
            self.__make_park_point()
            self.__delete_from_rdb_linkandnode()
            pass
        
        rdb_log.log(self.ItemName,'end make rdb park..','info')
        return 0
    
    def __make_rdb_park_link(self):
        rdb_log.log(self.ItemName,'start make rdb_park_link','info')
        
        self.CreateFunction2('rdb_makeshape2pixelbytea')
        self.CreateFunction2('rdb_smallint_2_octal_bytea')
        sqlcmd='''
                insert into rdb_park_link(park_link_id,park_link_id_t,park_link_oneway,park_s_node_id,
                                        park_e_node_id,park_link_length,park_link_connect_type,
                                        park_floor,park_link_lean,park_link_toll,park_link_add1,
                                        park_link_add2,park_region_id,geom_blob,the_geom,the_geom_4096)
                select a.link_id as park_link_id,
                    a.link_id_t as park_link_id_t,
                    a.one_way as park_link_oneway,
                    a.start_node_id as park_s_node_id,
                    a.end_node_id as park_e_node_id,
                    c.park_link_length as park_link_length,
                    c.park_link_connect_type as park_link_connect_type,
                    c.park_floor as park_floor,
                    a.tilt_flag as park_link_lean,
                    c.park_link_toll as park_link_toll,
                    c.park_link_add1 as park_link_add1,
                    c.park_link_add2 as park_link_add2,
                    c.park_region_id as park_region_id,
                    rdb_makeshape2pixelbytea(14::smallint, ((a.link_id_t >> 14) & 16383), (a.link_id_t & 16383), a.the_geom_4096 ),
                    a.the_geom as the_geom,
                    a.the_geom_4096 as the_geom_4096
                from
                (
                    select link_id,link_id_t,start_node_id,end_node_id,one_way,tilt_flag,the_geom,the_geom_4096
                    from rdb_link
                    where display_class in (20,21)
                    order by link_id
                )as a
                left join
                rdb_tile_link as b
                on a.link_id = b.tile_link_id
                left join
                park_link_tbl as c
                on b.old_link_id = c.park_link_id
                where c.park_link_id is not null
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log(self.ItemName,'end make rdb_park_link','info')
        return 0
        
    def __make_rdb_park_node(self):
        rdb_log.log(self.ItemName,'start make rdb_park_node','info')
        
        self.CreateFunction2('rdb_makenode2pixelbytea')
        self.CreateFunction2('rdb_get_outlink_park')
        self.CreateFunction2('rdb_delete_base_link')
        sqlcmd='''
                insert into rdb_park_node(park_node_id,park_node_id_t,park_base_id,
                                        park_node_type,park_connect_link_num,
                                        park_connect_link_array,park_link_num,park_branches,
                                        park_region_id,geom_blob,the_geom,the_geom_4096)
                select node_id as park_node_id,
                    node_id_t as park_node_id_t,
                    (case when park_node_type > 0 then node_id else null end) as park_base_id,
                    park_node_type as park_node_type,
                    array_upper(park_out_lib,1) as park_connect_link_num,
                    park_out_lib as park_connect_link_array,
                    array_upper(link_lib_park,1) as park_link_num,
                    link_lib_park as park_branches,
                    park_region_id,
                    rdb_makenode2pixelbytea(14::smallint, ((node_id_t >> 14) & 16383), (node_id_t & 16383), the_geom_4096 ),
                    the_geom,
                    the_geom_4096
                from
                (
                    select node_id,node_id_t,link_num,link_lib,d.park_node_type,
                        rdb_get_outlink_park(node_id,d.park_node_type,link_lib,
                            s_link_lib,e_link_lib,one_way_lib) as park_out_lib,rdb_delete_base_link(link_lib) as link_lib_park,
                            d.park_region_id,temp_c.the_geom,temp_c.the_geom_4096
                    from
                    (
                        select node_id,node_id_t,count(node_id) as link_num,array_agg(link_id) as link_lib,
                            array_agg(start_node_id) as s_link_lib,
                            array_agg(end_node_id) as e_link_lib,
                            array_agg(one_way) as one_way_lib,the_geom,the_geom_4096
                        from
                        (
                            select node_id,node_id_t,link_id,start_node_id,end_node_id,one_way,angle,the_geom,the_geom_4096
                            from
                            (
                                select node_id,node_id_t,link_id,start_node_id,end_node_id,one_way,rdb_node.the_geom,rdb_node.the_geom_4096,
                                    (case when rdb_link.start_node_id = rdb_node.node_id then fazm else tazm end) as angle
                                from rdb_link
                                join
                                rdb_node 
                                on rdb_link.start_node_id = rdb_node.node_id or rdb_link.end_node_id = rdb_node.node_id
                                where (rdb_node.extend_flag & 2) = 2
                            )as temp_a
                            order by node_id,angle
                        )as temp_b
                        group by node_id,node_id_t,the_geom,the_geom_4096
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
    
    def __make_rdb_update_linkAndnode(self):
        
        rdb_log.log(self.ItemName,'update park_link and node start','info')
        
        self.CreateTable2('temp_park_node_id')
        self.CreateFunction2('rdb_get_park_nodeid')
        self.pg.callproc('rdb_get_park_nodeid')
        self.pg.commit2()
        
        sqlcmd = '''
            update rdb_park_node
                set park_node_id = b.new_park_node_id
                from temp_park_node_id as b
                where park_base_id = b.base_node_id;
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            update rdb_park_link
                set park_s_node_id = b.new_park_node_id
                from temp_park_node_id as b
                where park_s_node_id = b.base_node_id;
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
            update rdb_park_link
                set park_e_node_id = b.new_park_node_id
                from temp_park_node_id as b
                where park_e_node_id = b.base_node_id;
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log(self.ItemName,'update park_link and node end','info')
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
                insert into rdb_park_region(park_region_id,park_region_tile,park_region_name,
                                park_type, park_toll, park_map_ID, geom_blob, the_geom,the_geom_4096)
                    select park_region_id, tile_id, park_name, park_type,
                           park_toll,park_map_ID,
                           rdb_makeshape2pixelbytea(14::smallint, ((tile_id >> 14) & 16383), (tile_id & 16383), rdb_transform_4326_to_4096(the_geom)),
                           the_geom,rdb_transform_4326_to_4096(the_geom) as the_geom_4096
                    from
                    (
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
                    ) as d
                    where ST_GeometryType(the_geom) = 'ST_Polygon'
                    order by park_region_id, tile_id;
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log(self.ItemName,'end make rdb_park_region','info')
        return 0
    
    def __delete_from_rdb_linkandnode(self):
        rdb_log.log(self.ItemName,'start delete from rad_link and rdb_node','info')
    
        #rdb_link_add_info
        self.CreateTable2('temp_rdb_link_add_info_contain_park')
        sqlcmd = '''
                delete from rdb_link_add_info
                where link_id in(
                    select park_link_id from rdb_park_link
                );
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        #rdb_linklane_info
        self.CreateTable2('temp_rdb_linklane_info_contain_park')
        sqlcmd = '''
                delete from rdb_linklane_info
                where lane_id not in(
                    select distinct lane_id
                    from rdb_link
                    left join
                    rdb_park_link
                    on rdb_link.link_id = rdb_park_link.park_link_id
                    where rdb_park_link.park_link_id is null
                );
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        #rdb_link
        self.CreateTable2('temp_rdb_link_contain_park')
        sqlcmd = '''
                delete from rdb_link
                where link_id in(
                    select park_link_id from rdb_park_link
                );
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        #rdb_node
        self.CreateTable2('temp_rdb_node_contain_park')
        sqlcmd = '''
            delete from rdb_node
            where node_id in(
                select park_node_id from rdb_park_node where park_node_type = 0
            );
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        rdb_log.log(self.ItemName,'end delete from rad_link and rdb_node','info')
    
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
                where a.park_node_type in (1,2,3) and c.node_name is not null;
                '''
        rdb_log.log(self.ItemName,'end make rdb_park_point','info')
        return 0