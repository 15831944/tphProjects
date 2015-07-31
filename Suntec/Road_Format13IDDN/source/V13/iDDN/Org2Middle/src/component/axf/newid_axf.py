'''
Created on 2012-3-23

@author: sunyifeng
'''

import base
import common

class comp_newid_axf(base.component_base.comp_base):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'New ID')

    def _DoCreateTable(self):
        
        if self.CreateTable2('temp_node_id') == -1:
            return -1
        
        if self.CreateTable2('temp_node_boundary_patch') == -1:
            return -1
        
        if self.CreateTable2('temp_node_boundary') == -1:
            return -1
        
        if self.CreateTable2('temp_link_mapping') == -1:
            return -1
        
        if self.CreateTable2('temp_node_mapping') == -1:
            return -1
        
        
        return 0
    
            
    def _DoCreateIndex(self):
        
        self.CreateIndex2('temp_node_mapping_meshid_node_idx')
        self.CreateIndex2('temp_link_mapping_meshid_road_idx')
        
        self.CreateIndex2('org_roadnode_meshid_idx')
        self.CreateIndex2('org_roadnode_node_idx')
        self.CreateIndex2('org_roadnode_comp_node_idx')
        self.CreateIndex2('org_roadsegment_meshid_idx')
        self.CreateIndex2('org_roadsegment_fnode_idx')
        self.CreateIndex2('org_roadsegment_tnode_idx')
        self.CreateIndex2('org_roadsegment_road_idx')
         
        return 0
        
    def _DoCreateFunction(self):

        
        return 0
        
    def _Do(self):
        if self.CreateIndex2('org_roadnode_meshid_node_idx') == -1:
            return -1
        
        if self.CreateIndex2('org_roadsegment_meshid_road_idx') == -1:
            return -1
        
        self.__makeGNodeID()
        
        self.__markBoundaryNode()
        
        self.__makeNodeMapping()
        
        self.__makeLinkMapping()
        
        return 0
    
    def __makeGNodeID(self):
        self.log.info('Now it is inserting to temp_node_id...')
        sqlcmd = """
                insert into temp_node_id (
                    meshid, node, g_node, x_coord, y_coord, x_coord_100, y_coord_100, x_coord_100_m, y_coord_100_m, the_geom
                    )
                select 
                    meshid, node,
                    (cast(meshid as bigint) << 32) | cast(node as bigint) as new_node,
                    x_coord,
                    y_coord,
                    cast(x_coord * 100 as bigint), 
                    cast(y_coord * 100 as bigint),
                    cast((x_coord * 100 - 0.5) as bigint), 
                    cast((y_coord * 100 - 0.5) as bigint),
                    the_geom
                from org_roadnode;
        """
        
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_node_id_g_node_idx')
        self.CreateIndex2('temp_node_id_meshid_idx')
        self.CreateIndex2('temp_node_id_meshid_node_idx')
        self.CreateIndex2('temp_node_id_xy_coord_100_idx')
        self.CreateIndex2('temp_node_id_xy_coord_100_m_idx')
        
        self.log.info('Inserting to temp_node_id succeeded.')
    
    def __markBoundaryNode(self):
        self.log.info('Now it is merging boundary node...')
        #
        boundary_patch = common.common_func.GetPath('boundary_patch')
        print boundary_patch
        if boundary_patch:
            objFileAnalyst = CFileAnalyst(boundary_patch)
            listrecord = objFileAnalyst.analyse()
            print len(listrecord)
            for record in listrecord:
                self.pg.insert('temp_node_boundary_patch', 
                              (
                               'mid',
                               'meshstr',
                               'node',
                               'x_coord',
                               'y_coord',
                               'boundary'
                              ),
                              (
                               record[0],
                               record[1],
                               record[2],
                               record[3],
                               record[4],
                               record[5]
                              )
                              )
            
            #
            self.pg.commit2()

        sqlcmd = """
                insert into temp_node_boundary (g_node, new_node)
                (
                    select g_node, min(adj_node) as adj_array
                    from 
                    (
                        select distinct g_node, adj_node
                        from
                        (
                            select a.g_node, unnest(ARRAY[a.g_node,b.g_node]) as adj_node
                            from temp_node_id as a
                            inner join temp_node_id as b
                            on a.meshid != b.meshid 
                               and
                               (
                                   (a.x_coord_100 = b.x_coord_100 and a.y_coord_100 = b.y_coord_100) 
                                   or 
                                   (a.x_coord_100_m = b.x_coord_100_m and a.y_coord_100_m = b.y_coord_100_m)
                               )
                            
                            union
                            
                            select d.g_node, unnest(ARRAY[d.g_node,f.g_node]) as adj_node
                            from temp_node_boundary_patch as a
                            inner join temp_node_boundary_patch as b
                            on a.mid = b.mid and (a.meshstr != b.meshstr or a.node != b.node)
                            left join mesh_mapping_tbl as c
                            on a.meshstr = c.meshid_str
                            left join temp_node_id as d
                            on c.globl_mesh_id = d.meshid and a.node = d.node
                            left join mesh_mapping_tbl as e
                            on b.meshstr = e.meshid_str
                            left join temp_node_id as f
                            on e.globl_mesh_id = f.meshid and b.node = f.node
                            
                            union
                            
                            select distinct g_node, adj_node
                            from
                            (
                                select unnest(adj_array) as g_node, adj_node
                                from
                                (
                                    select adj_array, unnest(adj_array) as adj_node
                                    from
                                    (
                                        select ARRAY[a.adj_node,b.adj_node] as adj_array
                                        from
                                        (
                                        select a.g_node, unnest(ARRAY[a.g_node,b.g_node]) as adj_node
                                        from temp_node_id as a
                                        inner join temp_node_id as b
                                        on a.meshid != b.meshid 
                                           and
                                           (
                                               (a.x_coord_100 = b.x_coord_100 and a.y_coord_100 = b.y_coord_100) 
                                               or 
                                               (a.x_coord_100_m = b.x_coord_100_m and a.y_coord_100_m = b.y_coord_100_m)
                                           )
                                        )as a
                                        inner join 
                                        (
                                        select d.g_node, unnest(ARRAY[d.g_node,f.g_node]) as adj_node
                                        from temp_node_boundary_patch as a
                                        inner join temp_node_boundary_patch as b
                                        on a.mid = b.mid and (a.meshstr != b.meshstr or a.node != b.node)
                                        left join mesh_mapping_tbl as c
                                        on a.meshstr = c.meshid_str
                                        left join temp_node_id as d
                                        on c.globl_mesh_id = d.meshid and a.node = d.node
                                        left join mesh_mapping_tbl as e
                                        on b.meshstr = e.meshid_str
                                        left join temp_node_id as f
                                        on e.globl_mesh_id = f.meshid and b.node = f.node
                                        )as b
                                        on a.g_node = b.g_node and a.g_node != a.adj_node and b.g_node != b.adj_node
                                    )as x
                                )as y
                            )as z
                        )as m
                    )as n
                    group by g_node
                )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_node_boundary_g_node_idx')
        
        self.log.info('Merging boundary node succeeded')  
        
    def __makeNodeMapping(self):
        sqlcmd = """
               insert into temp_node_mapping (
                    meshid, 
                    node, 
                    node_id,
                    g_node,
                    new_node, 
                    boundary_flag,
                    the_geom
                    ) 
                select 
                    a.meshid, 
                    a.node, 
                    a.node_id,
                    b.g_node,
                    case when c.new_node is null then b.g_node else c.new_node end as new_node, 
                    case when c.new_node is null then 0 else 1 end as boundary_flag,
                    case when d.the_geom is null then b.the_geom else d.the_geom end as the_geom
                from 
                org_roadnode as a
                left outer join temp_node_id as b 
                on a.meshid = b.meshid and a.node = b.node
                left outer join temp_node_boundary as c
                on b.g_node = c.g_node
                left outer join temp_node_id as d
                on c.new_node = d.g_node
            """
        
        self.log.info('Now it is inserting to temp_node_mapping...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('temp_node_mapping_g_node_idx')
        self.log.info('Inserting temp_node_mapping succeeded')

    def __makeLinkMapping(self):
        sqlcmd = """
                insert into temp_link_mapping (
                    meshid, road, road_id, new_road, new_fnode, new_tnode, the_geom
                    ) 
                SELECT
                    a.meshid, road, road_id,
                    (cast(a.meshid as bigint) << 32) | cast(road as bigint) as new_road,
                    b.new_node as new_fnode,
                    c.new_node as new_tnode,
                    case when (b.g_node = b.new_node) and (c.g_node = c.new_node) then a.the_geom
                         when (b.g_node != b.new_node) and (c.g_node = c.new_node) then st_setpoint(a.the_geom, 0, b.the_geom)
                         when (b.g_node = b.new_node) and (c.g_node != c.new_node) then st_setpoint(a.the_geom, st_numpoints(a.the_geom)-1, c.the_geom)
                         else st_setpoint(st_setpoint(a.the_geom, 0, b.the_geom), st_numpoints(a.the_geom)-1, c.the_geom)
                    end
                FROM 
                (
                    select meshid, road, road_id, fnode, tnode, ST_LineMerge(the_geom) as the_geom 
                    from org_roadsegment
                )as a
                left outer join temp_node_mapping as b
                on a.meshid = b.meshid and a.fnode = b.node
                left outer join temp_node_mapping as c
                on a.meshid = c.meshid and a.tnode = c.node
            """
        
        self.log.info('Now it is inserting to temp_link_mapping...')
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.log.info('Inserting temp_link_mapping succeeded')


class CFileAnalyst:
    def __init__(self, strDataPath):
        self.strDataPath = strDataPath
    
    def analyse(self):
        objFile = open(self.strDataPath)
        listline = objFile.readlines()
        objFile.close()
        
        listrecord = []
        for line in listline[1:]:
            line = line.strip()
            if line:
                record = self._analyseLine(line)
                if record:
                    listrecord.append(record)
        
        return listrecord
    
    def _analyseLine(self, line):
        record = []
        
        #
        listrow = line.split(",")
        
        # mid
        record.append(int(listrow[0]))
        
        # meshstr
        record.append(listrow[1])
        
        # node
        record.append(int(listrow[2]))
        
        # x_coord
        record.append(float(listrow[3]))
        
        # y_coord
        record.append(float(listrow[4]))
        
        # boundary
        record.append(int(listrow[5]))
        
        return record