# -*- coding: UTF-8 -*-
'''
Created on 2015-3-26

@author: wusheng
'''
import os
import codecs

import common.cache_file
import component.default.multi_lang_name
import component.default.guideinfo_building

class comp_guideinfo_building_ta(component.default.guideinfo_building.comp_guideinfo_building):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_building.comp_guideinfo_building.__init__(self)
        
    def _Do(self):
        
        self._loadBrandIcon()
        self._loadPOICategory_new()
        self._make_poi_category_mapping()
        self._loadCategoryPriority() 
        self._findLogmark()
        self._makePOIName()
        self._makeLogmark()
        return 0


    def _make_poi_category_mapping(self):
        self.log.info('make temp_poi_category_mapping ...')
        
        sqlcmd = '''
            drop table if exists temp_poi_category_mapping;
            
            create table temp_poi_category_mapping
            as
            (
        
               with cat1 as 
                (
                    select  distinct org_code, sub_category, genre_type, brand_name, per_code,filename
                    from temp_poi_category
                    where (genre_type <> '0' or genre_type is null) and sub_category <> 0 and brand_name <> '0'
                ),
                cat2 as 
                (
                    select distinct org_code, sub_category, genre_type, brand_name, per_code,filename
                    from temp_poi_category
                    where (genre_type <> '0' or genre_type is null) and sub_category != 0 and brand_name = '0'
                ),
                cat3 as 
                (
                    select distinct org_code, sub_category, genre_type, brand_name, per_code,filename
                    from temp_poi_category
                    where (genre_type <> '0' or genre_type is null) and sub_category = 0 and brand_name != '0'
                ),
                cat4 as 
                (
                    select distinct org_code, sub_category, genre_type, brand_name, per_code,filename
                    from temp_poi_category
                    where (genre_type <> '0' or genre_type is null) and sub_category = 0 and brand_name = '0'
                ),
                temp_poi_all as
                (
                    select id, feattyp, subcat,buaname as brandname from org_mn_pi
                    union 
                    select id, feattyp, subcat,brandname from org_mnpoi_pi 
                )
                
                select org_id1, org_id2, per_code,filename
                from (
                    select org_id1, org_id2, per_code, filename,row_number() over ( partition by org_id1, org_id2 order by pref ) as seq
                    from (
                        select p.id as org_id1, feattyp as org_id2, per_code,filename, 1 as pref
                        from temp_poi_all as p
                        join cat1
                        on  p.feattyp = cat1.org_code and p.subcat = cat1.sub_category and p.brandname = cat1.brand_name 
            
                        union 
                        select p.id as org_id1, feattyp as org_id2, per_code,filename, 2 as pref
                        from temp_poi_all as p
                        join cat2
                        on  p.feattyp = cat2.org_code and p.subcat = cat2.sub_category
            
                        union 
                        select p.id as org_id1, feattyp as org_id2, per_code,filename, 3 as pref
                        from temp_poi_all as p
                        join cat3
                        on  p.feattyp = cat3.org_code and p.brandname = cat3.brand_name
            
                        union 
                        select p.id as org_id1, feattyp as org_id2, per_code,filename, 4 as pref
                        from temp_poi_all as p
                        join cat4
                        on  p.feattyp = cat4.org_code
                    ) as b 
                ) as t
                where seq = 1
            )

         '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_poi_category_mapping_org_id1_org_id2_idx')
        
    
    # find poi with logmark
    def _findLogmark(self):
        self.log.info('make temp_poi_logmark...')
        
        self.CreateTable2('temp_mnpoi_logmark')
        sqlcmd = """ 
            insert into temp_mnpoi_logmark
            select omp.id, omp.feattyp, b.per_code, p.category_priority, omp.the_geom
            from org_mnpoi_pi omp
            join temp_poi_category_mapping as b
            on omp.id = b.org_id1 and omp.feattyp = b.org_id2
            join temp_category_priority as p
            on p.per_code = b.per_code
            where b.filename is not null

            union
            
            select omp.id, omp.feattyp, b.per_code, p.category_priority, omp.the_geom
            from org_mnpoi_pi omp
            join temp_poi_category_mapping as b
            on omp.id = b.org_id1 and omp.feattyp = b.org_id2
            join temp_category_priority as p
            on p.per_code = b.per_code and p.category_priority in (1,2,4)
            
            union
            --brand icon
            select bd.id, bd.feattyp, b.per_code, p.category_priority, bd.the_geom
            from
            (
                select a.id,a.feattyp, a.the_geom, a.name,a.brandname 
                from org_mnpoi_pi as a
                join temp_brand_icon as b
                on a.brandname = b.brandname
                
                union
                
                select c.id,c.feattyp, c.the_geom, c.name,c.brandname
                from 
                (
                    select a.id,a.feattyp, a.the_geom, a.name,a.brandname
                    from org_mnpoi_pi as a
                    left join temp_brand_icon as b
                    on a.brandname = b.brandname
                    where b.brandname is null
                ) as c
                join temp_brand_icon as d
                on c.name = d.brandname
            ) bd
            join temp_poi_category_mapping as b
            on bd.id = b.org_id1 and bd.feattyp = b.org_id2
            join temp_category_priority as p
            on p.per_code = b.per_code
            
        """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_mnpoi_logmark_id_idx')
         
        self.CreateTable2('temp_mn_logmark')                
        sqlcmd = """ 
                insert into temp_mn_logmark                
                select mp.id, mp.feattyp, b.per_code, p.category_priority, mp.the_geom
                from org_mn_pi mp
                join temp_poi_category_mapping as b
                on mp.id = b.org_id1 and mp.feattyp = b.org_id2
                join temp_category_priority as p
                on p.per_code = b.per_code
                where b.filename is not null
    
                union
                
                select mp.id, mp.feattyp, b.per_code, p.category_priority, mp.the_geom
                from org_mn_pi mp
                join temp_poi_category_mapping as b
                on mp.id = b.org_id1 and mp.feattyp = b.org_id2
                join temp_category_priority as p
                on p.per_code = b.per_code and p.category_priority in (1,2,4)     
                
                union
                
                --brand icon
                select bd.id, bd.feattyp, b.per_code, p.category_priority, bd.the_geom
                from
                (
                    select a.id,a.feattyp, a.the_geom, a.name,a.buaname 
                    from org_mn_pi as a
                    join temp_brand_icon as b
                    on a.buaname = b.brandname
                    
                    union
                    
                    select c.id,c.feattyp, c.the_geom, c.name,c.buaname
                    from 
                    (
                        select a.id,a.feattyp, a.the_geom, a.name,a.buaname
                        from org_mn_pi as a
                        left join temp_brand_icon as b
                        on a.buaname = b.brandname
                        where b.brandname is null
                    ) as c
                    join temp_brand_icon as d
                    on c.name = d.brandname
                ) bd
                join temp_poi_category_mapping as b
                on bd.id = b.org_id1 and bd.feattyp = b.org_id2
                join temp_category_priority as p
                on p.per_code = b.per_code 
                                    
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_mn_logmark_id_idx')
        
        
        self.CreateTable2('temp_poi_logmark')
        sqlcmd = '''
            insert into temp_poi_logmark
            select id, feattyp, per_code, category_priority, the_geom
            from temp_mnpoi_logmark
            
            union
            
            select id, feattyp, per_code, category_priority, the_geom
            from temp_mn_logmark
                       
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_poi_logmark_id_idx')
        
        
    def _makePOIName(self):
        self.log.info('make temp_poi_name...')
        self.CreateTable2('temp_poi_name')
        
        tableList = ['org_poi_pt','org_poi_ne','org_poi_nefa','org_poi_foa']
        for table in tableList:
            if not self.pg.IsExistTable(table):
                self.CreateTable2(table)
                
                
        self.CreateIndex2('org_poi_pt_ptid_idx')
        
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
        
        sqlcmd = '''
                drop table if exists temp_poi_name_all_language;
                create table temp_poi_name_all_language AS 
                (
                    select  pd.id,  
                            pd.langcode as language_code,
                            pd.name,
                            pd.nametype as nametyp,
                            COALESCE(pt.lanphonset, null) as phonetic_language_code, 
                            COALESCE(pt.transcription,null) as phonetic_string 
                    from
                    (
                        SELECT  e.id, f.ptid, f.langcode, f.name, e.nametype
                        FROM
                        (
                            SELECT  d.nameitemid, c.featdsetid, c.featsectid, 
                                    c.featlayerid,  c.id, d.nametype
                            FROM                  
                            (
                                select  b.featdsetid, b.featsectid, b.featlayerid, 
                                        b.featitemid, b.featcat, b.featclass,  
                                        a.id
                                from temp_mn_logmark as a
                                left join org_vm_foa as b
                                ON a.id = b.shapeid
                                    
                            ) AS c
                            left join org_vm_nefa d
                            ON  c.featdsetid = d.featdsetid and
                                c.featsectid = d.featsectid and
                                c.featlayerid = d.featlayerid and
                                c.featitemid = d.featitemid and
                                c.featcat = d.featcat and
                                c.featclass = d.featclass
                            --where d.nametype = 'ON'
                            where d.nametype in ('ON', 'AN', 'BN', '1Q', '8Y', 'AI')
                        ) AS e
                        left join org_vm_ne AS f
                        ON  e.nameitemid = f.nameitemid and 
                            e.featdsetid = f.namedsetid and
                            e.featsectid = f.namesectid and
                            e.featlayerid = f.namelayerid
                    ) AS pd
                    left join org_vm_pt AS pt
                    ON pd.ptid = pt.ptid and pt.preference = 1
                    
                    UNION

                    select  pd.id,  
                            pd.langcode as language_code,
                            pd.name,
                            pd.nametype as nametyp,
                            COALESCE(pt.lanphonset, null) as phonetic_language_code, 
                            COALESCE(pt.transcription,null) as phonetic_string                     
                    from
                    (
                        SELECT  e.id, f.ptid, f.langcode, f.name, e.nametype
                        FROM
                        (
                            SELECT  d.nameitemid, c.featdsetid, c.featsectid, 
                                    c.featlayerid,  c.id, d.nametype
                            FROM                  
                            (
                                select  b.featdsetid, b.featsectid, b.featlayerid, 
                                        b.featitemid, b.featcat, b.featclass,  
                                        a.id
                                from temp_mnpoi_logmark as a
                                left join org_poi_foa as b
                                ON a.id = b.shapeid            
                            ) AS c
                            left join org_poi_nefa d
                            ON  c.featdsetid = d.featdsetid and
                                c.featsectid = d.featsectid and
                                c.featlayerid = d.featlayerid and
                                c.featitemid = d.featitemid and
                                c.featcat = d.featcat and
                                c.featclass = d.featclass
                            --where d.nametype = 'ON'
                            where d.nametype in ('ON', 'AN', 'BN', '1Q', '8Y', 'AI')
                        ) AS e
                        left join org_poi_ne AS f
                        ON  e.nameitemid = f.nameitemid and 
                            e.featdsetid = f.namedsetid and
                            e.featsectid = f.namesectid and
                            e.featlayerid = f.namelayerid
                    ) AS pd
                    left join org_poi_pt AS pt
                    ON pd.ptid = pt.ptid and pt.preference = 1
                   
                    UNION 
                    
                    select  a.id, 
                            b.namelc as language_code,
                            b.name,
                            b.nametyp, 
                            null as phonetic_language_code, 
                            null as phonetic_string 
                    from temp_mn_logmark as a
                    left join org_mn_pinm as b
                    ON a.id = b.id
                    --where b.nametyp = 'ON' 
                    where b.nametyp in ('ON', 'AN', 'BN', '1Q', '8Y', 'AI')
                    
                    UNION 
                    
                     select  a.id, 
                            b.namelc as language_code,
                            b.name,
                            b.nametyp, 
                            null as phonetic_language_code, 
                            null as phonetic_string 
                    from temp_mnpoi_logmark as a
                    left join org_mnpoi_pinm as b
                    ON a.id = b.id 
                    --where b.nametyp = 'ON'
                    where b.nametyp in ('ON', 'AN', 'BN', '1Q', '8Y', 'AI')
                )          
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = ''' 
            select  id,
                    array_agg(1::integer) as name_id_array,
                    array_agg('office_name'::varchar) as name_type_array,
                    array_agg(language_code) as language_code_array,
                    array_agg(name) as name_array,
                    array_agg(phonetic_language_code) as phonetic_language_code_array,
                    array_agg(phonetic_string) as phonetic_string_array
            from
            (                 
                
                select  id, language_code, phonetic_language_code,
                        (array_agg(name))[1] as name,
                        (array_agg(phonetic_string))[1] as phonetic_string
                from
                (                       
                    select  id,
                            (case when language_code in ('UND') then 'ENG' else language_code end) as language_code,
                            name,
                            (case when nametyp = 'ON' then 1
                                  when nametyp = 'AN' then 2
                                  when nametyp = 'BN' then 3
                                  when nametyp = '1Q' then 4
                                  when nametyp = '8Y' then 5
                                  when nametyp = 'AI' then 6
                                  else 7 end ) as nametype,
                            (case when phonetic_language_code in ('UND') then 'ENG' 
                                  else phonetic_language_code end) as phonetic_language_code,
                            phonetic_string
                    from temp_poi_name_all_language
                    order by id, language_code, nametype,name, phonetic_language_code, phonetic_string   
                ) as a
                group by id, language_code, phonetic_language_code
            ) as b
            group by id
            order by id
        '''
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
        
     
    def _makeLogmark(self):
        self.log.info('make mid_logmark...')
        self.CreateTable2('mid_logmark')
        
        sqlcmd = """
                insert into mid_logmark(poi_id, type_code, type_code_priority, building_name, the_geom)
                select  a.id, 
                        a.per_code,
                        a.category_priority,
                        b.poi_name,
                        a.the_geom
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.id = b.poi_id
          
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
        