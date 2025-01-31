# -*- coding: cp936 -*-
'''
Created on 2012-11-1

@author: liuxinxing
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_admin(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self, 'Admin')
        
    def Do_CreateTable(self):
        self.CreateTable2('rdb_admin_zone')
        self.CreateTable2('rdb_tile_admin_zone')
        return 0

    def Do(self):
        
        if self.__judeadmin():
            
            self.__makeAdminZone()
    
            self.__makeTileLevel10()
            self.__intersectTileLevel10()
    
            self.__makeTileLevel12()
            self.__intersectTileLevel12()
    
            self.__makeTileLevel14()
    
            #self.__intersectTileLevel14()
            #self.__intersectTileLevel14Order8()
            self.__intersectTileLevel14Order8_new()
    
            self.__makeTempTileAdminZone()
            self.__makeHoleIndependent()
            self.__simplifyTileAdminZoneOrder8()
            self.__makeHoleColonial()
            self.__makeTileAdminZone()

        return 0
    
    def __judeadmin(self):
        rdb_log.log('Admin', 'jude admin start', 'info')

        sqlcmd = """
                    select count(*) from mid_admin_zone;
                 """
        self.pg.execute(sqlcmd)
        row = self.pg.fetchone2()
        
        if row[0] > 0 :
            return 1
        else:
            return 0
      
        rdb_log.log('Admin', 'jude admin end', 'info')
        
    def __makeAdminZone(self):
        rdb_log.log('Admin', 'converting rdb_admin_zone begin...', 'info')

        sqlcmd = """
                    insert into rdb_admin_zone(ad_code, ad_order, order0_id,
                                order1_id, order2_id, order8_id, ad_name)
                    select ad_code, ad_order, order0_id, order1_id,
                           order2_id, order8_id, ad_name
                    from mid_admin_zone
                    where not (ad_order = 8 and ad_code = order1_id)
                    -- delete admin zone without
                    -- unique ad_code in japen
                    order by gid
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('rdb_admin_zone_ad_code_idx')

        rdb_log.log('Admin', 'converting rdb_admin_zone end.', 'info')

    def __makeTileLevel10(self):
        rdb_log.log('Admin', 'converting temp_tile_level_10 begin...', 'info')

        self.CreateFunction2('rdb_cal_tilex')
        self.CreateFunction2('rdb_cal_tiley')
        self.CreateFunction2('rdb_cal_lon')
        self.CreateFunction2('rdb_cal_lat')
        self.CreateFunction2('rdb_get_tile_area_byxy')

        self.CreateTable2('temp_tile_level_10')

        sqlcmd = """
                    insert into temp_tile_level_10(tile_x, tile_y, the_geom)
                    select tile_x, tile_y,
                           rdb_get_tile_area_byxy(tile_x, tile_y, 10)
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
                                select  x_min, y_min, 
                                        (case when x_max >= 1024 then 1023 else x_max end) as x_max,
                                        (case when y_max >= 1024 then 1023 else y_max end) as y_max
                                from
                                (
                                    select rdb_cal_tilex(point_left_top, 10)
                                           as x_min,
                                           rdb_cal_tilex(point_right_bottom, 10)
                                           as x_max,
                                           rdb_cal_tiley(point_left_top, 10)
                                           as y_min,
                                           rdb_cal_tiley(point_right_bottom, 10)
                                           as y_max
                                    from
                                    (
                                        select  ST_PointN(admin_geom, 2)
                                                as point_left_top,
                                                ST_PointN(admin_geom, 4)
                                                as point_right_bottom
                                        from
                                        (
                                            select st_boundary(st_envelope(
                                                   st_collect(the_geom)))
                                                   as admin_geom
                                            from mid_admin_zone
                                            where ad_order = 1
                                        )as a
                                    )as b
                                )as b_new
                            )as c
                        )as d
                    )as e
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_tile_level_10_the_geom_idx')

        rdb_log.log('Admin', 'converting temp_tile_level_10 end.', 'info')

    def __intersectTileLevel10(self):
        rdb_log.log('Admin',
                    'converting temp_tile_level_10_intersect begin...',
                    'info')

        self.CreateTable2('temp_tile_level_10_admin_zone')

        sqlcmd = """
                    insert into temp_tile_level_10_admin_zone(tile_x,
                                tile_y, ad_code)
                    select a.tile_x, a.tile_y, b.ad_code
                    from temp_tile_level_10 as a
                    inner join (select * from mid_admin_zone
                                where ad_order = 1) as b
                    on st_intersects(a.the_geom, b.the_geom)
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_tile_level_10_admin_zone_ad_code_idx')

        rdb_log.log('Admin',
                    'converting temp_tile_level_10_intersect end.',
                    'info')

    def __makeTileLevel12(self):
        rdb_log.log('Admin', 'converting temp_tile_level_12 begin...', 'info')

        #
        self.CreateTable2('temp_tile_level_12')

        sqlcmd = """
                    insert into temp_tile_level_12(tile_x,
                                tile_y, ad_code, the_geom)
                    select tile_x, tile_y, ad_code,
                           rdb_get_tile_area_byxy(tile_x, tile_y, 12)
                           as the_geom
                    from
                    (
                        select  tile_x,
                                generate_series(tile_y << 2,
                                (tile_y << 2) + 3) as tile_y, ad_code
                        from
                        (
                            select generate_series(tile_x << 2,
                                   (tile_x << 2) + 3) as tile_x,
                                   tile_y, ad_code
                            from temp_tile_level_10_admin_zone
                        )as d
                    )as e
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_tile_level_12_ad_code_idx')
        self.CreateIndex2('temp_tile_level_12_the_geom_idx')

        rdb_log.log('Admin', 'converting temp_tile_level_12 end.', 'info')

    def __intersectTileLevel12(self):
        rdb_log.log('Admin',
                    'converting temp_tile_level_12_intersect begin...',
                    'info')

        self.CreateTable2('temp_tile_level_12_admin_zone')

        sqlcmd = """
                    insert into temp_tile_level_12_admin_zone(tile_x,
                                tile_y, ad_code)
                    select a.tile_x, a.tile_y, b.ad_code
                    from temp_tile_level_12 as a
                    inner join (select * from mid_admin_zone
                                where ad_order = 1) as b
                    on a.ad_code = b.ad_code and
                       st_intersects(a.the_geom, b.the_geom)
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_tile_level_12_admin_zone_ad_code_idx')

        rdb_log.log('Admin',
                    'converting temp_tile_level_12_intersect end.',
                    'info')

    def __makeTileLevel14(self):
        rdb_log.log('Admin', 'converting temp_tile_level_14 begin...', 'info')

        #
        self.CreateTable2('temp_tile_level_14')

        sqlcmd = """
                    insert into temp_tile_level_14(tile_id,
                                tile_x, tile_y, ad_code, the_geom)
                    select ((14 << 28) | (tile_x << 14) | tile_y) as tile_id,
                           tile_x, tile_y, ad_code,
                           rdb_get_tile_area_byxy(tile_x, tile_y, 14)
                           as the_geom
                    from
                    (
                        select  tile_x, generate_series(tile_y << 2,
                                (tile_y << 2) + 3) as tile_y, ad_code
                        from
                        (
                            select generate_series(tile_x << 2,
                                   (tile_x << 2) + 3) as tile_x,
                                   tile_y, ad_code
                            from temp_tile_level_12_admin_zone
                        )as d
                    )as e
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_tile_level_14_tile_id_idx')
        self.CreateIndex2('temp_tile_level_14_tile_xy_idx')
        self.CreateIndex2('temp_tile_level_14_ad_code_idx')
        self.CreateIndex2('temp_tile_level_14_the_geom_idx')

        rdb_log.log('Admin', 'converting temp_tile_level_14 end.', 'info')

    def __intersectTileLevel14(self):
        rdb_log.log('Admin',
                    'converting temp_tile_level_14_intersect begin...',
                    'info')

        self.CreateTable2('temp_tile_level_14_admin_zone')

        sqlcmd = """
                    insert into temp_tile_level_14_admin_zone(tile_id,
                           tile_x, tile_y, ad_code, the_geom)
                    select a.tile_id, a.tile_x, a.tile_y, b.ad_code, a.the_geom
                    from temp_tile_level_14 as a
                    inner join (select * from mid_admin_zone
                                where ad_order = 1) as b
                    on a.ad_code = b.ad_code and
                       st_intersects(a.the_geom, b.the_geom)
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_tile_level_14_admin_zone_tile_id_idx')
        self.CreateIndex2('temp_tile_level_14_admin_zone_ad_code_idx')
        self.CreateIndex2('temp_tile_level_14_admin_zone_the_geom_idx')

        rdb_log.log('Admin',
                    'converting temp_tile_level_14_intersect end.',
                    'info')

    def __intersectTileLevel14Order8(self):
        rdb_log.log('Admin',
                    'converting temp_tile_level_14_admin_zone_order8 begin...',
                    'info')
        #
        self.CreateTable2('temp_tile_level_14_admin_zone_order8')
        sqlcmd = """
                    insert into temp_tile_level_14_admin_zone_order8(tile_id,
                           tile_x, tile_y, ad_code, the_geom)
                    select tile_id, tile_x, tile_y, ad_code,
                           st_multi(st_intersection(tile_geom, admin_geom))
                           as the_geom
                    from
                    (
                        select  a.tile_id, a.tile_x, a.tile_y,
                                b.ad_code, a.the_geom as tile_geom, b.the_geom
                                as admin_geom
                        from temp_tile_level_14_admin_zone as a
                        inner join (select * from mid_admin_zone
                                    where ad_order = 8) as b
                        on a.ad_code = b.order1_id and
                           st_intersects(a.the_geom, b.the_geom)
                    )as a
                    order by tile_id, tile_x, tile_y, ad_code
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_tile_level_14_admin_zone_order8_tile_id_idx')
        self.CreateIndex2('temp_tile_level_14_admin_zone_order8_tile_xy_idx')
        self.CreateIndex2('temp_tile_level_14_admin_zone_order8_ad_code_idx')

        # insert abroad record for tile across the boundary of the country
        sqlcmd = """
                    insert into temp_tile_level_14_admin_zone_order8(tile_id,
                           tile_x, tile_y, ad_code, the_geom)
                    (
                        select tile_id, tile_x, tile_y, -1 as ad_code,
                               cast(null as geometry) as the_geom
                        from
                        (
                            select tile_id, tile_x, tile_y,
                                   st_union(admin_geom) as admin_geom,
                                   (array_agg(tile_geom))[1] as tile_geom
                            from
                            (
                                select  a.tile_id, a.tile_x, a.tile_y,
                                        a.the_geom as admin_geom,
                                        b.the_geom as tile_geom
                                from temp_tile_level_14_admin_zone_order8 as a
                                inner join temp_tile_level_14 as b
                                on a.tile_id = b.tile_id
                            )as a
                            group by tile_id, tile_x, tile_y
                        )as b
                        where not st_contains(admin_geom, tile_geom)
                    );
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log('Admin',
                    'converting temp_tile_level_14_admin_zone_order8 end.',
                    'info')

    def __intersectTileLevel14Order8_new(self):
        rdb_log.log('Admin',
                    'converting temp_tile_level_14_admin_zone_order8 begin...',
                    'info')
        #
        self.CreateTable2('temp_tile_level_14_admin_zone_order8')
#        sqlcmd = """
#                    insert into temp_tile_level_14_admin_zone_order8(tile_id,
#                                tile_x, tile_y, ad_code, the_geom)
#                    select tile_id, tile_x, tile_y, ad_code,
#                           st_multi(st_intersection(tile_geom, admin_geom))
#                           as the_geom
#                    from
#                    (
#                        select  a.tile_id, a.tile_x, a.tile_y,
#                                b.ad_code, a.the_geom as tile_geom,
#                                b.the_geom as admin_geom
#                        from temp_tile_level_14 as a
#                        inner join (select * from mid_admin_zone
#                                    where ad_order = 8) as b
#                        on a.ad_code = b.order1_id and
#                           st_intersects(a.the_geom, b.the_geom)
#                    )as a
#                    order by tile_id, tile_x, tile_y, ad_code
#                 """
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()
        sqlcmd = """
                    insert into temp_tile_level_14_admin_zone_order8(tile_id, tile_x, tile_y, ad_code, the_geom)
                    select a.tile_id, a.tile_x, a.tile_y, b.ad_code,
                           st_multi(st_intersection(a.the_geom, b.the_geom)) as the_geom
                    from temp_tile_level_14 as a
                    inner join (select * from mid_admin_zone where gid >= %s and gid <= %s) as b
                    on a.ad_code = b.order1_id and ad_order = 8 and st_intersects(a.the_geom, b.the_geom)
                    ;
                 """
        (min_gid, max_gid) = self.pg.getMinMaxValue('mid_admin_zone', 'gid')
        self.pg.multi_execute(sqlcmd, min_gid, max_gid, 4, 10, self.log)

        #remove point and linestring in temp_tile_level_14_admin_zone_order8
        #add by xuwenbo
        sqlcmd = '''
                 INSERT INTO temp_tile_level_14_admin_zone_order8
                 SELECT a.tile_id as tile_id, a.tile_x as tile_x,
                        a.tile_y as tile_y, a.ad_code as ad_code,
                        st_collectionextract (a.the_geom, 3) as the_geom
                 FROM temp_tile_level_14_admin_zone_order8 as a
                 WHERE st_geometrytype(a.the_geom) = 'ST_GeometryCollection'
                       and not st_isempty(st_collectionextract(a.the_geom, 3));

                 DELETE FROM temp_tile_level_14_admin_zone_order8
                 WHERE st_geometrytype(the_geom) in ('ST_MultiPoint',
                      'ST_MultiLinestring', 'ST_GeometryCollection')
                 '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('temp_tile_level_14_admin_zone_order8_tile_id_idx')
        self.CreateIndex2('temp_tile_level_14_admin_zone_order8_tile_xy_idx')
        self.CreateIndex2('temp_tile_level_14_admin_zone_order8_ad_code_idx')

        # insert abroad record for tile across the boundary of the country
        sqlcmd = """
                    insert into temp_tile_level_14_admin_zone_order8(tile_id,
                                tile_x, tile_y, ad_code, the_geom)
                    (
                        select  tile_id, tile_x, tile_y, -1 as ad_code,
                                --cast(null as geometry) as the_geom,
                                st_multi(st_difference(tile_geom, admin_geom))
                                as the_geom
                        from
                        (
                            select tile_id, tile_x, tile_y,
                                   st_union(admin_geom) as admin_geom,
                                   (array_agg(tile_geom))[1] as tile_geom
                            from
                            (
                                select  a.tile_id, a.tile_x, a.tile_y,
                                        a.the_geom as admin_geom,
                                        b.the_geom as tile_geom
                                from temp_tile_level_14_admin_zone_order8 as a
                                inner join temp_tile_level_14 as b
                                on a.tile_id = b.tile_id
                            )as a
                            group by tile_id, tile_x, tile_y
                        )as b
                        where not st_contains(admin_geom, tile_geom)
                    );
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        #remove point and linestring in
        #temp_tile_level_14_admin_zone_order8 again
        #add by xuwenbo
        sqlcmd = '''
                 INSERT INTO temp_tile_level_14_admin_zone_order8
                 SELECT a.tile_id as tile_id, a.tile_x as tile_x,
                        a.tile_y as tile_y, a.ad_code as ad_code,
                        st_collectionextract (a.the_geom, 3) as the_geom
                 FROM temp_tile_level_14_admin_zone_order8 as a
                 WHERE st_geometrytype(a.the_geom) = 'ST_GeometryCollection'
                       and not st_isempty(st_collectionextract(a.the_geom, 3));

                 DELETE FROM temp_tile_level_14_admin_zone_order8
                 WHERE st_geometrytype (the_geom) in ('ST_MultiPoint',
                       'ST_MultiLinestring', 'ST_GeometryCollection')
                 '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log('Admin',
                    'converting temp_tile_level_14_admin_zone_order8 end.',
                    'info')

    def __makeTempTileAdminZone(self):
        rdb_log.log('Admin', 'converting temp_tile_admin_zone begin...',
                    'info')

        #
        self.CreateTable2('temp_tile_admin_zone')
        sqlcmd = """
                    insert into temp_tile_admin_zone(tile_id, ad_code,
                                                     the_geom)
                    (
                        select  tile_id, ad_code,
                                --(
                                --case when ad_code = -1 then the_geom
                                --     else st_geometryn(the_geom,
                                --          polygon_index)
                                --end
                                --)as the_geom
                                st_geometryn(the_geom, polygon_index)
                                as the_geom
                        from
                        (
                            select  tile_id, ad_code, the_geom,
                                    generate_series(1,polygon_num)
                                    as polygon_index
                            from
                            (
                                select  tile_id, ad_code, the_geom,
                                        --(case when ad_code=-1 then 1
                                        --else st_numgeometries(the_geom) end)
                                        --as polygon_num
                                        st_numgeometries(the_geom)
                                        as polygon_num
                                from temp_tile_level_14_admin_zone_order8
                            )as a
                        )as b
                        order by tile_id, ad_code
                    )
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_tile_admin_zone_tile_id_idx')
        self.CreateIndex2('temp_tile_admin_zone_outer_polygon_gid_idx')

        rdb_log.log('Admin', 'converting temp_tile_admin_zone end.', 'info')

    def __makeHoleIndependent(self):
        rdb_log.log('Admin', 'making hole independent begin...', 'info')

        # make rings of polygon independent
        self.CreateTable2('temp_tile_admin_zone_hole')
        sqlcmd = """
                    insert into temp_tile_admin_zone_hole(outer_polygon_gid,
                                tile_id, ad_code, prior, the_geom)
                    select  outer_polygon_gid, tile_id, ad_code,
                            (case when polygon_index = 1 then 0
                                  else 1 end) as prior,
                            st_polygon(st_geometryn(the_geom,polygon_index),
                                       4326) as the_geom
                    from
                    (
                        select  outer_polygon_gid, tile_id, ad_code, the_geom,
                                generate_series(1,st_numgeometries(the_geom))
                                as polygon_index
                        from
                        (
                            select gid as outer_polygon_gid, tile_id, ad_code,
                                  st_boundary(the_geom) as the_geom
                            from temp_tile_admin_zone
                            where st_nrings(the_geom) >= 2
                        )as a
                    )as b
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_tile_admin_zone_hole_tile_id_idx')
        self.CreateIndex2('temp_tile_admin_zone_hole_outer_polygon_gid_idx')

        # update outer ring's geometry
        sqlcmd = """
                    update temp_tile_admin_zone as a set the_geom = b.the_geom
                    from
                    (
                        select outer_polygon_gid, the_geom
                        from temp_tile_admin_zone_hole
                        where prior = 0
                    )as b
                    where a.gid = b.outer_polygon_gid
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # get inner ring's related polygon
        self.CreateTable2('temp_tile_admin_zone_hole_tmp2old')
        sqlcmd = """
                    insert into temp_tile_admin_zone_hole_tmp2old(
                                tmp_polygon_gid, old_polygon_gid)
                    select  a.tmp_polygon_gid, b.gid as old_polygon_gid
                    from temp_tile_admin_zone_hole as a
                    inner join temp_tile_admin_zone as b
                    on a.prior > 0 and a.tile_id = b.tile_id
                       and st_contains(a.the_geom,b.the_geom)
                       and st_contains(b.the_geom,a.the_geom)
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2(
             'temp_tile_admin_zone_hole_tmp2old_tmp_polygon_gid_idx')

#        # insert inner ring out of admin area
#        sqlcmd = """
#                    insert into temp_tile_admin_zone(tile_id, ad_code,
#                                outer_polygon_gid, prior, the_geom)
#                    select a.tile_id, -1 as ad_code,
#                           a.outer_polygon_gid, 1, a.the_geom
#                    from temp_tile_admin_zone_hole as a
#                    left join temp_tile_admin_zone_hole_tmp2old as b
#                    on a.tmp_polygon_gid = b.tmp_polygon_gid
#                    where a.prior > 0 and b.tmp_polygon_gid is null
#                 """
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()

        # update inner ring
        sqlcmd = """
                    update temp_tile_admin_zone as a
                    set prior = b.prior,
                        outer_polygon_gid = b.outer_polygon_gid
                    from
                    (
                        select b.old_polygon_gid, a.prior,
                               a.outer_polygon_gid
                        from temp_tile_admin_zone_hole as a
                        inner join temp_tile_admin_zone_hole_tmp2old as b
                        on a.prior > 0 and
                           a.tmp_polygon_gid = b.tmp_polygon_gid
                    )as b
                    where a.gid = b.old_polygon_gid
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log('Admin', 'making hole independent end.', 'info')

    def __simplifyTileAdminZoneOrder8(self):
        rdb_log.log('Admin', 'simplifying tile_admin begin...', 'info')

        # get and simplify the boundary of admin area in tile
        self.CreateTable2('temp_tile_admin_zone_simple_edge')
        self.CreateFunction2('rdb_remove_polygon_from_collection')
        sqlcmd = """
                    insert into temp_tile_admin_zone_simple_edge
                                (tile_id, a_polygon_gid, b_polygon_gid, i_geom)
                    (
                        select  tile_id, a_polygon_gid, b_polygon_gid, i_geom
                        from
                        (
                            select  a.tile_id as tile_id,
                                    a.gid as a_polygon_gid,
                                    b.gid as b_polygon_gid,
                                    rdb_remove_polygon_from_collection(
                                    st_intersection(a.the_geom, b.the_geom))
                                    as i_geom
                            from temp_tile_admin_zone as a
                            inner join temp_tile_admin_zone as b
                            on a.prior = 0 and b.prior = 0 and
                               a.tile_id = b.tile_id and a.ad_code > b.ad_code
                        )as c
                        where not st_isempty(i_geom)

                        union

                        select  tile_id, gid as a_polygon_gid,
                                null as b_polygon_gid, i_geom
                        from
                        (
                            select gid, tile_id,
                                   st_boundary(the_geom) as i_geom
                            from temp_tile_admin_zone
                            where prior > 0
                        )as a
                    );
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_tile_admin_zone_simple_edge_a_polygon_gid_idx')
        self.CreateIndex2('temp_tile_admin_zone_simple_edge_b_polygon_gid_idx')

        # get every admin area and its simplified boundary
        self.CreateTable2('temp_tile_admin_zone_simple')
        sqlcmd = """
                    insert into temp_tile_admin_zone_simple
                                (tile_id, ad_code, polygon_gid, the_geom,
                                i_geom, i_simple_geom, the_simple_geom)
                    (
                        select tile_id, ad_code, polygon_gid,
                               the_geom, i_geom, i_simple_geom,
                               st_linemerge(st_union(st_difference(
                               st_boundary(the_geom),i_geom), i_simple_geom))
                               as the_simple_geom
                        from
                        (
                            select  tile_id, ad_code, polygon_gid,
                                    (array_agg(the_geom))[1] as the_geom,
                                    st_collect(i_geom) as i_geom,
                                    st_collect(i_simple_geom) as i_simple_geom
                            from
                            (
                                select  a.tile_id, a.ad_code,
                                        a.gid as polygon_gid,
                                        a.the_geom, b.i_geom,
                                        ST_SimplifyPreserveTopology(
                                        st_linemerge(b.i_geom), 0.00009)
                                        as i_simple_geom
                                from temp_tile_admin_zone as a
                                left join temp_tile_admin_zone_simple_edge as b
                                on a.gid in (b.a_polygon_gid, b.b_polygon_gid)
                            )as t
                            group by tile_id, ad_code, polygon_gid
                        )as t2
                    );
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2(
             'temp_tile_admin_zone_simple_tile_id_ad_code_polygon_gid_idx')

        # tackle special polygon which is not closed after being simplified
        sqlcmd = """
                    CREATE OR REPLACE FUNCTION rdb_reSimplifyPolygon()
                        RETURNS integer
                        LANGUAGE plpgsql
                    AS $$
                    DECLARE
                        special_polygon_gid     integer;
                        iSimplifyPara           float;
                    BEGIN
                        iSimplifyPara := 0.00009;
                        while true loop
                            iSimplifyPara := iSimplifyPara - 0.000017;
                            if iSimplifyPara < 0 then
                                exit;
                            end if;

                            drop table if exists
                                 temp_tile_admin_zone_simple_bug;
                            create temp table temp_tile_admin_zone_simple_bug
                            as
                            (
                                select  polygon_gid, tile_id
                                from temp_tile_admin_zone_simple
                                where (st_geometrytype(the_simple_geom)
                                      != 'ST_LineString')
                                      or (st_npoints(the_simple_geom) >= 4
                                      and not st_isclosed(the_simple_geom))

                                union

                                select a.polygon_gid, a.tile_id
                                from temp_tile_admin_zone_simple as a
                                inner join temp_tile_admin_zone as b
                                on  (a.polygon_gid = b.outer_polygon_gid)
                                    and (b.gid != b.outer_polygon_gid)
                                    and (st_geometrytype(a.the_simple_geom)
                                         = 'ST_LineString')
                                    and (st_npoints(a.the_simple_geom) >= 4)
                                    and st_isclosed(a.the_simple_geom)
                                where not st_contains(
                                      st_makepolygon(a.the_simple_geom),
                                      b.the_geom)
                            );

                            select  polygon_gid
                            from temp_tile_admin_zone_simple_bug
                            into special_polygon_gid;
                            if not FOUND then
                                exit;
                            end if;

                            update temp_tile_admin_zone_simple as a
                            set the_simple_geom = b.the_simple_geom
                            from
                            (
                                select  polygon_gid,
                                        st_linemerge(st_union(st_difference(
                                        st_boundary(the_geom),i_geom),
                                        i_simple_geom)) as the_simple_geom
                                from
                                (
                                    select  tile_id, ad_code, polygon_gid,
                                            (array_agg(the_geom))[1]
                                            as the_geom,
                                            st_collect(i_geom) as i_geom,
                                            st_collect(i_simple_geom)
                                            as i_simple_geom
                                    from
                                    (
                                        select  a.tile_id, a.ad_code,
                                                a.gid as polygon_gid,
                                                a.the_geom, b.i_geom,
                                                ST_SimplifyPreserveTopology(
                                                st_linemerge(b.i_geom),
                                                iSimplifyPara)
                                                as i_simple_geom
                                        from temp_tile_admin_zone as a
                                        inner join
                                        (
                                            select b.a_polygon_gid,
                                                   b.b_polygon_gid, b.i_geom
                                            from
                                            (
                                                select distinct tile_id from
                                                temp_tile_admin_zone_simple_bug
                                            )as a
                                            left join
                                            temp_tile_admin_zone_simple_edge
                                            as b
                                            on a.tile_id = b.tile_id
                                        ) as b
                                        on a.gid in (b.a_polygon_gid,
                                           b.b_polygon_gid)
                                    )as t
                                    group by tile_id, ad_code, polygon_gid
                                )as t2
                            )as b
                            where a.polygon_gid = b.polygon_gid;
                        end loop;
                        return 0;
                    END;
                    $$;
                    select rdb_reSimplifyPolygon();
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        # get new geometry for every admin area
        sqlcmd = """
                    delete from temp_tile_admin_zone as a
                    using
                    (
                        select  polygon_gid
                        from temp_tile_admin_zone_simple
                        where st_npoints(the_simple_geom) < 4
                    )as b
                    where a.gid = b.polygon_gid;

                    update temp_tile_admin_zone as a set the_geom = b.the_geom
                    from
                    (
                        select polygon_gid, st_polygon(the_simple_geom, 4326)
                               as the_geom
                        from temp_tile_admin_zone_simple
                        where (st_geometrytype(the_simple_geom) =
                              'ST_LineString')
                              and (st_npoints(the_simple_geom) >= 4
                              and st_isclosed(the_simple_geom))
                    )as b
                    where a.gid = b.polygon_gid;
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log('Admin', 'simplifying tile_admin end.', 'info')

    def __makeHoleColonial(self):
        rdb_log.log('Admin', 'making hole colonial begin...', 'info')

        # insert inner rings into outer ring
        sqlcmd = """
                    update temp_tile_admin_zone as a set the_geom = b.the_geom
                    from
                    (
                        select a.gid, st_makepolygon(st_boundary(a.the_geom),
                                inner_geom_array) as the_geom
                        from temp_tile_admin_zone as a
                        inner join
                        (
                            select  outer_polygon_gid,
                                    array_agg(st_boundary(the_geom))
                                    as inner_geom_array
                            from temp_tile_admin_zone
                            where (outer_polygon_gid > 0) and
                                  (gid != outer_polygon_gid)
                            group by outer_polygon_gid
                        )as b
                        on a.gid = b.outer_polygon_gid
                    )as b
                    where a.gid = b.gid;
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

