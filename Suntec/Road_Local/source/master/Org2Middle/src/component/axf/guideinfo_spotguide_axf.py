'''
Created on 2012-3-26

@author: sunyifeng
'''

from component.default.guideinfo_spotguide import comp_guideinfo_spotguide

class comp_guideinfo_spotguide_axf(comp_guideinfo_spotguide):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        comp_guideinfo_spotguide.__init__(self)
    
    def _DoCreateTable(self):
        self.CreateTable2('spotguide_tbl')
        
    def _Do(self):
        self.__convert_spotguide_on_node()
        self.__convert_spotguide_on_cross()
        self.__convert_spotguide_highway_single()
        self.__convert_spotguide_highway_double()
        return 0
    
    def __convert_spotguide_on_node(self):
        self.log.info('convert spotguide_tbl on node...')
        
        self.CreateIndex2('org_roadnodesaat_meshid_idx')
        self.CreateIndex2('org_roadnodesaat_node_idx')
        self.CreateIndex2('org_roadnodesaat_road_idx')
        self.CreateIndex2('org_roadnodemaat_meshid_idx')
        self.CreateIndex2('org_roadnodemaat_node_idx')
        self.CreateIndex2('org_roadnodemaat_from_road_idx')
        
        sqlcmd = """
            insert into spotguide_tbl
            (
                nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                direction, guideattr, namekind, guideclass, 
                patternno, arrowno, type
            )
            (
                select  x.new_node as nodeid, y.new_road as inlinkid, z.new_road as outlinkid, passlid, passlink_cnt,
                        direction, guideattr, namekind, guideclass, 
                        patternno, arrowno, type
                from
                (
                    select  a.meshid, a.node, a.from_road, a.to_road, null as passlid, 0 as passlink_cnt,
                            0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                            b.image_id as patternno, a.image_id as arrowno, 
                            (case when b.image_id like '8%' then 1 else 4 end) as type
                    from
                    (
                        select *
                        from org_roadnodemaat
                        where image_id is not null
                    )as a
                    left join org_roadnodesaat as b
                    on a.meshid = b.meshid and a.node = b.node and a.from_road = b.road
                    where not (b.image_id like '8%')
                    
                    union
                    
                    select  a.meshid, a.node, a.from_road, a.to_road, null as passlid, 0 as passlink_cnt,
                            0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                            b.real_cross as patternno, a.real_cross as arrowno, 5 as type
                    from
                    (
                        select *
                        from org_roadnodemaat
                        where real_cross is not null
                    )as a
                    left join org_roadnodesaat as b
                    on a.meshid = b.meshid and a.node = b.node and a.from_road = b.road
                    
                    union
                    
                    select  a.meshid, a.node, a.from_road, a.to_road, null as passlid, 0 as passlink_cnt,
                            0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                            b.real_hic as patternno, a.real_hic as arrowno, 2 as type
                    from
                    (
                        select *
                        from org_roadnodemaat
                        where real_hic is not null
                    )as a
                    left join org_roadnodesaat as b
                    on a.meshid = b.meshid and a.node = b.node and a.from_road = b.road
                )as m
                left join temp_node_mapping as x
                on m.meshid = x.meshid and m.node = x.node
                left join temp_link_mapping as y
                on m.meshid = y.meshid and m.from_road = y.road
                left join temp_link_mapping as z
                on m.meshid = z.meshid and m.to_road = z.road
                order by nodeid, inlinkid, outlinkid, passlid, patternno, arrowno, type
            )
            """
        self.pg.execute2(sqlcmd)
        
        self.pg.commit2()
        
        self.log.info('convert spotguide_tbl on node end.')

    
    def __convert_spotguide_on_cross(self):
        self.log.info('convert spotguide_tbl on cross...')
        
        self.CreateFunction2('mid_findInnerPath')
        
        self.CreateIndex2('org_roadcrosssaat_meshid_idx')
        self.CreateIndex2('org_roadcrosssaat_node_idx')
        self.CreateIndex2('org_roadcrosssaat_road_idx')
        self.CreateIndex2('org_roadcrossmaat_meshid_idx')
        self.CreateIndex2('org_roadcrossmaat_node_idx')
        self.CreateIndex2('org_roadcrossmaat_from_road_idx')
        
        sqlcmd = """
            insert into spotguide_tbl
            (
                nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                direction, guideattr, namekind, guideclass, 
                patternno, arrowno, type
            )
            (
                select  nodeid, links[1] as inlinkid, links[path_link_count] as outlinkid, 
                        array_to_string(links[2:path_link_count-1], '|') as passlid,
                        (path_link_count - 2) as passlink_cnt,
                        direction, guideattr, namekind, guideclass, 
                        patternno, arrowno, type
                from
                (
                    select  nodeid,
                            array_upper(links,1) as path_link_count,
                            links,
                            direction, guideattr, namekind, guideclass, 
                            patternno, arrowno, type
                    from
                    (
                        select  nodeid,
                                cast(string_to_array(unnest(path_array), '|') as bigint[]) as links,
                                direction, guideattr, namekind, guideclass, 
                                patternno, arrowno, type
                        from
                        (
                            select  (case when c.comp_node = a.node then d.new_fnode else d.new_tnode end) as nodeid,
                                    mid_findInnerPath(a.meshid, cast(a.from_road as integer), cast(a.to_road as integer), cast(a.node as integer)) as path_array,
                                    a.direction, a.guideattr, a.namekind, a.guideclass, 
                                    a.patternno, a.arrowno, a.type
                            from
                            (
                                select  a.meshid, a.node, a.from_road, a.to_road, 
                                        0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                                        b.image_id as patternno, a.image_id as arrowno, 
                                        (case when b.image_id like '8%' then 1 else 4 end) as type
                                from
                                (
                                    select *
                                    from org_roadcrossmaat
                                    where image_id is not null
                                )as a
                                left join org_roadcrosssaat as b
                                on a.meshid = b.meshid and a.node = b.node and a.from_road = b.road
                                where not (b.image_id like '8%')
                                
                                union
                                
                                select  a.meshid, a.node, a.from_road, a.to_road, 
                                        0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                                        b.real_cross as patternno, a.real_cross as arrowno, 5 as type
                                from
                                (
                                    select *
                                    from org_roadcrossmaat
                                    where real_cross is not null
                                )as a
                                left join org_roadcrosssaat as b
                                on a.meshid = b.meshid and a.node = b.node and a.from_road = b.road
                                
                                union
                                
                                select  a.meshid, a.node, a.from_road, a.to_road, 
                                        0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                                        b.real_hic as patternno, a.real_hic as arrowno, 2 as type
                                from
                                (
                                    select *
                                    from org_roadcrossmaat
                                    where real_hic is not null
                                )as a
                                left join org_roadcrosssaat as b
                                on a.meshid = b.meshid and a.node = b.node and a.from_road = b.road
                            )as a
                            left join org_roadsegment as b
                            on a.meshid = b.meshid and a.from_road = b.road
                            left join org_roadnode as c
                            on b.meshid = c.meshid and b.fnode = c.node
                            left join temp_link_mapping as d
                            on a.meshid = d.meshid and a.from_road = d.road
                        )as m
                        where path_array is not null
                    )as n
                )as t
                order by nodeid, inlinkid, outlinkid, passlid, passlink_cnt,direction, guideattr, namekind, guideclass, patternno, arrowno, type
            )
            """
        self.pg.execute2(sqlcmd)
        
        self.pg.commit2()
        
        self.log.info('convert spotguide_tbl on cross end.')
    
    def __convert_spotguide_highway_single(self):
        self.log.info('convert spotguide_tbl for highway single begin...')
        
        self.CreateIndex2('org_highwaysingle_mesh_idx')
        self.CreateIndex2('org_highwaysingle_node_idx')
        self.CreateIndex2('org_highwaysingle_from_road_idx')
        self.CreateIndex2('org_highwaysingle_to_road_idx')
        self.CreateIndex2('org_highwaysingle_image_id_idx')
        
        sqlcmd = """
                insert into spotguide_tbl
                (
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                    direction, guideattr, namekind, guideclass, 
                    patternno, arrowno, type
                )
                (
                    select  node_id, from_link_id, to_link_id, 
                            '' as passlid, 0 as passlink_cnt,
                            0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                            patternno, arrowno, 1 as type
                    from
                    (
                        select  c.new_node as node_id,
                                d.new_road as from_link_id,
                                e.new_road as to_link_id,
                                ('8' || substring(a.image_id, 2)) as patternno,
                                a.image_id as arrowno
                        from org_highwaysingle as a
                        left join mesh_mapping_tbl as b
                        on a.mesh = b.meshid_str
                        left join temp_node_mapping as c
                        on b.globl_mesh_id = c.meshid and a.node = c.node
                        left join temp_link_mapping as d
                        on b.globl_mesh_id = d.meshid and a.from_road = d.road
                        left join temp_link_mapping as e
                        on b.globl_mesh_id = e.meshid and a.to_road = e.road
                    )as t
                    order by node_id, from_link_id, to_link_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('convert spotguide_tbl for highway single end.')
    
    def __convert_spotguide_highway_double(self):
        self.log.info('convert spotguide_tbl for highway double begin...')
        
        self.CreateIndex2('org_highwaydouble_mesh_idx')
        self.CreateIndex2('org_highwaydouble_node_idx')
        self.CreateIndex2('org_highwaydoublepath_hwp_idx')
        
        sqlcmd = """
                insert into spotguide_tbl
                (
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                    direction, guideattr, namekind, guideclass, 
                    patternno, arrowno, type
                )
                (
                    select    node_id,
                        link_array[1] as from_link_id,
                        link_array[link_count] as to_link_id,
                        array_to_string(link_array[2:link_count-1], '|') as passlid,
                        (link_count - 2) as passlink_cnt,
                        0 as direction, 0 as guideattr, 0 as namekind, 0 as guideclass, 
                        patternno, arrowno, 1 as type
                    from
                    (
                        select    gid, node_id, patternno, arrowno,
                            array_agg(link_id) as link_array,
                            count(link_id) as link_count
                        from
                        (
                            select    *
                            from
                            (
                                select    t.gid,
                                    t.hwp_index,
                                    m2.new_node as node_id,
                                    n2.new_road as link_id,
                                    ('8' || substring(t.image_id, 2)) as patternno,
                                    t.image_id as arrowno
                                from
                                (
                                    select    a.gid,
                                        a.hwp_index,
                                        a.mesh as node_mesh_str, a.node,
                                        (case when a.hwp_index = -1 then a.mesh else b.mesh end) as road_mesh_str,
                                        (case when a.hwp_index = -1 then a.from_road else b.road end) as road,
                                        image_id
                                    from
                                    (
                                        select     *, generate_series(-1, hwp_cnt - 1) as hwp_index
                                        from org_highwaydouble
                                    )as a
                                    left join org_highwaydoublepath as b
                                    on (a.hwp_index != -1) and ((a.hwp + a.hwp_index) = b.hwp_id)
                                )as t
                                left join mesh_mapping_tbl as m1
                                on t.node_mesh_str = m1.meshid_str
                                left join temp_node_mapping as m2
                                on m1.globl_mesh_id = m2.meshid and t.node = m2.node
                                left join mesh_mapping_tbl as n1
                                on t.road_mesh_str = n1.meshid_str
                                left join temp_link_mapping as n2
                                on n1.globl_mesh_id = n2.meshid and t.road = n2.road
                            )as x
                            order by gid, node_id, patternno, arrowno, hwp_index
                        )as y
                        group by gid, node_id, patternno, arrowno
                    )as z
                    order by node_id, from_link_id, to_link_id
                );
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('convert spotguide_tbl for highway double end.')
    