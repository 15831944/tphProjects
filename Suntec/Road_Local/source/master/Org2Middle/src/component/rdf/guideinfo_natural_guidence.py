# -*- coding: UTF8 -*-
'''
Created on 2014-11-15
@author: liuxinxing
'''

import component.component_base
import common.cache_file
import common.networkx
import component.default.multi_lang_name

class comp_natural_guidence(component.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'NaturalGuidence')

    def _Do(self):
        self.__prepare()
        #self.__delete_dummy_guidence()
        self.__do_guidence_names()
        self.__do_guidence_paths()
        self.__do_guidence_conditions()
        self.__do_natural_guidence()
        return 0

    def __prepare(self):
        self.log.info('Prepare...')
        self.CreateTable2('natural_guidence_tbl')
        self.CreateTable2('guidence_condition_tbl')
        self.CreateTable2('temp_natural_guidence')
        self.CreateIndex2('temp_natural_guidence_asso_id_idx')
    
    def __delete_dummy_guidence(self):
        self.log.info('deleting dummy guidence...')
        sqlcmd = """
            delete from temp_natural_guidence as a
            using
            (
               select unnest(gid_array[2:gid_count]) as gid
                from
                (
                    select  count(*) as gid_count, 
                            array_agg(gid) as gid_array
                    from
                    (
                        select  *
                        from temp_natural_guidence
                        order by link_id, direction, f_link_id, t_link_id, calc_importance desc
                    )as t
                    group by link_id, direction, f_link_id, t_link_id
                    having count(*) > 1
                )as t2
            )as b
            where a.gid = b.gid
            ;
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def __do_guidence_names(self):
        self.log.info('making natural guidence names...')
        
        # init language code for MultiLangName
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
            
        #self.__make_asso_name()
        #self.__make_fp_name()
        #self.__make_fp_preposition()
        self.CreateTable2('temp_natural_guidence_name')
        
        sqlcmd = """
                drop table if exists temp_natural_guidence_name_all_language;
                create table temp_natural_guidence_name_all_language
                as
                (
                    select  a.t_fp_id as fp_id,
                            xc.language_code,
                            xc.name as feat_name,
                            xe.phonetic_language_code,
                            xe.phonetic_string as feat_phonetic_string,
                            zc.preposition as prep_name,
                            ze.phonetic_string as prep_phonetic_string,
                            (zc.position in ('1','2')) as is_prep_feat,
                            'Y' as preferred,
                            xd.preferred as feat_preferred,                           -- 'Y'/'N'
                            xe.transcription_method as feat_transcription_method,     -- 'S'/'M'   
                            zd.preferred as prep_preferred,                           -- 'Y'/'N'
                            ze.transcription_method as prep_transcription_method      -- 'S'/'M'
                    from temp_natural_guidence as a
                    -- feat name
                    left join rdf_feature_point_names as xb
                        on a.t_fp_id = xb.fp_id
                    left join rdf_feature_point_name as xc
                        on xb.name_id = xc.name_id
                    left join vce_feature_point_name as xd
                        on xc.name_id = xd.name_id
                    left join vce_phonetic_text as xe
                        on xd.phonetic_id = xe.phonetic_id
                    -- prep
                    left join rdf_feature_point_prep as zb
                        on a.t_fp_id = zb.fp_id
                    left join rdf_meta_preposition as zc
                        on zb.preposition_code = zc.preposition_code and zc.language_code = xc.language_code
                    left join vce_meta_preposition as zd
                        on zc.preposition_code = zd.preposition_code
                    left join vce_phonetic_text as ze
                        on zd.phonetic_id = ze.phonetic_id and ze.phonetic_language_code = xe.phonetic_language_code
                    where (xe.phonetic_id is not null) and (ze.phonetic_id is not null)
                    
                    union
                    
                    select  a.t_fp_id as fp_id,
                            xc.language_code,
                            xc.name as feat_name,
                            xe.phonetic_language_code,
                            xe.phonetic_string as feat_phonetic_string,
                            zc.preposition as prep_name,
                            ze.phonetic_string as prep_phonetic_string,
                            (zc.position in ('1','2')) as is_prep_feat,
                            'N' as preferred,
                            xd.preferred as feat_preferred,                           -- 'Y'/'N'
                            xe.transcription_method as feat_transcription_method,      -- 'S'/'M'   
                            zd.preferred as prep_preferred,                           -- 'Y'/'N'
                            ze.transcription_method as prep_transcription_method      -- 'S'/'M'
                    from temp_natural_guidence as a
                    -- asso name
                    left join rdf_asso_names as xb
                        on a.asso_id = xb.asso_id
                    left join rdf_asso_name as xc
                        on xb.name_id = xc.name_id
                    left join vce_asso_name as xd
                        on xc.name_id = xd.name_id
                    left join vce_phonetic_text as xe
                        on xd.phonetic_id = xe.phonetic_id
                    -- prep
                    left join rdf_feature_point_prep as zb
                        on a.t_fp_id = zb.fp_id
                    left join rdf_meta_preposition as zc
                        on zb.preposition_code = zc.preposition_code and zc.language_code = xc.language_code
                    left join vce_meta_preposition as zd
                        on zc.preposition_code = zd.preposition_code
                    left join vce_phonetic_text as ze
                        on zd.phonetic_id = ze.phonetic_id and ze.phonetic_language_code = xe.phonetic_language_code
                    where (xe.phonetic_id is not null) and (ze.phonetic_id is not null)
                );
                """
        self.pg.execute(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
                select  fp_id,
                        array_agg(name_id) as name_id_array,
                        array_agg('office_name'::varchar) as name_type_array,
                        array_agg(language_code) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phonetic_language_code) as phonetic_language_code_array,
                        array_agg(phonetic_string) as phonetic_string_array
                from
                (
                    -- connect prep_phonetic_string and feat_phonetic_string
                    select  fp_id, language_code, phonetic_language_code,
                            0 as name_id,
                            (
                            case 
                            when is_prep_feat then (prep_name || ' ' || feat_name)
                            else (feat_name || ' ' || prep_name)
                            end
                            )as name,
                            (
                            case 
                            when is_prep_feat then (prep_phonetic_string || ' ' || feat_phonetic_string)
                            else (feat_phonetic_string || ' ' || prep_phonetic_string)
                            end
                            )as phonetic_string
                    from
                    (
                        -- choose the best phonetic_string
                        select  fp_id, language_code, phonetic_language_code, 
                                0 as name_id,
                                (array_agg(is_prep_feat))[1] as is_prep_feat,
                                (array_agg(feat_name))[1] as feat_name,
                                (array_agg(feat_phonetic_string))[1] as feat_phonetic_string,
                                (array_agg(prep_name))[1] as prep_name,
                                (array_agg(prep_phonetic_string))[1] as prep_phonetic_string
                        from
                        (
                            select *
                            from temp_natural_guidence_name_all_language
                            order by fp_id, language_code, phonetic_language_code, preferred desc, 
                                     feat_preferred desc, feat_transcription_method desc, 
                                     prep_preferred desc, prep_transcription_method desc
                        )as t2
                        group by fp_id, language_code, phonetic_language_code
                    )as t3
                    order by fp_id, name_id, language_code, phonetic_language_code
                )as t4
                group by fp_id
                ;
                """
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('natural_guidence_name')
        for asso_rec in asso_recs:
            fp_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string_multi_phon(asso_rec[1], 
                                                                                                            asso_rec[2], 
                                                                                                            asso_rec[3], 
                                                                                                            asso_rec[4], 
                                                                                                            asso_rec[5], 
                                                                                                            asso_rec[6])
            temp_file_obj.write('%d\t%s\n' % (fp_id, json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_natural_guidence_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_natural_guidence_name_fp_id_idx')
    
    def __make_asso_name(self):
        # 
        self.log.info('making asso name...')
        self.CreateTable2('temp_natural_guidence_asso_name')
        
        sqlcmd = """
                select  asso_id,
                        array_agg(name_id) as name_id_array,
                        array_agg(language_code) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phonetic_string) as phonetic_string_array
                from
                (
                    select  asso_id, name_id, language_code, name,
                            (array_agg(phonetic_string))[1] as phonetic_string    -- choose the best phonetic_string
                    from
                    (
                        select *
                        from
                        (
                            select  a.asso_id,
                                    b.name_id,
                                    c.language_code,
                                    c.name,
                                    e.phonetic_string,
                                    d.preferred,                -- 'Y'/'N'
                                    e.transcription_method      -- 'S'/'M'
                            from rdf_asso as a
                            left join rdf_asso_names as b
                            on a.asso_id = b.asso_id
                            left join rdf_asso_name as c
                            on b.name_id = c.name_id
                            left join vce_asso_name as d
                            on c.name_id = d.name_id
                            left join vce_phonetic_text as e
                            on d.phonetic_id = e.phonetic_id
                                --and c.language_code = e.phonetic_language_code --sometimes not the same(ENG != UKE)
                            where e.phonetic_string is not null
                        )as t
                        order by asso_id, name_id, language_code, name, 
                                 preferred desc, transcription_method desc, phonetic_string
                    )as t2
                    group by asso_id, name_id, language_code, name
                    order by asso_id, name_id, language_code, name
                )as t3
                group by asso_id
                ;
                """
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('natural_guidence_asso_name')
        for asso_rec in asso_recs:
            asso_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string(asso_rec[1], 
                                                                                         asso_rec[2], 
                                                                                         asso_rec[3], 
                                                                                         asso_rec[4])
            temp_file_obj.write('%d\t%s\n' % (asso_id, json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_natural_guidence_asso_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_natural_guidence_asso_name_asso_id_idx')
    
    def __make_fp_name(self):
        # 
        self.log.info('making fp name...')
        self.CreateTable2('temp_natural_guidence_fp_name')
        
        sqlcmd = """
                select  fp_id,
                        array_agg(name_id) as name_id_array,
                        array_agg(language_code) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phonetic_string) as phonetic_string_array
                from
                (
                    select  fp_id, name_id, language_code, name,
                            (array_agg(phonetic_string))[1] as phonetic_string    -- choose the best phonetic_string
                    from
                    (
                        select *
                        from
                        (
                            select  b.fp_id,
                                    b.name_id,
                                    c.language_code,
                                    c.name,
                                    e.phonetic_string,
                                    d.preferred,                -- 'Y'/'N'
                                    e.transcription_method      -- 'S'/'M'
                            from temp_natural_guidence as a
                            left join rdf_feature_point_names as b
                            on a.t_fp_id = b.fp_id
                            left join rdf_feature_point_name as c
                            on b.name_id = c.name_id
                            left join vce_feature_point_name as d
                            on c.name_id = d.name_id
                            left join vce_phonetic_text as e
                            on d.phonetic_id = e.phonetic_id 
                                --and c.language_code = e.phonetic_language_code --sometimes not the same(ENG != UKE)
                            where e.phonetic_string is not null
                        )as t
                        order by fp_id, name_id, language_code, name, 
                                 preferred desc, transcription_method desc, phonetic_string
                    )as t2
                    group by fp_id, name_id, language_code, name
                    order by fp_id, name_id, language_code, name
                )as t3
                group by fp_id
                ;
                """
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('natural_guidence_fp_name')
        for asso_rec in asso_recs:
            asso_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string(asso_rec[1], 
                                                                                         asso_rec[2], 
                                                                                         asso_rec[3], 
                                                                                         asso_rec[4])
            temp_file_obj.write('%d\t%s\n' % (asso_id, json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_natural_guidence_fp_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_natural_guidence_fp_name_fp_id_idx')
    
    def __make_fp_preposition(self):
        # 
        self.log.info('making fp preposition...')
        self.CreateTable2('temp_natural_guidence_fp_preposition')
        
        sqlcmd = """
                select  fp_id,
                        array_agg(name_id) as name_id_array,
                        array_agg(language_code) as language_code_array,
                        array_agg(name) as name_array,
                        array_agg(phonetic_string) as phonetic_string_array
                from
                (
                    select  fp_id, name_id, language_code, name,
                            (array_agg(phonetic_string))[1] as phonetic_string    -- choose the best phonetic_string
                    from
                    (
                        select *
                        from
                        (
                            select  a.fp_id,
                                    --b.preposition_code,
                                    0 as name_id,
                                    b.language_code,
                                    b.preposition as name,
                                    d.phonetic_string,
                                    d.transcription_method      -- 'S'/'M'
                            from rdf_feature_point_prep as a
                            left join rdf_meta_preposition as b
                            on a.preposition_code = b.preposition_code
                            left join vce_meta_preposition as c
                            on b.preposition_code = c.preposition_code
                            left join vce_phonetic_text as d
                            on c.phonetic_id = d.phonetic_id
                            where d.phonetic_string is not null
                        )as t
                        order by fp_id, name_id, language_code, name, 
                                 transcription_method desc, phonetic_string
                    )as t2
                    group by fp_id, name_id, language_code, name
                    order by fp_id, name_id, language_code, name
                )as t3
                group by fp_id
                ;
                """
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('natural_guidence_fp_preposition')
        for asso_rec in asso_recs:
            asso_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string(asso_rec[1], 
                                                                                         asso_rec[2], 
                                                                                         asso_rec[3], 
                                                                                         asso_rec[4])
            temp_file_obj.write('%d\t%s\n' % (asso_id, json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_natural_guidence_fp_preposition')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_natural_guidence_fp_preposition_fp_id_idx')
    
    def __do_guidence_paths(self):
        # find paths
        self.log.info('Find paths of junction guidence...')
        sqlcmd = """
            select  a.gid,
                    (
                    case 
                    when a.link_id != t_link_id then a.link_id 
                    else f_link_id 
                    end
                    ) as flink,
                    t_link_id as tlink,
                    (
                    case 
                    when a.link_id = t_link_id then null
                    when a.direction = 'F' then b.nonref_node_id
                    when a.direction = 'T' then b.ref_node_id
                    else null
                    end
                    ) as fnode,
                    null as tnode
            from temp_natural_guidence as a
            left join temp_rdf_nav_link as b
            on a.link_id = b.link_id
            --where asso_type = 'JG' and not (a.link_id = a.t_link_id and a.f_link_id = a.t_link_id)
            where not (a.link_id = a.t_link_id and a.f_link_id = a.t_link_id)
            ;
        """
        self.CreateTable2('temp_natural_guidence_paths')
        temp_file_obj = common.cache_file.open('natural_guidence_paths')
        graph = common.networkx.CGraph_PG()
        
        rec_list = self.pg.get_batch_data2(sqlcmd)
        for rec in rec_list:
            gid = rec[0]
            inlink = rec[1]
            outlink = rec[2]
            innode = rec[3]
            outnode = rec[4]
            
            try:
                graph.prepareData()
                paths = graph.searchShortestPaths(inlink, outlink, innode, outnode)
                graph.clearData()
                if not paths:
                    self.log.warning('JG path does not exist, inlink = %d, outlink = %d.' % (inlink, outlink))
                else:
                    for (cur_distance, path) in paths:
                        rec_string = '%s\t{%s}\n' % (str(gid), ','.join([str(x) for x in path]))
                        temp_file_obj.write(rec_string)
            except:
                self.log.error('JG path does not exist, inlink = %d, outlink = %d.' % (inlink, outlink))
        
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_natural_guidence_paths')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('temp_natural_guidence_paths_gid_idx')
    
    def __do_guidence_conditions(self):
        self.log.info('making guidence conditions...')
        
        self.CreateTable2('guidence_condition_tbl')
        self.CreateTable2('temp_guidence_datetime_mapping')
        self.CreateFunction2('mid_convert_guidence_condition')
        self.pg.callproc('mid_convert_guidence_condition')
        self.pg.commit2()
    
    def __do_natural_guidence(self):
        self.log.info('making natural guidence...')
        
        self.CreateFunction2('mid_get_connected_node')
        sqlcmd = """
                insert into natural_guidence_tbl
                (
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                    feat_position, feat_importance, preposition_code, feat_name, condition_id
                )
                (
                    select  mid_get_connected_node(b.link_array[1], b.link_array[2]) as node_id,
                            b.link_array[1] as inlinkid, 
                            b.link_array[array_upper(b.link_array,1)] as outlinkid,
                            array_to_string(b.link_array[2:array_upper(b.link_array,1)-1], '|') as passlid,
                            array_upper(b.link_array,1) - 2 as passlink_cnt,
                            (case a.f_side when 'L' then 1 when 'R' then 2 when 'B' then 3 else 0 end) as feat_position,
                            a.calc_importance as feat_importance, 
                            0 as preposition_code,
                            c.feat_name,
                            d.condition_id
                    from temp_natural_guidence as a
                    left join temp_natural_guidence_paths as b
                        on a.gid = b.gid
                    left join temp_natural_guidence_name as c
                        on a.t_fp_id = c.fp_id
                    --where asso_type = 'JG' and b.link_array is not null
                    left join temp_guidence_datetime_mapping as d
                        on a.dt_id = d.dt_id
                    where (b.link_array is not null) and (c.feat_name is not null)
                    order by link_array, calc_importance desc, feat_name
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        
        