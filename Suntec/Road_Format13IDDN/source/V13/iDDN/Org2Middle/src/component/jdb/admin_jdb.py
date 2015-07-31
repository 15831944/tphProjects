# -*- coding: UTF8 -*-
'''
Created on 2012-2-23

@author: sunyifeng
alter on 2014-4-9
'''
import common
import io
import os
import base

class comp_admin_jdb(base.component_base.comp_base):
    
    _admin_order1_scale = 10000
    
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Admin')

    def _DoCreateTable(self):
        self.CreateTable2('mid_admin_zone')
        self.CreateTable2('temp_admin_order0')
        self.CreateTable2('temp_admin_order1')
        self.CreateTable2('temp_admin_order8')
        
    def _Do(self):
        self.log.info('making admin info ...')
        
        self.__add_country_table()
        self.__Create_org_Idx()
        self.__makeAdmin0_8()
        self.__makeAdminZone()
        self.__make_admin_wav()
        
        self.log.info('making admin info end.')
        
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_replace_adminstr')
        
    def _DoCreateIndex(self):
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        self.CreateIndex2('mid_admin_zone_the_geom_idx')
                
    def __Create_org_Idx(self):
        self.CreateIndex2('base_admin_4326_geom_idx')
        self.CreateIndex2('base_admin_4326_citycode_idx')
        self.CreateIndex2('citycode_citycode_idx')
        self.CreateIndex2('citycode_prefname_idx')
        
    def __add_country_table(self):
        self.log.info('making country info...')
        sqlcmd = '''
                 DROP TABLE IF EXISTS org_country CASCADE;
                 select 0 as type, 
                        1 as id, 
                        '日本' as name_kanji, 
                        null as name_yomi, 
                        138.471 as lon, 
                        33.520 as lat 
                        into org_country;
                 '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __makeAdmin0_8(self):
        self.log.info('making admin_a0_a8 start..')
        
        sqlcmd = """
                CREATE OR REPLACE FUNCTION mid_inster_order0_8()
                    RETURNS boolean
                    LANGUAGE plpgsql
                AS $$
                DECLARE
                BEGIN
                    insert into temp_admin_order8(order8_id,order1_id,name_kanji,name_yomi,the_geom)
                    select a.citycode as order8_id,
                         substring(a.citycode,1,2) as order1_id,
                         b.cityname as name_kanji,
                         b.cityyomi as name_yomi,
                         a.the_geom as the_geom
                    from
                    (
                        select citycode,st_multi(st_union(the_geom)) as the_geom
                        from base_admin_4326
                        group by citycode
                        order by citycode
                    )as a
                    left join
                    citycode as b
                    on a.citycode = b.citycode;
                    
                    insert into temp_admin_order1(order1_id,order0_id,name_kanji,name_yomi,the_geom)
                    select a.order1_id as order1_id,
                         {0} as order0_id,
                         b.prefname as name_kanji,
                         null as name_yomi,
                         a.the_geom  as the_geom
                    from
                    (
                        select order1_id,st_multi(st_union(the_geom)) as the_geom
                        from temp_admin_order8
                        group by order1_id
                        order by order1_id
                    )as a
                    left join
                    (
                        select substring(citycode,1,2) as citycode1,prefname
                        from citycode
                        group by citycode1,prefname
                        order by citycode1,prefname
                    )as b
                    on a.order1_id = b.citycode1;
                    
                    insert into temp_admin_order0(order0_id,order0_code,name_kanji,name_yomi,the_geom)
                    select {0} as order0_id,
                           {0} as order0_code,
                            a.name_kanji,
                            a.name_yomi,
                            b.the_geom
                    from org_country as a
                    left join
                    (
                        select 1 as country_id,
                            st_multi(st_union(the_geom)) as the_geom
                        from temp_admin_order1
                    )as b
                    on a.id = b.country_id;
                    
                    return true;
                END;
                $$;
                select mid_inster_order0_8();
        """
        str_scale_order0 = str(self._admin_order1_scale*100)
        self.pg.execute2(sqlcmd.format(str_scale_order0))
        self.pg.commit2()
    
        self.log.info('making admin_a0_a8 end..')
    def __makeAdminZone(self):
        self.log.info('making admin_zone info...')
        
        str_scale_order0 = str(self._admin_order1_scale*100)
        #order8
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  order8_id::integer as ad_code,
                    8::smallint as ad_order,
                    {0}::integer as order0_id,
                    (order1_id::integer)*{1} as order1_id,
                    null as order2_id,
                    order8_id::integer as order8_id,
                    mid_get_json_string_for_japan_name(mid_replace_adminstr(name_kanji),name_yomi) as ad_name,
                    st_multi(the_geom) as the_geom
            from temp_admin_order8
            order by order8_id
            ;
            """
        self.pg.execute2(sqlcmd.format(str_scale_order0, self._admin_order1_scale))
        self.pg.commit2()
        
        #order1
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  (order1_id::integer)*{0} as ad_code,
                    1::smallint as ad_order,
                    {1}::integer as order0_id,
                    (order1_id::integer)*{0} as order1_id,
                    null as order2_id,
                    null as order8_id,
                    mid_get_json_string_for_japan_name(mid_replace_adminstr(name_kanji),name_yomi) as ad_name,
                    st_multi(the_geom) as the_geom
            from temp_admin_order1
            order by order1_id
            ;
            """
        
        self.pg.execute2(sqlcmd.format(self._admin_order1_scale, str_scale_order0))
        self.pg.commit2()
        
        #order0
        sqlcmd = """
            insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id, order2_id, order8_id, ad_name, the_geom)
            select  order0_id::integer as ad_code,
                    0::smallint as ad_order,
                    order0_id::integer as order0_id,
                    null as order1_id,
                    null as order2_id,
                    null as order8_id,
                    mid_get_json_string_for_japan_name(mid_replace_adminstr(name_kanji),name_yomi) as ad_name,
                    st_multi(the_geom) as the_geom
            from temp_admin_order0
            order by order0_id
            ;
            """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('making admin_zone info OK.')
    
    def __make_admin_wav(self):
        self.log.info('start making admin_wav.txt')
        
        admin_wav_path = common.GetPath('admin_wav')
        admin_wav_file = io.open(admin_wav_path, 'w', 8192, 'utf8')
        admin_code_base = 1
        admin_wav_base = 111
        
        sqlcmd = '''
                select ad_code
                from mid_admin_zone
                where ad_order = 1
                order by ad_code;
                '''

        rows = self.get_batch_data(sqlcmd)
        strlines = ''
        for row in rows:
            admin_code = int(row[0]/self._admin_order1_scale)
            admin_wav = admin_wav_base + admin_code - admin_code_base
            strlines = strlines + str(row[0]) + ',' + str(admin_wav) + '\n'
            
        admin_wav_file.writelines(unicode(strlines,'utf8')) 
        admin_wav_file.close()
        
        self.log.info('end making admin_wav.txt')