#        # delete independent ring outside admin area
#        sqlcmd = """
#                    delete from temp_tile_admin_zone
#                    where prior > 0 and ad_code = -1;
#                 """
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()

        # recover prior of inner ring to 0
        sqlcmd = """
                    update temp_tile_admin_zone set prior = 0
                    where prior > 0;
                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        rdb_log.log('Admin', 'making hole colonial end.', 'info')

    def __makeTileAdminZone(self):
        rdb_log.log('Admin', 'converting rdb_tile_admin_zone begin...', 'info')

        #
        sqlcmd = """
                    insert into rdb_tile_admin_zone(tile_id,
                                ad_code, the_geom, the_geom_900913)
                    (
                        select  tile_id, ad_code, the_geom,
                                st_transform(the_geom,3857)
                        from
                        (
                            select tile_id, ad_code, prior,
                                   st_npoints(the_geom) as points_num, the_geom
                            from temp_tile_admin_zone
                        )as b
                        order by tile_id asc, prior desc, points_num asc,
                                 ad_code asc
                    );
                 """
#        # store N-1 ad_code's geom
#        sqlcmd = """
#                    insert into rdb_tile_admin_zone(tile_id,
#                                                    ad_code, the_geom)
#                    (
#                        select  tile_id, ad_code, the_geom
#                        from
#                        (
#                            select tile_id, ad_code, prior,
#                                   st_npoints(the_geom) as points_num,
#                                   the_geom
#                            from
#                            (
#                                select  a.tile_id, a.ad_code, a.prior,
#                                        (case when b.tile_id is null
#                                              then a.the_geom
#                                              else cast(null as geometry) end)
#                                         as the_geom
#                                from temp_tile_admin_zone as a
#                                left join
#                                (
#                                    select  tile_id,
#                                            (array_agg(ad_code))[1] as ad_code
#                                    from
#                                    (
#                                        select tile_id, ad_code,
#                                               sum(st_npoints(the_geom))
#                                               as points_num
#                                        from temp_tile_admin_zone
#                                        where prior = 0
#                                        group by tile_id, ad_code
#                                        order by tile_id asc, points_num desc
#                                    )as a
#                                    group by tile_id
#                                )as b
#                                on a.tile_id = b.tile_id and
#                                   a.ad_code = b.ad_code and a.prior = 0
#                            )as a
#                        )as b
#                        order by tile_id asc, prior desc, points_num asc,
#                              ad_code asc
#                    );
#                 """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('rdb_tile_admin_zone_tile_id_idx')
        self.CreateIndex2('rdb_tile_admin_zone_the_geom_idx')

        #
        self.CreateTable2('rdb_tile_out_of_admin_zone')
        self.CreateIndex2('rdb_tile_out_of_admin_zone_tile_id_idx')
        self.CreateIndex2('rdb_tile_out_of_admin_zone_tile_x_tile_y_idx')

        rdb_log.log('Admin', 'converting rdb_tile_admin_zone end.', 'info')

