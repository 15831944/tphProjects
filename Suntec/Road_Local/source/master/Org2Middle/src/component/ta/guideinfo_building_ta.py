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
        self._loadCategoryPriority()
        self._loadPOICategory()
        self._findLogmark()
        self._makePOIName()
        self._makeLogmark()
        return 0
    
    # find poi with logmark
    def _findLogmark(self):
        self.log.info('make temp_poi_logmark...')
        self.CreateTable2('temp_poi_logmark')
        if self.CreateFunction2('cat_condition_judge') == -1:
            return -1
        
        sqlcmd = """
                insert into temp_poi_logmark
                select omp.id, omp.feattyp, omp.subcat, omp.brandname, tpc.ucode, 
                omp.the_geom, tpc.org_code, tpc.condition, 
                cat_condition_judge(tpc.condition, omp.subcat, omp.brandname)
                from org_mnpoi_pi omp
                inner join temp_poi_category as tpc
                on omp.feattyp = tpc.org_code and 
                   cat_condition_judge(tpc.condition, omp.subcat, omp.brandname) and
                   tpc.logmark <> ''
                   
                UNION
                
                select mp.id, mp.feattyp, mp.subcat, mp.buaname, tpc.ucode, 
                mp.the_geom, tpc.org_code, tpc.condition, 
                cat_condition_judge(tpc.condition, mp.subcat, mp.buaname)
                from org_mn_pi mp
                inner join temp_poi_category as tpc
                on mp.feattyp = tpc.org_code and 
                   cat_condition_judge(tpc.condition, mp.subcat, mp.buaname) and
                   tpc.logmark <> ''                                  
                """

        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('temp_poi_logmark_id_idx')
        
    def _makePOIName(self):
        self.log.info('make temp_poi_name...')
        self.CreateTable2('temp_poi_name')
        self.CreateIndex2('org_poi_pt_ptid_idx')
        
        # init language code for MultiLangName
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
        
        sqlcmd = '''
                drop table if exists temp_poi_name_all_language;
                create table temp_poi_name_all_language AS 
                (
                    select  pd.id, 
                            pd.ucode, 
                            pd.the_geom, 
                            COALESCE(pt.lanphonset, '') as namelc, 
                            pd.name, 
                            COALESCE(pt.transcription,'') as transcription 
                    from
                    (
                        SELECT e.ucode, e.the_geom, e.id, f.ptid, f.name
                        FROM
                        (
                            SELECT  d.nameitemid, c.featdsetid, c.featsectid, 
                                    c.featlayerid, c.ucode, c.the_geom, c.id
                            FROM                  
                            (
                                select  b.featdsetid, b.featsectid, b.featlayerid, 
                                        b.featitemid, b.featcat, b.featclass,  
                                        a.ucode, a.the_geom, a.id
                                from temp_poi_logmark as a
                                left join org_vm_foa as b
                                ON a.id = b.shapeid
                                
                                UNION
                                
                                select  b.featdsetid, b.featsectid, b.featlayerid, 
                                b.featitemid, b.featcat, b.featclass,  
                                a.ucode, a.the_geom, a.id
                                from temp_poi_logmark as a
                                left join org_poi_foa as b
                                ON a.id = b.shapeid
                                
                            ) AS c
                            left join org_vm_nefa d
                            ON  c.featdsetid = d.featdsetid and
                                c.featsectid = d.featsectid and
                                c.featlayerid = d.featlayerid and
                                c.featitemid = d.featitemid and
                                c.featcat = d.featcat and
                                c.featclass = d.featclass
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
                    pd.ucode, 
                    pd.the_geom, 
                    COALESCE(pt.lanphonset, '') as namelc, 
                    pd.name, 
                    COALESCE(pt.transcription,'') as transcription 
                    from
                    (
                    SELECT e.ucode, e.the_geom, e.id, f.ptid, f.name
                    FROM
                    (
                        SELECT  d.nameitemid, c.featdsetid, c.featsectid, 
                            c.featlayerid, c.ucode, c.the_geom, c.id
                        FROM                  
                        (
                        select  b.featdsetid, b.featsectid, b.featlayerid, 
                            b.featitemid, b.featcat, b.featclass,  
                            a.ucode, a.the_geom, a.id
                        from temp_poi_logmark as a
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
                    
                    select a.id, a.ucode, a.the_geom, b.namelc, b.name, '' as transcription
                    from temp_poi_logmark as a
                    left join org_mn_pinm as b
                    ON a.id = b.id 
                    where b.nametyp in ('ON', 'AN', 'BN', '1Q', '8Y', 'AI')
                    
                    UNION 
                    
                    select a.id, a.ucode, a.the_geom, b.namelc, b.name, '' as transcription
                    from temp_poi_logmark as a
                    left join org_mnpoi_pinm as b
                    ON a.id = b.id 
                    where b.nametyp in ('ON', 'AN', 'BN', '1Q', '8Y', 'AI')
                )          
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = '''
                select  id,
                        array_agg(name_id),
                        array_agg(namelc), 
                        array_agg(name), 
                        array_agg(transcription)
                from 
                (
                    select  id, 
                            1 as name_id, 
                            namelc,
                            (array_agg(name))[1] as name, 
                            (array_agg(transcription))[1] as transcription
                    from 
                    (
                        select  id,
                                (case when namelc = 'UND' then 'ENG' else namelc end) as namelc,
                                transcription,
                                name
                        from temp_poi_name_all_language
                        where namelc <> 'ENU'
                        order by id, namelc, transcription desc
                    )as a
                    group by id, namelc
                ) as b
                group by id
        '''
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
        
    def _loadPOICategory(self):
        self.log.info('make temp_poi_category...')
        self.CreateTable2('temp_poi_category')
        
        category_file_path = common.config.CConfig.instance().getPara('POI_Code')
        for line in open(category_file_path):
            line = line.strip()
            if line[0] == '#':
                continue
            line = line.replace('"', '')
            fields = line.split(';')
            sqlcmd = '''
                      insert into temp_poi_category values(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s);
                     '''
            self.pg.execute(sqlcmd, fields)
        self.pg.commit()
     
    def _makeLogmark(self):
        self.log.info('make mid_logmark...')
        self.CreateTable2('mid_logmark')
        
        sqlcmd = """
                insert into mid_logmark(poi_id, type_code, type_code_priority, building_name, the_geom)
                select  a.id, 
                        a.ucode,
                        (case when c.category_priority is null then d.category_priority 
                                else c.category_priority end),
                        b.poi_name,
                        a.the_geom
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.id = b.poi_id
                left join temp_category_priority as c
                on c.u_code = a.ucode::character
                left join temp_category_priority as d
                on d.u_code = 'other';      
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
        