import component.component_base

class comp_link_zenrin(component.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Link')

    def _DoCreateTable(self):
        
        self.CreateTable2('link_tbl')
    
    def _DoCreateIndex(self):
        
        self.CreateIndex2('link_tbl_link_id_idx')
        self.CreateIndex2('link_tbl_s_node_idx')
        self.CreateIndex2('link_tbl_e_node_idx')
        self.CreateIndex2('link_tbl_the_geom_idx')
        
    def _DoCreateFunction(self):

        self.CreateFunction2('zenrin_cnv_width')
        self.CreateFunction2('zenrin_cnv_speed_class')
        self.CreateFunction2('zenrin_cnv_toll')
        self.CreateFunction2('zenrin_cnv_road_type')
        self.CreateFunction2('zenrin_cnv_link_type')
        self.CreateFunction2('zenrin_cnv_disp_class')

        
    def _Do(self):

        self.CreateIndex2('org_road_meshcode_linkno_idx')
        self.CreateIndex2('org_underpass_meshcode_linkno_idx')
        self.CreateIndex2('org_tunnelname_meshcode_roadno_idx')
        self.CreateIndex2('org_divider_meshcode_linkno_idx')
        self.CreateIndex2('org_lanenum_meshcode_roadno_idx')
        self.CreateIndex2('org_speed_meshcode_roadno_idx')
        
        self.CreateIndex2('temp_link_name_link_id_idx')
        self.CreateIndex2('temp_link_shield_link_id_idx')
       
        sqlcmd = '''
        insert into link_tbl (
                link_id, iso_country_code, s_node, e_node, display_class, link_type, road_type, 
                toll, speed_class, length, function_class, lane_dir, lane_num_s2e, lane_num_e2s, 
                elevated, structure, tunnel, rail_cross, paved, uturn,
                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, 
                width_s2e,  width_e2s, one_way_code, one_way_condition, pass_code, pass_code_condition, 
                road_name, road_number, name_type, ownership, car_only, slope_code, slope_angle, 
                disobey_flag, up_down_distinguish, access, extend_flag, etc_only_flag, urban, the_geom
                )
        select link_id, iso_country_code, s_node, e_node, display_class, link_type, road_type, 
                toll, speed_class, length, function_class, lane_dir, 
                lane_num_s2e,lane_num_e2s,elevated, structure, tunnel, rail_cross, paved, uturn,
                speed_limit_s2e, speed_limit_e2s, speed_source_s2e, speed_source_e2s, 
                width_s2e,  width_e2s, one_way_code, one_way_condition, pass_code, pass_code_condition, 
                road_name, road_number, name_type, ownership, car_only, slope_code, slope_angle, 
                disobey_flag, up_down_distinguish, access, extend_flag, etc_only_flag, urban, the_geom_4326
        from
        (
            select a.link_id,
            'CHN' as iso_country_code,
            h.node_id as s_node,
            i.node_id as e_node,
            zenrin_cnv_disp_class(elcode) as display_class,
            zenrin_cnv_link_type(elcode) as link_type,
            zenrin_cnv_road_type(elcode) as road_type,
            zenrin_cnv_toll(elcode) as toll,
            case when f.meshcode is not null then zenrin_cnv_speed_class(f.speed)
                                 ELSE 8 END as speed_class,
            ST_Length_Spheroid(a.the_geom_4326,'SPHEROID("WGS_84", 6378137, 298.257223563)') as length,
            case netlevel when 0 then 5
                          when 1 then 4
                          when 2 then 3
                          when 3 then 2
                          when 4 then 2
                          when 5 then 1
                          end as function_class,
            0 as lane_dir,
            case when one_way_code in (1,2,4) then e.lanenum else 0 end as lane_num_s2e,
            case when one_way_code in (1,3,4) then e.lanenum else 0 end as lane_num_e2s,
            case when Substr(elcode,5,1)='5' then 1 else 0 end as elevated,
            case when b.meshcode is not null and c.meshcode is null then 2
                 when Substr(elcode,5,1)='3' then 3 else 0 end as structure,
            case when Substr(elcode,5,1)='2' and not (b.meshcode is not null and c.meshcode is null) then 1 else 0 end as tunnel,
            0 as rail_cross,
            0 as paved,
            0 as uturn,
            case when one_way_code in (1,2) and f.meshcode is not null then f.speed 
                else 0 end as speed_limit_s2e,
            case when one_way_code in (1,3) and f.meshcode is not null then f.speed 
                else 0 end as speed_limit_e2s,
            case when one_way_code in (1,2) and f.status=1 then 1 
                 when one_way_code in (1,2) and f.status=0 then 2
                 else 0 end as speed_source_s2e,
            case when one_way_code in (1,3) and f.status=1 then 1 
                 when one_way_code in (1,3) and f.status=0 then 2
                 else 0 end as speed_source_e2s,
            case when one_way_code in (1,2,4) then zenrin_cnv_width(elcode)
                else 4 end as width_s2e,
            case when one_way_code in (1,3,4) then zenrin_cnv_width(elcode)
                else 4 end as width_e2s,
            one_way_code,
            0 as one_way_condition,
            0 as pass_code,
            0 as pass_code_condition,
            j.name as road_name,
            k.shield as road_number,
            null::smallint as name_type,
            null::smallint as ownership, 
            null::smallint as car_only, 
            null::smallint as slope_code, 
            null::smallint as slope_angle,
            case when d.meshcode is not null then 1 else 0 end as disobey_flag,
            0 as up_down_distinguish,
            null::smallint as access,
            0 as extend_flag,
            0 as etc_only_flag,
            0 as urban,
            st_geometryn(a.the_geom_4326,1) as the_geom_4326
            from (
                select gid, meshcode, elcode, linkno, snodeno, enodeno, 
                            case when substr(elcode,3,1)='6' or substr(elcode,3,1)='7' or oneway=2 then 4
                                 when oneway=1 then 2
                                 when oneway=3 then 3
                                 when oneway=0 then 1
                                 end as one_way_code, 
                            netlevel, attrnmno, the_geom_4326,link_id
                from (
                     select gid, meshcode, elcode, linkno, snodeno, enodeno, 
                            case when c.link_id is null then oneway 
                                 when oneway=1 and 3=ANY(c.linkdir_array) then 0 
                                 when oneway=2 and c.linkdir_array=array[2]::smallint[] then 1 
                                 when oneway=2 and c.linkdir_array=array[3]::smallint[] then 3
                                 when oneway=2 and 2=ANY(c.linkdir_array) and 3=ANY(c.linkdir_array) then 0 end
                                 as oneway, 
                            netlevel, attrnmno, the_geom_4326,b.link_id
                    FROM org_road a
                    left join temp_link_mapping b 
                    using(meshcode,linkno) 
                    left join temp_link_regulation_permit_traffic c
                    on b.link_id=c.link_id
                    ) a
                ) a
            left join org_underpass b using(meshcode,linkno)
            left join org_tunnelname c on a.meshcode=c.meshcode and a.linkno=c.roadno
            left join org_divider d on a.meshcode=d.meshcode and a.linkno=d.linkno and d.divflg='1' -- and d.divtype='0' : physical divider ???
            left join org_lanenum e on a.meshcode=e.meshcode and a.linkno=e.roadno
            left join org_speed f on a.meshcode=f.meshcode and a.linkno=f.roadno
            join temp_node_mapping h on a.meshcode=h.meshcode and a.snodeno=h.nodeno
            join temp_node_mapping i on a.meshcode=i.meshcode and a.enodeno=i.nodeno
            left join temp_link_name j on a.link_id=j.link_id
            left join temp_link_shield k on a.link_id=k.link_id 
        ) a
        '''
        
        self.log.info('Now it is inserting to link_tbl...')
        self.pg.do_big_insert2(sqlcmd)
        self.log.info('Inserting link succeeded')
        


