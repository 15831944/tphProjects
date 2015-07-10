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

class comp_guideinfo_building_ni(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_building')

    def _Do(self):
#        self.create_language()
        self._loadBrandIcon()
        self._loadPOICategory()
        self._findLogmark()
        self._makePOIName()
        self._makeLogmark()
        return 0

    
    def _loadBrandIcon(self):                            
        self.log.info('make temp_brand_icon...')
        self.CreateTable2('temp_brand_icon')
        brand_icon_file_path = common.config.CConfig.instance().getPara('brand_icon')
        for line in open(brand_icon_file_path):
                
                if line[0] == '#':
                    continue
                line = line.strip()
                fields=[]
                fields.append(line)               
                sqlcmd = '''
                          insert into temp_brand_icon values(%s);
                         '''
                self.pg.execute(sqlcmd, fields)
        self.pg.commit()
                                      
        
    def _loadPOICategory(self):
        self.log.info('make temp_poi_category...')
        self.CreateTable2('temp_poi_category')
        
        category_file_path = common.config.CConfig.instance().getPara('POI_Code')
        for line in open(category_file_path):
            line = line.strip()
            if line[0] == '#':
                continue
            fields = line.split(';')
#            print fields
            sqlcmd = '''
                      insert into temp_poi_category values(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s);
                     '''
            self.pg.execute(sqlcmd, fields)
        self.pg.commit()

        
    def _findLogmark(self):       
        # find poi with logmark
        self.log.info('make temp_poi_logmark...')
        self.CreateTable2('temp_poi_logmark')
        
        sqlcmd = """
                insert into temp_poi_logmark
                select a.poi_id, a.kind,tpc.u_code,a.display_x, a.display_y,a.the_geom
                from org_poi as a
                inner join temp_brand_icon as b
                on a.chaincode = b.brandname
                inner join temp_poi_category as tpc
                on tpc.org_code=a.kind
                order by poi_id::bigint
                
                --union
                
               -- select a.poi_id, a.kind,tpc.u_code,a.display_x, a.display_y,a.the_geom
               -- from org_poi as a                
                --inner join temp_poi_category as tpc
                --on tpc.logmark = 'Y' and a.kind=tpc.org_code ;
                                                
                """

        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_poi_logmark_poi_id_idx')



    def create_language(self):
        sqlcmd = """
                CREATE TABLE language_tbl
                (
                  language_id smallint NOT NULL,
                  l_full_name character varying(40) NOT NULL,
                  l_talbe character varying(80) NOT NULL,
                  pronunciation character varying(40),
                  p_table character varying(80),
                  language_code character varying(3) NOT NULL,
                  language_code_client character varying(3) NOT NULL,
                  language_id_client smallint NOT NULL,
                  exist_flag boolean DEFAULT false,
                  CONSTRAINT language_tbl_pkey PRIMARY KEY (language_id)
                )
                WITH (
                  OIDS=FALSE
                );
                """
                
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
   
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
            select *
                 from( 
                     select  a.poi_id,
                             b.name as poi_name,
                             --b.language as language,
                             (case when b.language = '1'  then  'CHI'
                                   when b.language = '3'  then  'ENG' end) as language,
                             b.language as name_id,  
                             a.the_geom,
                             c.name as phoneme
                    from temp_poi_logmark as a        
                    join org_pname as b
                    on b.featid = a.poi_id and b.nametype = '9' and b.seq_nm = '1'
                    left join org_pname_phon as c
                    on c.featid= a.poi_id and b.language = '1' and c.phontype='1' and c.seq_nm = '1'
                        
                    union
 
                    select  a.poi_id,
                            c.name as poi_name,
                            'CTN' as language,
                            '2' as name_id,  
                            a.the_geom,
                            b.name as phoneme
                    from temp_poi_logmark as a        
                    join org_pname_phon as b
                    on b.featid= a.poi_id and b.phontype='3' and b.seq_nm = '1' 
                    join org_pname as c
                    on c.featid = a.poi_id and c.nametype = '9' and c.language='1'and c.seq_nm = '1'                
                    ) as k
                   order by poi_id::bigint,name_id                
                );     
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        
        sqlcmd = """
                select  poi_id,
                    array_agg(name_id) as name_id_array,
                    array_agg(language) as language_code_array,
                    array_agg(poi_name) as name_array,
                    array_agg(phoneme) as phonetic_string_array
                from  temp_poi_name_all_language
                group by poi_id
                order by poi_id::bigint;
                """         
               
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('poi_name')
        for asso_rec in asso_recs:
            poi_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string(asso_rec[1], 
                                                                                                 asso_rec[2], 
                                                                                                 asso_rec[3], 
                                                                                                 asso_rec[4])
            temp_file_obj.write('%d\t%s\n' % (int(poi_id), json_name))
        
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
                insert into mid_logmark(poi_id, type_code, building_name, the_geom)
                select  a.poi_id::bigint, 
                        a.per_code,
                        b.poi_name,
                        a.the_geom
                from temp_poi_logmark as a
                left join temp_poi_name as b
                on a.poi_id = b.poi_id
                order by a.poi_id::bigint;      
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_logmark_the_geom_idx')
        

        