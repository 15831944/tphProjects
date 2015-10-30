# -*- coding: UTF8 -*-
'''
Created on 2012-02-23
@author: sunyifeng
'''

import component.default.admin

class comp_admin_rdf(component.default.admin.comp_admin):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.default.admin.comp_admin.__init__(self)

    def _DoCreateTable(self):

        return 0

    def _Do(self):
        self.__makeAdminZone()
        self.__do_admin_time()
        self.do_admin_time_order()
        self.__alter_admin_for_specially()
        self.make_geom_for_order()
        self.__do_admin_order0_iso_country_mapping()
        self.CreateOrder1()
        return 0
    
    def __makeAdminZone(self):       
        self.log.info('Now create mid_admin_zone...')
        self.CreateIndex2('rdf_country_country_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_admin_place_id_idx')
        self.CreateIndex2('rdf_carto_named_place_id_idx')
        self.CreateIndex2('rdf_carto_face_carto_id_idx')
        self.CreateIndex2('temp_wkt_face_face_id_idx')
        self.CreateIndex2('mid_temp_feature_name_feature_id_idx')
        
        self.CreateTable2('mid_admin_zone')
        sqlcmd = """
                    INSERT INTO mid_admin_zone (ad_code, ad_order, order0_id,
                                                order1_id, order2_id, order8_id,
                                                ad_name, the_geom)
                    SELECT a.admin_place_id AS ad_code, a.admin_order AS ad_order,
                           a.country_id AS order0_id, a.order1_id AS order1_id,
                           a.order2_id AS order2_id, a.order8_id AS order8_id,
                           b.feature_name AS ad_name,
                           st_multi (st_union (e.the_geom)) as the_geom
                    FROM rdf_admin_hierarchy as a    
                    LEFT OUTER JOIN mid_temp_feature_name AS b
                    ON a.admin_place_id = b.feature_id
    
                    LEFT JOIN rdf_carto AS c
                    ON a.admin_place_id = c.named_place_id
                    LEFT JOIN rdf_carto_face AS d
                    ON c.carto_id = d.carto_id
                    LEFT JOIN temp_wkt_face AS e
                    ON d.face_id = e.face_id
                    inner join rdf_country as h
                    on a.country_id = h.country_id
    
                    WHERE a.admin_order in (0, 1, 2, 8)
                    GROUP BY ad_code, ad_order, order0_id, order1_id, order2_id,
                             order8_id, ad_name
                    ORDER BY ad_code;
                """

        self.log.info('Now it is inserting to mid_admin_zone...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        
    def __alter_admin_for_specially(self):       
        self.log.info('Now alter admin for specially...')
        #HKGMAC admin
        self.__alter_HKGMAC()
        #BRA admin
        self.__alter_BRA()
        #SGP admin
        self.__alter_SGP()
        
    def __jude_country(self, country_str):
        sqlcmd = '''
                select count(*)
                from rdf_country
                where iso_country_code = '%s';
                '''%country_str
                
        self.pg.execute2(sqlcmd)
        if (self.pg.fetchone2())[0] > 0:
            return True
        else:
            return False
                
    def __alter_SGP(self):
        self.log.info('alter sgp admin begin...')
        
        if self.__jude_country('SGP'):        
            #special area: in SGP, order9 is set to order8
            if self.pg.IsExistTable('sgp_builtup_region'):
                sqlcmd = """
                        --create table
                        drop table if exists temp_order8_SGP;
                        create table temp_order8_SGP
                        as
                        (
                            select a.ad_code, a.ad_name, a.time_zone, a.summer_time_id
                            from mid_admin_zone as a
                            join rdf_admin_hierarchy as b
                            on a.ad_code = b.admin_place_id and 
                               b.iso_country_code = 'SGP' and 
                               b.admin_order = 8
                        );
                        
                        --delete from mid_admin_zone
                        delete from mid_admin_zone
                        where ad_code in
                        (
                            select admin_place_id
                            from rdf_admin_hierarchy
                            where iso_country_code = 'SGP' and admin_order = 8
                        );
                        
                        --insert into new order8
                        INSERT INTO mid_admin_zone (ad_code, ad_order, order0_id,
                                                    order1_id, order2_id, order8_id,
                                                    ad_name, time_zone, summer_time_id, the_geom)
                        SELECT  a.admin_place_id AS ad_code,
                                8 AS ad_order,
                                a.country_id AS order0_id,
                                a.order1_id AS order1_id,
                                a.order2_id AS order2_id,
                                a.admin_place_id AS order8_id,
                                e.feature_name AS ad_name,
                                sgp_order8.time_zone,
                                sgp_order8.summer_time_id,
                                st_multi (st_union(d.the_geom)) as the_geom
                        FROM rdf_admin_hierarchy AS a
                        left join rdf_feature_names as b
                        on a.admin_place_id = b.feature_id
                        left join rdf_feature_name as c
                        on b.name_id = c.name_id
                        left join sgp_builtup_region as d
                        on c.name = d.name
                        LEFT OUTER JOIN mid_temp_feature_name AS e
                        ON a.admin_place_id = e.feature_id
                        left join temp_order8_SGP as sgp_order8
                        on TRUE
                        WHERE a.iso_country_code = 'SGP' and a.admin_order = 9 and d.the_geom is not null
                        GROUP BY a.admin_place_id, a.country_id, a.order1_id, a.order2_id, e.feature_name, sgp_order8.time_zone, sgp_order8.summer_time_id
                        ORDER BY ad_code;
                        """
                self.pg.execute2(sqlcmd)
                self.pg.commit2()
            else:
                self.log.error('sgp_builtup_region is not exit!')
           
    def __alter_BRA(self):
        self.log.info('alter bra admin begin...')
        
        if self.__jude_country('BRA'):
            #delete order1
            sqlcmd = '''
                    delete from mid_admin_zone
                    where ad_code in (
                        select admin_place_id
                        from rdf_admin_hierarchy
                        where admin_order = 1 and iso_country_code = 'BRA'
                    );
                    '''
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            #update order2 order8
            sqlcmd = '''
                    update mid_admin_zone as a
                    set order1_id = NULL
                    from rdf_admin_hierarchy as b
                    where a.ad_code = b.admin_place_id and b.iso_country_code = 'BRA'
                     and  a.ad_order in (2, 8) and b.admin_place_id is not null;
                    
                    '''
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
    
    def __do_admin_time(self):
        self.log.info('making admin time...')
        
        # do summer time table
        self.CreateTable2('mid_admin_summer_time')
        self.CreateTable2('temp_admin_dst_mapping')
        self.CreateFunction2('mid_convert_summer_time')
        self.pg.callproc('mid_convert_summer_time')
        self.pg.commit2()
        
        # do admin's time_zone & summer_time
        sqlcmd = """
            update mid_admin_zone as a 
            set time_zone = b.time_zone, 
                summer_time_id = b.summer_time_id
            from
            (
                select  a.admin_place_id as ad_code,
                        (a.time_zone::smallint + 120) as time_zone,
                        b.summer_time_id
                from
                (
                    select admin_place_id, time_zone, dst_id
                    from rdf_admin_place
                    where time_zone is not null or dst_id is not null                    
                )as a
                left join temp_admin_dst_mapping as b
                on a.dst_id = b.dst_id
            )as b
            where a.ad_code = b.ad_code;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('making admin time OK.')
    
    def __do_admin_order0_iso_country_mapping(self):
        self.log.info('start to make order0 and iso_country mapping')
        
        self.CreateTable2('rdb_admin_order0_iso_country_mapping')
        sqlcmd = """
                INSERT INTO rdb_admin_order0_iso_country_mapping(order0_id, iso_country_code)
                (
                    SELECT country_id, iso_country_code
                    FROM rdf_country
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        self.log.info('end to make order0 and iso_country mapping')
        return 0
    
    def __alter_HKGMAC(self):
        self.log.info('start get HKG_MAC order08')
        
        if self.__jude_country('HKG') or self.__jude_country('MAC'):
            #判断是否含有香港澳门
            sqlcmd = """
                    select distinct iso_country_code, country_id
                    from rdf_country;
                    """
            country_list = list([row[0],row[1]] for row in self.get_batch_data(sqlcmd))
            for country in country_list:
                if country[0] == 'HKG' or country[0] == 'MAC' :
                    #原有的8级做成1级
                    sqlcmd = """
                            DROP TABLE if exists temp_order01_hkgmac;
                            CREATE TABLE temp_order01_hkgmac
                            as
                            (
                              select ad_code, ad_order, order0_id, order1_id, order2_id, 
                                   order8_id, ad_name, time_zone, summer_time_id, the_geom
                              from mid_admin_zone
                              where ad_order = 8 and order0_id = %s
                            );
                                                    
                            delete from mid_admin_zone
                            where ad_order in (1,8) and order0_id = %s;
                            
                            insert into mid_admin_zone(ad_code, ad_order, order0_id,
                                                order1_id, order2_id, order8_id,
                                                ad_name, time_zone, summer_time_id, the_geom)
                            select ad_code, 1 as ad_order, order0_id, order8_id as order1_id, 
                                   null as order2_id, null as order8_id, 
                                   ad_name, time_zone, summer_time_id, the_geom
                            from temp_order01_hkgmac;
                            """
                            
                    self.pg.execute2(sqlcmd, (country[1], country[1]))
                    self.pg.commit2()
                    
                    #创建形点信息,创建link关联表单
                    sqlcmd = """
                            DROP TABLE if exists temp_admin_builtup;
                            CREATE TABLE temp_admin_builtup
                            as
                            (
                                SELECT distinct c.face_id, c.the_geom
                                FROM rdf_carto as a
                                left join rdf_carto_face as b
                                on a.carto_id = b.carto_id
                                left join temp_wkt_face as c
                                on b.face_id = c.face_id
                                where feature_type = 908003 -- Cartographic Settlement Boundary
                            );
                            
                            DROP TABLE if exists temp_link_admin;
                            CREATE TABLE temp_link_admin
                            as
                            (
                                SELECT left_admin_place_id as admin_id, (array_agg(the_geom))[1] as the_geom
                                FROM temp_rdf_nav_link
                                where left_admin_place_id = right_admin_place_id and iso_country_code = %s
                                group by left_admin_place_id
                            );
                            """
                    self.pg.execute2(sqlcmd, (country[0],))
                    self.pg.commit2()
                    
                    #
                    sqlcmd = """
                            insert into mid_admin_zone(ad_code, ad_order, order0_id,
                                                order1_id, order2_id, order8_id,
                                                ad_name, time_zone, summer_time_id, the_geom)
                            select a.admin_id, 8 as ad_order, d.country_id, d.order8_id as order1_id, null,
                                a.admin_id, c.feature_name AS ad_name, time_zone, summer_time_id, st_multi(a.the_geom)
                            from 
                            (
                                select b.admin_id, a.the_geom
                                from temp_admin_builtup as a
                                left join temp_link_admin as b
                                on ST_Contains(a.the_geom, b.the_geom)
                                where b.admin_id is not null
                            )a
                            left join mid_temp_feature_name as c
                            on a.admin_id = c.feature_id
                            left join rdf_admin_hierarchy as d
                            on a.admin_id = d.admin_place_id
                            left join temp_order01_hkgmac as e
                            on d.order8_id = e.ad_code
                            where d.iso_country_code = %s and d.admin_place_id is not null;
                            """
                    self.pg.execute2(sqlcmd,(country[0],))
                    self.pg.commit2()
                        
    
