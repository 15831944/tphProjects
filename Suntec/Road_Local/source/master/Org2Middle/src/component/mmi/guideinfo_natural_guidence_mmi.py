# -*- coding: utf8 -*-
'''
Created on 2015-4-9

@author: lsq
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
    
    def _DoCreateTable(self):
 
        self.CreateTable2('guidence_condition_tbl')
        return 0
    
    def _Do(self):
        
        self._update_temp_natural_guidance_poi_link_tbl()
        self._update_temp_natural_guidance_poi_link_node_tbl()
        self._update_temp_natural_guidance_node_tbl()
        self._update_temp_natural_guidance_in_out_link_rel()
        self._update_temp_natural_guidance_tbl()
        self._update_mid_temp_natural_guidance_tbl()
        self._update_mid_temp_natural_guidence_name()
        self._update_natural_guidence_tbl()
        return 0
    
    def _DoCreateIndex(self):
              
        return 0
        
    def _DoCreateFunction(self):
        
        return 0
    
    def _update_temp_natural_guidance_poi_link_tbl(self):
        
        self.CreateTable2('temp_natural_guidance_poi_link_tbl')
        sqlcmd = """
                insert into temp_natural_guidance_poi_link_tbl (
                    poi_lid,
                    poi_nme, 
                    poi_prop,
                    the_geom
                    ) 
                select 
                    poi_lid, 
                    poi_nme, 
                    poi_prop, 
                    the_geom
                from (
                    select 
                        c.link_id as poi_lid,
                        b.poi_nme, 
                        b.poi_prop,
                        b.the_geom
                    from (
                        select 
                            poi_nme, 
                            poi_prop, 
                            ST_SetSRID(ST_MakePoint(longitude, latitude), 4326) as the_geom
                        from 
                            org_natural_guidance
                    ) as b
                    left join link_tbl c 
                        on ST_DWithin(b.the_geom, c.the_geom, 0.000009)
                ) as d
            """
        
        self.log.info('Now it is inserting to temp_natural_guidance_poi_link_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.CreateIndex2('temp_natural_guidance_poi_link_tbl_poi_lid_idx')
        self.CreateIndex2('temp_natural_guidance_poi_link_tbl_the_geom_idx')
        self.log.info('Inserting temp_natural_guidance_poi_link_tbl succeeded') 
        return 0
    
    def _update_temp_natural_guidance_poi_link_node_tbl(self):
        
        self.CreateTable2('temp_natural_guidance_poi_link_node_tbl')
        sqlcmd = """
                insert into temp_natural_guidance_poi_link_node_tbl (
                    poi_lid,
                    node_id,
                    poi_nme, 
                    poi_prop,
                    the_geom
                    ) 
                    select 
                        e.link_id as poi_lid, 
                        e.node_id, 
                        e.poi_nme, 
                        e.poi_prop, 
                        e.the_geom
                    from (
                        select 
                            c.link_id, 
                            c.node_id, 
                            c.poi_nme, 
                            c.poi_prop, 
                            c.the_geom,
                            d.the_geom as node_geom
                        from (
                            select 
                                b.link_id, 
                                b.s_node as node_id, 
                                a.poi_nme, 
                                a.poi_prop, 
                                a.the_geom
                            from temp_natural_guidance_poi_link_tbl a
                            inner join link_tbl b
                                on a.poi_lid = b.link_id
                            union
                            select 
                                b.link_id, 
                                b.e_node as node_id, 
                                a.poi_nme, 
                                a.poi_prop, 
                                a.the_geom
                            from temp_natural_guidance_poi_link_tbl a
                            inner join link_tbl b
                                on a.poi_lid = b.link_id
                            order by link_id, node_id
                        ) as c
                        inner join node_tbl d
                            on c.node_id = d.node_id
                    ) as e
                    inner join temp_natural_guidance_poi_link_tbl f
                        on e.link_id = f.poi_lid and ST_Distance_Sphere(f.the_geom, e.node_geom) < 30.0 --ST_DWithin(f.the_geom, e.node_geom, 0.00027)
            """
        
        self.log.info('Now it is inserting to temp_natural_guidance_poi_link_node_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.CreateIndex2('temp_natural_guidance_poi_link_node_tbl_poi_lid_idx')
        self.CreateIndex2('temp_natural_guidance_poi_link_node_tbl_node_id_idx')
        self.log.info('Inserting temp_natural_guidance_poi_link_node_tbl succeeded')
        return 0
    
    def _update_temp_natural_guidance_node_tbl(self):
        
        self.CreateTable2('temp_natural_guidance_node_tbl')
        self.CreateFunction2('mmi_array_delete')
        self.CreateFunction2('mmi_get_inner_node_by_node_id')
        sqlcmd = """
                insert into temp_natural_guidance_node_tbl (
                    node_id,
                    poi_nme, 
                    poi_prop,
                    poi_the_geom,
                    node_the_geom
                    ) 
                select 
                    b.node_id, 
                    b.poi_nme as poi_nme, 
                    b.poi_prop as poi_prop, 
                    b.poi_the_geom as poi_the_geom, 
                    c.the_geom as node_the_geom
                from (
                    select distinct
                        unnest(mmi_get_inner_node_by_node_id(a.node_id)) as node_id, 
                        a.poi_nme as poi_nme, 
                        a.poi_prop as poi_prop, 
                        a.the_geom as poi_the_geom
                    from 
                        temp_natural_guidance_poi_link_node_tbl a
                ) as b
                inner join node_tbl c
                    on b.node_id = c.node_id
                where ST_Distance_Sphere(b.poi_the_geom, c.the_geom) < 50.0 --ST_DWithin(b.poi_the_geom, c.the_geom, 0.00045) --poi point and guide point within 50 meters
            """
        
        self.log.info('Now it is inserting to temp_natural_guidance_node_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.CreateIndex2('temp_natural_guidance_node_tbl_node_id_idx')
        self.log.info('Inserting temp_natural_guidance_node_tbl succeeded')
        return 0
    
    def _update_temp_natural_guidance_in_out_link_rel(self):
      
        self.CreateTable2('temp_natural_guidance_in_out_link_rel')  
        self.CreateFunction2('mmi_get_canPassLink_count')
        sqlcmd = """
                insert into temp_natural_guidance_in_out_link_rel (
                    in_link_id,
                    in_s_node,
                    in_e_node,
                    in_link_type,
                    in_one_way_code,
                    node_id,
                    out_link_id,
                    out_s_node,
                    out_e_node,
                    out_link_type,
                    out_one_way_code,
                    poi_nme, 
                    poi_prop,
                    the_geom
                    ) 
                select distinct
                    in_link_id, 
                    in_s_node, 
                    in_e_node, 
                    in_link_type, 
                    in_one_way_code, 
                    node_id, 
                    out_link_id, 
                    out_s_node, 
                    out_e_node, 
                    out_link_type, 
                    out_one_way_code,
                    poi_nme, 
                    poi_prop,
                    the_geom
                from (
                    select 
                        b.link_id as in_link_id, 
                        b.s_node as in_s_node,
                        b.e_node as in_e_node,
                        b.link_type as in_link_type,
                        b.one_way_code as in_one_way_code,
                        a.node_id as node_id, 
                        c.link_id as out_link_id,
                        c.s_node as out_s_node,
                        c.e_node as out_e_node,
                        c.link_type as out_link_type,
                        c.one_way_code as out_one_way_code,
                        a.poi_nme as poi_nme,
                        a.poi_prop as poi_prop,
                        a.poi_the_geom as the_geom
                    from temp_natural_guidance_node_tbl a
                    left join link_tbl b
                        on 
                            (a.node_id = b.e_node and b.one_way_code in (1, 2)) or
                            (a.node_id = b.s_node and b.one_way_code in (1, 3))
                    left join link_tbl c
                        on 
                            (a.node_id = c.s_node and c.one_way_code in (1, 2)) or
                            (a.node_id = c.e_node and c.one_way_code in (1, 3)) 
                ) as d
                where 
                    in_link_type <> 4 and
                    in_link_id <> out_link_id and
                    mmi_get_canPassLink_count(in_link_id, node_id) > 1
                order by in_link_id, in_s_node, in_e_node, in_link_type, in_one_way_code, node_id, out_link_id, out_s_node, out_e_node, out_link_type, out_one_way_code
            """
        
        self.log.info('Now it is inserting to temp_natural_guidance_in_out_link_rel...')
        self.pg.do_big_insert2(sqlcmd)
        self.CreateIndex2('temp_natural_guidance_in_out_link_rel_in_link_id_idx')
        self.CreateIndex2('temp_natural_guidance_in_out_link_rel_out_link_id_idx')
        self.log.info('Inserting temp_natural_guidance_in_out_link_rel succeeded')
        return 0
    
    def _update_temp_natural_guidance_tbl(self):
        
        self.CreateTable2('temp_natural_guidance_tbl')
        self.CreateFunction2('mmi_update_temp_natural_guidance_tbl')
        self.CreateFunction2('mmi_get_inner_link_count')
        self.CreateFunction2('mmi_one_update_temp_natural_guidance_tbl')
        self.CreateFunction2('mmi_one_sub_update_temp_natural_guidance_tbl')
        self.CreateFunction2('mmi_get_canPassLink_list')
        sqlcmd = """
                select mmi_update_temp_natural_guidance_tbl()
            """
        
        self.log.info('Now it is inserting to temp_natural_guidance_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info('Inserting temp_natural_guidance_tbl succeeded')
        return 0 
    
    def _update_mid_temp_natural_guidance_tbl(self):
        
        self.CreateTable2('mid_temp_natural_guidance_tbl')
        self.CreateFunction2('mmi_get_preposition_code')
        self.CreateFunction2('mmi_get_feat_position')
        sqlcmd = """
                insert into mid_temp_natural_guidance_tbl (
                    in_link_id,
                    in_node_id,
                    out_link_id,
                    pass_link_array,
                    pass_link_count,
                    feat_position,
                    preposition_code,
                    poi_nme,
                    poi_prop,
                    the_geom
                    ) 
                select distinct
                    in_link_id,
                    in_node_id,
                    out_link_id,
                    link_array as pass_link_array,
                    (case when link_array is not null then array_upper(link_array, 1) else 0 end) as pass_link_count,
                    mmi_get_feat_position(gid) as feat_position,
                    mmi_get_preposition_code(gid) as preposition_code,
                    poi_nme,
                    poi_prop,
                    the_geom 
                from temp_natural_guidance_tbl
            """
        
        self.log.info('Now it is inserting to mid_temp_natural_guidance_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info('Inserting mid_temp_natural_guidance_tbl succeeded')
        return 0
    
    def _update_mid_temp_natural_guidence_name(self):
        
        self.CreateTable2('mid_temp_natural_guidence_name')
        if not component.default.multi_lang_name.MultiLangName.is_initialized():
            component.default.multi_lang_name.MultiLangName.initialize()
            
        sqlcmd = """
                select 
                    c.gid, 
                    array_agg(c.name_id) as name_id_array, 
                    array_agg(c.language_code) as language_code_array,
                    array_agg(c.name) as name_array,
                    array_agg(c.phonetic_string) as phonetic_string_array
                from (
                    select 
                        a.gid, 
                        0::integer as name_id, 
                        'ENG'::varchar as language_code,
                        (
                        case 
                        when preposition_code = 1 THEN ('after'::varchar || ' ' || a.poi_nme)
                        when preposition_code = 5 THEN ('before'::varchar || ' ' || a.poi_nme)
                        else ('at'::varchar || ' ' || a.poi_nme)
                        end
                        ) as name,
                        NULL::varchar as phonetic_string
                   from mid_temp_natural_guidance_tbl a
                ) as c
                group by c.gid
                """
        asso_recs = self.pg.get_batch_data2(sqlcmd)
        
        temp_file_obj = common.cache_file.open('natural_guidence_name')
        for asso_rec in asso_recs:
            fp_id = asso_rec[0]
            json_name = component.default.multi_lang_name.MultiLangName.name_array_2_json_string(asso_rec[1], 
                                                                                         asso_rec[2], 
                                                                                         asso_rec[3], 
                                                                                         asso_rec[4])
            temp_file_obj.write('%d\t%s\n' % (fp_id, json_name))
        
        #
        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'mid_temp_natural_guidence_name')
        self.pg.commit2()
        common.cache_file.close(temp_file_obj,True)
        self.CreateIndex2('mid_temp_natural_guidence_name_gid_idx')
        return 0
    
    def _update_natural_guidence_tbl(self):
        
        self.CreateTable2('natural_guidence_tbl')
        sqlcmd = """
                insert into natural_guidence_tbl (
                    nodeid,
                    inlinkid,
                    outlinkid,
                    passlid,
                    passlink_cnt,
                    feat_position,
                    feat_importance,
                    preposition_code,
                    feat_name,
                    condition_id
                    ) 
                select 
                    a.in_node_id as nodeid, 
                    a.in_link_id as inlinkid, 
                    a.out_link_id as outlinkid, 
                    array_to_string(a.pass_link_array, '|') as passlid, 
                    a.pass_link_count as passlink_cnt,
                    a.feat_position, 
                    0 as feat_importance, 
                    a.preposition_code, 
                    b.feat_name, 
                    0 as condition_id
                from 
                    mid_temp_natural_guidance_tbl a
                inner join mid_temp_natural_guidence_name b
                    on a.gid = b.gid
                order by nodeid, inlinkid, outlinkid, passlid, passlink_cnt, feat_position, feat_importance, preposition_code, feat_name, condition_id
            """
        
        self.log.info('Now it is inserting to natural_guidence_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info('Inserting natural_guidence_tbl succeeded')
        return 0
