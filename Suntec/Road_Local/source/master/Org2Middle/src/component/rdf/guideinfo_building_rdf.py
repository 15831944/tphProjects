# -*- coding: UTF-8 -*-
'''
Created on 2014-6-27

@author: zhangyongmu
'''
import os
import codecs

import common.cache_file
import component.component_base
import component.default.multi_lang_name
import component.default.guideinfo_building

import common.config
import common.common_func

class comp_guideinfo_building_rdf(component.default.guideinfo_building.comp_guideinfo_building):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_building.comp_guideinfo_building.__init__(self)
        
    def _Do(self):
        proj_name = common.common_func.GetProjName()
        proj_country = common.common_func.getProjCountry()
          
        if proj_name.lower() == 'rdf' and  proj_country.lower() in ['arg','bra','hkg','mea','ase']:
            self._loadPOICategory_new()
        else:
            self._loadPOICategory()

        self._loadCategoryPriority()
        self._makeTempPoi()
        self._findLogmark()
        self._makePOIName()
        self._makePOILocation()
        self._makeLogmark()   
        
 
        return 0
     
          
    def _findLogmark(self):
        # find poi with logmark
        self.log.info('make temp_poi_logmark...')
        self.CreateTable2('temp_poi_logmark')
        
        sqlcmd = '''
            insert into temp_poi_logmark
            select a.poi_id
            from temp_rdf_poi as a
            left join temp_poi_category as tpc
            on tpc.org_code = a.cat_id and tpc.chain = a.chain 
               and tpc.sub = a.sub and tpc.cuisine = a.cuisine and tpc.building = a.building
            where tpc.filename is not null and tpc.filename <> ''
        
            union
        
            select a.poi_id
            from temp_rdf_poi as a
            join temp_poi_category as tpc
            on  tpc.org_code = a.cat_id 
                and (tpc.chain = a.chain or tpc.chain is null)
                and (tpc.sub = a.sub or tpc.sub is null)
                and (tpc.cuisine = a.cuisine or tpc.cuisine is null)
                and (tpc.building = a.building or tpc.building is null)
            join temp_category_priority as p
            on p.per_code = tpc.per_code and p.category_priority in (1,2,4)
        
            '''
        
        sqlcmd = self.create_sqlcmd(sqlcmd)

        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_poi_logmark_poi_id_idx')
    
    def create_sqlcmd(self,sqlcmd):
        
        proj_name = common.common_func.GetProjName()
        proj_country = common.common_func.getProjCountry()
            
        sqlcmd_poi_icon = '''
            union
            select distinct a.poi_id
            from temp_rdf_poi as a
            join 
            (
                select distinct poi_id 
                from rdf_poi_chains 
            ) ch
            on a.poi_id = ch.poi_id
            join temp_poi_category as tpc 
            on tpc.org_code = a.cat_id and tpc.chain = a.chain 
            and tpc.sub = a.sub and tpc.cuisine = a.cuisine and tpc.building = a.building
            left join rdf_file_feature as b
            on a.poi_id = b.feature_id
            join rdf_file as c
            on c.file_id = b.file_id
            where c.file_type > 18 and  c.file_type < 24 and b.owner = 'P'
      
            
        '''
        
        if proj_name.lower() == 'rdf' and  proj_country.lower() in ['mea','me8','arg','bra']:
            return sqlcmd
        else:
            return sqlcmd + sqlcmd_poi_icon
            
    def _makeTempPoi(self):
        
        sqlcmd = '''
            drop table if exists temp_rdf_poi;
            create table temp_rdf_poi
            as
            (
                select  distinct 
                        p.poi_id, 
                        p.cat_id,
                        COALESCE( c.chain_id, 0 )       as chain,  
                        COALESCE( sc.subcategory, 0 )   as sub,
                        COALESCE( rt.cuisine_id, 0 )    as cuisine, 
                        COALESCE( w.building_type, 0 )  as building
                
                from rdf_poi                         as p
                left join rdf_poi_chains             as c
                on p.poi_id = c.poi_id and c.chain_type = '1'
                left join rdf_poi_subcategory        as sc
                on p.poi_id = sc.poi_id  and sc.seq_num = 1
                left join rdf_poi_restaurant         as rt
                on p.poi_id = rt.poi_id
                left join rdf_poi_place_of_worship   as w
                on p.poi_id = w.poi_id
            );
        '''  
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_rdf_poi_cat_id_chain_sub_cuisine_building_idx')
        self.CreateIndex2('temp_rdf_poi_poi_id_idx')
        
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
                            b.name_id,
                            c.language_code,
                            (case when c.short_name is null then c.name else c.short_name end) as name,
                            e.phonetic_language_code,
                            e.phonetic_string,
                            d.preferred,
                            e.transcription_method
                    from temp_poi_logmark as a
                    left join rdf_poi_names as b
                    on a.poi_id = b.poi_id and b.name_type = 'B' and b.is_exonym = 'N'
                    left join rdf_poi_name as c
                    on b.name_id = c.name_id
                    left join rdf_poi_address as x
                    on a.poi_id = x.poi_id and c.language_code = x.language_code
                    left join vce_poi_name as d
                    on b.name_id = d.name_id
                    left join vce_phonetic_text as e
                    on d.phonetic_id = e.phonetic_id
                    left join vce_language_code_map as f
                    on x.iso_country_code = f.iso_country_code and e.phonetic_language_code = f.phonetic_language_code
                    where   (c.name is not null or c.short_name is not null)
                            and
                            (f.language_code is null or c.language_code = f.language_code)
                    
                    union
                    
                    select  a.poi_id,
                            b.name_id,
                            c.transliteration_type as language_code,
                            (case when c.short_name is null then c.name else c.short_name end) as name, 
                            e.phonetic_language_code,
                            null as phonetic_string,
                            d.preferred,
                            e.transcription_method
                    from temp_poi_logmark as a
                    left join rdf_poi_names as b
                    on a.poi_id = b.poi_id and b.is_exonym = 'N'
                    left join rdf_poi_name_trans as c
                    on b.name_id = c.name_id
                    
                    left join rdf_poi_address as x
                    on a.poi_id = x.poi_id and c.transliteration_type = x.language_code
                    left join vce_poi_name as d
                    on b.name_id = d.name_id
                    left join vce_phonetic_text as e
                    on d.phonetic_id = e.phonetic_id
                    left join vce_language_code_map as f
                    on x.iso_country_code = f.iso_country_code and e.phonetic_language_code = f.phonetic_language_code
                    where   (c.name is not null or c.short_name is not null)
                            and
                            (f.language_code is null or c.transliteration_type = f.language_code)                    
    
                );
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
                                  
        sqlcmd = """
                select  poi_id,
                        array_agg(name_id) as name_id_array,
                        array_agg('office_name'::varchar) as name_type_array,
                        array_agg(language_code) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phonetic_language_code) as phonetic_language_code_array,
                        array_agg(phonetic_string) as phonetic_string_array
                from
                (
                    -- connect prep_phonetic_string and feat_phonetic_string
                    select  poi_id, language_code, phonetic_language_code,
                            name_id, name, phonetic_string
                    from
                    (
                        -- choose the best phonetic_string
                        select  poi_id, language_code, phonetic_language_code,
                                (array_agg(name_id))[1] as name_id,
                                (array_agg(name))[1] as name,
                                (array_agg(phonetic_string))[1] as phonetic_string
                        from
                        (
                            select *
                            from temp_poi_name_all_language as a
                            order by poi_id, language_code, phonetic_language_code, 
                                     preferred desc, transcription_method desc
                        )as t2
                        group by poi_id, language_code, phonetic_language_code
                    )as t3
                    order by poi_id, name_id, language_code, phonetic_language_code
                )as t4
                group by poi_id
                ;
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
            temp_file_obj.write('%d\t%s\n' % (poi_id, json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_poi_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_poi_name_poi_id_idx')
        
    def _makePOILocation(self):
        self.log.info('make temp_poi_location...')
        sqlcmd = '''
            drop index if exists rdf_poi_address_poi_id_idx;
            create index rdf_poi_address_poi_id_idx
            on rdf_poi_address
            using btree
            (poi_id);
            
            drop index if exists rdf_poi_address_location_id_idx;
            create index rdf_poi_address_location_id_idx
            on rdf_poi_address
            using btree
            (location_id);
        
            drop index if exists rdf_location_location_id_idx;
            create index rdf_location_location_id_idx
            on rdf_location
            using btree
            (location_id);        

        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                drop table if exists temp_poi_location;
                create table temp_poi_location 
                as
                (
                    select  m.poi_id,
                            st_setsrid(st_point(n.lon / 100000.0, n.lat / 100000.0), 4326) as the_geom
                    from
                    (
                        select distinct a.poi_id, b.location_id
                        from temp_poi_logmark as a
                        left join rdf_poi_address as b
                        on a.poi_id = b.poi_id
                    )as m
                    left join rdf_location as n
                    on m.location_id = n.location_id
                );
                
                create index temp_poi_location_poi_id_idx
                    on temp_poi_location
                    using btree
                    (poi_id);
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
       
  
    def _makeLogmark(self):
        self.log.info('make mid_logmark...')
        self.CreateTable2('mid_logmark')
        
        sqlcmd = """
                insert into mid_logmark(poi_id, type_code, type_code_priority, building_name, the_geom)
                select  a.poi_id, 
                        tpc.per_code,
                        cc.category_priority,      
                        b.poi_name,
                        e.the_geom
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.poi_id = b.poi_id
                left join temp_rdf_poi as c
                on c.poi_id = a.poi_id
                left join temp_poi_category as tpc
                on  tpc.org_code = c.cat_id 
                    and (tpc.chain = c.chain or tpc.chain is null)
                    and (tpc.sub = c.sub or tpc.sub is null)
                    and (tpc.cuisine = c.cuisine or tpc.cuisine is null)
                    and (tpc.building = c.building or tpc.building is null)
                
                left join temp_poi_location as e
                on a.poi_id = e.poi_id
                left join temp_category_priority as cc
                on cc.per_code = tpc.per_code
                order by a.poi_id;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
   
   
           
