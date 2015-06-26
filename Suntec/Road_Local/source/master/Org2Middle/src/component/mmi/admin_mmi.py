# -*- coding: UTF8 -*-
'''
Created on 2014-3-6

@alter  zhaojie
'''
import component.component_base
from common import cache_file
from component.default.multi_lang_name import MultiLangName
from component.default.multi_lang_name import NAME_TYPE_OFFICIAL
from component.default.multi_lang_name import NAME_TYPE_SHIELD
from component.default.multi_lang_name import NAME_TYPE_ALTER


class comp_admin_mmi(component.component_base.comp_base):
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
        
        self.CreateTable2('temp_admin_order0')
        self.CreateTable2('temp_admin_order1')
        self.CreateTable2('temp_admin_order2')
        self.CreateTable2('temp_admin_order8')
        self.CreateTable2('temp_admin_order8_part')
        self.CreateTable2('temp_admin_order8_compare')
        self.CreateTable2('temp_adminid_newandold')
       
        self.CreateTable2('mid_admin_zone')
        self.CreateTable2('mid_admin_summer_time')
        
        return 0

    def _DoCreateFunction(self):
        self.CreateFunction2('mid_admin_judge_country_array')
        self.CreateFunction2('mid_admin_add_order8_geom')
        self.CreateFunction2('mid_admin_add_order1_geom')
        self.CreateFunction2('mid_admin_add_order0_geom')
        
    #alter table    
    def _Do(self):
        #prepare for admin
        self.__Create_org_Idx()
        #start do admin,get difference
        self.__DoAdmin_Begin()
        #insert into order0_8       
        self.__makeAdminOrder0_8()
        #Get MultiLangName
