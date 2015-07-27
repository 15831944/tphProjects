# -*- coding: UTF-8 -*-
'''
Created on 2015-3-26

@author: wusheng
'''
import os
import codecs

import common.cache_file
import component.component_base
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
        self._loadPOICategory()
        self._loadCategoryPriority()
        
        self._findLogmark()
        self._makePOIName()
        self._makeLogmark()
        return 0


    
    # find poi with logmark
    def _findLogmark(self):
        self.log.info('make temp_poi_logmark...')
        
        self.CreateTable2('temp_poi_logmark1')
        sqlcmd = """ 
              insert into temp_poi_logmark1
              select omp.id, omp.feattyp, omp.subcat, omp.brandname, tpc.ucode, 
                omp.the_geom, tpc.org_code
                from org_mnpoi_pi omp
                inner join temp_poi_category as tpc
                on omp.feattyp = tpc.org_code
                join temp_brand_icon as b
                on b.brandname = omp.brandname 

              union

              select omp.id, omp.feattyp, omp.subcat, omp.brandname, tpc.ucode, 
                omp.the_geom, tpc.org_code
                from org_mnpoi_pi omp
                inner join temp_poi_category as tpc
                on omp.feattyp = tpc.org_code
                join temp_category_priority as p
                on p.u_code::bigint = tpc.ucode and p.category_priority in (1,2,4)
        """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_poi_logmark1_id_idx')
         
        self.CreateTable2('temp_poi_logmark2')                
        sqlcmd = """ 
                insert into temp_poi_logmark2                
                select mp.id, mp.feattyp, mp.subcat, mp.buaname, tpc.ucode, 
                mp.the_geom, tpc.org_code
                from org_mn_pi mp
                inner join temp_poi_category as tpc
                on mp.feattyp = tpc.org_code
                join temp_brand_icon as b
                on b.brandname = mp.buaname 

             union

                select mp.id, mp.feattyp, mp.subcat, mp.buaname, tpc.ucode, 
                mp.the_geom, tpc.org_code
                from org_mn_pi mp
                inner join temp_poi_category as tpc
                on mp.feattyp = tpc.org_code
                join temp_category_priority as p
                on p.u_code::bigint = tpc.ucode and p.category_priority in (1,2,4)        
                                     
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_poi_logmark2_id_idx')
        
        
        self.CreateTable2('temp_poi_logmark')
        sqlcmd = '''
            insert into temp_poi_logmark
            select id, feattyp, subcat, brandname, ucode, the_geom, org_code 
            from temp_poi_logmark1
            
            union
            
            select id, feattyp, subcat, brandname, ucode, the_geom, org_code  
            from temp_poi_logmark2
                       
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
                            COALESCE(pt.lanphonset, null) as phonetic_language_code, 
                            COALESCE(pt.transcription,null) as phonetic_string 
                    from
                    (
                        SELECT  e.id, f.ptid, f.langcode, f.name
                        FROM
                        (
                            SELECT  d.nameitemid, c.featdsetid, c.featsectid, 
                                    c.featlayerid,  c.id
                            FROM                  
                            (
                                select  b.featdsetid, b.featsectid, b.featlayerid, 
                                        b.featitemid, b.featcat, b.featclass,  
                                        a.id
                                from temp_poi_logmark2 as a
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
                            COALESCE(pt.lanphonset, null) as phonetic_language_code, 
                            COALESCE(pt.transcription,null) as phonetic_string                     
                    from
                    (
                        SELECT e.ucode, e.the_geom, e.id, f.ptid, f.langcode, f.name
                        FROM
                        (
                            SELECT  d.nameitemid, c.featdsetid, c.featsectid, 
                                    c.featlayerid, c.ucode, c.the_geom, c.id
                            FROM                  
                            (
                                select  b.featdsetid, b.featsectid, b.featlayerid, 
                                        b.featitemid, b.featcat, b.featclass,  
                                        a.ucode, a.the_geom, a.id
                                from temp_poi_logmark1 as a
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
                            null as phonetic_language_code, 
                            null as phonetic_string 
                    from temp_poi_logmark2 as a
                    left join org_mn_pinm as b
                    ON a.id = b.id
                    --where b.nametyp = 'ON' 
                    where b.nametyp in ('ON', 'AN', 'BN', '1Q', '8Y', 'AI')
                    
                    UNION 
                    
                     select  a.id, 
                            b.namelc as language_code,
                            b.name, 
                            null as phonetic_language_code, 
                            null as phonetic_string 
                    from temp_poi_logmark1 as a
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
                    select  id,
                            (case when language_code in ('UND') then 'ENG' else language_code end) as language_code,
                            name,
                            (case when phonetic_language_code in ('UND') then 'ENG' else phonetic_language_code end) as phonetic_language_code,
                            phonetic_string
                    from temp_poi_name_all_language
                    --where namelc <> 'ENU'
                    order by id, language_code, name, phonetic_language_code, phonetic_string
                )as a
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
                        a.ucode,
                        c.category_priority,
                        b.poi_name,
                        a.the_geom
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.id = b.poi_id
                join temp_category_priority as c
                on c.u_code::bigint = a.ucode    
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
        