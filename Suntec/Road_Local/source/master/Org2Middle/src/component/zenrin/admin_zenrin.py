# -*- coding: UTF8 -*-
'''
Created on 2015-6-24

@author: liuxinxing
'''

import component.component_base
import component.default.multi_lang_name

class comp_admin_zenrin(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        component.component_base.comp_base.__init__(self, 'Admin')
        
    def _Do(self):
        self.__do_admin_zone()
        self.__do_admin_time()
        return 0
    
    def __do_admin_zone(self):
        #
        self.log.info('making admin zone...')
        self.CreateTable2('mid_admin_zone')
        self.__do_all_admin_geom()
        self.__do_all_admin_name()
        self.__do_admin_order1()
        self.__do_admin_order8()
        
        #
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        self.CreateIndex2('mid_admin_zone_the_geom_idx')
        
        return 0
        
    def __do_all_admin_geom(self):
        #
        self.log.info('making admin geom...')
        sqlcmd = '''
                drop table if exists temp_admin_zone_list;
                create table temp_admin_zone_list
                as
                (
                    select  gid, elcode, name1, name2,
                            case when name1 in ('金門縣','澎湖縣','連江縣') then mid_transtwd67totwd97_119(the_geom)
                                 else mid_transtwd67totwd97_121(the_geom)
                            end as the_geom
                    from
                    (
                        select  row_number() over (order by elcode, name1, name2) as gid,
                                elcode, name1, name2,
                                st_scale(st_multi(st_union(the_geom)), 1.0 / 3600000, 1.0 / 3600000) as the_geom
                        from
                        (
                            select a.elcode, b.name1, b.name2, a.the_geom
                            FROM org_p_area_administration as a
                            left join org_attribute_name as b
                            on a.meshcode = b.meshcode and a.attrnmno = b.attrnmno
                            where a.elcode in ('550000', '570000', '560000', '580000')
                        )as t
                        group by elcode, name1, name2
                    )as t
                    order by elcode, name1, name2
                );
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __do_all_admin_name(self):
        #
        self.log.info('making admin name...')
        sqlcmd = '''
                select  gid,
                        array_agg(name_id) as name_id_array,
                        array_agg(name_type) as name_type_array,
                        array_agg(language_code) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phonetic_language) as phonetic_language_array,
                        array_agg(phoneme) as phoneme_array
                from
                (
                    select  gid,
                            gid as name_id,
                            'office_name'::varchar as name_type,
                            'CHT'::varchar as language_code,
                            (
                            case
                            when elcode in ('550000', '570000') then name1
                            else name2
                            end
                            )as name,
                            null::varchar as phonetic_language,
                            null::varchar as phoneme
                    from temp_admin_zone_list
                )as t
                group by gid
                '''
        self.name_array_table_2_json_name_table(sqlcmd, 'temp_admin_name')
        
    def __do_admin_order1(self):
        #
        self.log.info('making admin order1...')
        sqlcmd = '''
                drop table if exists temp_admin_order1_mapping;
                create table temp_admin_order1_mapping
                as
                (
                    select  name1,
                            row_number() over (order by elcode, name1) as ad_code
                    from temp_admin_zone_list
                    where elcode in ('550000', '570000')
                    order by elcode, name1
                );
                create index temp_admin_order1_mapping_name_idx
                    on temp_admin_order1_mapping
                    using btree
                    (name1);
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order1_id, ad_name, ad_name_single, the_geom)
                (
                    select  b.ad_code,
                            1 as ad_order,
                            b.ad_code as order1_id,
                            c.json_name,
                            a.name1,
                            a.the_geom
                    from temp_admin_zone_list as a
                    left join temp_admin_order1_mapping as b
                    on a.name1 = b.name1
                    left join temp_admin_name as c
                    on a.gid = c.id
                    where a.elcode in ('550000', '570000')
                    order by ad_code
                );
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __do_admin_order8(self):
        #
        self.log.info('making admin order8...')
        sqlcmd = '''
                drop table if exists temp_admin_order8_mapping;
                create table temp_admin_order8_mapping
                as
                (
                    select  name1, name2, order1_id,
                            (order1_id * 100 + sub_code) as ad_code
                    from
                    (
                        select  a.name1, a.name2, 
                                b.ad_code as order1_id,
                                row_number() over (partition by b.ad_code order by a.elcode, a.name1, a.name2) as sub_code
                        from temp_admin_zone_list as a
                        left join temp_admin_order1_mapping as b
                        on a.name1 = b.name1
                        where a.elcode in ('560000', '580000')
                    )as t
                    order by name1, name2
                );
                create index temp_admin_order8_mapping_name_idx
                    on temp_admin_order8_mapping
                    using btree
                    (name1, name2);
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order1_id, order8_id, ad_name, ad_name_single, the_geom)
                (
                    select  b.ad_code,
                            8 as ad_order,
                            b.order1_id,
                            b.ad_code as order8_id,
                            c.json_name,
                            a.name2,
                            a.the_geom
                    from temp_admin_zone_list as a
                    left join temp_admin_order8_mapping as b
                    on a.name1 = b.name1 and a.name2 = b.name2
                    left join temp_admin_name as c
                    on a.gid = c.id
                    where a.elcode in ('560000', '580000')
                    order by ad_code
                );
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def __do_admin_time(self):
        self.CreateTable2('mid_admin_summer_time')
        #
        self.log.info('making admin time...')
        sqlcmd = '''
                update mid_admin_zone 
                set time_zone = 200, summer_time_id = 0;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        