#        self.__DoJude_Admin_Area()
        #insert into mid_admin_zone
        self.__makeAdminZone()
        
        self.__update_admin_time_zone()
       
        return 0
    
    def _DoCreateIndex(self):
        self.CreateIndex2('mid_admin_zone_ad_code_idx')
        self.CreateIndex2('mid_admin_zone_ad_order_idx')
        self.CreateIndex2('mid_admin_zone_order0_id_idx')
        self.CreateIndex2('mid_admin_zone_order1_id_idx')
        self.CreateIndex2('mid_admin_zone_order2_id_idx')
        self.CreateIndex2('mid_admin_zone_order8_id_idx')
        self.CreateIndex2('mid_admin_zone_the_geom_idx')
                
    def __Create_org_Idx(self):
        self.CreateIndex2('org_state_region_geom_idx')
        self.CreateIndex2('org_state_district_geom_idx')
        self.CreateIndex2('org_area_id_idx')
        self.CreateIndex2('org_state_islands_region_geom_idx')

    def __DoAdmin_Begin(self):
        self.log.info('Begin admin different..')
        
        self.CreateTable2('temp_admin_state_difference_district')       
        self.CreateIndex2('temp_admin_state_difference_district_geom_idx')
        
        self.CreateTable2('temp_admin_district_new_all')
        self.CreateTable2('temp_admin_district_new_combine')
               
        self.log.info('end admin different..')     
    
    def __makeAdminOrder0_8(self):           
        self.log.info('Begin make admin table 0_8.')
       
       
        self.CreateFunction2('mid_admin_get_order0_8')          
        self.pg.callproc('mid_admin_get_order0_8')
        self.pg.commit2()
        
        self.log.info('End make admin table 0_8.')  
    
    def __DoJude_Admin_Area(self):   
        sqlcmd = '''
                 select id,"name" as name
                 from org_area
                 where kind = '10'
                 group by id,name
                 order by id,name;
                 '''
        code_dict = dict()
        code_country_dict = dict()
        code_country_code = dict()
        rows = self.get_batch_data(sqlcmd)
        for row in rows:
            if row[1].lower() == 'india':
                code_dict['ENG'] = None
                code_country_dict[row[1]] = 'ENG'
                code_country_code[row[1]] = 'IND'
            else:
                self.log.error('can not found ')
        self.__DoGet_Admin_name(code_dict,code_country_dict,code_country_code)
   
    def __DoGet_Admin_name(self,code_dict,code_country_dict,code_country_code):          
        self.log.info('begin admin_name ..')
        sqlcmd = '''
                 select id,name,name_alt,country_name
                 from
                 (   
                     select id,"name" as name,"names" as name_alt,"name" as country_name
                     from 
                     (
                         select id,"name","names"
                         from org_area
                         where kind = '10'
                     )as temp
                     
                     union
                     
                     select a.id,a.stt_nme as name,a.stt_alt as name_alt,b."name" as country_name
                     from 
                     (
                         select stt_id as id,stt_nme,stt_alt,parent_id
                         from org_state_region
                         where kind = '9'
                         group by stt_id,stt_nme,stt_alt,parent_id
                         order by stt_id,stt_nme,stt_alt,parent_id
                     )as a
                     left join
                     (
                         select id,"name"
                         from org_area
                         where kind = '10'
                         group by id,"name"
                         order by id,"name"
                     )as b
                     on a.parent_id = b.id
                     
                     union
                     
                     select dst_id as id,dst_nme as name,dst_alt as name_alt,d."name" as country_name
                     from org_district_region as c
                     left join
                     (
                         select state.id,country."name"
                         from 
                         (
                             select stt_id as id,parent_id
                             from org_state_region
                             where kind = '9'
                             group by stt_id,parent_id
                             order by stt_id,parent_id
                         )as state
                         left join
                         (
                             select id,"name"
                             from org_area
                             where kind = '10'
                             group by id,"name"
                             order by id,"name"
                         )as country
                         on state.parent_id = country.id
                     )as d
                     on c.parent_id = d.id              
                     group by dst_id,dst_nme,dst_alt,d."name"
                 )as temp_all
                 where (name is not null) or (name_alt is not null)    
                 order by id,name,name_alt,country_name;
                 '''
        #test set_language
        
        MultiLangName.set_language_code(code_dict)
        
        temp_file_obj = cache_file.open('temp_admin_name_new')
        rows = self.get_batch_data(sqlcmd) 
        for row in rows:
            id = int(row[0])
            name = row[1]
            namealtes = row[2]
            country_name = row[3]
            
            if name:
                ml_name = MultiLangName(code_country_dict[country_name], name, NAME_TYPE_OFFICIAL)
            if namealtes:
                # ";" separation
                for namealte in namealtes.split(';'):
                    alt_name = MultiLangName(code_country_dict[country_name], namealte, NAME_TYPE_ALTER)
                    if ml_name:
                        ml_name.add_alter(alt_name)
                    else:
                        ml_name = alt_name
                
            json_name = ml_name.json_format_dump()
            if not json_name:
                self.log.error('Json Name is null. id=%d' % id)
            else:
                self.__store_name_to_temp_file(temp_file_obj, id, json_name,code_country_code[country_name])
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
    
    def __makeAdminZone(self):
        self.log.info('making admin_zone info...')
        
        self.CreateFunction2('mid_admin_insert_mid_admin_zone')       
        self.pg.callproc('mid_admin_insert_mid_admin_zone')
        
        self.pg.commit2()
        
        self.log.info('making admin_zone info OK.')
    
    def __update_admin_time_zone(self):
        self.log.info('update admin time zone...')
        
        sqlcmd = '''
                 select new_id,"name" as name
                 from org_area as a
                 left join temp_adminid_newandold as b
                 on a.id = b.id_old::double precision 
                 where kind = '10'
                 group by new_id,name
                 order by new_id,name;
                 '''
        country_dict = dict()
        rows = self.get_batch_data(sqlcmd)
        for row in rows:
            country_dict[row[0]] = row[1]
            if row[1].lower() != 'india':
                self.log.error('admin name is not india, %s',row[1])
                
        sqlcmd =  '''
                update mid_admin_zone
                set time_zone = %s
                where order0_id = %s;
                '''
        for key in country_dict.keys():
            if country_dict[key].lower() == 'india':
                self.pg.execute2(sqlcmd, (175, key))
                self.pg.commit2()    
        
        return 0
        
    
    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
            
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
    
        