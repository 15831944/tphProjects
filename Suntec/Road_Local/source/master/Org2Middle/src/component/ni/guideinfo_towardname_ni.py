# -*- coding: UTF8 -*-
'''
Created on 2015-7-31

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
        
        self._make_poi_towardname()
        self._make_signpost_uc_towardname()
        self._make_towardname_tbl()
     


    def _make_towardname_tbl(self):
        
        self.log.info('make towardname_tbl...')
        self.CreateTable2('towardname_tbl')
        
        sqlcmd = '''
            insert into towardname_tbl(id,nodeid,inlinkid,outlinkid,passlid,passlink_cnt,direction,
                                            guideattr,namekind,toward_name)
            select  gid, 
                    nodeid, 
                    inlinkid, 
                    outlinkid, 
                    passlid, 
                    passlink_cnt, 
                    direction, 
                    guideattr, 
                    namekind, 
                    toward_name
            from temp_towardname_tbl
            order by gid,nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, 
                                            guideattr, namekind, toward_name
            '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        self.CreateIndex2('towardname_tbl_node_id_idx')
        
        
    def _make_signpost_uc_towardname(self):
        
        self.log.info('make signpost_uc_towardname...')
        self._find_not_roundabout_path_id()
        self._make_temp_signpost_uc()
        self._make_temp_towardname_signpost_uc()
        
    

    def _find_not_roundabout_path_id(self):
        
        sqlcmd = '''
            drop table if exists temp_not_roundabout_path_id;
            create table temp_not_roundabout_path_id
            as
            (
                SELECT path_id
                FROM temp_signpost_uc_path as a
                join 
                (
                    select k.*  
                    from 
                    (
                        SELECT id, nodeid, inlinkid, outlinkid, b.name
                        FROM org_ic as a
                        join org_fname as b
                        on a.id = b.featid and nametype = '3' and b.signnumflg = '0'
                        
                        union 
                        
                        SELECT id, nodeid, inlinkid, outlinkid, b.name
                        FROM org_dr as a
                        join org_fname as b
                        on a.id = b.featid and nametype = '4' and b.signnumflg = '0'
                    
                    ) as k
                    where k.name not like '%环岛'
                ) as d
                using(nodeid,inlinkid,outlinkid)
                order by path_id, nodeid, inlinkid, outlinkid, passlid, id_array
            )
        '''
     
        self.pg.execute2(sqlcmd)
        self.pg.commit2()       
        self.CreateIndex2('temp_not_roundabout_path_id_path_id_idx')
 
 
    def _make_temp_signpost_uc(self):
        sqlcmd = '''
        drop table if exists temp_signpost_uc;
        create table temp_signpost_uc
        as 
        (
            select sign_id, 
                   (case when a.s_node in (b.s_node, b.e_node) then a.s_node
                         when a.e_node in (b.s_node, b.e_node) then a.e_node
                         else -1::bigint end) as nodeid,
                    inlinkid, outlinkid, passlid, passlink_cnt, signpost_name, route_no1,
                    route_no2, route_no3, route_no4, exit_no
            from
            (
                SELECT a.sign_id, inlinkid::bigint, outlinkid::bigint, 
                       (case when passlid is null or length(passlid) < 1 then null 
                             else passlid end) as passlid, 
                       (case when passlid is null or length(passlid) < 1 then 0
                             else array_upper(string_to_array(passlid,'|'), 1) end ) as passlink_cnt,
                       (case when passlid is null or length(passlid) < 1 then outlinkid 
                             else (string_to_array(passlid,'|'))[1] end)::bigint as second_link,
                       signpost_name, route_no1, route_no2,route_no3, route_no4, exit_no
                FROM temp_signpost_uc_name AS a
                LEFT JOIN temp_signpost_uc_path as b
                ON a.sign_id = b.path_id
            )temp
            left join link_tbl as a
            on temp.inlinkid = a.link_id
            left join link_tbl as b
            on temp.second_link = b.link_id
            order by sign_id
        )       
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()       
        self.CreateIndex2('temp_signpost_uc_sign_id_idx')
        

    def _make_temp_towardname_signpost_uc(self):
        
        self.log.info('make temp_towardname_signpost_uc...')
        sqlcmd = '''
            insert into temp_towardname_tbl( nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, 
                                            guideattr, namekind, toward_name ) 
            (               
                select nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                       0 as direction,
                       0 as guideattr,
                       3 as namekind,
                       route_no1
                from temp_signpost_uc
                where route_no1 is not null
                
                union
                
                select nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                       0 as direction,
                       0 as guideattr,
                       3 as namekind,
                       route_no2
                from temp_signpost_uc
                where route_no2 is not null
                
                union
                
                select nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                       0 as direction,
                       0 as guideattr,
                       3 as namekind,                
                       route_no3
                from temp_signpost_uc
                where route_no3 is not null
                
                union
                
                select nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                       0 as direction,
                       0 as guideattr,
                       3 as namekind,
                       route_no4
                from temp_signpost_uc            
                where route_no4 is not null
                
                union
                
                select nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                       0 as direction,
                       0 as guideattr,
                       1 as namekind,                
                       exit_no
                from temp_signpost_uc
                where exit_no is not null 
                
                union
                
                select a.nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                       0 as direction,
                       0 as guideattr,
                       0 as namekind,                
                       signpost_name
                from temp_signpost_uc as a
                join temp_not_roundabout_path_id as b
                on a.sign_id = b.path_id
                
                order by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, guideattr ,namekind
            )
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()

        
        
        

    def _make_poi_towardname(self):
        
        self._make_poi_inlink()
        self._make_towardname_name_poi()
        self._make_temp_towardname_poi()
        
        
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
                         and b.road_type in (0,1)
                       )
                    where a.kind = '8401'
                )
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()       
        self.CreateIndex2('temp_poi_inlink_poi_id_idx')          
   
   
        
    def _make_towardname_name_poi(self):
        
        self.log.info('make temp_towardname_name_poi...')
        self.CreateTable2('temp_towardname_name_poi')
        
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
                          
        sqlcmd = '''
                drop table if exists temp_towardname_name_all_language_poi;
                create table temp_towardname_name_all_language_poi
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
                        select * from temp_towardname_name_all_language_poi
                        order by poi_id::bigint, name_id::bigint, phoneme_lang                                   
                    ) as a
                group by poi_id
                order by poi_id::bigint;
                '''                  
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('towardname_name_poi')
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
        self.pg.copy_from2(temp_file_obj, 'temp_towardname_name_poi')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_towardname_name_poi_poi_id_idx') 
 
        
    def _make_temp_towardname_poi(self):
 
 
        self.log.info('make temp_towardname_poi_tbl...')
        self.CreateTable2('temp_towardname_tbl')
        
        sqlcmd = '''
                insert into temp_towardname_tbl( nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, guideattr, namekind, toward_name )                
                (
                select    d.nodeid,
                          d.inlinkid,
                          null::bigint as outlinkid,
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
                    left join temp_towardname_name_poi as b
                    on a.poi_id = b.poi_id
                    left join temp_poi_find as c
                    on c.poi_id::bigint = a.poi_id
                ) as d
                order by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, guideattr ,namekind, toward_name
                )
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()

 
 