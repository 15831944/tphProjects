# -*- coding: cp936 -*-
'''
Created on 2012-6-21

@author: hongchenzai
'''
import component

class comp_guideinfo_towardname_axf(component.guideinfo_towardname.comp_guideinfo_towardname):
    '''方面名称
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.guideinfo_towardname.comp_guideinfo_towardname.__init__(self)
    
    def _DoCreateTable(self):
        
        component.guideinfo_towardname.comp_guideinfo_towardname._DoCreateTable(self)
        
        return 0
     
    def _DoCreateFunction(self):
        'Create Function'
        self.CreateFunction2('mid_findInnerPath')
        self.CreateFunction2('mid_get_connected_nodeid')
        self.CreateFunction2('mid_get_pass_lid')
        self.CreateFunction2('mid_string_count')
        self.CreateFunction2('mid_cvt_tw_guideattr')
        self.CreateFunction2('mid_cvt_tw_namekind')
        self.CreateFunction2('mid_get_connect_node')
        
        return 0
    
    def _Do(self):
        # 给原始表添加索引
        self.CreateIndex2('org_roadnode_meshid_node_idx')
        self.CreateIndex2('org_roadnode_meshid_comp_node_idx')
        self.CreateIndex2('org_roadsegment_meshid_fnode_idx')
        self.CreateIndex2('org_roadsegment_meshid_tnode_idx')
        self.CreateIndex2('org_roadsegment_meshid_road_idx')
        self.CreateIndex2('org_roadnodesignpost_sp_meshid_idx')
        self.CreateIndex2('org_roadcrosssignpost_sp_meshid_idx')
        
        # 非复合点
        self.__MakeNodeTowardName()
   
        # 复合点
        self.__MakeCrossTowardName()
        return 0
    
    def __MakeNodeTowardName(self):
        "非复合点的TowarName."
        sqlcmd = """
                INSERT INTO towardname_tbl(
                            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
                            direction, guideattr, namekind, namekind2, name_id, "type")
                (
                SELECT a.sp, e.new_node, f.new_road as from_road, g.new_road as to_road, 
                        null as passlid, 0 as passlink_cnt, 
                       0 as direction, mid_cvt_tw_guideattr(sp_type, cont_type), 
                       1 as namekind,
                       mid_cvt_tw_namekind(sp_type, cont_type), new_name_id, 0 as _type
                  FROM (
                      SELECT node, from_road, to_road, meshid, 
                            (sp + generate_series(1, sp_cnt) - 1) AS sp,  
                             generate_series(1, sp_cnt) as seq_nm
                      FROM org_roadnodemaat
                      where sp > 0
                  ) AS a
                  LEFT JOIN org_roadnodesignpost as b
                  on a.sp = b.sp and a.meshid = b.meshid
                  LEFT JOIN temp_node_signpost_dict as c
                  on b.sp = c.sp and b.meshid = c.meshid
                  LEFT JOIN temp_node_mapping as e
                  on a.node = e.node and a.meshid = e.meshid
                  LEFT JOIN temp_link_mapping as f
                  on a.from_road = f.road and a.meshid = f.meshid
                  LEFT JOIN temp_link_mapping as g
                  on a.to_road = g.road and a.meshid = g.meshid
                  order by e.new_node, f.new_road, g.new_road, a.meshid, sp
                );
                """
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        return 0
    
    def __MakeCrossTowardName(self):
        "复合点的TowarName."
        sqlcmd = """
INSERT INTO towardname_tbl(
            id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, 
            direction, guideattr, namekind, namekind2, name_id, "type")
(
SELECT c.sp, f.new_node, g.new_road as from_road, h.new_road as to_road, 
        mid_get_pass_lid(link_lid, '|') as passlid, 
      (mid_string_count(link_lid, '|') - 1) as passlid_cnt,
       0 as direction, mid_cvt_tw_guideattr(sp_type, cont_type), 
       1 as namekind,
       mid_cvt_tw_namekind(sp_type, cont_type), new_name_id, 0 as _type
FROM (
SELECT node, from_road, to_road, (sp + generate_series(1, sp_cnt) - 1) AS sp,  
        generate_series(1, sp_cnt) as seq_nm,link_lid,
        meshid
FROM (
    SELECT connect_node as node, from_road, to_road, sp, sp_cnt, 
        --regexp_split_to_table(array_to_string(passlid_array, ','),  E'\\,+') as link_lid,
        unnest(passlid_array) as link_lid,
        meshid
      FROM (
        SELeCT node, from_road, to_road, sp, sp_cnt, 
           mid_findInnerPath(meshid, from_road::integer, to_road::integer, node::integer) as passlid_array,
           mid_get_connect_node(from_road::integer, node::integer, meshid) as connect_node,
           meshid
          FROM org_roadcrossmaat
          WHERE sp > 0
        ) AS a
        where passlid_array is not null
    ) as b
) as c
LEFT JOIN org_roadcrosssignpost as d
on c.sp = d.sp and c.meshid = d.meshid
LEFT JOIN temp_cross_signpost_dict as e
on c.sp = e.sp and c.meshid = e.meshid
LEFT JOIN temp_node_mapping as f
on c.node = f.node and c.meshid = f.meshid
LEFT JOIN temp_link_mapping as g
on c.from_road = g.road and c.meshid = g.meshid
LEFT JOIN temp_link_mapping as h
on c.to_road = h.road and c.meshid = h.meshid
order by f.new_node, g.new_road, h.new_road, link_lid, c.meshid, c.sp
);
"""
        self.pg.execute2(sqlcmd) 
        self.pg.commit2()
        return 0
   
        
    
    