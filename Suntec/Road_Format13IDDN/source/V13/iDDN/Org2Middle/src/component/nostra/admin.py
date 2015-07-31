'''
Created on 2013-12-5

@author: liuxinxing
'''

import base

class comp_admin_nostra(base.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Admin') 
            
    def _Do(self):
        self.log.info('making admin info ...')
        
        self.CreateTable2('mid_admin_zone')
        self.CreateFunction2('mid_get_json_string_for_name')
        self.CreateFunction2('mid_get_json_string_for_list')
        self.CreateFunction2('mid_get_json_string_for_thailand_name')
        self._add_country_table()
        self.__makeAdminZone()
        
        self.log.info('making admin info end.')
        
    def _add_country_table(self):
        self.log.info('making country info...')
        sqlcmd = '''
                 DROP TABLE IF EXISTS org_country CASCADE;
                 select 0 as type, 1 as id, namt, name, 100.5018272 as lon, 13.7541276 as lat into org_country
                 from (
                      select distinct namt, name
                        from org_sea
                       where name = 'Thailand'
                  ) as t
                 '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
          
    def __makeAdminZone(self):
        #
        self.log.info('making province info...')
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name,
                    st_multi(st_union(the_geom)) as the_geom
            from
            (
                select  prov_code::integer as ad_code,
                        1 as ad_order,
                        1::integer as order0_id,
                        prov_code::integer as order1_id,
                        null::integer as order2_id,
                        null::integer as order8_id,
                        mid_get_json_string_for_thailand_name(prov_namt,prov_name) as ad_name,
                        the_geom
                from org_admin_poly
            )as t
            group by ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name
            order by ad_code
            ;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        self.log.info('making district info...')
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name,
                    st_multi(st_union(the_geom)) as the_geom
            from
            (
                select  proamp::integer as ad_code,
                        8 as ad_order,
                        1::integer as order0_id,
                        prov_code::integer as order1_id,
                        null::integer as order2_id,
                        proamp::integer as order8_id,
                        mid_get_json_string_for_thailand_name(amp_namt,amp_name) as ad_name,
                        the_geom
                from org_admin_poly
            )as t
            group by ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name
            order by ad_code
            ;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('making country info...')
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name,
                    st_multi(st_union(the_geom)) as the_geom
            from
            (
                select  a.id::integer as ad_code,
                        0 as ad_order,
                        a.id::integer as order0_id,
                        null::integer as order1_id,
                        null::integer as order2_id,
                        null::integer as order8_id,
                        mid_get_json_string_for_thailand_name(a.namt, a.name) as ad_name,
                        b.order1_geom as the_geom
                from org_country as a
                left join
                (
                    select order0_id,st_multi(st_union(the_geom)) as order1_geom
                    from mid_admin_zone
                    where ad_order = 1
                    group by order0_id
                )as b
                on a.id = b.order0_id
            )as t
            group by ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name
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
