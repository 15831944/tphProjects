# -*- coding: cp936 -*-

from common import cache_file
import component.component_base
import common.common_func

class comp_link_dupli_or_circle(component.component_base.comp_base):

    def __init__(self):

        component.component_base.comp_base.__init__(self, 'Link_overlay_or_circle')

    def _Do(self):

#        self.__recovery_tables()

        self.__prepare_dealing()
        self.__dupli_link_dealing()

        self.__circle_link_dealing()

        self.__check()
        return 0

    def __prepare_dealing(self):

        self.CreateIndex2('link_tbl_s_node_e_node_idx')
        self.CreateTable2('dupli_link')

        self.CreateIndex2('dupli_link_gid_idx')
        self.CreateIndex2('dupli_link_link_id_idx')
        self.CreateIndex2('dupli_link_the_geom_idx')
        self.CreateIndex2('dupli_link_s_node_e_node_idx')
        return 0

    def __dupli_link_dealing(self):

        self.CreateTable2('temp_dupli_link_tmp')
        self.CreateTable2('temp_dupli_link')
        self.CreateIndex2('temp_dupli_link_aid_idx')

        self.__backup_tables_dupli()

        self.__dupli_link_dealing_link()
        self.__dupli_link_dealing_node()
        self.__dupli_link_dealing_guide()
        self.__dupli_link_dealing_regulation()
        self.__dupli_link_dealing_safetyzone()
        self.__dupli_link_dealing_safetyalert()

        return 0

    def __circle_link_dealing(self):

        self.__backup_tables_circle()

        self.CreateTable2('temp_circle_link')
        self.CreateIndex2('temp_circle_link_ageom_idx')
        self.CreateIndex2('temp_circle_link_aid_idx')

        sqlcmd = """
            drop sequence if exists temp_new_link_id_seq;
            create sequence temp_new_link_id_seq;
            select setval('temp_new_link_id_seq', cast(max_id as bigint) + 1000000)
                from
                (
                select max(link_id) as max_id
                from link_tbl
                )as a;

            drop sequence if exists temp_new_node_id_seq;
            create sequence temp_new_node_id_seq;
            select setval('temp_new_node_id_seq', cast(max_id as bigint) + 1000000)
            from
            (
                select max(node_id) as max_id
                from node_tbl
            )as a;
           """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        self.CreateFunction2('mid_split_circle_link')
        self.CreateTable2('temp_circle_link_new_seq_link')

        self.__circle_link_dealing_regulation()
        self.__circle_link_dealing_guide()
        self.__circle_link_dealing_link()
        self.__circle_link_dealing_node()
        self.__circle_link_dealing_safetyzone()
        self.__circle_link_dealing_safetyalert()

        return 0

    def __backup_tables_dupli(self):
        self.log.info('backup mid tables...')

        sqlcmd = """
                drop table if exists [the_guide_tbl]_bak_dupli_dealing;
                create table [the_guide_tbl]_bak_dupli_dealing
                as select  *
                    from [the_guide_tbl];
                """

        guide_table_list = [
                            'link_tbl',
                            'node_tbl',
                            'regulation_item_tbl',
                            'regulation_relation_tbl',
                            'spotguide_tbl',
                            'signpost_tbl',
                            'signpost_uc_tbl',
                            'lane_tbl',
                            'force_guide_tbl',
                            'towardname_tbl',
                            'caution_tbl',
                            'safety_zone_tbl',
                            'safety_alert_tbl',
                            'hook_turn_tbl'
                            ]

        for the_guide_table in guide_table_list:
#            self.log.info('backuping %s...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()

        self.log.info('backup mid tables end.')

    def __backup_tables_circle(self):
        self.log.info('backup mid tables...')

        sqlcmd = """
                drop table if exists [the_guide_tbl]_bak_circle_dealing;
                create table [the_guide_tbl]_bak_circle_dealing
                as select  *
                    from [the_guide_tbl]
                """

        guide_table_list = [
                            'link_tbl',
                            'node_tbl',
                            'regulation_item_tbl',
                            'regulation_relation_tbl',
                            'spotguide_tbl',
                            'signpost_tbl',
                            'signpost_uc_tbl',
                            'lane_tbl',
                            'force_guide_tbl',
                            'towardname_tbl',
                            'caution_tbl',
                            'safety_zone_tbl',
                            'safety_alert_tbl',
                            'hook_turn_tbl'
                            ]

        for the_guide_table in guide_table_list:
