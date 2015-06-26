# -*- coding: UTF8 -*-
'''
Created on 2014-8-13

@alter  zhaojie
'''
import component.component_base
from common import cache_file
from component.default.multi_lang_name import MultiLangName
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import NAME_TYPE_SHIELD
from component.default.multi_lang_name import NAME_TYPE_ALTER

class comp_admin_msm(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor 
        '''
        component.component_base.comp_base.__init__(self, 'Admin')
    
    #Create table
    def _DoCreateTable(self):
        
        self.CreateTable2('mid_admin_zone')
        self.CreateTable2('mid_admin_summer_time')
        
        return 0
    def _Do(self):
        
        #get a0,a1
        self.__Get_state_level()
        #get a8
        self.__Get_city_level()
        #get name
        self.__make_admin_name()
        #insert into admin
        self.__make_mid_admin_zone()
        
        return 0
    
    def __Get_state_level(self):
        self.log.info('Begin insert into state')
        
        self.CreateTable2('temp_country')
        sqlcmd = '''
            insert into temp_country(code, name, the_geom)
            (
                select (case when strname = 'MALAYSIA'  then 45800000
                        when strname = 'SINGAPORE' then 70200000
                        when strname = 'BRUNEI' then 9600000
                        else 0 end) as code,strname,ST_Multi(st_union(muti_geom)) as all_geom
                from
                (
                    select name_lib[3]::varchar as strname,muti_geom
                    from
                    (
                        select regexp_split_to_array("name",E'\\\,+') as name_lib, 
                                ST_Multi(st_union(the_geom)) as muti_geom
                        from "org_Country_State_Admin_Area_20"
                        group by "name"
                    )temp
                    where (name_lib[4]::varchar) <> 'INA'
                )temp1
                group by strname
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('temp_state')
        
        self.CreateFunction2('mid_insert_into_state')
        self.pg.callproc('mid_insert_into_state')
        self.pg.commit2()
        
        self.log.info('end insert into state')
        return 0
    
    def __Get_city_level(self):
        self.log.info('Begin insert into city')
        
        self.CreateTable2('temp_district_different_city')
        sqlcmd = '''
            insert into temp_district_different_city(city_name,the_geom)
            (
                select (regexp_split_to_array("name",E'\\\,+'))[1]::varchar as strname
                        ,st_multi(ST_Difference(a.the_geom,b.muti_geom))
                from "org_District_City_Subcity_Taman_Admin_Area_20" as a
                left join
                (
                    select datalevel,ST_Multi(st_union(the_geom)) as muti_geom
                    from "org_District_City_Subcity_Taman_Admin_Area_20"
                    where datalevel = 3
                    group by datalevel
                )b
                on b.datalevel = 3
                where a.datalevel = 4 and ((ST_GeometryType(ST_Difference(a.the_geom,b.muti_geom)) = 'ST_MultiPolygon')
                        or (ST_GeometryType(ST_Difference(a.the_geom,b.muti_geom)) = 'ST_Polygon'))
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('temp_state_different_city')
        sqlcmd = '''
            insert into temp_state_different_city(city_name,the_geom)
            (
                select null, ST_Multi(ST_Difference(a.the_geom,b.muti_geom))
                from temp_state as a
                left join
                (
                    select level,ST_Multi(st_union(the_geom)) as muti_geom
                    from
                    (
                        select 1 as level,the_geom
                        from "org_District_City_Subcity_Taman_Admin_Area_20"
                        where datalevel = 3 or datalevel = 4
                        
                        union
                        
                        select 1 as level,the_geom
                        from sgp_builtup_region
                    )temp
                    group by level
                )b
                on b.level = 1
                where (ST_GeometryType(ST_Difference(a.the_geom,b.muti_geom)) = 'ST_MultiPolygon')
                        or (ST_GeometryType(ST_Difference(a.the_geom,b.muti_geom)) = 'ST_Polygon')
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        self.CreateTable2('temp_city')
        self.CreateTable2('temp_code_city')
        self.CreateTable2('temp_code_city_temp')
        self.CreateFunction2('mid_insert_into_city')
        self.pg.callproc('mid_insert_into_city')
        self.pg.commit2()
        
        self.log.info('end insert into city')
        
        return 0
    
    def __make_mid_admin_zone(self):
        self.log.info('Begin make mid_admin_zone')
        
        #a8
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id,
                                        order2_id, order8_id, ad_name ,the_geom)
                (
                    select a.code,8,b.country_code,a.state_code,null,a.code,c.admin_name,a.the_geom
                    from temp_city as a
                    left join temp_state as b
                    on a.state_code = b.code
                    left join temp_admin_name as c
                    on a.code = c.admin_id::integer
                );
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        #a1
        sqlcmd = '''
                update temp_state set the_geom = b.multi_geom
                from
                (
                    select state_code, ST_Multi(st_union(the_geom)) as multi_geom
                    from temp_city
                    group by state_code
                )b
                where code = b.state_code;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id,
                                        order2_id, order8_id, ad_name ,the_geom)
                (
                    select code,1,a.country_code,code,null,null,b.admin_name,the_geom
                    from temp_state as a
                    left join temp_admin_name as b
                    on a.code = b.admin_id::integer
                );
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #a0
        sqlcmd = '''
                update temp_country set the_geom = b.multi_geom
                from
                (
                    select country_code, ST_Multi(st_union(the_geom)) as multi_geom
                    from temp_state
                    group by country_code
                )b
                where code = b.country_code;
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
                insert into mid_admin_zone(ad_code, ad_order, order0_id, order1_id,
                                        order2_id, order8_id, ad_name ,the_geom)
                (
                    select code,0,code,null,null,null,b.admin_name,the_geom
                    from temp_country as a
                    left join temp_admin_name as b
                    on a.code = b.admin_id::integer
                );
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        self.CreateIndex2('mid_admin_zone_the_geom_idx')
        
        
        self.log.info('end make mid_admin_zone')
        return 0
    
    def __make_admin_name(self):   
        sqlcmd = '''
                 select "name" as name
                 from temp_country
                 order by name;
                 '''
        code_dict = dict()
        code_country_dict = dict()
        rows = self.get_batch_data(sqlcmd)
        for row in rows:
            if row[0].lower() == 'brunei':
                code_dict['MAY'] = None
                code_country_dict[row[0]] = 'MAY'
            elif row[0].lower() == 'singapore':
                code_dict['ENG'] = None
                code_country_dict[row[0]] = 'ENG'
            elif row[0].lower() == 'malaysia':
                code_dict['MAY'] = None
                code_country_dict[row[0]] = 'MAY'
            else:
                self.log.error('can not found ')
        self.__DoGet_Admin_name(code_dict,code_country_dict)
   
    def __DoGet_Admin_name(self,code_dict,code_country_dict):          
        self.log.info('begin admin_name ..')
        sqlcmd = '''
                 select id, name,country_name
                 from
                 (   
                     select code as id, "name" as name,"name" as country_name
                     from temp_country
                     
                     union
                     
                     select a.code as id,a."name" as name,b."name" as country_name
                     from temp_state as a
                     left join temp_country as b
                     on a.country_code = b.code
                     
                     union
                     
                     select a.code as id, a."name" as name, c."name" as country_name
                     from temp_city as a
                     left join temp_state as b
                     on a.state_code = b.code
                     left join temp_country as c
                     on b.country_code = c.code
                 )as temp_all
                 where name is not null  
                 order by id, name, country_name;
                 '''
        #test set_language
        
        self.CreateTable2('temp_admin_name')
        MultiLangName.set_language_code(code_dict)
        
        temp_file_obj = cache_file.open('temp_admin_name_new')
        rows = self.get_batch_data(sqlcmd) 
        for row in rows:
            id = row[0]
            name = row[1]
            country_name = row[2]
            
            if name:
                ml_name = MultiLangName(code_country_dict[country_name], name, NAME_TYPE_OFFICIAL)
                
            json_name = ml_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is null. id=%d' % id)
            else:
                self.__store_name_to_temp_file(temp_file_obj, id, json_name,code_country_dict[country_name])
        # ## 把名称导入数据库
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_admin_name')
        self.pg.commit2()
        # close file
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)
        
        self.log.info('end admin_name ..')

    def __store_name_to_temp_file(self, file_obj, strid, json_name,country_code):
        if file_obj:
            file_obj.write('%s\t%s\t%s\n' % (strid, json_name,country_code))
        return 0
    
    def _DoCheckValues(self):
        self.log.info('begin Check_admin_Values..')
        sqlcmd = '''
                 select ad_code
                 from mid_admin_zone
                 where ad_order not in(0,1,8)
                 '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('ad_order is not in(0,1,8)')
            return -1
        self.log.info('end Check_admin_Values..')
        return 0
    
    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
    
    def _DoCheckNumber(self):
        self.log.info('begin Check_admin_Number..')
        sqlcmd = '''
                 select ad_code
                 from mid_admin_zone
                 where ad_order = 0
                 '''
        rows = self.__GetRows(sqlcmd)
        if not rows:
            self.log.error('mid_admin_zone ad_order = %d  number is %d',0,0)
            return -1              
        sqlcmd = '''
                 select ad_code
                 from mid_admin_zone
                 where ad_order = 1
                 '''
        rows = self.__GetRows(sqlcmd)
        if not rows:
            self.log.error('mid_admin_zone ad_order = %d  number is %d',1,0)
            return -1 
        sqlcmd = '''
                 select ad_code
                 from mid_admin_zone
                 where ad_order = 8
                 '''
        rows = self.__GetRows(sqlcmd)
        if not rows:
            self.log.error('mid_admin_zone ad_order = %d  number is %d',8,0)
            return -1 
        self.log.info('end Check_admin_Number..')
        return 0
    
    def _DoCheckLogic(self):
        self.log.info('begin Check_admin_Logic..')
        #check a8 parent_id is null
        sqlcmd = '''
                 select order8,order1
                 from
                 (
                     select a.ad_code as order8,b.ad_code as order1
                     from 
                     (
                         select ad_code,order1_id
                         from mid_admin_zone
                         where ad_order = 8
                     )as a
                     left join
                     (
                         select ad_code
                         from mid_admin_zone
                         where ad_order = 1
                     )as b
                     on a.order1_id = b.ad_code
                 ) as c
                 where c.order1 is null;
                 '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('ad_code%d is not found parent_id ',8)
            return -1
        #check a1 parent_id is null
        sqlcmd = '''    
                 select order1,order0
                 from
                 (
                     select a.ad_code as order1,b.ad_code as order0
                     from 
                     (
                         select ad_code,order0_id
                         from mid_admin_zone
                         where ad_order = 1
                     )as a
                     left join
                     (
                         select ad_code
                         from mid_admin_zone
                         where ad_order = 0
                     )as b
                     on a.order0_id = b.ad_code
                 ) as c
                 where c.order0 is null;
                 '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('ad_code%d is not found parent_id ',1)
            return -1
        #check a8 geom  within a1 
        sqlcmd = '''    
                 select order1
                 from
                 (
                     select a.ad_code as order1,a.the_geom as order1_geom,b.the_geom as order8_geom
                     from 
                     (
                         select ad_code,the_geom
                         from mid_admin_zone
                         where ad_order = 1
                     )as a
                     left join
                     (
                         select order1_id,ST_Multi(st_union(the_geom)) as the_geom
                         from mid_admin_zone
                         where ad_order = 8
                         group by order1_id
                     )as b
                     on a.ad_code = b.order1_id
                 ) as temp_all
                 where ST_Equals(order8_geom,order1_geom) = false;
                 '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('order%d geom is not Equals order%d geom',8,1)
            return -1
        #check a1 geom  within a0 
        sqlcmd = '''
                 select order0
                 from
                 (
                     select a.ad_code as order0,a.the_geom as order0_geom,b.the_geom as order1_geom
                     from 
                     (
                         select ad_code,the_geom
                         from mid_admin_zone
                         where ad_order = 0
                     )as a
                     left join
                     (
                         select order0_id,ST_Multi(st_union(the_geom)) as the_geom
                         from mid_admin_zone
                         where ad_order = 1
                         group by order0_id
                     )as b
                     on a.ad_code = b.order0_id
                 ) as temp_all
                 where ST_Equals(order0_geom,order1_geom) = false;
                 '''
        rows = self.__GetRows(sqlcmd)
        if rows:
            self.log.error('order%d geom is not Equals order%d geom',1,0)
            return -1
         
        self.log.info('end Check_admin_Logic..')
        return 0
    
        