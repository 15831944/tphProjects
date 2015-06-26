'''
Created on 2012-2-23

@author: sunyifeng
'''

import component.component_base


class comp_admin_axf(component.component_base.comp_base):
    
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Admin')
        
    def _Do(self):
        self.__makeProvinceCity()
        self.__makeAdminZone()
        return 0
        
    def __makeProvinceCity(self):
        #
        self.CreateTable2('temp_admin_province')
        self.CreateTable2('temp_admin_city')
        self.CreateTable2('rdb_admin_province')
        sqlcmd = """
            SELECT AddGeometryColumn('public', 'rdb_admin_province', 'the_geom', 4326, 'MULTIPOLYGON', 2);
            ALTER TABLE rdb_admin_province DROP CONSTRAINT enforce_geotype_the_geom;
            ALTER TABLE rdb_admin_province ADD CONSTRAINT enforce_geotype_the_geom CHECK 
            (geometrytype(the_geom) = 'MULTIPOLYGON'::text OR geometrytype(the_geom) = 'POLYGON'::text OR the_geom IS NULL);
            ALTER TABLE rdb_admin_province OWNER TO postgres;  
        """
        self.pg.execute2(sqlcmd)
        self.CreateTable2('rdb_admin_city')
        sqlcmd = """
            SELECT AddGeometryColumn('public', 'rdb_admin_city', 'the_geom', 4326, 'MULTIPOLYGON', 2);
            ALTER TABLE rdb_admin_city DROP CONSTRAINT enforce_geotype_the_geom;
            ALTER TABLE rdb_admin_city ADD CONSTRAINT enforce_geotype_the_geom CHECK 
            (geometrytype(the_geom) = 'MULTIPOLYGON'::text OR geometrytype(the_geom) = 'POLYGON'::text OR the_geom IS NULL);
            ALTER TABLE rdb_admin_city OWNER TO postgres;  
        """
        self.pg.execute2(sqlcmd)
        
        #
        sqlcmd = """
            insert into temp_admin_province(ad_cd, ad_name, ad_py)
            select distinct cast(ad_code as integer), name_chn, name_py from org_adminarea 
            where ad_level = 2;
        """
        
        self.log.info('Now it is inserting to temp_admin_province...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting temp_admin_province succeeded')  
            
       
        sqlcmd = """
            insert into temp_admin_city(ad_cd, ad_name, ad_py)
            select distinct cast(substring(d.ad_code,1,4) || '00' as integer) as cd, 
                        d.name_chn, d.name_py from org_adminarea as d
            where ad_level = 3 and substring(d.ad_code, 3, 1) != '9' 
            union
            select distinct cast(d.ad_code as integer) as cd, 
                        d.name_chn, d.name_py from org_adminarea as d
            where ad_level = 4 and substring(d.ad_code, 3, 1) = '9';
        """
            
        self.log.info('Now it is inserting to temp_admin_city...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting temp_admin_city succeeded')  
            
        sqlcmd = """
            insert into rdb_admin_province(ad_cd, ad_name, ad_py, the_geom)
            select ad.ad_cd as code, ad.ad_name as name, ad.ad_py as py, st_union(the_geom) as the_geom 
            from org_adminarea as d
            inner join temp_admin_province as ad 
            on cast(substring(d.ad_code,1,2) || '0000' as integer) = ad.ad_cd
            group by code, name, py;
        """    
        
        self.log.info('Now it is inserting to rdb_admin_province...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting rdb_admin_province succeeded')  
    
      
        sqlcmd = """
            insert into rdb_admin_city(ad_cd, ad_name, ad_py, the_geom)
            select ad.ad_cd as code, ad.ad_name as name, ad.ad_py as py, st_union(the_geom) as the_geom 
            from org_adminarea as d 
            inner join  temp_admin_city as ad 
            on cast(substring(d.ad_code,1,4) || '00' as integer) = ad.ad_cd
            where ad.ad_cd / 10000 not in (11,12,31,50)
            group by code, ad.ad_name, ad.ad_py
            union
            select ad.ad_cd as code, ad.ad_name as name, ad.ad_py as py, st_union(the_geom) as the_geom 
            from org_adminarea as d 
            inner join  temp_admin_city as ad 
            on cast(d.ad_code as integer) = ad.ad_cd
            where substring(d.ad_code,3,1) = '9' and ad.ad_cd / 10000 not in (11,12,31,50)
            group by code, ad.ad_name, ad.ad_py;
        """
        
        self.log.info('Now it is inserting to rdb_admin_city...')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            self.log.info('Inserting rdb_admin_city succeeded')  
        
        return 0
    
    def __makeAdminZone(self):
        self.log.info('making admin_zone info...')
        
        #
        self.CreateTable2('mid_admin_zone')
        self.CreateTable2('mid_admin_summer_time')
        
        # province
        sqlcmd = """
                insert into mid_admin_zone
                (ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
                
                select  ad_code/10000 as ad_code, 
                        1::smallint as ad_order, 
                        null::integer as order0_id, 
                        ad_code/10000 as order1_id, 
                        null::integer as order2_id, 
                        null::integer as order8_id, 
                        name_chn as ad_name, 
                        the_geom
                from
                (
                    select  ad_code::integer as ad_code, name_chn, 
                            st_multi(st_union(the_geom)) as the_geom
                    from
                    (
                        select * 
                        from org_adminarea 
                        where ad_level = 2
                    )as t
                    group by ad_code, name_chn
                )as t2
                order by ad_code
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # city
        sqlcmd = """
                drop table if exists temp_admin_zone_virtual_city;
                create table temp_admin_zone_virtual_city
                as
                (
                    select distinct ad_code, name_chn
                    from org_adminarea
                    where (ad_level = 3) and (substring(ad_code, 1, 2) in ('11','12','31','50'))
                    union
                    select distinct a.ad_code, a.name_chn
                    from org_adminarea as a
                    inner join org_adminarea as b
                    on (a.ad_level = 3) and (b.ad_level in (2,4)) and (a.name_chn = b.name_chn)
                );
                create index temp_admin_zone_virtual_city_ad_code_idx
                    on temp_admin_zone_virtual_city
                    using btree
                    (ad_code);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                insert into mid_admin_zone
                (ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
                
                select  ad_code/100 as ad_code, 
                        2::smallint as ad_order, 
                        null::integer as order0_id, 
                        ad_code/10000 as order1_id, 
                        ad_code/100 as order2_id, 
                        null::integer as order8_id, 
                        name_chn as ad_name, 
                        the_geom
                from
                (
                    select  ad_code::integer as ad_code, name_chn, 
                            st_multi(st_union(the_geom)) as the_geom
                    from
                    (
                        select a.*
                        from org_adminarea as a
                        left join temp_admin_zone_virtual_city as b
                        on a.ad_code = b.ad_code
                        where (a.ad_level = 3) and (b.ad_code is null)
                    )as t
                    group by ad_code, name_chn
                )as t2
                order by ad_code
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # county
        sqlcmd = """
                insert into mid_admin_zone
                (ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
                
                select  ad_code as ad_code, 
                        8::smallint as ad_order, 
                        null::integer as order0_id, 
                        ad_code/10000 as order1_id, 
                        city_code as order2_id, 
                        ad_code as order8_id, 
                        name_chn as ad_name, 
                        the_geom
                from
                (
                    select  ad_code::integer as ad_code, name_chn, city_code, 
                            st_multi(st_union(the_geom)) as the_geom
                    from
                    (
                        select  a.*, 
                                (case when b.ad_code is null then a.ad_code::integer/100 else null::integer end) as city_code
                        from org_adminarea as a
                        left join temp_admin_zone_virtual_city as b
                        on a.ad_code::integer/100 = b.ad_code::integer/100
                        where   (a.ad_level = 4)
                            and (a.ad_code::integer/10000 not in (71)) --exclude taiwan
                    )as t
                    group by ad_code, name_chn, city_code
                )as t2
                order by ad_code
                ;
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        self.CreateIndex2('mid_admin_zone_the_geom_idx')
        
        self.log.info('making admin_zone info OK.')
        