#            self.log.info('backuping %s...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()

        self.log.info('backup mid tables end.')

    def __recovery_tables(self):
        self.log.info('recovery tables...')

        sqlcmd = """
                drop table if exists [the_guide_tbl];
                create table [the_guide_tbl]
                as select  *
                    from [the_guide_tbl]_bak_dupli_dealing
                """

        guide_table_list = [
                            'link_tbl',
                            'node_tbl',
                            'regulation_item_tbl',
                            'regulation_relation_tbl',
                            'spotguide_tbl',
                            'signpost_tbl',
                            'signpost_uc_tbl',
                            'lane_tbl',
                            'force_guide_tbl',
                            'towardname_tbl',
                            'caution_tbl',
                            'safety_zone_tbl',
                            'safety_alert_tbl',
                            'hook_turn_tbl',
                            'natural_guidence_tbl'
                            ]

        for the_guide_table in guide_table_list:
            self.log.info('recoverying %s...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()

        self.CreateIndex2('link_tbl_s_node_e_node_idx')
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        self.CreateIndex2('link_tbl_bak_dupli_dealing_link_id_idx')

        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')
        self.CreateIndex2('lane_tbl_node_id_idx')
        self.CreateIndex2('lane_tbl_inlinkid_idx')
        self.CreateIndex2('lane_tbl_outlinkid_idx')
        self.CreateIndex2('force_guide_tbl_node_id_idx')
        self.CreateIndex2('signpost_tbl_node_id_idx')
        self.CreateIndex2('spotguide_tbl_node_id_idx')
        self.CreateIndex2('towardname_tbl_node_id_idx')
#        self.CreateIndex2('name_dictionary_tbl_name_id_idx')
#        self.CreateIndex2('link_name_relation_tbl_link_id_idx')
#        self.CreateIndex2('link_name_relation_tbl_name_id_idx')
#        self.CreateIndex2('link_shield_tbl_link_id_idx')
#
#        sqlcmd = """
#
#                alter table [the_guide_tbl] drop column gid;
#                alter table [the_guide_tbl] add column gid serial not null;
#                """
#
#        guide_table_list_gid = [
#                            'link_name_relation_tbl',
#                            'name_dictionary_tbl',
#                            'link_shield_tbl',
#                            ]
#
#        for the_guide_table in guide_table_list_gid:
#            self.log.info('create gid: %s...' % the_guide_table)
#            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
#            self.pg.execute2(the_sql_cmd)
#            self.pg.commit2()
        self.log.info('recovery tables end.')

    def __dupli_link_dealing_link(self):
        self.CreateIndex2('link_tbl_bak_dupli_dealing_link_id_idx')
        self.log.info('dealing dupli_link --- link_tbl ...')
        self.CreateTable2('temp_dupli_name_shield')
        # 合并重叠link的名称和番号，结果存在表：temp_dupli_name_shield_comp
        # 注：必须放在删除link_tbl表之前
        self.__merge_dupli_link_name_shield()
        self.CreateTable2('link_tbl')
        sqlcmd = """
           insert into link_tbl
                    (
                        link_id, iso_country_code, tile_id, s_node, e_node, link_type, road_type, toll, speed_class, length, function_class,
                        lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn,
                        speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                        one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number,
                        name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish,
                        access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, 
                        erp, rodizio, soi, display_class, fazm, tazm, feature_string, feature_key, the_geom
                    )
                    (
                        select  link_id, iso_country_code, tile_id, s_node, e_node, link_type, road_type, toll, speed_class, length, function_class,
                                    lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn,
                                    speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                    one_way_code, one_way_condition, pass_code, pass_code_condition
                                    , case when name is not null then name
                                        else a.road_name end as road_name
                                    , case when shield is not null then shield
                                        else a.road_number end as road_number,
                                    name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish,
                                    access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, 
                                    erp, rodizio, soi, display_class, fazm, tazm, feature_string, feature_key, the_geom
                        from (
                            select  a.*, b.aid, name.name, shield.shield
                            from link_tbl_bak_dupli_dealing as a
                            left join temp_dupli_link as b
                            on a.link_id = b.aid
                            left join temp_dupli_name_shield_comp name
                            on a.link_id = name.id
                            left join temp_dupli_name_shield_comp shield
                            on a.link_id = shield.id
                        ) a where aid is null
                    );
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')

        self.log.info('end of dealing dupli_link --- link_tbl ...')

        return 0

    def __merge_dupli_link_name_shield(self):
        '''合并重叠link的名称和番号'''
        self.CreateTable2('temp_dupli_name_shield_comp')
        sqlcmd = """
            SELECT array_agg(link_id_a) as link_del, link_id_b
            from temp_dupli_name_shield
            group by link_id_b;
        """
        import subproj
        proj_object = subproj.proj_factory.proj_factory.instance().createProject(common.common_func.GetPath('proj_name'))
        comp_factory = proj_object.comp_factory
        proc_dict = comp_factory.CreateOneComponent('Dictionary')
        proc_dict.set_language_code()
        temp_file_obj = cache_file.open('diff_name')  # 创建临时文件
        features = self.get_batch_data(sqlcmd)
        for feature in features:
            link_id_del = feature[0]
            link_id_keep = feature[1]
            link_id_list = link_id_del
            link_id_list.append(link_id_keep)
            rtn_name_shield = proc_dict.merge_links_name(link_id_list)
            rtn_name, rtn_shield = rtn_name_shield
            if not rtn_name:
                rtn_name = ''
            if not rtn_shield:
                rtn_shield = ''
            temp_file_obj.write('%d\t%s\t%s\n' %
                                (link_id_keep, rtn_name, rtn_shield))

        temp_file_obj.seek(0)
        self.pg.copy_from2(temp_file_obj, 'temp_dupli_name_shield_comp')
        self.pg.commit2()
        #temp_file_obj.close()
        cache_file.close(temp_file_obj,True)

    def __dupli_link_dealing_node(self):

        self.log.info('dealing dupli_link --- node_tbl ...')
        self.CreateTable2('node_tbl')

        sqlcmd = """
                insert into node_tbl(
                    node_id,kind,light_flag,stopsign_flag,toll_flag,bifurcation_flag,
                    org_boundary_flag,tile_boundary_flag,mainnodeid,node_lid,
                    node_name,feature_string,feature_key,the_geom)
                select node_id,kind,light_flag,stopsign_flag,toll_flag,bifurcation_flag,
                    org_boundary_flag,tile_boundary_flag,mainnodeid,
                    array_to_string(array_agg(conne_link),'|') as node_lid,
                    node_name,feature_string,feature_key,the_geom
                from (
                    select  a.node_id,
                        a.kind,
                        a.light_flag,
                        a.stopsign_flag,
                        a.toll_flag,
                        a.bifurcation_flag,
                        a.org_boundary_flag,
                        a.tile_boundary_flag,
                        a.mainnodeid,
                        case when b.aid is not null then b.bid else a.conne_link end as conne_link,
                        a.node_name,
                        a.feature_string,
                        a.feature_key,
                        a.the_geom
                    from (
                            select node_id,kind,light_flag,stopsign_flag,toll_flag,bifurcation_flag,
                            org_boundary_flag,tile_boundary_flag,mainnodeid,
                            cast(unnest(string_to_array(node_lid,'|')) as bigint) as conne_link,
                            node_name,feature_string,feature_key,the_geom
                            from node_tbl_bak_dupli_dealing
                     ) a
                    left join temp_dupli_link b
                    on a.conne_link = b.aid
                ) c
                group by node_id,kind,light_flag,stopsign_flag,toll_flag,bifurcation_flag,
                org_boundary_flag,tile_boundary_flag,mainnodeid,node_name,feature_string,feature_key,the_geom;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('node_tbl_node_id_idx')
        self.CreateIndex2('node_tbl_the_geom_idx')

        self.log.info('end of dealing dupli_link --- node_tbl ...')

        return 0

    def __dupli_link_dealing_guide(self):

        self.log.info('dealing dupli_link --- guide ...')
        self.CreateTable2('caution_tbl')

        sqlcmd = """

        insert into caution_tbl(id,inlinkid,nodeid,outlinkid,passlid,passlink_cnt,data_kind,voice_id)
        select gid,inlinkid,nodeid,outlinkid,array_to_string(array_agg(passlink),'|') as passlid,passlink_cnt,data_kind,voice_id from (
            select * from (
                select  a.gid,
                    case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                    a.nodeid,
                    case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,
                    case when d.aid is not null then d.bid else a.passlink end as passlink,
                    a.passlink_cnt,a.link_index,a.data_kind,a.voice_id
                from (
                select gid,nodeid,inlinkid,outlinkid,case when link_array is not null then  cast(unnest(link_array) as bigint) else null end as passlink,
                    passlink_cnt,case when link_array is not null then generate_series(1, passlink_cnt) else 1 end as link_index,data_kind,voice_id
                from (
                    select gid,inlinkid,nodeid,outlinkid,case when passlid <> '' and passlid is not null then string_to_array(passlid,'|') else null end as link_array,passlink_cnt,data_kind,voice_id
                    from caution_tbl_bak_dupli_dealing
                ) a
                      ) a
                    left join temp_dupli_link b
                    on a.inlinkid = b.aid
                    left join temp_dupli_link c
                    on a.outlinkid = c.aid
                    left join temp_dupli_link d
                    on a.passlink = d.aid
            ) a order by gid,inlinkid,nodeid,outlinkid,passlink_cnt,data_kind,voice_id,link_index
        ) a group by gid,inlinkid,nodeid,outlinkid,passlink_cnt,data_kind,voice_id ;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()


        self.CreateTable2('force_guide_tbl')

        sqlcmd = """

        insert into force_guide_tbl(force_guide_id,nodeid,inlinkid,outlinkid,passlid,passlink_cnt,guide_type,position_type)
        select force_guide_id,nodeid,inlinkid,outlinkid,array_to_string(array_agg(passlink),'|') as passlid,passlink_cnt,guide_type,position_type from (
            select * from (
                select  a.force_guide_id,
                a.nodeid,
                case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,
                case when d.aid is not null then d.bid else a.passlink end as passlink,
                a.passlink_cnt,a.link_index,a.guide_type,a.position_type
                from (
                    select force_guide_id,nodeid,inlinkid,outlinkid,case when link_array is not null then  cast(unnest(link_array) as bigint) else null end as passlink,
                        passlink_cnt,case when link_array is not null then generate_series(1, passlink_cnt) else 1 end as link_index,guide_type,position_type
                    from (
                        select force_guide_id,nodeid,inlinkid,outlinkid,case when passlid <> '' and passlid is not null then string_to_array(passlid,'|') else null end as link_array,passlink_cnt,guide_type,position_type
                        from force_guide_tbl_bak_dupli_dealing
                    ) a
                  ) a
                left join temp_dupli_link b
                on a.inlinkid = b.aid
                left join temp_dupli_link c
                on a.outlinkid = c.aid
                left join temp_dupli_link d
                on a.passlink = d.aid
            ) a order by force_guide_id,nodeid,inlinkid,outlinkid,passlink_cnt,guide_type,position_type,link_index
        ) a group by force_guide_id,nodeid,inlinkid,outlinkid,passlink_cnt,guide_type,position_type;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()


        self.CreateTable2('lane_tbl')

        sqlcmd = """

        insert into lane_tbl(id,nodeid,inlinkid,outlinkid,passlid,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo)
        select gid,nodeid,inlinkid,outlinkid,array_to_string(array_agg(passlink),'|') as passlid,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo from (
            select * from (
                select  a.gid,
                a.nodeid,
                case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,
                case when d.aid is not null then d.bid else a.passlink end as passlink,
                a.passlink_cnt,a.link_index,a.lanenum,a.laneinfo,a.arrowinfo,a.lanenuml,a.lanenumr,a.buslaneinfo
                from (
                    select gid,nodeid,inlinkid,outlinkid,case when link_array is not null then  cast(unnest(link_array) as bigint) else null end as passlink,
                        passlink_cnt,case when link_array is not null then generate_series(1, passlink_cnt) else 1 end as link_index,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo
                    from (
                        select gid,nodeid,inlinkid,outlinkid,case when passlid <> '' and passlid is not null then string_to_array(passlid,'|') else null end as link_array,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo
                        from lane_tbl_bak_dupli_dealing
                    ) a
                  ) a
                left join temp_dupli_link b
                on a.inlinkid = b.aid
                left join temp_dupli_link c
                on a.outlinkid = c.aid
                left join temp_dupli_link d
                on a.passlink = d.aid
            ) a order by gid,nodeid,inlinkid,outlinkid,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo,link_index
          ) a group by gid,nodeid,inlinkid,outlinkid,passlink_cnt,lanenum,laneinfo,arrowinfo,lanenuml,lanenumr,buslaneinfo;                   """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateTable2('signpost_uc_tbl')

        sqlcmd = """

        insert into signpost_uc_tbl(id,nodeid,inlinkid,outlinkid,passlid,passlink_cnt,sp_name,route_no1,route_no2,route_no3,route_no4,exit_no)
        select id,nodeid,inlinkid,outlinkid,array_to_string(array_agg(passlink),'|') as passlid,passlink_cnt,sp_name,route_no1,route_no2,route_no3,route_no4,exit_no from (
            select * from (
                select  a.id,
                a.nodeid,
                case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,
                case when d.aid is not null then d.bid else a.passlink end as passlink,
                a.passlink_cnt,a.link_index,a.sp_name,route_no1,a.route_no2,route_no3,route_no4,a.exit_no
                from (
                    select id,nodeid,inlinkid,outlinkid,case when link_array is not null then  cast(unnest(link_array) as bigint) else null end as passlink,
                        passlink_cnt,case when link_array is not null then generate_series(1, passlink_cnt) else 1 end as link_index,sp_name,route_no1,route_no2,route_no3,route_no4,exit_no
                    from (
                            select id,nodeid,inlinkid,outlinkid,case when passlid <> '' and passlid is not null then string_to_array(passlid,'|') else null end as link_array,passlink_cnt,sp_name,route_no1,route_no2,route_no3,route_no4,exit_no
                            from signpost_uc_tbl_bak_dupli_dealing
                        ) a
                  ) a
                left join temp_dupli_link b
                on a.inlinkid = b.aid
                left join temp_dupli_link c
                on a.outlinkid = c.aid
                left join temp_dupli_link d
                on a.passlink = d.aid
            ) a order by id,nodeid,inlinkid,outlinkid,passlink_cnt,sp_name,route_no1,route_no2,route_no3,route_no4,exit_no,link_index
        ) a group by id,nodeid,inlinkid,outlinkid,passlink_cnt,sp_name,route_no1,route_no2,route_no3,route_no4,exit_no;
                   """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateTable2('spotguide_tbl')

        sqlcmd = """
            insert into spotguide_tbl(id,nodeid,inlinkid,outlinkid,passlid,passlink_cnt,direction,guideattr,namekind,guideclass,patternno,arrowno,type)
            select id,nodeid,inlinkid,outlinkid,array_to_string(array_agg(passlink),'|') as passlid,passlink_cnt,direction,guideattr,namekind,guideclass,patternno,arrowno,type from (
                select * from (
                    select  a.id,
                            a.nodeid,
                            case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                            case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,
                            case when d.aid is not null then d.bid else a.passlink end as passlink,
                            a.passlink_cnt,a.link_index,a.direction,a.guideattr,a.namekind,a.guideclass,a.patternno,a.arrowno,a.type
                        from (
                            select id,nodeid,inlinkid,outlinkid,case when link_array is not null then  cast(unnest(link_array) as bigint) else null end as passlink,
                                passlink_cnt,case when link_array is not null then generate_series(1, passlink_cnt) else 1 end as link_index,direction,guideattr,namekind,guideclass,patternno,arrowno,type
                            from (

                                select id,nodeid,inlinkid,outlinkid,case when passlid <> '' and passlid is not null then string_to_array(passlid,'|') else null end as link_array,passlink_cnt,direction,guideattr,namekind,guideclass,patternno,arrowno,type
                                from spotguide_tbl_bak_dupli_dealing
                                ) a
                              ) a
                    left join temp_dupli_link b
                    on a.inlinkid = b.aid
                    left join temp_dupli_link c
                    on a.outlinkid = c.aid
                    left join temp_dupli_link d
                    on a.passlink = d.aid
                ) a order by id,nodeid,inlinkid,outlinkid,passlink_cnt,direction,guideattr,namekind,guideclass,patternno,arrowno,type,link_index
            ) a group by id,nodeid,inlinkid,outlinkid,passlink_cnt,direction,guideattr,namekind,guideclass,patternno,arrowno,type;
            """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateTable2('towardname_tbl')

        sqlcmd = """
            insert into towardname_tbl(id,nodeid,inlinkid,outlinkid,passlid,passlink_cnt,direction,guideattr,namekind,namekind2,toward_name,type)
            select gid,nodeid,inlinkid,outlinkid,array_to_string(array_agg(passlink),'|') as passlid,passlink_cnt,direction,guideattr,namekind,namekind2,toward_name,type from (
                select * from (
                    select  a.gid,
                            a.nodeid,
                            case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                            case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,
                            case when d.aid is not null then d.bid else a.passlink end as passlink,
                            a.passlink_cnt,a.link_index,a.direction,a.guideattr,a.namekind,a.namekind2,a.toward_name,a.type
                    from (
                            select gid,nodeid,inlinkid,outlinkid,case when link_array is not null then  cast(unnest(link_array) as bigint) else null end as passlink,
                                passlink_cnt,case when link_array is not null then generate_series(1, passlink_cnt) else 1 end as link_index,direction,guideattr,namekind,namekind2,toward_name,type
                            from (
                                    select gid,nodeid,inlinkid,outlinkid,case when passlid <> '' and passlid is not null then string_to_array(passlid,'|') else null end as link_array,passlink_cnt,direction,guideattr,namekind,namekind2,toward_name,type
                                    from towardname_tbl_bak_dupli_dealing
                            ) a
                     ) a
                    left join temp_dupli_link b
                    on a.inlinkid = b.aid
                    left join temp_dupli_link c
                    on a.outlinkid = c.aid
                    left join temp_dupli_link d
                    on a.passlink = d.aid
                ) a order by gid,nodeid,inlinkid,outlinkid,passlink_cnt,direction,guideattr,namekind,namekind2,toward_name,type,link_index
            ) a group by gid,nodeid,inlinkid,outlinkid,passlink_cnt,direction,guideattr,namekind,namekind2,toward_name,type
            ;
                   """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateTable2('hook_turn_tbl')

        sqlcmd = """
            insert into hook_turn_tbl(id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt)
            select gid,nodeid,inlinkid,outlinkid,array_to_string(array_agg(passlink),'|') as passlid,passlink_cnt from (
                select * from (
                    select  a.gid,
                            a.nodeid,
                            case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                            case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,
                            case when d.aid is not null then d.bid else a.passlink end as passlink,
                            a.passlink_cnt,a.link_index
                    from (
                            select gid,nodeid,inlinkid,outlinkid,case when link_array is not null then  cast(unnest(link_array) as bigint) else null end as passlink,
                                passlink_cnt,case when link_array is not null then generate_series(1, passlink_cnt) else 1 end as link_index
                            from (
                                    select gid,nodeid,inlinkid,outlinkid,case when passlid <> '' and passlid is not null then string_to_array(passlid,'|') else null end as link_array,passlink_cnt
                                    from hook_turn_tbl_bak_dupli_dealing
                            ) a
                     ) a
                    left join temp_dupli_link b
                    on a.inlinkid = b.aid
                    left join temp_dupli_link c
                    on a.outlinkid = c.aid
                    left join temp_dupli_link d
                    on a.passlink = d.aid
                ) a order by gid,nodeid,inlinkid,outlinkid,passlink_cnt,link_index
            ) a group by gid,nodeid,inlinkid,outlinkid,passlink_cnt
            ;
                   """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()


        self.log.info('end of dealing dupli_link --- guide ...')

        return 0

    def __dupli_link_dealing_regulation(self):

        self.log.info('dealing dupli_link --- regulation ...')
        self.CreateTable2('regulation_item_tbl')

        sqlcmd = """
            insert into regulation_item_tbl(regulation_id,linkid,nodeid,seq_num)
            select a.regulation_id,case when b.aid is not null then b.bid else a.linkid end as linkid,a.nodeid,a.seq_num from regulation_item_tbl_bak_dupli_dealing a
            left join temp_dupli_link b
            on a.linkid = b.aid;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateTable2('regulation_relation_tbl')

        sqlcmd = """
            insert into regulation_relation_tbl(regulation_id,nodeid,inlinkid,outlinkid,condtype,cond_id,gatetype,slope)
            select  a.regulation_id,a.nodeid,
                case when b.aid is not null then b.bid else a.inlinkid end as inlinkid,
                case when c.aid is not null then c.bid else a.outlinkid end as outlinkid,a.condtype,a.cond_id,a.gatetype,a.slope
                from regulation_relation_tbl_bak_dupli_dealing a
            left join temp_dupli_link b
            on a.inlinkid = b.aid
            left join temp_dupli_link c
            on a.outlinkid = c.aid;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.log.info('end of dealing dupli_link --- regulation ...')

        return 0
    def __dupli_link_dealing_safetyzone(self):
        
        self.log.info('dealing dupli_link --- safetyzone ...')

        sqlcmd = """
            update safety_zone_tbl a
            set linkid=b.bid,
            direction=case when b.node_inverse=0 then a.direction else 3-a.direction end
            from temp_dupli_link b
            where a.linkid=b.aid
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end of dealing dupli_link --- safetyzone ...')
        return 0

    def __dupli_link_dealing_safetyalert(self):
        
        self.log.info('dealing dupli_link --- safetyalert ...')

        sqlcmd = """
            update safety_alert_tbl a
            set link_id=b.bid,
            s_dis=case when b.node_inverse=0 then a.s_dis else a.e_dis end,
            e_dis=case when b.node_inverse=1 then a.s_dis else a.e_dis end 
            from temp_dupli_link b
            where a.link_id=b.aid;
            
            update safety_alert_tbl a
            set orglink_id=b.bid
            from temp_dupli_link b
            where a.orglink_id=b.aid;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end of dealing dupli_link --- safetyalert ...')  
        return 0  
        
    def __circle_link_dealing_link(self):

        self.log.info('dealing circle_link --- link_tbl ...')

        self.CreateFunction2('mid_cal_zm')

        sqlcmd = """
                   insert into link_tbl
                    (
                        link_id, iso_country_code, tile_id, s_node, e_node, link_type, road_type, toll, speed_class, length, function_class,
                        lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn,
                        speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                        one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number,
                        name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish,
                        access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, soi,
                        display_class, fazm, tazm, feature_string, feature_key, the_geom
                    )
                    (
                        select  b.link_id, a.iso_country_code, b.tile_id, b.s_node, b.e_node, link_type, road_type, toll, speed_class, ST_Length_Spheroid(b.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length,
                                function_class, lane_dir, lane_num_s2e, lane_num_e2s, elevated, structure, tunnel, rail_cross, paved, uturn,
                                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, width_s2e, width_e2s,
                                one_way_code, one_way_condition, pass_code, pass_code_condition, road_name, road_number,
                                name_type, ownership, car_only, slope_code, slope_angle, disobey_flag, up_down_distinguish,
                                access, extend_flag, etc_only_flag, bypass_flag, matching_flag, highcost_flag, ipd, urban, erp, rodizio, soi,
                                display_class, mid_cal_zm(b.the_geom, 1) as fazm, mid_cal_zm(b.the_geom, -1) as tazm, feature_string, feature_key,
                                b.the_geom as the_geom
                        from link_tbl as a
                        inner join temp_circle_link_new_seq_link as b
                        on a.link_id = b.old_link_id
                    );

                    delete from link_tbl as a
                    using (select distinct old_link_id from temp_circle_link_new_seq_link) as b
                    where a.link_id = b.old_link_id;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.log.info('end of dealing circle_link --- link_tbl ...')

        return 0

    def __circle_link_dealing_node(self):

        self.log.info('dealing circle_link --- node_tbl ...')

        sqlcmd = """
            insert into node_tbl (node_id, kind, light_flag,stopsign_flag, toll_flag, bifurcation_flag,
                mainnodeid, node_lid, node_name, feature_string, feature_key, the_geom )
            select node_id,null as kind,null as light_flag,null as stopsign_flag,
                null as toll_flag,null as bifurcation_flag,0 as mainnodeid,
                array_to_string(array_agg(link_id),'|'), null as node_name, 
                old_link_id::varchar as feature_string,
                md5(old_link_id::varchar) as feature_key,
                the_geom 
            from (
                select e_node as node_id,link_id, old_link_id, e_geom as the_geom
                    from temp_circle_link_new_seq_link where sub_index = 1
                union
                select s_node as node_id,link_id, old_link_id, e_geom as the_geom
                    from temp_circle_link_new_seq_link where sub_index = 2
            ) a group by node_id, old_link_id, the_geom;
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('end of dealing circle_link --- node_tbl ...')

        return 0

    def __circle_link_dealing_guide(self):

        self.log.info('dealing circle_link --- guide ...')

        sqlcmd = """
                drop index if exists [the_guide_tbl]_inlinkid_idx;
                CREATE INDEX [the_guide_tbl]_inlinkid_idx
                    ON [the_guide_tbl]
                    USING btree
                    (inlinkid);

                drop index if exists [the_guide_tbl]_outlinkid_idx;
                CREATE INDEX [the_guide_tbl]_outlinkid_idx
                    ON [the_guide_tbl]
                    USING btree
                    (outlinkid);

                drop index if exists [the_guide_tbl]_passlid_idx;
                CREATE INDEX [the_guide_tbl]_passlid_idx
                    ON [the_guide_tbl]
                    USING btree
                    (passlid);

                drop index if exists temp_circle_link_new_seq_link_old_link_id_idx;
                CREATE INDEX temp_circle_link_new_seq_link_old_link_id_idx
                    ON temp_circle_link_new_seq_link
                    USING btree
                    (old_link_id);

                drop table if exists temp_circle_link_update_linkrow_[the_guide_tbl];
                create table temp_circle_link_update_linkrow_[the_guide_tbl]
                as
                (
                    select  gid,
                            link_num,
                            link_array,
                            nodeid,
                            link_array[1] as inlinkid,
                            (case when link_num = 1 then null else link_array[link_num] end) as outlinkid,
                            (case when link_num > 2 then (link_num - 2) else 0 end) as passlink_cnt,
                            array_to_string(link_array[2:link_num-1], '|') as passlid
                    from
                    (
                        select  gid, nodeid, link_array, array_upper(link_array, 1) as link_num
                        from
                        (
                            select  gid, nodeid,
                                    rdb_get_split_linkrow_circle_dealing(nodeid, link_array, s_array, e_array) as link_array
                            from
                            (
                                select     gid,
                                        nodeid,
                                        array_agg(link_id) as link_array,
                                        array_agg(s_node) as s_array,
                                        array_agg(e_node) as e_array
                                from
                                (
                                    select     c.gid, c.nodeid, c.link_index, c.link_id, d.s_node, d.e_node
                                    from
                                    (
                                        select    gid, nodeid, link_index, link_array[link_index] as link_id
                                        from
                                        (
                                            select  gid,
                                                    nodeid,
                                                    link_num,
                                                    link_array,
                                                    generate_series(1,link_num) as link_index
                                            from
                                            (
                                                select     [the_guide_tbl].gid,
                                                    nodeid,
                                                    (passlink_cnt + 2) as link_num,
                                                    (
                                                    case
                                                    when passlink_cnt = 0 then ARRAY[inlinkid, outlinkid]
                                                    else ARRAY[inlinkid] || cast(string_to_array(passlid, '|') as bigint[]) || ARRAY[outlinkid]
                                                    end
                                                    )as link_array
                                                from [the_guide_tbl]
                                            )as a
                                        )as b
                                    )as c
                                    left join link_tbl as d
                                    on c.link_id = d.link_id
                                    order by c.gid, c.nodeid, c.link_index
                                )as a
                                group by gid, nodeid
                            )as b
                        )as c
                    )as d
                );

                CREATE INDEX temp_circle_link_update_linkrow_[the_guide_tbl]_gid_idx
                    ON temp_circle_link_update_linkrow_[the_guide_tbl]
                    USING btree
                    (gid);

                update [the_guide_tbl] as a
                set inlinkid = b.inlinkid, outlinkid = b.outlinkid, passlid = b.passlid, passlink_cnt = b.passlink_cnt
                from temp_circle_link_update_linkrow_[the_guide_tbl] as b
                where a.gid = b.gid;

                """

        guide_table_list = [
                            'spotguide_tbl',
                            'signpost_tbl',
                            'signpost_uc_tbl',
                            'lane_tbl',
                            'force_guide_tbl',
                            'towardname_tbl',
                            'caution_tbl',
                            'natural_guidence_tbl',
                            'hook_turn_tbl'
                            ]

        for the_guide_table in guide_table_list:
#            self.log.info('updating %s...' % the_guide_table)
            the_sql_cmd = sqlcmd.replace('[the_guide_tbl]', the_guide_table)
            self.pg.execute2(the_sql_cmd)
            self.pg.commit2()

        self.log.info('end of dealing circle_link --- guide ...')

    def __circle_link_dealing_regulation(self):

        self.log.info('dealing circle_link --- regulation ...')

        self.CreateFunction2('rdb_get_junction_node_list')
        self.CreateFunction2('rdb_get_split_linkrow_circle_dealing')
        self.CreateFunction2('rdb_get_split_link_circle_dealing')
        self.CreateFunction2('rdb_get_split_link_array_circle_dealing')

        self.CreateTable2('temp_circle_link_update_linkrow_regulation')

        self.CreateIndex2('link_tbl_bak_circle_dealing_link_id_idx')

        self.CreateIndex2('regulation_relation_tbl_bak_circle_dealing_regulation_id_idx')
        self.CreateIndex2('regulation_item_tbl_bak_circle_dealing_regulation_id_idx')
        self.CreateIndex2('regulation_item_tbl_bak_circle_dealing_linkid_idx')


        self.CreateTable2('regulation_relation_tbl')
        sqlcmd = """
            insert into regulation_relation_tbl
                (regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope)
            (
            select a.regulation_id, a.nodeid, a.inlinkid, a.outlinkid, condtype, cond_id, gatetype, slope
            from regulation_relation_tbl_bak_circle_dealing as a
            left join temp_circle_link_update_linkrow_regulation as b
            on a.regulation_id = b.regulation_id
            where b.regulation_id is null
            );

            insert into regulation_relation_tbl
                (regulation_id, nodeid, inlinkid, outlinkid, condtype, cond_id, gatetype, slope)
            (
            select a.regulation_id, a.nodeid, b.inlinkid, b.outlinkid, condtype, cond_id, gatetype, slope
            from regulation_relation_tbl_bak_circle_dealing as a
            inner join temp_circle_link_update_linkrow_regulation as b
            on a.regulation_id = b.regulation_id
            );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateTable2('regulation_item_tbl')
        sqlcmd = """
            insert into regulation_item_tbl
                (regulation_id, linkid, nodeid, seq_num)
            (
            select a.regulation_id, a.linkid, a.nodeid, a.seq_num
            from regulation_item_tbl_bak_circle_dealing as a
            left join temp_circle_link_update_linkrow_regulation as b
            on a.regulation_id = b.regulation_id
            where b.regulation_id is null
            order by a.regulation_id, a.seq_num
            );

            insert into regulation_item_tbl
                (regulation_id, linkid, nodeid, seq_num)
            (
            select  regulation_id,
                (
                case
                when seq_num = 1 then link_array[1]
                when seq_num = 2 then null
                else link_array[seq_num - 1]
                end
                )as linkid,
                (
                case
                when seq_num = 2 then nodeid
                else null
                end
                )as nodeid,
                seq_num
            from
            (
                select regulation_id, nodeid, link_array, generate_series(1,seq_count) as seq_num
                from
                (
                select  a.regulation_id, b.nodeid, b.link_array,
                    (case when b.link_num = 1 then 1 else link_num + 1 end) as seq_count
                from (select * from regulation_item_tbl_bak_circle_dealing where seq_num = 1) as a
                inner join temp_circle_link_update_linkrow_regulation as b
                on a.regulation_id = b.regulation_id
                )as c
            )as d
            order by regulation_id, seq_num
            );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.log.info('end of dealing circle_link --- regulation ...')

    def __circle_link_dealing_safetyzone(self):
        
        self.log.info('dealing circle_link --- safetyzone ...')

        sqlcmd = """
            INSERT INTO safety_zone_tbl(
                    safetyzone_id,linkid, speedlimit, speedunit_code, direction, safety_type)
            select safetyzone_id,b.link_id,speedlimit, speedunit_code, direction, safety_type
            from safety_zone_tbl a
            left join 
            temp_circle_link_new_seq_link b
            on a.linkid=b.old_link_id
            where b.old_link_id is not null;
            
            delete from safety_zone_tbl
            where linkid in
            (select old_link_id from temp_circle_link_new_seq_link)
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end of dealing circle_link --- safetyzone ...')
        return 0

    def __circle_link_dealing_safetyalert(self):
        
        self.log.info('dealing circle_link --- safetyalert ...')
        
        self.CreateFunction2('mid_get_fraction')
        
        sqlcmd = """
            update safety_alert_tbl a
            set link_id=b.newlink_id,
            s_dis=b.s_dis,
            e_dis=b.e_dis
            from
            (   with a as
                (
                    select a.featid,a.link_id,b.link_id as newlink_id,st_distance(a.the_geom,b.the_geom) as dist,
                    (ST_Length_Spheroid(b.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*mid_get_fraction(b.the_geom,a.the_geom))::int as s_dis,
                    (ST_Length_Spheroid(b.the_geom,'SPHEROID("WGS_84", 6378137, 298.257223563)')*(1-mid_get_fraction(b.the_geom,a.the_geom)))::int as e_dis 
                    from safety_alert_tbl a
                    left join temp_circle_link_new_seq_link b
                    on a.link_id=b.old_link_id
                    where b.old_link_id is not null
                ),
                b as
                (
                    select featid,link_id,min(dist)
                    from a
                    group by a.featid,a.link_id
                )
                select a.featid,a.link_id,newlink_id,s_dis,e_dis
                from a
                left join b
                on a.link_id=b.link_id and a.featid=b.featid
                where a.dist=b.min
            ) b
            where a.link_id=b.link_id and a.featid=b.featid;
            
            update safety_alert_tbl a
            set orglink_id=b.newlink_id
            from
            (   with a as
                (
                    select a.featid,a.orglink_id as link_id,b.link_id as newlink_id,st_distance(a.the_geom,b.the_geom) as dist 
                    from safety_alert_tbl a
                    left join temp_circle_link_new_seq_link b
                    on a.orglink_id=b.old_link_id
                    where b.old_link_id is not null
                ),
                b as
                (
                    select featid,link_id,min(dist)
                    from a
                    group by a.featid,a.link_id
                )
                select a.featid,a.link_id,newlink_id
                from a
                left join b
                on a.link_id=b.link_id and a.featid=b.featid
                where a.dist=b.min
            ) b
            where a.orglink_id=b.link_id and a.featid=b.featid; 
           """

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('end of dealing circle_link --- safetyalert ...')
        return 0            

    def __check(self):
        self.log.info('check --- start ...')

        sqlcmd = """
            select count(*) from link_tbl a
            left join temp_dupli_link b
            on a.link_id = b.aid
            where b.aid is not null

            union

            select count(*) from link_tbl a
            left join temp_circle_link b
            on a.link_id = b.aid
            where b.aid is not null

           """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            if self.pg.getcnt2() == 1:
                self.log.info('check --- end.')
            else:
                self.log.error('check link overlay or circle Failed.')