#    def __makeTileAdminZone(self):
#        rdb_log.log('Admin', 'converting tile_admin begin...', 'info')
#
#        self.CreateTable2('rdb_tile_admin_zone')
#        sqlcmd = """
#                    insert into rdb_tile_admin_zone(tile_id, ad_code)
#                    select ((a.tile_x << 14) | a.tile_y) as tile_id, b.ad_code
#                    from temp_tile_level_14_admin_zone as a
#                    left join (select * from mid_admin_zone
#                               where ad_order = 8) as b
#                    on a.ad_code = b.order1_id and
#                       st_intersects(a.the_geom, b.the_geom)
#                    where b.ad_code is not null
#                    order by a.tile_x, a.tile_y, b.ad_code
#                 """
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()
#
#        self.CreateIndex2('rdb_tile_admin_zone_tile_id_idx')
#
#        rdb_log.log('Admin', 'converting tile_admin end.', 'info')

#    def Do(self):
#
#        self.__MakeLinkAdmin()
#        self.__MakeTileAdmin()
#
#        return 0
#
#    def __MakeLinkAdmin(self):
#        rdb_log.log('Admin', 'converting link_admin begin...', 'info')
#
#        self.CreateTable2('rdb_link_admin_zone')
##        sqlcmd = """
##                    insert into rdb_link_admin_zone(link_id, ad_code)
##                    (
##                        select a.link_id, b.ad_code
##                        from
##                        (
##                            select a.link_id, b.ad_code as ad_order1_code,
##                                   a.the_geom
##                            from rdb_link as a
##                            left join (select * from mid_admin_zone
##                                       where ad_order = 1) as b
##                            on st_intersects(a.the_geom, b.the_geom)
##                        )as a
##                        left join (select * from mid_admin_zone
#                                    where ad_order = 8) as b
##                        on a.ad_order = b.order1_id and
##                           st_intersects(a.the_geom, b.the_geom)
##                        where b.ad_code is not null
##                        order by a.link_id, b.ad_code
##                    )
##                 """
#        sqlcmd = """
#                    insert into rdb_link_admin_zone(link_id, ad_code)
#                    (
#                        select a.link_id, b.ad_code
#                        from
#                        (
#                            select a.link_id, b.ad_code, a.the_geom
#                            from (select * from rdb_link
#                                  where link_id_t != end_node_id_t) as a
#                            left join (select * from mid_admin_zone
#                                       where ad_order = 1) as b
#                            on st_intersects(a.the_geom, b.the_geom)
#                            where b.ad_code is not null
#                        )as a
#                        left join (select * from mid_admin_zone where
#                                   ad_order = 8) as b
#                        on a.ad_code = b.order1_id and
#                           st_intersects(a.the_geom, b.the_geom)
#                        where b.ad_code is not null
#                        order by a.link_id, b.ad_code
#                    )
#                 """
#
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()
#
#        self.CreateIndex2('rdb_link_admin_zone_link_id_idx')
#
#        rdb_log.log('Admin', 'converting link_admin end.', 'info')
#
#    def __MakeTileAdmin(self):
#        rdb_log.log('Admin', 'converting tile_admin begin...', 'info')
#
#        self.CreateTable2('rdb_tile_admin_zone')
##        sqlcmd = """
##                    insert into rdb_tile_admin_zone(tile_id, ad_code)
##                    select distinct b.link_id_t as tile_id, a.ad_code
##                    from rdb_link_admin_zone as a
##                    left join rdb_link as b
##                    on a.link_id = b.link_id
##                    where b.ad_code is not null
##                    order by b.link_id_t, a.ad_code
##                 """
#
#        self.CreateFunction2('rdb_cal_lon')
#        self.CreateFunction2('rdb_cal_lat')
#        self.CreateFunction2('rdb_get_tile_area_byid')
#        sqlcmd = """
#                    insert into rdb_tile_admin_zone(tile_id, ad_code)
#                    (
#                        select distinct tile_id, ad_code
#                        from
#                        (
#                            select a.tile_id, b.ad_code
#                            from
#                            (
#                                select a.tile_id, b.ad_code, a.the_geom
#                                from
#                                (
#                                    select tile_id,
#                                           rdb_get_tile_area_byid(tile_id, 14)
#                                           as the_geom
#                                    from
#                                    (
#                                        select distinct link_id_t as tile_id
#                                        from rdb_link
#                                        where link_id_t = end_node_id_t
#                                    )as t
#                                )as a
#                                left join (select * from mid_admin_zone
#                                           where ad_order = 1) as b
#                                on st_intersects(a.the_geom, b.the_geom)
#                                where b.ad_code is not null
#                            )as a
#                            left join (select * from mid_admin_zone
#                                       where ad_order = 8) as b
#                            on a.ad_code = b.order1_id and
#                               st_intersects(a.the_geom, b.the_geom)
#                            where b.ad_code is not null
#
#                            union
#
#                            select distinct b.link_id_t as tile_id, a.ad_code
#                            from rdb_link_admin_zone as a
#                            left join rdb_link as b
#                            on a.link_id = b.link_id
#                        )as ta
#                        order by tile_id, ad_code
#                    )
#                 """
#
#        self.pg.execute2(sqlcmd)
#        self.pg.commit2()
#
#        self.CreateIndex2('rdb_tile_admin_zone_tile_id_idx')
#
#        rdb_log.log('Admin', 'converting tile_admin end.', 'info')

    def _DoCheckLogic(self):
        rdb_log.log('Admin', 'Check admin...', 'info')

        self.CreateFunction2('rdb_check_admin_zone')
        self.pg.callproc('rdb_check_admin_zone')

        rdb_log.log('Admin', 'Check admin end.', 'info')
        return 0
