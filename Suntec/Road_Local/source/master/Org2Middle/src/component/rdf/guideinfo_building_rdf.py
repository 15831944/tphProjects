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

class comp_guideinfo_building_rdf(component.default.guideinfo_building.comp_guideinfo_building):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_building.comp_guideinfo_building.__init__(self)
        
    def _Do(self):
        self._loadCategoryPriority()
        self._findLogmark()
        self._makePOIName()
        self._makePOILocation()
        self._loadPOICategory()
        self._makeLogmark()
        return 0
    
    def _findLogmark(self):
        # find poi with logmark
        self.log.info('make temp_poi_logmark...')
        self.CreateTable2('temp_poi_logmark')
        
        sqlcmd = """
                insert into temp_poi_logmark
                select distinct a.poi_id
                from rdf_poi as a
                inner join rdf_poi_chains as b
                on a.poi_id = b.poi_id;
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
                            b.name_id,
                            c.language_code,
                            (case when c.short_name is null then c.name else c.short_name end) as name,
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
                            e.phonetic_string,
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
                        array_agg(language_code) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phonetic_string) as phonetic_string_array
                from
                (
                    -- connect prep_phonetic_string and feat_phonetic_string
                    select  poi_id, language_code, 
                            name_id, name, phonetic_string
                    from
                    (
                        -- choose the best phonetic_string
                        select  poi_id, language_code, 
                                (array_agg(name_id))[1] as name_id,
                                (array_agg(name))[1] as name,
                                (array_agg(phonetic_string))[1] as phonetic_string
                        from
                        (
                            select *
                            from temp_poi_name_all_language as a
                            order by poi_id, language_code, 
                                     preferred desc, transcription_method desc
                        )as t2
                        group by poi_id, language_code
                    )as t3
                    order by poi_id, name_id, language_code
                )as t4
                group by poi_id
                ;
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
        
    def _makePOILocation(self):
        self.log.info('make temp_poi_location...')
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
        
    def _loadPOICategory(self):
        self.log.info('make temp_poi_category...')
        self.CreateTable2('temp_poi_category')
        
        category_file_path = common.config.CConfig.instance().getPara('POI_Code')
        for line in open(category_file_path):
            line = line.strip()
            if line[0] == '#':
                continue
            fields = line.split(';')
            sqlcmd = '''
                      insert into temp_poi_category values(%s,%s,%s,%s,%s,%s,%s,%s,%s)
                     '''
            self.pg.execute(sqlcmd, fields)
        self.pg.commit()
        self.CreateIndex2('temp_poi_category_org_code_idx')
    
    def _makeLogmark(self):
        self.log.info('make mid_logmark...')
        self.CreateTable2('mid_logmark')
        
        sqlcmd = """
                insert into mid_logmark(poi_id, type_code, type_code_priority, building_name, the_geom)
                select  a.poi_id, 
                        d.per_code,
                        (case when cc.category_priority is null  then dd.category_priority
                              else cc.category_priority end),
                        b.poi_name,
                        e.the_geom
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.poi_id = b.poi_id
                left join rdf_poi as c
                on a.poi_id = c.poi_id
                left join temp_poi_category as d
                on c.cat_id = d.org_code
                left join temp_poi_location as e
                on a.poi_id = e.poi_id
                left join temp_category_priority as cc
                on cc.u_code = d.per_code ::character
                left join temp_category_priority as dd
                on dd.u_code = 'other'
                order by a.poi_id;
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
    
