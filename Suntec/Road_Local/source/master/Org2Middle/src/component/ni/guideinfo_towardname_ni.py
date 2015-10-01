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
        
        self._make_highway_building_name()
        self._make_signpost_uc_name()
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
        
        
    def _make_signpost_uc_name(self):
        
        self.log.info('make signpost_uc_name...')
        self._make_temp_signpost_uc_path_info()
        self._find_roundabout_id()
        self._make_temp_sp_json_name()
        self._make_temp_towardname_signpost_uc()
    
    
        
    def _make_temp_signpost_uc_path_info(self):
        
        sqlcmd = '''
            drop table if exists temp_signpost_uc_path_info;
            create table temp_signpost_uc_path_info
            as
            (
                select distinct b.*,
                       (case when c.s_node in (d.s_node, d.e_node) then c.s_node
                             when c.e_node in (d.s_node, d.e_node) then c.e_node
                             else -1::bigint end) as tnodeid
                from
                (
                    SELECT a.id::bigint, a.nodeid, inlinkid::bigint, outlinkid::bigint, passlid,type,
                    
                           (case when passlid is null or length(passlid) < 1 then 0
                                 else array_upper(string_to_array(passlid,'|'), 1) end ) as passlink_cnt,
                                 
                           (case when passlid is null or length(passlid) < 1 then outlinkid 
                                 else (string_to_array(passlid,'|'))[1] end)::bigint as second_link         
                    FROM
                    (
                        select id, nodeid, inlinkid, outlinkid, 
                               (case when passlid2 is null or length(passlid2) < 1 then passlid
                                     else passlid ||'|'||passlid2 end) as passlid, 2 as type
                        from org_br
                        
                        union
                        
                        select id, nodeid, inlinkid, outlinkid, passlid, 3 as type
                        from org_ic
                        
                        union
                        
                        select id, nodeid, inlinkid, outlinkid, 
                               (case when passlid2 is null or length(passlid2) < 1 then passlid
                                     else passlid ||'|'||passlid2 end) as passlid, 4 as type
                        from org_dr
                    ) as a
                ) b
                left join link_tbl as c
                on b.inlinkid = c.link_id
                left join link_tbl as d
                on b.second_link = d.link_id
                order by id    
            )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()       
        self.CreateIndex2('temp_signpost_uc_path_info_id_idx')
        
       
        
    def _find_roundabout_id(self):
        
        sqlcmd = '''
            drop table if exists temp_roundabout_id;
            create table temp_roundabout_id
            as
            (  
                select distinct sign_id as id, name 
                from
                (
                    SELECT a.id as sign_id, nodeid, inlinkid, outlinkid, passlid, "type", passlink_cnt, 
                            second_link, tnodeid, r.id, rn.pathname, l.link_type, c.name
                    FROM temp_signpost_uc_path_info as a
                    left join org_r as r
                    on a.tnodeid::varchar in (r.snodeid,r.enodeid)
                    left join org_r_lname as rl
                    on rl.id = r.id
                    left join org_r_name as rn
                    on rn.route_id = rl.route_id
                    left join link_tbl as l
                    on l.link_id = r.id::bigint
                    left join
                    (
                        select * from
                        (
                            SELECT  a.id, b.name
                            FROM org_ic as a
                            join org_fname as b
                            on a.id = b.featid and nametype = '3' and b.signnumflg = '0'
                            
                            union 
                            
                            SELECT  a.id, b.name
                            FROM org_dr as a
                            join org_fname as b
                            on a.id = b.featid and nametype = '4' and b.signnumflg = '0'
                        ) cc
                        order by cc.id
                    ) as c
                    on c.id::bigint = a.id
                    
                ) as d
                where link_type in (0,10,11) and pathname = name  and type in (3,4)
                order by sign_id
            )
        '''
     
        self.pg.execute2(sqlcmd)
        self.pg.commit2()       
        self.CreateIndex2('temp_roundabout_id_id_name_idx')
 
    def _make_temp_sp_json_name(self):
#        self.log.info('make temp_poi_name...')
        self.CreateTable2('temp_sp_json_name')
        self.CreateFunction2('mid_cnv_shield_ni')
        self.CreateFunction2('mid_convertstring')
        sqlcmd = '''
            drop table if exists temp_sp_json_name_all_language;
            create table temp_sp_json_name_all_language
            as
            (
                select id, name_kind,nameflag,seq_nm, 
                        (case when c.language = '1'  then  'CHI'
                              when c.language = '2'  then  'CHT'                                                               
                              when c.language = '3'  then  'ENG' 
                              when c.language = '4'  then  'POR' end ) as language,
                        mid_convertstring(name,1) as name,
                        (case when phontype = '1' then 'PYM'
                              when phontype = '3' then 'PYT' end) as phontype, 
                        phoneme 
                from
                (
                    SELECT  a.id, 1 as name_kind,b.language,b.nameflag,c.seq_nm, b.name as name, c.name as phoneme, c.phontype
                    FROM org_br as a
                    join org_fname as b
                    on a.id = b.featid and nametype = '2'
                    left join org_fname_phon as c
                    on c.featid = b.featid and c.nametype = b.nametype 
                        and c.seq_nm = b.seq_nm and b.language in ('1','2')
                        and c.phontype in ('1', '3')
                    
                    union
                    
                    select  id, name_kind, language,nameflag, seq_nm,
                            (case when name like '%方向' then split_part(name,'方向',1)
                                  when name like '%方向）' then split_part(name,'方向）',1) || '）'
                                  else name end ) as name,
                            ( case when (name like '%方向' or name like '%方向）' ) then null
                                else phoneme end ) as phoneme,
                            phontype
                    from
                    (
                        select kk.* , rd.id as roundabout from
                        (
                            SELECT  a.id, 0 as name_kind,b.language,b.nameflag, c.seq_nm,b.name as name, c.name as phoneme, c.phontype
                            FROM org_ic as a
                            join org_fname as b
                            on a.id = b.featid and nametype = '3' and b.signnumflg = '0'
                            left join org_fname_phon as c
                            on c.featid = b.featid and c.nametype = b.nametype 
                            and c.seq_nm = b.seq_nm and b.language in ('1','2')
                            and c.phontype in ('1', '3')
                            
                            union 
                            
                            SELECT  a.id, 0 as name_kind,b.language,b.nameflag,c.seq_nm, b.name as name, c.name as phoneme, c.phontype
                            FROM org_dr as a
                            join org_fname as b
                            on a.id = b.featid and nametype = '4' and b.signnumflg = '0'
                            left join org_fname_phon as c
                            on c.featid = b.featid and c.nametype = b.nametype 
                            and c.seq_nm = b.seq_nm and b.language in ('1','2')
                            and c.phontype in ('1', '3')
                        ) kk
                        left join  temp_roundabout_id as rd
                        on rd.id = kk.id::bigint and rd.name = kk.name
                    ) as temp
                    where roundabout is null                    
                         
                    union
                       
                    SELECT   a.id, 3 as name_kind, b.language, b.nameflag, b.seq_nm, mid_cnv_shield_ni(a.folder,b.signnumflg, b.name) as name, 
                             --null as phoneme, null as phontype
                    c.name as phoneme, c.phontype
                    FROM org_ic as a
                    join org_fname as b
                    on a.id = b.featid and nametype = '3' and b.signnumflg <> '0'
                    left join org_fname_phon as c
                    on c.featid = b.featid and c.nametype = b.nametype 
                    and c.seq_nm = b.seq_nm and b.language in ('1','2')
                    and c.phontype in ('1', '3')
                    
                    union 
                    
                    SELECT  a.id,3 as name_kind,b.language,b.nameflag,b.seq_nm, mid_cnv_shield_ni(a.folder,b.signnumflg, b.name) as name,  
                           -- null as phoneme, null as phontype
                    c.name as phoneme, c.phontype
                    FROM org_dr as a
                    join org_fname as b
                    on a.id = b.featid and nametype = '4' and b.signnumflg <> '0'
                    left join org_fname_phon as c
                    on c.featid = b.featid and c.nametype = b.nametype 
                    and c.seq_nm = b.seq_nm and b.language in ('1','2')
                    and c.phontype in ('1', '3')
                ) as c
                order by id::bigint, name_kind,language,nameflag,seq_nm,name,phontype,phoneme
            )
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()       
#        self.CreateIndex2('temp_sp_exit_name_all_language_id_idx')
        
        
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
        
        sqlcmd = """
                select  id,
                        name_kind,
                        array_agg(1) as name_id_array,
                        array_agg((case when a.name_kind = 3 then 'shield'::varchar 
                                        else 'office_name'::varchar end )) as name_type_array,
                        array_agg(language) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phontype) as phonetic_language_array,
                        array_agg(phoneme) as phoneme_array
                from temp_sp_json_name_all_language as a
                group by id,name_kind,seq_nm
                order by id::bigint,name_kind,seq_nm
                """         
               
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('sp_json_name')
        for asso_rec in asso_recs:
            sign_id = asso_rec[0]
            name_kind = asso_rec[1]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(asso_rec[2], 
                                                                                                            asso_rec[3], 
                                                                                                            asso_rec[4],
                                                                                                            asso_rec[5], 
                                                                                                            asso_rec[6],
                                                                                                            asso_rec[7] )
            temp_file_obj.write('%d\t%d\t%s\n' % (int(sign_id),int(name_kind), json_name))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_sp_json_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_sp_json_name_id_idx')   
    

        

    def _make_temp_towardname_signpost_uc(self):
        
        self.log.info('make temp_towardname_signpost_uc...')
        sqlcmd = '''
            insert into temp_towardname_tbl( nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, 
                                            guideattr, namekind, toward_name ) 
            ( 
            select  b.tnodeid, b.inlinkid, b.outlinkid, b.passlid,b.passlink_cnt,0 as direction,
                    0 as guideattr, a.namekind, a.towardname
            from temp_sp_json_name as a
            left join temp_signpost_uc_path_info as b
            on a.id = b.id
            order by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, guideattr ,namekind
            )
        '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()

        
        
        

    def _make_highway_building_name(self):
        
        self._make_poi_inlink()
        self._make_highway_building_json_name()
        self._make_temp_towardname_highway_building()
        
        
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
   
   
        
    def _make_highway_building_json_name(self):
        
        self.log.info('make highway_building_json_name...')
        self.CreateTable2('temp_highway_building_json_name')
        self.CreateFunction2('mid_convertstring')
        
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
                          
        sqlcmd = '''
                drop table if exists temp_highway_building_name_all_language;
                create table temp_highway_building_name_all_language
                as
              (
                     select  a.poi_id,
                             mid_convertstring(b.name,1) as poi_name,
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
                        select * from temp_highway_building_name_all_language
                        order by poi_id::bigint, name_id::bigint, phoneme_lang                                   
                    ) as a
                group by poi_id
                order by poi_id::bigint;
                '''                  
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('highway_building_name')
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
        self.pg.copy_from2(temp_file_obj, 'temp_highway_building_json_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_highway_building_json_name_poi_id_idx') 
 
        
    def _make_temp_towardname_highway_building(self):
 
 
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
                    left join temp_highway_building_json_name as b
                    on a.poi_id = b.poi_id
                    left join temp_poi_find as c
                    on c.poi_id::bigint = a.poi_id
                ) as d
                order by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, direction, guideattr ,namekind, toward_name
                )
                '''
        self.pg.execute(sqlcmd)
        self.pg.commit2()

 
 