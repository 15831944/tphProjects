# -*- coding: UTF8 -*-
'''
Created on 2014-6-25

@author: zhaojie
'''
import component.component_base

class comp_fac_name_new_jdb(component.component_base.comp_base):
    '''
    Facility name(高速道或有料道设施名。)
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Facility_Name_new')        
        
    def _DoCreateTable(self):
        if self.CreateTable2('mid_fac_name_temp') == -1:
            return -1
        if self.CreateTable2('mid_fac_name_temp_shortpath') == -1:
            return -1
        if self.CreateTable2('mid_fac_name') == -1:
            return -1
        if self.CreateTable2('mid_temp_multi_highway_node') == -1:
            return -1
        
        self.CreateTable2('temp_node_num_for_towardname')
        self.CreateIndex2('temp_node_num_for_towardname_node_id_idx')        
        return 0
    
    def _DoCreateFunction(self):     
        self.CreateFunction2('mid_cut_path')
        self.CreateFunction2('mid_turn_array')
        self.CreateFunction2('mid_change_symbol')
        self.CreateFunction2('mid_add_out_type')
        self.CreateFunction2('mid_no_merge_node')
        self.CreateFunction2('mid_access_in_path')
        self.CreateFunction2('mid_delete_cycle')
        self.CreateFunction2('mid_delete_long_path')
        self.CreateFunction2('mid_judge_link_in_array')
        self.CreateFunction2('mid_jude_path_appear')
        self.CreateFunction2('mid_get_middle_intersect_node')
        self.CreateFunction2('mid_get_outmulti_path')
        self.CreateFunction2('mid_get_access_byrode')
        self.CreateFunction2('mid_get_sapa_array')
        self.CreateFunction2('mid_onedata_in_array')
        self.CreateFunction2('mid_get_single_out_node_num')
        self.CreateFunction2('mid_get_node_inarray')
        self.CreateFunction2('mid_sapa_smartic_multi')
        self.CreateFunction2('mid_get_link_7type')
        self.CreateFunction2('mid_onenode_in_allpath_for7type')
        self.CreateFunction2('mid_get_inlink_array')
        self.CreateFunction2('mid_get_link_array_type')
        self.CreateFunction2('mid_get_all_link')
        self.CreateFunction2('mid_get_link_array')
                        
        return 0
    
    def _Do(self):
        #get all path
#        self.__MakeFullPath()
#        self.__MakeOutSinglePath()
#        self.__MakeOutMultiPath()
#        
#        #self.__MakeEnterSinglePath()
#        self.__MakeEnterMultiPath()
        #enter
        self.__MakeInPath()
        
        #out
        #self.__MakeOutSinglePath()
        #self.__MakeOutMultiPath()
                
        #self.__AddJCTPath()
        self.__Mergepath()
        self.__Makelink()
        
        self._MakeFacName2Toward()    # 添加TowardName
        return 0
    
    def __MakeInPath(self):
        self.log.info('start make In path')
        self.CreateFunction2('mid_move_node')
        self.CreateFunction2('mid_move_node_array')
        '''
                        已经算好的入口路径，根据已经算好的路径
        '''       
        sqlcmd = '''
            insert into mid_fac_name_temp(node_id,in_link_id,facilclass_c,inout_c,
                                    node_array,sp_name)
            (
                select distinct node_id,link_lib[array_upper(link_lib,1)] as in_link_id,facilclass_c,inout_c,
                    (case when array_upper(link_lib,1) > 2 then array_to_string(link_lib[1:array_upper(link_lib,1)-2],'|')|| '|' || node_lib
                        else node_lib end) as node_array,sp_name
                from
                (
                    select node_id,(regexp_split_to_array(unnest(inlink_array),E'\\\|+'))::bigint[] as link_lib,
                            facilclass_c,inout_c,node_lib,sp_name
                    from
                    (
                        select node_id, mid_get_inlink_array(node_id,in_link_id) as inlink_array,facilclass_c,inout_c,sp_name,node_lib
                        from
                        (
                            select t_node_id as node_id,link_id as in_link_id,t_facilcls as facilclass_c,
                                t_inout_c as inout_c,unnest(node_array) as node_lib,sp_name
                            from
                            (
                                select t_node_id,link_id,t_facilcls,t_inout_c,s_node,e_node,
                                    array_agg(array_to_string(mid_turn_array(node_lid_temp),'|')) as node_array,sp_name
                                from
                                (
                                    select node_id as t_node_id,t_facilcls, t_inout_c,d.link_id,d.s_node,d.e_node,
                                        sp_name,node_lid_temp
                                    from
                                    (
                                        select mid_move_node(t_node_id,mid_change_symbol(node_lid)) as node_id,t_facilcls, t_inout_c,
                                            (case when name_kanji is null then null 
                                              else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name,
                                              mid_move_node_array(t_node_id,mid_change_symbol(node_lid)) as node_lid_temp
                                        from mid_hwy_ic_path as a1
                                        left join mid_road_hwynode_middle_nodeid as b1
                                        on a1.t_node_id = b1.node_id and a1.t_road_code = b1.road_code and a1.t_road_point = b1.road_seq
                                        where a1.s_inout_c = 1
                                    )as a                                
                                    left join link_tbl as d
                                    on (a.node_id = d.s_node and d.one_way_code in(1,3)) 
                                        or (a.node_id = d.e_node and d.one_way_code in(1,2))                                             
                                )temp
                                group by t_node_id,link_id,t_facilcls,t_inout_c,s_node,e_node,sp_name
                            )temp1
                            where mid_judge_link_in_array(s_node,e_node,node_array)
                        )temp2
                    )temp3
                )temp4
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end make In path')
        return 0
    
    def __MakeOutSinglePath(self):
        self.log.info('start make out Single path')
        '''
                        取单节点已经算好的出口路径，
        '''
        sqlcmd = '''
            insert into mid_fac_name_temp(node_id,in_link_id,facilclass_c,inout_c,
                                    node_array,sp_name)
            (
                SELECT s_node_id, a.inlinkid,s_facilcls, s_inout_c, 
                   (case when s_facilcls in (1,2,3) then array_to_string((regexp_split_to_array(mid_change_symbol(node_lid),E'\\\|+'))[1:2],'|')
                     else mid_change_symbol(node_lid) end) as node_array,
                  (case when name_kanji is null then null 
                      else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name
                FROM mid_hwy_ic_path as a
                left join mid_road_hwynode_middle_nodeid as b
                on a.s_node_id = b.node_id and a.s_road_code = b.road_code and a.s_road_point = b.road_seq
                left join mid_temp_multi_highway_node as c
                on a.s_node_id = c.node_id
                left join link_tbl as d
                on (a.s_node_id = d.s_node and d.one_way_code in(1,3)) 
                    or (a.s_node_id = d.e_node and d.one_way_code in(1,2))
                    and d.link_type in (1,2,3)
                where c.num = 1 and a.s_inout_c = 2 and c.node_id is not null

            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end make out Single path')
        return 0
    
    def __MakeFullPath(self):
        self.log.info('start make all path')
        #self.CreateIndex2('highway_node_add_nodeid_midid_nodeid_idx')
        
        sqlcmd = """         
            INSERT INTO mid_temp_full_path(node_id, in_link_id , full_pass_node)
            (
                select node_id, c.link_id as inlink,
                    array_agg(mid_change_symbol(b.node_lib1)) as node_array
                from 
                (
                    select distinct node_id
                    from mid_road_hwynode_middle_nodeid 
                    where facilclass_c != 6 and tollclass_c = 0 and inout_c = 2
                )as a
                left join 
                (
                    select s_node_id,(case when s_node = node_lid_array[array_upper(node_lid_array,1)]
                                        then node_lid_temp || '|' || e_node::varchar
                                        else node_lid_temp || '|' || s_node::varchar end) as node_lib1
                    from
                    (                    
                        select s_node_id,s_node,e_node,mid_change_symbol(node_lid) as node_lid_temp,
                            regexp_split_to_array(mid_change_symbol(node_lid),E'\\\|+')::bigint[] as node_lid_array
                        from mid_hwy_ic_path as temp1
                        left join link_tbl as temp2
                        on temp1.outlinkid = temp2.link_id
                        where s_facilcls = 3
                    )temp3
                    
                    union
                    
                    select s_node_id,mid_change_symbol(node_lid) as node_lib1
                    from mid_hwy_ic_path
                    where s_facilcls <> 3
                    
                ) as b
                on a.node_id = b.s_node_id
                left join link_tbl as c
                on (a.node_id = c.s_node and c.one_way_code in(1,3)) 
                    or (a.node_id = c.e_node and c.one_way_code in(1,2))
                where b.s_node_id is not null
                group by node_id,c.link_id              
            );
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('mid_temp_full_path_nodeid_idx')
            
        self.log.info('end make all path')
        return 0
    
    
    def __MakeOutSinglePathold(self):
        self.log.info('start make OutSingle path')
        
        sqlcmd = '''
            insert into mid_fac_name_temp(node_id,in_link_id,facilclass_c,inout_c,
                                    node_array,sp_name)
            (
                select distinct node_id,in_link_id,facilclass_c,inout_c,node_array[path_seq_num],sp_name
                from
                (
                    select node_id, in_link_id,facilclass_c,inout_c,node_array,sp_name
                        ,generate_series(1, array_upper(node_array, 1)) as path_seq_num
                    from
                    (
                        select a.node_id, b.in_link_id,a.facilclass_c,a.inout_c,
                            mid_get_single_path(a.road_code,a.road_seq,b.full_pass_node,a.facilclass_c,true) as node_array,
                            (case when name_kanji is null then null 
                                else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name
                        from mid_road_hwynode_middle_nodeid as a
                        left join mid_temp_full_path as b
                        on a.node_id = b.node_id
                        left join mid_temp_multi_highway_node as c
                        on a.node_id = c.node_id
                        where tollclass_c = 0 and inout_c = 2 and c.num = 1
                    )temp
                    where node_array is not null   
                )temp1
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end make OutSingle path')
        return 0
    
    def __MakeOutMultiPath(self):
        self.log.info('start make OutMulti Path')
        
        '''
                    复合节点出口
        '''
        self.CreateTable2('temp_node_num_for_towardname')
        self.CreateIndex2('temp_node_num_for_towardname_node_id_idx')
        
        sqlcmd = '''
            insert into mid_fac_name_temp(node_id,in_link_id,facilclass_c,inout_c,
                                    node_array,sp_name)
            (
                select distinct s_node_id,inlinkid,s_facilcls,s_inout_c,unnest(node_array_all),sp_name  
                from
                (
                    select s_node_id,inlinkid,s_facilcls,s_inout_c,sp_name,
                        mid_get_outmulti_path(s_node_id,s_road_code,s_road_point,s_facilcls,array_agg(node_lib1),type_array) as node_array_all
                    from
                    (
                        SELECT s_node_id, a.inlinkid,s_facilcls, s_inout_c, node_lib1,s_road_code,s_road_point,type_array,
                          (case when name_kanji is null then null 
                              else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name
                        FROM 
                        (
                            select s_node_id,inlinkid,s_facilcls,s_inout_c,s_road_code,s_road_point,
                                (case when s_node = node_lid_array[array_upper(node_lid_array,1)]
                                then node_lid_temp || '|' || e_node::varchar
                                else node_lid_temp || '|' || s_node::varchar end) as node_lib1
                            from
                            (                    
                                select s_node_id,inlinkid,s_facilcls,s_inout_c,s_road_code,s_road_point,s_node,e_node,
                                    mid_change_symbol(node_lid) as node_lid_temp,regexp_split_to_array(mid_change_symbol(node_lid),E'\\\|+')::bigint[] as node_lid_array
                                from mid_hwy_ic_path as temp1
                                left join link_tbl as temp2
                                on temp1.outlinkid = temp2.link_id
                                where s_facilcls = 3
                            )temp3
                            
                            union
                            
                            select s_node_id,inlinkid,s_facilcls,s_inout_c,s_road_code,s_road_point,mid_change_symbol(node_lid) as node_lib1
                            from mid_hwy_ic_path
                            where s_facilcls <> 3
                        ) as a
                        left join mid_road_hwynode_middle_nodeid as b
                        on a.s_node_id = b.node_id and a.s_road_code = b.road_code and a.s_road_point = b.road_seq
                        left join mid_temp_multi_highway_node as c
                        on a.s_node_id = c.node_id
                        left join link_tbl as d
                        on (a.s_node_id = d.s_node and d.one_way_code in(1,3)) 
                            or (a.s_node_id = d.e_node and d.one_way_code in(1,2))
                        where c.num > 1 and a.s_inout_c = 2 and c.node_id is not null
                    )temp3
                    group by s_node_id,link_id,s_facilcls,s_inout_c,sp_name,s_road_code,s_road_point,type_array  
                )temp4
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end make OutMulti Path')
        return 0
    
    def __MakeEnterSinglePath(self):
        self.log.info('start make EnterSingle Path')
        
        sqlcmd = '''
            insert into mid_fac_name_temp(node_id,in_link_id,facilclass_c,inout_c,
                                    node_array,sp_name)
            (
                select distinct node_id,link_lib[array_upper(link_lib,1)] as in_link_id,facilclass_c,inout_c,
                    (case when array_upper(link_lib,1) > 2 then array_to_string(link_lib[1:array_upper(link_lib,1)-2],'|') || '|' || node_lib
                        else node_lib end) as node_array,sp_name
                from
                (
                    select node_id,(regexp_split_to_array(inlink_array[inlink_seq],E'\\\|+'))::bigint[] as link_lib
                        ,facilclass_c,inout_c,node_lib,sp_name
                    from
                    (
                        select node_id,inlink_array,facilclass_c,inout_c,node_array[path_seq_num] as node_lib,sp_name,
                            generate_series(1,array_upper(inlink_array,1)) as inlink_seq
                        from
                        (
                            select node_id, mid_get_inlink_array(node_id,in_link_id) as inlink_array,facilclass_c,inout_c,node_array,sp_name
                                ,generate_series(1, array_upper(node_array, 1)) as path_seq_num
                            from
                            (
                                select a.node_id, b.in_link_id,a.facilclass_c,a.inout_c,
                                    mid_get_single_path(b.full_pass_node,a.facilclass_c,false) as node_array,
                                    (case when name_kanji is null then null 
                                        else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name
                                from mid_road_hwynode_middle_nodeid as a
                                left join mid_temp_full_path as b
                                on a.node_id = b.node_id
                                left join mid_temp_multi_highway_node as c
                                on a.node_id = c.node_id
                                where (tollclass_c = 0 or tollclass_c = 4) and inout_c = 1 and c.num = 1
                            )temp
                            where node_array is not null   
                        )temp1
                    )temp2
                )temp3
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end make EnterSingle path')
        return 0
    
    def __MakeEnterMultiPath(self):
        self.log.info('start make EnterMulti path')
        
        sqlcmd = '''
            insert into mid_fac_name_temp(node_id,in_link_id,facilclass_c,inout_c,
                                    node_array,sp_name)
            (
                select distinct node_id,link_lib[array_upper(link_lib,1)] as in_link_id,facilclass_c,inout_c,
                    (case when array_upper(link_lib,1) > 2 then array_to_string(link_lib[1:array_upper(link_lib,1)-2],'|')|| '|' || node_lib
                        else node_lib end) as node_array,sp_name
                from
                (
                    select node_id,(regexp_split_to_array(inlink_array[inlink_seq],E'\\\|+'))::bigint[] as link_lib,
                        facilclass_c,inout_c,node_lib,sp_name
                    from
                    (
                        select node_id,inlink_array,facilclass_c,inout_c,node_array[path_seq_num] as node_lib,sp_name,
                            generate_series(1,array_upper(inlink_array,1)) as inlink_seq
                        from
                        (
                            select node_id, mid_get_inlink_array(node_id,in_link_id) as inlink_array,facilclass_c,inout_c,node_array,sp_name
                                ,generate_series(1, array_upper(node_array, 1)) as path_seq_num
                            from
                            (
                                select a.node_id, b.in_link_id ,a.facilclass_c,a.inout_c,
                                    mid_get_outmulti_path(a.road_code,a.road_seq,a.facilclass_c,b.full_pass_node,c.type_array,false) as node_array,
                                    (case when name_kanji is null then null 
                                        else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name
                                from mid_road_hwynode_middle_nodeid as a
                                left join mid_temp_full_path as b
                                on a.node_id = b.node_id
                                left join mid_temp_multi_highway_node as c
                                on a.node_id = c.node_id
                                --where (tollclass_c = 0 or tollclass_c = 4) and inout_c = 1 and c.num > 1
                                where (tollclass_c = 0 or tollclass_c = 4) and inout_c = 1
                            )temp
                            where node_array is not null   
                        )temp1
                    )temp2
                )temp3
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end make EnterMulti path')
        return 0
    
    def __AddJCTPath(self):
        self.log.info('start add jct path')
        '''
                        增加本线JCT
        '''
        sqlcmd = '''
            insert into mid_fac_name_temp(node_id,in_link_id,facilclass_c,inout_c,
                                    node_array,sp_name)
            (
                select node_id,in_link_id,facilclass_c,inout_c,node_array,sp_name
                from
                (
                    select a.node_id,a.in_link_id,a.facilclass_c,a.inout_c,c.node_lib,
                        (case when a.node_id = b.s_node then a.node_id::varchar || '|' || b.e_node::varchar else
                         a.node_id::varchar || '|' || b.s_node::varchar end) as node_array,a.sp_name
                    from
                    (
                        select distinct tp1.node_id,tp2.in_link_id,tp1.facilclass_c,tp1.inout_c,
                            (case when name_kanji is null then null 
                                    else mid_get_json_string_for_japan_name(name_kanji,name_yomi) end) as sp_name
                        from mid_road_hwynode_middle_nodeid as tp1
                        left join mid_fac_name_temp as tp2
                            on tp1.node_id = tp2.node_id
                        where tp1.facilclass_c = 3 and tp1.inout_c = 2 and tp1.tollclass_c = 0
                    )as a
                    left join link_tbl as b
                    on (a.node_id = b.s_node and b.one_way_code in (1,2))
                        or (a.node_id = b.e_node and b.one_way_code in (1,3))
                    left join 
                    (
                        select node_id,in_link_id,facilclass_c,inout_c,sp_name,array_agg(node_array) as node_lib
                        from mid_fac_name_temp
                        group by node_id,in_link_id,facilclass_c,inout_c,sp_name
                    ) as c
                    on a.node_id = c.node_id and a.in_link_id = c.in_link_id and 
                        a.facilclass_c = c.facilclass_c and a.inout_c = c.inout_c and a.sp_name = c.sp_name
                    where b.link_type in (1,2,3) and b.link_id is not null
                )temp
                where  mid_jude_path_appear(node_array,node_lib)
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end add jct path')
        
    def __Mergepath(self):
        self.log.info('start merge path for same name and type!')
        self.CreateFunction2('mid_jude_nodename')
        self.CreateFunction2('mid_get_first_in_link')
        '''
                        合并路径
        '''
        self.CreateIndex2('mid_fac_name_temp_nodeid_idx')
        #
        sqlcmd = '''
            INSERT INTO mid_fac_name_temp_shortpath(node_id,in_link_id,facilclass_c,inout_c, node_array,sp_name)
            (
                select distinct node_id,in_link_id,facilclass_c,inout_c,
                (case when name_flage then mid_get_first_in_link(node_id,unnest(node_lib)) else unnest(node_lib) end) as node_lib_one,sp_name
                from
                (
                    select node_id,in_link_id,facilclass_c,inout_c,(case when inout_c = 1 then mid_jude_nodename(node_id) else false end) as name_flage,
                        mid_cut_path(array_agg(node_array || '|' || 1::varchar)::varchar[]) as node_lib,sp_name
                    from mid_fac_name_temp
                    where node_id is not null
                    group by node_id,in_link_id,facilclass_c,inout_c,sp_name
                )temp          
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end merge path for same name and type!')
    
    
    def __Makelink(self):
        self.log.info('start transform node array to link array!')
        '''
                        转换为link
        '''
        self.CreateIndex2('mid_fac_name_temp_shortpath_nodeid_idx')
        #get link array
        sqlcmd = '''
            INSERT INTO mid_fac_name(node_id, in_link_id, out_link_id, pass_link, passlink_cnt, 
                facilclass_c, inout_c, sp_name )
            (
                select mid_get_middle_intersect_node(in_link_id, link_array[1]::bigint) as node_id
                    ,in_link_id,link_array[array_upper(link_array,1)] as out_link_id,
                    (case when array_upper(link_array,1) > 1 then array_to_string(link_array[1:array_upper(link_array,1)-1],'|')
                        else null end) as pass_link,
                    array_upper(link_array,1) as passlink_cnt,
                    facilclass_c,inout_c,sp_name
                from
                (
                    select distinct node_id,in_link_id,mid_get_link_array(node_array) as link_array,
                        facilclass_c,inout_c,sp_name
                    from mid_fac_name_temp_shortpath
                )temp
                where link_array is not null
            );
            '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end transform node array to link array!')    
        
    def _MakeFacName2Toward(self):
        '''把Facility name做到表towardname_tbl，但是namekind=2(地点名称)'''
        self.log.info('Insert Facility Name into TowardName table...')
        
        self.CreateFunction2('mid_convert_fac_name_guideattr')
        sqlcmd = """
                INSERT INTO towardname_tbl(
                            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                            direction, guideattr, namekind, namekind2, toward_name, "type")
                (
                SELECT gid, node_id, fromlinkid, tolinkid, passlid,
                       passlink_cnt,
                        0, mid_convert_fac_name_guideattr(facilclass_c) as guideattr, 
                        2 as namekind, 0, toward_name, 1
                  FROM (
                    SELECT gid, in_link_id as fromlinkid, 
                            node_id, 
                            out_link_id as tolinkid, 
                           pass_link  as passlid,
                           (CASE WHEN pass_link is null THEN 0 ELSE array_upper(regexp_split_to_array(pass_link, E'\\\|+'), 1) END) as passlink_cnt,
                           facilclass_c, inout_c, toward_name
                        from (
                            SELECT gid, node_id, in_link_id, out_link_id, pass_link,
                                facilclass_c, inout_c, sp_name as toward_name
                            FROM mid_fac_name
                            where node_id is not null and in_link_id is not null and out_link_id is not null
                                    and in_link_id <> out_link_id
                      ) as foo
                      
                      union
                      
                      select gid, inlinkid as fromlinkid, nodeid as node_id, outlinkid as tolinkid,
                          passlid, passlink_cnt, facilcls as facilclass_c, 2 as inout_c, sp_name as toward_name
                      from mid_hwy_facility_name_for_towardname
                       
                 ) AS d
                 order by gid
                );
            """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        
        return 0
    
    def _CreateView(self):
        #return 0
        self.CreateIndex2('name_dictionary_tbl_name_id_idx')
        
        sqlcmd = """
        create or replace view view_towardname_fac_name
        as 
        (
        SELECT towardname_tbl.gid, id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
               direction, guideattr, namekind, namekind2, towardname_tbl.toward_name, "type", the_geom
          FROM towardname_tbl
          left join node_tbl
          on nodeid = node_id
          where namekind = 2
        );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        return 0
    
    def _DoCheckNumber(self):
#        self._DoCheckJCT()
#        self._DoCheckSA()
#        self._DoCheckPA()
#        self._DoCheckICExit()
#        self._DoCheckICEnter()
#        self._DoCheckRampExit()
#        self._DoCheckRampEnter()
#        self._DoCheckSmartICExit()
#        self._DoCheckSmartICEnter()
        return 0
    
    def _DoCheckJCT(self):
        # 判断JCT出口和入口，是否少了
        # 注：这种方法的缺点是，不能保证JCT出口到入口两两之间的关系都能找出来
        sqlcmd = """
            select count(*)
            from (
                    -- JCT OUT
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 3 and inout_c = 2 and
                            node_id not in (
                            select distinct node_id
                              from mid_temp_facilit_matched
                              where facilclass_c = 3
                            )
                    union  
                    -- JCT IN
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 3 and inout_c = 1 and
                            node_id not in (
                            select distinct to_node
                              from mid_temp_facilit_matched
                              where facilclass_c = 3
                            )
              ) as a;
            """
            
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d JCT, not included.' % rst)
        return 0
    
    def _DoCheckSA(self):
        # SA
        sqlcmd = """
            select count(*)
            from (  -- SA Out
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 1 and inout_c = 2 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 1 --and inout_c = 2
                            )
                    union  
                    -- SA IN
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 1 and inout_c = 1 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 1 --and inout_c = 2
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d SA, not included.' % rst)
        return 0 
     
    def _DoCheckPA(self):
        # PA
        sqlcmd = """
            select count(*)
            from (
                    -- PA Out
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 2 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 2 --and inout_c = 2
                            )
                    union  
            
                    -- PA IN
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 2 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 2 --and inout_c = 2
                            )
              ) as a;
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d PA, not included.' % rst)
        return 0
    
    def _DoCheckICExit(self):
        # IC Exit
        # 有两个点没有出来Node_id: 4358709 和 2942207
        # 其中node_id: 2942207，好像是原数据tollclass_c做错了,做成了4
        sqlcmd = """
            select count(*)
            from (  -- IC exit on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 2
                            )
                    union
                    -- IC exit on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 2 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 2
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d IC Exit, not included.' % rst)
        return 0
    
    def _DoCheckICEnter(self):
        # IC Enter
        # 有两个点没有出来Node_id: 4358708 和 2942205
        # 其中node_id: 2942205，好像是原数据tollclass_c做错了,做成了4
        sqlcmd = """
            select count(*)
            from (  -- IC enter on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 1
                            )
                    union
                    -- IC enter on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 5 and inout_c = 1 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 5 and inout_c = 1
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d IC Enter, not included.' % rst)
        return 0
    
    def _DoCheckRampExit(self):
        # Ramp 出口
        # 有两个点没有出来Node_id: 4358709 和 2942207
        # 其中node_id: 2942207，好像是数据做错了,tollclass_c做成了4
        sqlcmd = """
            select count(*)
            from (  -- Ramp exit on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 2
                            )
                    union  
                    -- Ramp exit on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 2 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 2
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Ramp Exit, not included.' % rst)
        return 0
    
    def _DoCheckRampEnter(self):
        # Ramp 入口
        sqlcmd = """
            select count(*)
            from (  -- Ramp enter on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 1
                            )
                    union
                    -- Ramp enter on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 4 and inout_c = 1 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 4 and inout_c = 1
                            )
              ) as a;
          """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Ramp Enter, not included.' % rst)
        return 0

    def _DoCheckSmartICExit(self):
        # Smart IC Exit
        # 注：Smart IC有很多没有做成，是因为有SAPA存在。当SAPA同时存在时，SmartIC不做成。
        sqlcmd = """
            select count(*)
            from (  -- SmartIC exit on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 2 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 2
                            )
                    union  
                    -- SmartIC exit on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 2 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 2
                            )
              ) as a;
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Smart IC Exit, not included.' % rst)
        return 0
    
    def _DoCheckSmartICEnter(self):
        # Smart IC Enter
        sqlcmd = """
            select count(*)
            from (  -- Ramp enter on HWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 1 and tollclass_c = 0 and
                            node_id not in (
                                    select distinct to_node
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 1
                            )
                    union
                    -- Ramp enter on NWY
                    SELECT distinct node_id
                      FROM highway_node_add_nodeid
                      where facilclass_c = 7 and inout_c = 1 and tollclass_c = 4 and
                            node_id not in (
                                    select distinct node_id
                                      from mid_temp_facilit_matched
                                      where facilclass_c = 7 and inout_c = 1
                            )
              ) as a;
            """
        rst = self._CheckResult(sqlcmd)
        if rst > 0:
            self.log.warning('There are %d Smart IC Enter, not included.' % rst)
        return 0
    
    def _CheckResult(self, sqlcmd):
        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            if row[0] > 0: 
                return row[0]
        return 0