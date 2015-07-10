# -*- coding: UTF-8 -*-
'''
Created on 2015-5-4

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

class comp_guideinfo_building_ni(component.default.guideinfo_building.comp_guideinfo_building):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_building.comp_guideinfo_building.__init__(self)
      

    def _Do(self):
        self._loadBrandIcon()
        self._loadPOICategory()
        self._loadCategoryPriority()
        self._findLogmark()
        self._makePOIName()
        self._makeLogmark()
        return 0

       
    def _findLogmark(self):       
        # find poi with logmark
        self.log.info('make temp_poi_logmark...')
        self.CreateTable2('temp_poi_logmark')
        
        self.CreateIndex2('org_poi_chaincode_idx')
        self.CreateIndex2('org_poi_kind_idx')
        self.CreateIndex2('temp_poi_category_org_code_idx')
        self.CreateIndex2('temp_brand_icon_brandname_idx')
        sqlcmd = """
                insert into temp_poi_logmark
                (
                select a.poi_id, a.kind,tpc.u_code,a.display_x, a.display_y,a.the_geom
                from org_poi as a
                inner join temp_brand_icon as b
                on a.chaincode = b.brandname
                inner join temp_poi_category as tpc
                on tpc.org_code=a.kind and tpc.chaincode=a.chaincode

                
                union
                
               select a.poi_id, a.kind,tpc.u_code,a.display_x, a.display_y,a.the_geom
               from org_poi as a                
               inner join temp_poi_category as tpc
               on tpc.logmark = 'Y' and a.kind=tpc.org_code and a.chaincode=tpc.chaincode
               );                                 
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
                             b.name as poi_name,
                             (case when b.language = '1'  then  'CHI'
                                   when b.language = '2'  then  'CHT'                                                               
                                   when b.language = '3'  then  'ENG' 
                                   when b.language = '4'  then  'POR' end ) as language,
                             b.language as name_id,
                            'office_name'::varchar  as name_type,                             
                             c.phontype as phoneme_lang,                                                          
                             c.name as phoneme,
                             a.the_geom
                    from temp_poi_logmark as a        
                    join org_pname as b
                    on b.featid = a.poi_id and b.nametype = '9' and b.seq_nm = '1'
                    left join 
                    (               
                        select  featid, nametype, 
                                (case when phontype = '1' then 'PYM'
                                     when phontype = '3' then 'PYT' end) as phontype, 
                                (case when "name" like '%|%'  then split_part(name,'|',1)
                                else "name" end) as "name", 
                                seq_nm
                        from  org_pname_phon
                        where seq_nm = '1' and phontype <> '2'                                                              
 
                    ) as c
                    on c.featid= a.poi_id and b.language in ('1','2')            
            );     
                """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        sqlcmd = """
                select  poi_id,
                    array_agg(name_id) as name_id_array,
                    array_agg(name_type) as name_type_array,
                    array_agg(language) as language_code_array,
                    array_agg(poi_name) as name_array,
                    array_agg(phoneme_lang) as phonetic_language_array,
                    array_agg(phoneme) as phoneme_array
                from 
                    (
                        select * from temp_poi_name_all_language
                        order by poi_id::bigint, name_id::bigint, phoneme_lang                                   
                    ) as a
                group by poi_id
                order by poi_id::bigint;
                """         
               
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('poi_name')
        for asso_rec in asso_recs:
            poi_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(asso_rec[1], 
                                                                                                            asso_rec[2], 
                                                                                                            asso_rec[3], 
                                                                                                            asso_rec[4],
                                                                                                            asso_rec[5], 
                                                                                                            asso_rec[6])
            temp_file_obj.write('%d\t%s\n' % (int(poi_id), json_name))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_poi_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_poi_name_poi_id_idx')         

    
    def _makeLogmark(self):
        self.log.info('make mid_logmark...')
        self.CreateTable2('mid_logmark')
        
        sqlcmd = """
                insert into mid_logmark(poi_id, type_code,type_code_priority, building_name, the_geom)
                select  a.poi_id::bigint, 
                        a.per_code,
                        (case when c.category_priority is null then dd.category_priority
                              else c.category_priority end),
                        b.poi_name,
                        a.the_geom
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.poi_id = b.poi_id
                left join temp_category_priority as c
                on c.u_code = a.per_code::character
                left join temp_category_priority as dd
                on dd.u_code = 'other' 
                order by a.poi_id::bigint;      
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
        

        