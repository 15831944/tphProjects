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
        
        self._loadBrandIcon()
        self._loadPOICategory()
        self._loadCategoryPriority()
        self._make_poi_category_mapping()
        self._findLogmark()
        self._makePOIName()
        self._makeLogmark()
        return 0

    def _make_poi_category_mapping(self):
        self.log.info('make temp_poi_category_mapping...')
        sqlcmd = '''
            drop table if exists temp_poi_category_mapping;
            create table temp_poi_category_mapping
            as
            (
                with cat as 
                (
                    select distinct org_code, brand_name, per_code,filename
                    from temp_poi_category
                    where genre_type not in( '0','1') or genre_type is null
                ),
                
                p as
                (
                    select  p.uid as poi_id, org_id1, org_id2, p.cat_code,
                            ( case when p.brand_nme is null then '0'
                                   else p.brand_nme end ) as brand
                    from org_poi_point as p
                    join 
                    (
                        select uid, row_number() over(order by uid ) as org_id1, 1000 as org_id2
                        from org_poi_point
                        where cat_code <> 'SHPAUT' and cat_code <> 'SHPREP' or brand_nme = 'Toyota'
                        order by uid
                    ) as t
                    on p.uid     = t.uid
                )
                
                select poi_id::bigint, per_code, filename
                from 
                (
                    select p.poi_id, org_id1, org_id2, cat.per_code, cat.filename
                    from  p
                    join cat
                    on p.cat_code = cat.org_code and p.brand = cat.brand_name
                    
                    union 
                    select poi_id, org_id1, org_id2, cat.per_code, cat.filename
                    from 
                    (
                        select poi_id,org_id1, org_id2, cat_code
                        from 
                        (
                            select p.poi_id, org_id1, org_id2, cat_code, row_number() over(partition by org_id1, org_id2) as seq
                            from p
                            left join cat
                            on p.cat_code = cat.org_code and p.brand = cat.brand_name 
                            where per_code is null
                        ) as z 
                        where seq = 1
                        
                    ) as a
                    join cat
                    on a.cat_code = cat.org_code and cat.brand_name = '0' 
                ) as b
                order by poi_id::bigint
                
            )
           
        '''  
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_poi_category_mapping_poi_id_idx')
        
        
         
    def _findLogmark(self):       
        # find poi with logmark
        self.log.info('make temp_poi_logmark...')
        self.CreateTable2('temp_poi_logmark')
        
        sqlcmd = """
                insert into temp_poi_logmark
                select a.uid as poi_id, a.std_name, b.per_code, c.category_priority, a.the_geom
                from org_poi_point as a
                join temp_poi_category_mapping as b
                on a.uid::bigint = b.poi_id
                left join temp_category_priority as c
                on c.per_code = b.per_code
                where b.filename is not null or c.category_priority in (1,2,4)
                --order by poi_id
                
                union
                 
                select a.uid as poi_id, a.std_name, b.per_code, c.category_priority, a.the_geom
                from org_poi_point as a
                join temp_brand_icon as i
                on a.std_name = split_part(i.brandname,'.',1) 
                join temp_poi_category_mapping as b
                on a.uid::bigint = b.poi_id
                left join temp_category_priority as c
                on c.per_code = b.per_code             
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
                        a.type_code_priority,
                        b.poi_name,
                        ST_GeometryN(a.the_geom,1)
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.poi_id = b.poi_id
                order by a.poi_id    
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
        

        