# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author: liuxinxing
'''
import component.component_base

class comp_guideinfo_signpost_axf(component.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'GuideInfo_SignPost')
    
    def _DoCreateTable(self):
        self.CreateTable2('signpost_tbl')
        
    def _Do(self):
        self.log.info('Begin convert SignPost...')
        
        self.CreateIndex2('org_roadfurnituresignpost_meshid_idx')
        self.CreateIndex2('org_roadfurnituresignpost_node_idx')
        self.CreateIndex2('org_rfsppath_meshid_idx')
        self.CreateIndex2('org_rfsppath_link_idx')
        self.CreateIndex2('org_rfsppathlink_meshid_link_idx')
        self.CreateIndex2('mesh_mapping_tbl_meshid_str_idx')
        
        self.CreateFunction2('mid_findTargetNodeIndex')
        
        sqlcmd = """
            insert into signpost_tbl
            (
                nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                direction, guideattr, namekind, guideclass, 
                patternno, arrowno, name_id
            )
            (
                select  nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                        0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                        patternno, arrowno, -1 as name_id
                from
                (
                    select  nodeid, link_array[from_road_index] as inlinkid, link_array[link_cnt] as outlinkid, 
                            array_to_string(link_array[from_road_index+1:link_cnt-1], '|') as passlid, 
                            (link_cnt-from_road_index-1) as passlink_cnt,
                            patternno, arrowno
                    from
                    (
                        select  nodeid, link_array, link_cnt, patternno, arrowno,
                                mid_findTargetNodeIndex(fnode_array, tnode_array, nodeid) as from_road_index
                        from
                        (
                            select  a.meshid, c.new_node as nodeid, image_id as patternno, 
                                    generate_series(cast(path as bigint), cast(path+path_cnt-1 as bigint)) as path
                            from org_roadfurnituresignpost as a
                            left join mesh_mapping_tbl as b
                            on a.mesh = b.meshid_str
                            left join temp_node_mapping as c
                            on b.globl_mesh_id = c.meshid and a.node = c.node
                        )as s
                        left join
                        (
                            select  meshid, path, arrowno, link_cnt, 
                                    array_agg(new_road) as link_array, 
                                    array_agg(new_fnode) as fnode_array,
                                    array_agg(new_tnode) as tnode_array
                            from
                            (
                                select  a.meshid, path, arrowno, link_cnt, a.link, d.new_road, d.new_fnode, d.new_tnode
                                from
                                (
                                    select  meshid, path, image_id as arrowno, link_cnt,
                                            generate_series(cast(link as bigint), cast(link+link_cnt-1 as bigint)) as link
                                    from org_rfsppath
                                )as a
                                left join org_rfsppathlink as b
                                on a.meshid = b.meshid and a.link = b.link
                                left join mesh_mapping_tbl as c
                                on b.mesh = c.meshid_str
                                left join temp_link_mapping as d
                                on c.globl_mesh_id = d.meshid and b.road = d.road
                                order by a.meshid, path, arrowno, link_cnt, a.link
                            )as c
                            group by meshid, path, arrowno, link_cnt
                        )as p
                        on s.meshid = p.meshid and s.path = p.path
                    )as m
                    where from_road_index > 0
                )as n
                order by nodeid, inlinkid, outlinkid, passlid, patternno, arrowno
            )
            """
        
        self.pg.execute2(sqlcmd)
        
        self.pg.commit2()
        
        self.log.info('End convert SignPost.')
        
        return 0
    