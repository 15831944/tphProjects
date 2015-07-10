# -*- coding: UTF-8 -*-
'''
Created on 2015-4-17

@author: wushengbing
'''
import os
import codecs

import common.config
import common.cache_file
from common import cache_file
import component.component_base
import component.default.multi_lang_name
import component.default.guideinfo_building

class comp_guideinfo_building_mmi(component.default.guideinfo_building.comp_guideinfo_building):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_building.comp_guideinfo_building.__init__(self)

    def _Do(self):

        self._loadPOICategory()
        self._loadBrandIcon()
        self._loadCategoryPriority()
        self._findLogmark()
        self._makePOIName()
        self._makeLogmark()
        return 0

       
    def _findLogmark(self):       
        # find poi with logmark
        self.log.info('make temp_poi_logmark...')
        self.CreateTable2('temp_poi_logmark')
        
        sqlcmd = """
                insert into temp_poi_logmark
                select a.uid as poi_id,a.std_name,a.cat_code,tpc.per_code,a.lat,a.lon,a.the_geom
                from org_poi_point as a
                inner join temp_brand_icon as b
                on a.std_name = b.brandname
                inner join temp_poi_category as tpc
                on tpc.org_code=a.cat_code
                
                union
                
                select a.uid,a.std_name,a.cat_code,tpc.per_code,a.lat,a.lon,a.the_geom
                from org_poi_point as a
                inner join temp_poi_category as tpc
                on tpc.logmark = 'Y' and a.cat_code=tpc.org_code ;
                                                
                """

        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_poi_logmark_poi_id_idx')

   
    def _makePOIName(self):
        self.log.info('make temp_poi_name...')
        self.CreateTable2('temp_poi_name')
        
        # init language code for MultiLangName
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
            
        sqlcmd = """
                drop table if exists temp_poi_name_all_language;
                create table temp_poi_name_all_language
                as
                (
                  
                 select  a.poi_id,
                         a.std_name,
                         'ENG' as language_code,
                         1 as name_id,  
                         a.the_geom,
                         b.name_nuance
                    from temp_poi_logmark as a        
                    left join org_phoneme as b
                    on b."table"='POI' and a.poi_id=b.id
                    
                    union
                    
                    select  a.poi_id,
                            b.name_regional,
                           (case when b.regional_lang_type = 'Hindi' then 'HIN'  
                                 when b.regional_lang_type = 'Malayalam' then 'MAL' 
                                 when b.regional_lang_type = 'Punjabi' then 'PAN'  
                                 when b.regional_lang_type = 'Tamil' then 'TAM' end) as language_code ,
                            
                            (case when b.regional_lang_type = 'Hindi' then 2  
                                  when b.regional_lang_type = 'Malayalam' then 3 
                                  when b.regional_lang_type = 'Punjabi' then 4  
                                  when b.regional_lang_type = 'Tamil' then 5 end) as name_id,                           
                            a.the_geom,
                            null
                    from temp_poi_logmark as a        
                    inner join org_regional_data as b
                    on b."table"='POI' and a.poi_id = b.id 
                    --left join org_phoneme as c
                    --on c.id =b.id 
                    order by poi_id,name_id,language_code                    
                    );     
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        
        sqlcmd = """
                select  poi_id,
                    array_agg(name_id) as name_id_array,
                    array_agg(language_code) as language_code_array,
                    array_agg(std_name) as name_array,
                    array_agg(name_nuance) as phonetic_string_array
                from  temp_poi_name_all_language
                group by poi_id;
                """         
               
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('poi_name')
        for asso_rec in asso_recs:
            poi_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string(asso_rec[1], 
                                                                                                 asso_rec[2], 
                                                                                                 asso_rec[3], 
                                                                                                 asso_rec[4])
            temp_file_obj.write('%d\t%s\n' % (poi_id, json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_poi_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_poi_name_poi_id_idx')         

    
    def _makeLogmark(self):
        self.log.info('make mid_logmark...')
        self.CreateTable2('mid_logmark')
        
        sqlcmd = """
                insert into mid_logmark(poi_id, type_code, type_code_priority, building_name, the_geom)
                select  a.poi_id, 
                        a.type_code,
                        (case when c.category_priority is null then dd.category_priority
                              else c.category_priority end),
                        b.poi_name,
                        ST_GeometryN(a.the_geom,1)
                from temp_poi_logmark as a
                left join temp_poi_name as b
                 on a.poi_id = b.poi_id
                left join temp_category_priority as c
                on c.u_code = a.type_code::character
                left join temp_category_priority as dd
                on dd.u_code = 'other' ;
                    
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
        

        