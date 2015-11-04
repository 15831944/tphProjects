# -*- coding: UTF8 -*-
'''
Created on 2012-9-10

@alter 2014.02.07 zhaojie
'''

import component.default.admin

class comp_admin_ta(component.default.admin.comp_admin):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        component.default.admin.comp_admin.__init__(self)
    
    def _DoCreateTable(self):
        self.CreateTable2('rdb_admin_order0_iso_country_mapping')

        return 0

    def _Do(self):
        self.__Create_admin_newid()
        self.__makeAdminZone()
        self.__makeAdminTimeZone()
        self.do_admin_time_order()
        self.__alter_admin_for_specially()
        self.make_geom_for_order()
        self.__do_admin_order0_iso_country_mapping()
        self.CreateOrder1()
        return 0
    
    def __Create_admin_newid(self):
        self.log.info("make admin new_id begin...")
        
        self.CreateIndex2('org_a0_id_idx')
        self.CreateIndex2('org_a0_order00_idx')
        self.CreateIndex2('org_a1_id_idx')
        self.CreateIndex2('org_a1_order00_idx')
        self.CreateIndex2('org_a1_order01_idx')
        self.CreateIndex2('org_a7_id_idx')
        self.CreateIndex2('org_a7_order00_idx')
        self.CreateIndex2('org_a7_order01_idx')
        self.CreateIndex2('org_a7_order07_idx')
        self.CreateIndex2('org_a8_id_idx')
        self.CreateIndex2('org_a8_order00_idx')
        self.CreateIndex2('org_a8_order01_idx')
        self.CreateIndex2('org_a8_order07_idx') 
        self.CreateIndex2('org_a8_order08_idx')                             
        
        self.CreateTable2('temp_adminid_newandold')
        sqlcmd = '''
                insert into temp_adminid_newandold(id_old, level, id_new)
                select id, level, substr(id::varchar, 2, 3)::integer * 100000 + level*10000 + num
                from
                (
                    select id, order00, 0 as level, 1 as num
                    from org_a0
                    where order00 not like '$%' and name <> 'Outer World'
                    group by level, order00, id
                    
                    union
                    
                    select a.id, a.order00, 1 as level, ROW_NUMBER()OVER (partition by a.order00 order by a.order00, a.id) as num
                    from org_a1 as a
                    left join org_a0 as b
                    on a.order00 = b.order00 and b.order00 not like '$%' and b.name <> 'Outer World'
                    where b.order00 is not null
                    group by level, a.order00, a.id
                    
                    union
                    
                    select a.id, a.order00, 2 as level, ROW_NUMBER()OVER (partition by a.order00 order by a.order00, a.id) as num
                    from org_a7 as a
                    left join org_a0 as b
                    on a.order00 = b.order00 and b.order00 not like '$%' and b.name <> 'Outer World'
                    where b.order00 is not null
                    group by level, a.order00, a.id
                    
                    union
                    
                    select a.id, a.order00, 4 as level, ROW_NUMBER()OVER (partition by a.order00 order by a.order00, a.id) as num
                    from org_a8 as a
                    left join org_a0 as b
                    on a.order00 = b.order00 and b.order00 not like '$%' and b.name <> 'Outer World'
                    where b.order00 is not null
                    group by level, a.order00, a.id
                    order by level, order00, id
                )temp;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_adminid_newandold_id_old_idx')
        self.CreateIndex2('temp_adminid_newandold_id_new_idx')
        self.CreateIndex2('temp_adminid_newandold_level_idx')
               
    def __makeAdminZone(self):
        self.log.info("make mid_admin_zone begin...")
        
        self.CreateTable2('mid_admin_zone')
        self.CreateIndex2('mid_temp_admin_name_admin_id_idx')
        
        #order0
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order0_id, ad_name, the_geom)
                select id_new as ad_code,
                       0 as ad_order,
                       id_new as order0_id,
                       (array_agg(c.admin_name))[1] as ad_name,
                       st_multi(st_union(b.the_geom)) as the_geom                       
                from temp_adminid_newandold as a
                left join org_a0 as b
                on a.id_old = b.id
                left join mid_temp_admin_name as c
                on a.id_old::bigint = c.admin_id
                where level = 0
                group by id_new;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #order1
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, 
                                            ad_name, the_geom)
                select a.id_new as ad_code,
                       1 as ad_order,
                       a.id_new - a.id_new%100000 + 1 as order0_id,
                       a.id_new as order1_id,
                       c.admin_name as ad_name,
                       st_multi(st_union(b.the_geom)) as the_geom                       
                from temp_adminid_newandold as a
                left join org_a1 as b
                on a.id_old = b.id
                left join mid_temp_admin_name as c
                on a.id_old::bigint = c.admin_id
                where a.level = 1
                group by a.id_new, c.admin_name;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        #order2
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, 
                                            order2_id, ad_name, the_geom)
                select a.id_new as ad_code,
                       2 as ad_order,
                       a.id_new - a.id_new%100000 + 1 as order0_id,
                       e.id_new as order1_id,
                       a.id_new as order2_id,
                       c.admin_name as ad_name,
                       st_multi(st_union(b.the_geom)) as the_geom                       
                from temp_adminid_newandold as a
                left join org_a7 as b
                on a.id_old = b.id
                left join mid_temp_admin_name as c
                on a.id_old::bigint = c.admin_id
                
                left join org_a1 as d
                on b.order01 is not distinct from d.order01 
                    and b.order00 = d.order00
                left join temp_adminid_newandold as e
                on d.id = e.id_old and e.level = 1
                               
                where a.level = 2
                group by a.id_new, e.id_new, c.admin_name;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()        

        #order8
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, 
                                            order2_id, order8_id, ad_name, the_geom)
                select a.id_new as ad_code,
                       8 as ad_order,
                       a.id_new - a.id_new%100000 + 1 as order0_id,
                       e_1.id_new as order1_id,
                       e.id_new as order2_id,
                       a.id_new as order8_id,
                       c.admin_name as ad_name,
                       st_multi(st_union(b.the_geom)) as the_geom                       
                from temp_adminid_newandold as a
                left join org_a8 as b
                on a.id_old = b.id
                left join mid_temp_admin_name as c
                on a.id_old::bigint = c.admin_id
                
                left join org_a7 as d
                on b.order07 is not distinct from d.order07 
                  and b.order01 is not distinct from d.order01 
                  and b.order00 = d.order00
                left join temp_adminid_newandold as e
                on d.id = e.id_old and e.level = 2
                  
                left join org_a1 as d_1
                on b.order01 is not distinct from d_1.order01 
                  and b.order00 = d_1.order00
                left join temp_adminid_newandold as e_1
                on d_1.id = e_1.id_old and e_1.level = 1
                             
                where a.level = 4
                group by a.id_new, e.id_new, e_1.id_new, c.admin_name;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
    
    def __makeAdminTimeZone(self):
        self.log.info('making admin time zone begin...')
        
        self.CreateTable2('mid_admin_summer_time')
        self.CreateTable2('temp_admin_timedom_mapping')
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
                select  distinct
                        a.id_new as ad_code,
                        (replace(b.attvalue, '.30', '.50')::float * 10 + 120) as time_zone,
                        d.summer_time_id
                from temp_adminid_newandold as a
                left join org_ae as b
                on a.id_old = b.id and b.atttyp = 'TZ'
                left join org_ae as c
                on a.id_old = c.id and c.atttyp = 'VP'
                left join temp_admin_timedom_mapping as d
                on c.attvalue = d.timedom
                where b.id is not null or c.id is not null
            )as b
            where a.ad_code = b.ad_code;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('making admin time zone end.')
    def __alter_admin_for_specially(self):
        self.log.info('Now alter admin for specially...')
        #VNM
        self.__alter_VNM_admin()
        #AUS,NZL
        self.__alter_AUSNZL_admin()
    
    def __jude_country(self, country_str):
        sqlcmd = '''
                select count(*)
                from org_a0
                where order00 = '%s';
                '''%country_str
                
        self.pg.execute2(sqlcmd)
        if (self.pg.fetchone2())[0] > 0:
            return True
        else:
            return False
        
    def __alter_VNM_admin(self):
        self.log.info('Now alter admin for VNM...')
        
        if self.__jude_country('VNM'):       
            sqlcmd = '''
                    update mid_admin_zone as a
                    set ad_order = 8, order8_id = a.order2_id, order2_id = null
                    where ad_order = 2 and order0_id in (
                        select distinct id_new
                        from org_a0 as a
                        left join temp_adminid_newandold as b
                        on a.id = b.id_old
                        where order00 = 'VNM' and order00 not like '$%' and name <> 'Outer World'
                    );
                    '''
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        
    def __alter_AUSNZL_admin(self):
        self.log.info('Now alter admin for AUS and NZL...')
        if self.__jude_country('AUS') or self.__jude_country('NZL'):
            sqlcmd = '''
                    delete from mid_admin_zone
                    where ad_order = 2 and order0_id in (
                        select distinct id_new
                        from org_a0 as a
                        left join temp_adminid_newandold as b
                        on a.id = b.id_old
                        where order00 in ('AUS', 'NZL') and order00 not like '$%' and name <> 'Outer World'
                    );
                    '''
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            sqlcmd = '''
                    update mid_admin_zone
                    set order2_id = null
                    where ad_order = 8 and order0_id in (
                        select distinct id_new
                        from org_a0 as a
                        left join temp_adminid_newandold as b
                        on a.id = b.id_old
                        where order00 in ('AUS', 'NZL') and order00 not like '$%' and name <> 'Outer World'
                    );
                    '''
            self.pg.execute2(sqlcmd)
            self.pg.commit2()         
            
    
            
    def __do_admin_order0_iso_country_mapping(self):
        self.log.info('start to make order0 and iso_country mapping')
        self.CreateTable2('rdb_admin_order0_iso_country_mapping')
        
        sqlcmd = """
                INSERT INTO rdb_admin_order0_iso_country_mapping(order0_id, iso_country_code)
                (
                    SELECT distinct cast(substring(cast(id as varchar),2,3) as int) * 10000 + 1, order00
                    FROM org_a0
                    WHERE order00 not like '$%' and name <> 'Outer World'
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        self.log.info('end to make order0 and iso_country mapping')
        return 0