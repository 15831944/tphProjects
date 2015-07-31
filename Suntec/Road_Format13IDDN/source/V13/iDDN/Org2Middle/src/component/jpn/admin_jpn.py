'''
Created on 2012-2-23

@author: sunyifeng
'''

import base

class comp_admin_jpn(base.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Admin') 
            
    def _Do(self):
        self.log.info('making admin info ...')
        
        self.__makeAdminProvince()
        self.__makeAdminZone()
        
        self.log.info('making admin info end.')
        
    def __makeAdminProvince(self):
        self.log.info('making admin_province info ...')
        
        #
        self.CreateTable2('temp_admin_province')
        self.CreateTable2('rdb_admin_province')
        sqlcmd = 'Alter table rdb_admin_province drop CONSTRAINT enforce_geotype_the_geom;'
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        sqlcmd = """
            insert into temp_admin_province(ad_cd, ad_name, ad_py)
            SELECT distinct cast(substring(citycode, 1, 2) || '000' as integer) as cd, prefname as nm, null as py
              FROM basemap_citycode order by cd;
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    

        sqlcmd = """
            insert into rdb_admin_province(ad_cd, ad_name, ad_py, the_geom)
            select ad.ad_cd as code, ad.ad_name as name, ad.ad_py as py, st_transform(st_union(the_geom), 4326) as the_geom from basemap_adm as d
            inner join temp_admin_province as ad on cast(substring(d.citycode,1,2) || '000' as integer) = ad.ad_cd
            group by code, name, py
            order by code, name, py;
        """    
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        self.CreateIndex2('rdb_admin_province_the_geom_idx')
        
        self.log.info('making admin_province info OK.')
        
        return 0
        
    def __makeAdminZone(self):
        self.log.info('making admin_zone info...')
        
        self.CreateTable2('mid_admin_zone')
        
        #
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  ad_cd as ad_code,
                    1 as ad_order,
                    null as order0_id,
                    ad_cd as order1_id,
                    null as order2_id,
                    null as order8_id,
                    ad_name,
                    st_multi(the_geom) as the_geom
            from rdb_admin_province
            order by ad_code
            ;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  ad_code, 
                    8 as ad_order,
                    null as order0_id,
                    cast(substring(a.citycode, 1, 2) || '000' as integer) as order1_id,
                    null as order2_id,
                    ad_code as order8_id,
                    b.cityname as ad_name, 
                    st_multi(a.the_geom) as the_geom
            from
            (
                select  cast(citycode as integer) as ad_code, 
                        citycode, 
                        st_transform(st_union(the_geom), 4326) as the_geom
                from basemap_adm
                group by citycode
            )as a
            left join basemap_citycode as b
            on a.citycode = b.citycode
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

