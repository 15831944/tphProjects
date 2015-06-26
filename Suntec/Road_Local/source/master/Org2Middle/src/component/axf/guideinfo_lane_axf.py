'''
Created on 2012-6-19

@author: zhangliang
'''

import component.component_base

class comp_guideinfo_lane_axf(component.component_base.comp_base):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Lane')
        
    def _DoCreateTable(self):
        if self.CreateTable2('lane_tbl') == -1:
            return -1
        
        if self.CreateTable2('temp_crosslane_tbl_full_table') == -1:
            return -1
        
        if self.CreateTable2('temp_lane_dircect_mapping') == -1:
            return -1
        
        # need to add temp_crosslane_tbl_full_table index of from_road, to_road
        
        if self.CreateTable2('temp_nodelane_tbl_full_table') == -1:
            return -1
        
        # add table, to abstract all exception lane with line
        if self.CreateTable2('temp_crosslane_exception_lane_with_maat') == -1:
            return -1
        
        if self.CreateTable2('temp_nodelane_exception_lane_with_maat') == -1:
            return -1
        
        if self.CreateTable2('temp_org_xlpath') == -1:
            return -1
                
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        
        if self.CreateIndex2('lane_tbl_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('lane_tbl_inlinkid_idx') == -1:
            return -1
        
        if self.CreateIndex2('lane_tbl_outlinkid_idx') == -1:
            return -1

        return 0
    
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('mid_has_additional_lane') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_arrow_info') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_access_lane_info') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_lane_xlpath_info') == -1:
            return -1
        
        if self.CreateIndex2('org_xlpath_tbl_xlpath_idx') == -1:
            return -1
        
        if self.CreateFunction2('mid_get_cross_xlpath_access_lane') == -1:
            return -1
        
        if self.CreateFunction2('mid_findInnerPath') == -1:
            return -1

        if self.CreateFunction2('mid_get_specific_char_num') == -1:
            return -1
        
        return 0
    
    def _Do(self):
        
        self.log.info(self.ItemName + ': begin of making lane ...')
        
        # step 1: insert into lane direction relationship table          
        sqlcmd = """
            insert into temp_lane_dircect_mapping values
             (1, ARRAY['0', '2', '4', '7', '9', 'A', 'F', 'G', 'J', 'K', 'N', 'R', 'U', 'V', 'Y']);
            insert into temp_lane_dircect_mapping values
             (64, ARRAY['1', '2', '6', '7', 'B', 'F', 'H', 'J', 'L', 'M', 'N', 'P', 'W', 'Y', 'Z']);
            insert into temp_lane_dircect_mapping values
             (4, ARRAY['3', '4', '6', '7', 'C', 'E', 'L', 'N', 'S', 'U', 'W', 'X', 'Y']);
            insert into temp_lane_dircect_mapping values
             (16, ARRAY['5','9', 'B', 'E', 'F', 'I', 'K', 'M']);
            insert into temp_lane_dircect_mapping values
             (2048, ARRAY['8', 'A', 'C', 'P', 'T', 'V', 'X']);
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
         
         
        # insert cross lane info without xlpath
        sqlcmd = """
             insert into lane_tbl(nodeid, inlinkid, outlinkid, passlid, passlink_cnt, lanenum, laneinfo, arrowinfo, lanenuml,lanenumr, buslaneinfo)
            (
                select I.new_node, J.new_road, K.new_road, str_pass_links, passlink_cnt,
                lanenum, laneinfo, target_arrow_info, lanenuml, lanuenumr, buslaneinfo
                from
                (
                    select case when start_comp_node = comp_node then fnode else tnode end as nodeid,from_road, to_road, case when pass_links_cnt > 2 then array_to_string(pass_links[2:pass_links_cnt-1],'|') else null end as str_pass_links , 
                        (pass_links_cnt - 2) as passlink_cnt, length(all_lane_info) as lanenum, laneinfo, target_arrow_info,
                        case when extend_lane_info = 3 or extend_lane_info =2 then 1 else 0 end as lanenuml,
                        case when extend_lane_info = 3 or extend_lane_info =1 then 1 else 0 end as lanuenumr,
                        null as buslaneinfo, meshid from 
                    (
                        select comp_node, from_road, fnode, start_comp_node, tnode, end_comp_node, 
                        to_road, lane_info_target_direction, target_arrow_info, all_lane_info, pass_links, F.lane_array, 
                        array_upper(pass_links,1) as pass_links_cnt,
                        mid_get_access_lane_info(all_lane_info,target_arrow_info,F.lane_array, lane_info_target_direction) as laneinfo,
                        mid_has_additional_lane(all_lane_info) as extend_lane_info,
                        meshid from 
                        (
                        select A.node as comp_node, from_road, B.fnode, C.comp_node as start_comp_node, B.tnode, D.comp_node as end_comp_node, 
                        to_road, lane_info_target_direction, mid_get_arrow_info(lane_info_target_direction, T.exception_lane_no_array) as target_arrow_info, all_lane_info, 
                        string_to_array(unnest(mid_findInnerPath(A.meshid, cast(A.from_road as int), cast(A.to_road as int), cast(A.node as int))),'|') as pass_links,
                        A.meshid  
                        from 
                        (
                            select * from 
                            (
                                select distinct(maat), min(maat_id) as maat_id, min(node) as node, min(from_road) as from_road, min(to_road) as to_road, 
                                min(lane_info_target_direction) as lane_info_target_direction, min(all_lane_info) as all_lane_info,min(meshid) as meshid 
                                from 
                                (
                                    select * from temp_crosslane_tbl_full_table 
                                    order by meshid, maat
                                ) as Z
                                group by meshid,maat
                            ) as W
                            order by meshid,maat
                        )as A
                        left join org_roadsegment as B
                        on A.meshid = B.meshid and A.from_road = B.road
                        left join org_roadnode as C
                        on B.meshid = C.meshid and B.fnode = C.node
                        left join org_roadnode as D
                        on B.meshid = D.meshid and B.tnode = D.node
                        left join temp_crosslane_exception_lane_with_maat as T
                        on A.meshid = T.meshid and A.maat = T.maat
                        ) as E
                        left join temp_lane_dircect_mapping as F
                        on E.target_arrow_info = F.lane_direction
                    ) as G where G.target_arrow_info <> -1
                ) as H 
                left join temp_node_mapping as I on H.meshid = I.meshid and H.nodeid = I.node
                left join temp_link_mapping as J on H.meshid = J.meshid and H.from_road = J.road
                left join temp_link_mapping as K on H.meshid = K.meshid and H.to_road = K.road
            )
         """
         
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        # insert cross lane info with xlpath
        sqlcmd = """
         insert into lane_tbl(nodeid, inlinkid, outlinkid, passlid, passlink_cnt, lanenum, laneinfo, arrowinfo, lanenuml,lanenumr, buslaneinfo)
        (
            select I.new_node, J.new_road, to_road, string_pass_links, passlink_cnt,
            total_lane_num, laneinfo, target_arrow_info, lanenuml, lanuenumr, buslaneinfo from 
            (
                 select cast(case when start_comp_node = comp_node then fnode else tnode end as bigint) as nodeid, cast(from_road as bigint), cast(pass_links[xlpath_cnt] as bigint) as to_road, case when xlpath_cnt > 2 then array_to_string(pass_links[2:xlpath_cnt-1],'|') else null end as string_pass_links , (xlpath_cnt - 2) as passlink_cnt,
                total_lane_num, mid_get_cross_xlpath_access_lane(lane_no, lane_info_target_direction, all_lane_info, Q.lane_array) as laneinfo, target_arrow_info, 
                case when extend_lane_info = 3 or extend_lane_info =2 then 1 else 0 end as lanenuml,
                case when extend_lane_info = 3 or extend_lane_info =1 then 1 else 0 end as lanuenumr,
                null as buslaneinfo, F.meshid from 
                (
                    select A.node as comp_node, from_road, B.fnode, C.comp_node as start_comp_node, B.tnode, D.comp_node as end_comp_node, 
                    to_road, lane_no, lane_div, mid_get_arrow_info(lane_div,array[-1::smallint]) as target_arrow_info,lane_info_target_direction, all_lane_info, length(all_lane_info) as total_lane_num,
                    mid_get_lane_xlpath_info(cast(xlpath as bigint), xlpath_cnt,A.meshid) as pass_links, xlpath_cnt, mid_has_additional_lane(all_lane_info) as extend_lane_info ,A.meshid 
                    from 
                    (
                        select * from 
                        ( 
                            select * from temp_crosslane_tbl_full_table
                            order by meshid, maat
                        ) as Z
                         where xlpath is not null 
                    ) as A 
                    left join org_roadsegment as B
                    on A.from_road = B.road and A.meshid = B.meshid
                    left join org_roadnode as C
                    on B.meshid = C.meshid and B.fnode = C.node
                    left join org_roadnode as D
                    on B.meshid = D.meshid and B.tnode = D.node
                ) as F 
                left join temp_lane_dircect_mapping as Q
                on F.target_arrow_info = Q.lane_direction
            ) as H
            left join temp_node_mapping as I on H.meshid = I.meshid and H.nodeid = I.node
            left join temp_link_mapping as J on H.meshid = J.meshid and H.from_road = J.road
        )
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
         
        # deal with normal condition 
        # insert into node lane info without xlpath
        sqlcmd = """
            insert into lane_tbl(nodeid, inlinkid, outlinkid, passlid, passlink_cnt, lanenum, laneinfo, arrowinfo, lanenuml,lanenumr, buslaneinfo)
           (
            select I.new_node, J.new_road, K.new_road, passlinks, passlink_cnt,
            total_lane_num, laneinfo, target_arrow_info, lanenuml, lanuenumr, buslaneinfo from 
            (
                select cast(node as bigint), cast(from_road as bigint), cast(to_road as bigint), null as passlinks, 0 as passlink_cnt, 
                total_lane_num, 
                mid_get_access_lane_info(all_lane_info,target_arrow_info,G.lane_array, lane_info_target_direction) as laneinfo, 
                target_arrow_info,
                case when extend_lane_info = 3 or extend_lane_info =2 then 1 else 0 end as lanenuml,
                case when extend_lane_info = 3 or extend_lane_info =1 then 1 else 0 end as lanuenumr,
                null as buslaneinfo,meshid from
                (
                    select node, from_road, to_road, all_lane_info, total_lane_num,
                    target_arrow_info, extend_lane_info , F.lane_array, lane_info_target_direction, meshid from
                    (
                    select node, from_road, to_road, all_lane_info, length(all_lane_info) as total_lane_num,
                     mid_get_arrow_info(lane_info_target_direction,T.exception_lane_no_array) as target_arrow_info,
                     mid_has_additional_lane(all_lane_info) as extend_lane_info, lane_info_target_direction,A.meshid 
                     from 
                     (
                         select * from
                        (
                            select distinct(maat), min(maat_id) as maat_id, min(node) as node, min(from_road) as from_road, min(to_road) as to_road, 
                            min(lane_info_target_direction) as lane_info_target_direction, min(all_lane_info) as all_lane_info,min(meshid) as meshid 
                            from 
                            (
                                select * from temp_nodelane_tbl_full_table
                                order by meshid, maat
                            ) as Z
                            group by meshid,maat
                        ) as W
                        order by meshid, maat
                     ) as A
                     left join temp_nodelane_exception_lane_with_maat as T
                     on A.meshid = T.meshid and A.maat = T.maat
                    ) as B
                    left join temp_lane_dircect_mapping as F
                     on B.target_arrow_info = F.lane_direction
                ) as G where G.target_arrow_info <> -1
            ) as H
            left join temp_node_mapping as I on H.meshid = I.meshid and H.node = I.node
            left join temp_link_mapping as J on H.meshid = J.meshid and H.from_road = J.road
            left join temp_link_mapping as K on H.meshid = K.meshid and H.to_road = K.road
           )
       """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        # insert node lane info with xlpath
        sqlcmd = """
        insert into lane_tbl(nodeid, inlinkid, outlinkid, passlid, passlink_cnt, lanenum, laneinfo, arrowinfo, lanenuml,lanenumr, buslaneinfo)
        (
            select I.new_node, J.new_road, to_road,string_path_links, passlink_cnt,
            total_lane_num, laneinfo, target_arrow_info,
            lanenuml,lanuenumr, buslaneinfo from 
            (
               select node, cast(from_road as bigint), cast(pass_links[xlpath_cnt] as bigint) as to_road, case when xlpath_cnt>2 then array_to_string(pass_links[2:xlpath_cnt-1],'|') else null end as string_path_links , (xlpath_cnt - 2) as passlink_cnt,
                total_lane_num, mid_get_cross_xlpath_access_lane(lane_no, lane_info_target_direction, all_lane_info, Q.lane_array) as laneinfo, target_arrow_info, 
                case when extend_lane_info = 3 or extend_lane_info =2 then 1 else 0 end as lanenuml,
                case when extend_lane_info = 3 or extend_lane_info =1 then 1 else 0 end as lanuenumr,
                null as buslaneinfo,meshid  from 
                (
                    select node, from_road, to_road, lane_no, lane_div, mid_get_arrow_info(lane_div,array[-1::smallint]) as target_arrow_info,lane_info_target_direction, all_lane_info, length(all_lane_info) as total_lane_num,
                    mid_get_lane_xlpath_info(cast(xlpath as bigint), xlpath_cnt,meshid) as pass_links, xlpath_cnt, mid_has_additional_lane(all_lane_info) as extend_lane_info ,A.meshid 
                    from 
                    (
                        select * from 
                        (
                            select * from temp_nodelane_tbl_full_table 
                            order by meshid, maat
                        ) as Z
                        where xlpath is not null 
                    ) as A 
                ) as B  
                 left join temp_lane_dircect_mapping as Q
                on B.target_arrow_info = Q.lane_direction 
            ) as H
            left join temp_node_mapping as I on H.meshid = I.meshid and H.node = I.node
            left join temp_link_mapping as J on H.meshid = J.meshid and H.from_road = J.road
        )
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
       
            
        self.log.info(self.ItemName + ': end of making lane ...')
                  
        return 0
    

    
