# -*- coding: UTF8 -*-
'''
Created on 2012-02-23
@author: sunyifeng
'''

import component.component_base

class comp_admin_rdf(component.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Admin')

    def _DoCreateTable(self):
        self.CreateTable2('rdb_admin_country')
        self.CreateTable2('rdb_admin_order1')

        return 0

    def _Do(self):
        self.__do_country_city()
        self.__do_admin_zone()
        self.__update_admin_for_hkgAndtwn()
        self.__do_admin_time()
        return 0

    def __do_country_city(self):
        self.CreateIndex2('rdf_country_country_id_idx')
        self.CreateIndex2('rdf_admin_hierarchy_admin_place_id_idx')
        self.CreateIndex2('rdf_feature_names_feature_id_idx')
        self.CreateIndex2('rdf_feature_name_name_id_idx')
        self.CreateIndex2('rdf_carto_named_place_id_idx')
        self.CreateIndex2('rdf_carto_face_carto_id_idx')
        self.CreateIndex2('temp_wkt_face_face_id_idx')

        sqlcmd = """
                INSERT INTO rdb_admin_country(country_id, iso_country_code,
                            country_name, the_geom)
                SELECT e.country_id, e.iso_country_code, g.name,
                       st_multi (st_union(c.the_geom)) as the_geom
                FROM rdf_carto as a
                inner join rdf_carto_face as b
                on a.carto_id = b.carto_id
                inner join temp_wkt_face as c
                on b.face_id = c.face_id
                inner join rdf_admin_hierarchy as e
                on a.named_place_id = e.admin_place_id
                inner join rdf_feature_names as f
                on e.country_id = f.feature_id
                inner join rdf_feature_name as g
                on f.name_id = g.name_id
                inner join rdf_country as h
                on e.country_id = h.country_id and
                   g.language_code = h.language_code
                where f.name_type = 'B'
                group by e.country_id, e.iso_country_code, g.name;
                """

        self.log.info('Now it is inserting to rdb_admin_country...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting rdb_admin_country succeeded')

        self.log.info('Now it is inserting to rdb_admin_order1...')
        sqlcmd = """
                INSERT INTO rdb_admin_order1(order1_id, country_id,
                                             order1_name, the_geom)
                SELECT e.order1_id, e.country_id, g.name,
                       st_multi (st_union(c.the_geom)) as the_geom
                FROM rdf_carto as a
                inner join rdf_carto_face as b
                on a.carto_id = b.carto_id
                inner join temp_wkt_face as c
                on b.face_id = c.face_id
                inner join rdf_admin_hierarchy as e
                on a.named_place_id = e.admin_place_id
                inner join rdf_feature_names as f
                on e.order1_id = f.feature_id
                inner join rdf_feature_name as g
                on f.name_id = g.name_id
                inner join rdf_country as h
                on e.country_id = h.country_id and
                   g.language_code = h.language_code
                where e.order1_id is not null and f.name_type = 'B'
                group by e.order1_id, e.country_id, g.name;
                """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        sqlcmd = """
                INSERT INTO rdb_admin_order1(order1_id, country_id,
                                             order1_name, the_geom)
                SELECT e.order8_id, e.country_id, g.name,
                       st_multi (st_union(c.the_geom)) as the_geom
                FROM rdf_carto as a
                inner join rdf_carto_face as b
                on a.carto_id = b.carto_id
                inner join temp_wkt_face as c
                on b.face_id = c.face_id
                inner join rdf_admin_hierarchy as e
                on a.named_place_id = e.admin_place_id
                inner join rdf_feature_names as f
                on e.order8_id = f.feature_id
                inner join rdf_feature_name as g
                on f.name_id = g.name_id
                inner join rdf_country as h
                on e.country_id = h.country_id and
                   g.language_code = h.language_code
                where e.order1_id is null and f.name_type = 'B'
                group by e.order8_id, e.country_id, g.name;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.log.info('Inserting rdb_admin_order1 succeeded')

    def __do_admin_zone(self):
        '''
        make mid_admin_zone
        @author: xuwenbo
        @date: 2014-01-13
        '''
        self.CreateTable2('mid_admin_zone')

        self.CreateIndex2('mid_temp_feature_name_feature_id_idx')

        #insert information into mid_admin_zone
        sqlcmd = """
                INSERT INTO mid_admin_zone (ad_code, ad_order, order0_id,
                                            order1_id, order2_id, order8_id,
                                            ad_name, the_geom)
                SELECT a.admin_place_id AS ad_code, a.admin_order AS ad_order,
                       a.country_id AS order0_id, a.order1_id AS order1_id,
                       a.order2_id AS order2_id, a.order8_id AS order8_id,
                       b.feature_name AS ad_name,
                       st_multi (st_union (e.the_geom)) as the_geom
                FROM rdf_admin_hierarchy AS a

                LEFT OUTER JOIN mid_temp_feature_name AS b
                ON a.admin_place_id = b.feature_id

                LEFT JOIN rdf_carto AS c
                ON a.admin_place_id = c.named_place_id
                LEFT JOIN rdf_carto_face AS d
                ON c.carto_id = d.carto_id
                LEFT JOIN temp_wkt_face AS e
                ON d.face_id = e.face_id

                WHERE a.admin_order in (0, 1, 2, 8)
                GROUP BY ad_code, ad_order, order0_id, order1_id, order2_id,
                         order8_id, ad_name
                ORDER BY ad_code;
                """

        self.log.info('Now it is inserting to mid_admin_zone...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('Inserting mid_admin_zone succeeded')

        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        
        #special area: in SGP, order9 is set to order8
        if self.pg.IsExistTable('sgp_builtup_region'):
            self.log.info('Set SGP order9 to order8...')
            sqlcmd = """
                    delete from mid_admin_zone
                    where ad_code in
                    (
                        select admin_place_id
                        from rdf_admin_hierarchy
                        where iso_country_code = 'SGP' and admin_order = 8
                    );
                    
                    INSERT INTO mid_admin_zone (ad_code, ad_order, order0_id,
                                                order1_id, order2_id, order8_id,
                                                ad_name, the_geom)
                    SELECT  a.admin_place_id AS ad_code,
                            8 AS ad_order,
                            a.country_id AS order0_id,
                            a.order1_id AS order1_id,
                            a.order2_id AS order2_id,
                            a.admin_place_id AS order8_id,
                            e.feature_name AS ad_name,
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
                    WHERE a.iso_country_code = 'SGP' and a.admin_order = 9 and d.the_geom is not null
                    GROUP BY ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name
                    ORDER BY ad_code;
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            self.log.info('Set SGP order9 to order8 end.')

        #update geometry ad_order = 2 and the_geom is null
        sqlcmd = """
                UPDATE mid_admin_zone AS a SET the_geom = b.the_geom
                FROM
                (
                    SELECT a.ad_code,
                           st_multi (st_union (b.the_geom)) AS the_geom
                    FROM mid_admin_zone AS a
                    INNER JOIN mid_admin_zone AS b
                    ON a.ad_order = 2 AND a.the_geom IS NULL AND b.ad_order = 8
                       AND b.order2_id = a.ad_code
                    GROUP by a.ad_code
                ) as b
                WHERE a.ad_code = b.ad_code
                """

        self.log.info('Now it is updating mid_admin_zone ad_order = 2...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('updating mid_admin_zone ad_order = 2 succeeded')

        #update geometry if ad_order = 1 and the_geom is null
        sqlcmd = """
                UPDATE mid_admin_zone AS a SET the_geom = b.the_geom
                FROM
                (
                    SELECT a.ad_code,
                           st_multi (st_union (b.the_geom)) AS the_geom
                    FROM mid_admin_zone AS a
                    INNER JOIN mid_admin_zone AS b
                    ON a.ad_order = 1 AND a.the_geom IS NULL AND b.ad_order = 8
                       AND b.order1_id = a.ad_code
                    GROUP by a.ad_code
                ) as b
                WHERE a.ad_code = b.ad_code
                """

        self.log.info('Now it is updating mid_admin_zone ad_order = 1...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('updating mid_admin_zone ad_order = 1 succeeded')

        #update geometry if ad_order = 0 and the_geom is null
        sqlcmd = """
                UPDATE mid_admin_zone AS a SET the_geom = b.the_geom
                FROM
                (
                    SELECT a.ad_code,
                           st_multi (st_union (b.the_geom)) AS the_geom
                    FROM mid_admin_zone AS a
                    INNER JOIN mid_admin_zone AS b
                    ON a.ad_order = 0 AND a.the_geom IS NULL AND b.ad_order = 1
                       AND b.order0_id = a.ad_code
                    GROUP by a.ad_code
                ) as b
                WHERE a.ad_code = b.ad_code
                """

        self.log.info('Now it is updating mid_admin_zone ad_order = 0...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('updating mid_admin_zone ad_order = 0 succeeded')

        self.CreateIndex2('mid_admin_zone_the_geom_idx')

        self.log.info('making mid_admin_zone OK.')
        return 0
    
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
        
        # update admin's time_zone & summer_time
        sqlcmd = """
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order = 0 and [time_attr] is not null
            )as b
            where a.ad_order = 1 and a.[time_attr] is null and a.order0_id = b.ad_code;
            ;
            
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order = 1 and [time_attr] is not null
            )as b
            where a.ad_order = 2 and a.[time_attr] is null and a.order1_id = b.ad_code;
            ;
            
            update mid_admin_zone as a 
            set [time_attr] = b.[time_attr]
            from
            (
                select ad_code, [time_attr]
                from mid_admin_zone
                where ad_order in (1,2) and [time_attr] is not null
            )as b
            where  a.ad_order = 8 and a.[time_attr] is null 
                   and 
                   (
                       (a.order2_id = b.ad_code)
                       or
                       (a.order2_id is null and a.order1_id = b.ad_code)
                   )
            ;
        """
        for time_attr in ['time_zone', 'summer_time_id']:
            self.pg.execute2(sqlcmd.replace('[time_attr]', time_attr))
            self.pg.commit2()
        
        self.log.info('making admin time OK.')
    
    def __update_admin_for_hkgAndtwn(self):
        self.log.info('start alter mid_admin_zone for hkg and twn.')
           
        sqlcmd = '''
            select distinct iso_country_code,country_id
            from rdf_admin_hierarchy;
            '''
        self.pg.execute2(sqlcmd)
        phlist = self.pg.fetchall2()
        
        for ph in phlist:
            #
            if ph[0] == 'HKG':
                sqlcmd = '''
                        update mid_admin_zone
                        set ad_name = replace(replace(ad_name,'Hong Kong','Hong Kong Island'),'香港','香港島')
                        where ad_name like '%香港%' and ad_order = 2 and ad_name not like '%香港島%'
                        '''
                self.pg.execute2(sqlcmd)
                self.pg.commit2()
             
            sqlcmd = '''
                    select count(*)
                    from mid_admin_zone
                    where order0_id = %s and ad_order = 1;
                '''
            self.pg.execute2(sqlcmd,(ph[1],))
            order1_num = self.pg.fetchone2()[0]
            if order1_num == 0 :
                sqlcmd = '''
                    select max(ad_code)
                    from mid_admin_zone
                    where order0_id = %s;
                    '''
                self.pg.execute2(sqlcmd,(ph[1],))
                max_ad_code = self.pg.fetchone2()[0] + 1
                
                sqlcmd = '''
                        insert into mid_admin_zone(ad_code, ad_order, order0_id,
                                            order1_id, order2_id, order8_id,
                                            ad_name, the_geom)
                        (
                            select %s,1,a.order0_id,%s,a.order2_id,a.order8_id,null,
                                    st_multi (st_union (b.the_geom)) as the_geom
                            from mid_admin_zone as a
                            left join mid_admin_zone as b
                            on a.order0_id = b.order0_id and b.ad_order = 8
                            where a.order0_id = %s and a.ad_order = 0
                            group by a.order0_id,a.order2_id,a.order8_id
                        );
                    '''
                self.pg.execute2(sqlcmd,(max_ad_code,max_ad_code,ph[1]))
                self.pg.commit2()
                
                sqlcmd = '''
                        update mid_admin_zone
                        set order1_id = %s
                        where ad_order in (2,8) and order0_id = %s;
                    '''
                self.pg.execute2(sqlcmd,(max_ad_code,ph[1]))
                self.pg.commit2()
                
                sqlcmd = '''
                        update mid_admin_zone as a
                        set the_geom = b.the_geom
                        from mid_admin_zone as b
                        where b.ad_order = 1 and b.order0_id = a.order0_id and a.ad_order = 0 and a.order0_id = %s;
                    '''
                self.pg.execute2(sqlcmd,(ph[1],))
                self.pg.commit2()
                
                 
        self.log.info('end alter mid_admin_zone for hkg and twn.')
        return 0
