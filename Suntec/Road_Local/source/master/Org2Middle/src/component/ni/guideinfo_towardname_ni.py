# -*- coding: UTF8 -*-
#!/usr/bin/python
'''
Created on 2015-5-27

@author: wushengbing
'''

import component.default.multi_lang_name
import common.cache_file
import component.component_base

class comp_guideinfo_towardname_ni(component.component_base.comp_base):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self,'Guideinfo_TowardName')
        
        
    def _Do(self):
        
        self._make_poi_inlink()
        self._make_towardname_name()
        self._make_towardname()
        
        return 0
    
    
    def _make_poi_inlink(self):
        
        self.log.info('make temp_poi_inlink...')
        self.CreateTable2('temp_poi_inlink')
        self.CreateFunction2('ni_mid_search_poi_inlink')
        
        sqlcmd = '''
                drop table if exists temp_poi_find;
                create table temp_poi_find
                as
                (
                    select poi_id, kind, linkid, null as node_id,the_geom from org_poi 
                    where kind in ('8301','8380','8381')
                    
                union
                    
                    SELECT poi_id, a.kind, linkid, c.node_id,a.the_geom
                    FROM org_poi as a
                    left join link_tbl as b 
                    on b.link_id = a.linkid::bigint
                    left join node_tbl as c
                    on c.toll_flag = 1 and c.node_id in (b.s_node,b.e_node)
                    where b.link_type in (1,2) and b.road_type in (0,1)
                          and a.kind in ('8401')                      
                )               
               '''
        
        self.pg.execute(sqlcmd)
        self.pg.commit2()       
        self.CreateIndex2('temp_poi_find_poi_id_idx')        

  
        sqlcmd = '''
                select ni_mid_search_poi_inlink(a.poi_id,a.linkid) from temp_poi_find as a
                where a.kind in ('8301','8380','8381') 
               '''
        
        self.pg.execute(sqlcmd)
        self.pg.commit2()       
       
        ##查找收费站的inlink、node
        sqlcmd = '''
                insert into temp_poi_inlink(poi_id, inlink, node)
                (
                    select a.poi_id::bigint,
                            b.link_id,
                            a.node_id
                    from temp_poi_find as a
                    join link_tbl as b
                    on ( (a.node_id = b.e_node and b.one_way_code = 2
                         or
                         a.node_id = b.s_node and b.one_way_code = 3
                         )
                         --and  b.link_type in (1,2) 
                         and b.road_type in (0,1)
                       )
                    where a.kind = '8401'
                )
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()       
        self.CreateIndex2('temp_poi_inlink_poi_id_idx')          
   
   
        
    def _make_towardname_name(self):
        
        self.log.info('make temp_towardname_name...')
        self.CreateTable2('temp_towardname_name')
        
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
                          
        sqlcmd = '''
                drop table if exists temp_towardname_name_all_language;
                create table temp_towardname_name_all_language
                as
              (
                     select  a.poi_id,
                             b.name as poi_name,
                             (case when b.language = '1'  then  'CHI'
                                   when b.language = '2'  then  'CHT'
                                   when b.language = '3'  then  'ENG'
                                   when b.language = '4'  then  'POR' end) as language,
                             b.language as name_id,                           
                            'office_name'::varchar as name_type,                             
                             c.phontype as phoneme_lang,                                                          
                             c.name as phoneme,
                             a.the_geom
                    from temp_poi_find as a    
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
                '''       
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
             
        sqlcmd = '''
                select  poi_id,
                    array_agg(name_id) as name_id_array,
                    array_agg(name_type) as name_type_array,
                    array_agg(language) as language_code_array,
                    array_agg(poi_name) as name_array,
                    array_agg(phoneme_lang) as phonetic_language_array,
                    array_agg(phoneme) as phoneme_array
                from 
                    (
                        select * from temp_towardname_name_all_language
                        order by poi_id::bigint, name_id::bigint, phoneme_lang                                   
                    ) as a
                group by poi_id
                order by poi_id::bigint;
                '''                  
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('towardname_name')
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
        self.pg.copy_from2(temp_file_obj, 'temp_towardname_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_towardname_name_poi_id_idx') 
 
        
    def _make_towardname(self):
 
 
        self.log.info('make towardname_tbl...')
        self.CreateTable2('towardname_tbl')
        
        sqlcmd = '''
                insert into towardname_tbl( id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, guideattr, namekind, toward_name )                
                (
                select    row_number() over(order by nodeid,inlinkid,namekind,guideattr,toward_name ) as id,                                            
                          d.nodeid,
                          d.inlinkid,
                          null as outlinkid,
                          '' as passlid,
                          0 as passlink_cnt,
                          0 as direction, 
                          guideattr,
                          namekind,
                          d.toward_name
                from 
                (
                    select    distinct 
                              a.node as nodeid, 
                              a.inlink as inlinkid,
                              (case when c.kind = '8301' then 1
                                    when c.kind = '8380' then 4
                                    when c.kind = '8381' then 5 
                                    when c.kind = '8401' then 7 end ) as guideattr,
                              2 as namekind,
                              b.toward_name as toward_name           
                    from  temp_poi_inlink as a
                    left join temp_towardname_name as b
                    on a.poi_id = b.poi_id
                    left join temp_poi_find as c
                    on c.poi_id::bigint = a.poi_id
                ) as d
                )
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('towardname_tbl_nodeid_idx') 
 
 
 
 