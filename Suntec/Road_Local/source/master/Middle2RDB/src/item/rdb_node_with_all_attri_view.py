# -*- coding: UTF8 -*-
'''
Created on 2013-12-17

@author: hongchenzai
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log


class rdb_node_with_all_attri_view(ItemBase):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        ItemBase.__init__(self,
                          'Node_With_All_Attri_View',
                          'rdb_node',
                          'gid',
                          'rdb_node_with_all_attri_view',
                          'gid',
                          False)

    def Do_CreateTable(self):
        self.CreateTable2('rdb_node_with_all_attri_view')

    def Do(self):
        sqlcmd = """
            drop table if exists temp_rdb_node_bigint_2_int_mapping cascade;
            
            select node_id, node_id_t, rdb_split_tileid(node_id) as node_id_32 into temp_rdb_node_bigint_2_int_mapping from rdb_node;
            
            create index temp_rdb_node_bigint_2_int_mapping_nodeid_idx
            on temp_rdb_node_bigint_2_int_mapping
            using btree
            (node_id);
            
            create index temp_rdb_node_bigint_2_int_mapping_nodeid_32_idx
            on temp_rdb_node_bigint_2_int_mapping
            using btree
            (node_id_32);
            
            create index temp_rdb_node_bigint_2_int_mapping_nodeid_t_idx
            on temp_rdb_node_bigint_2_int_mapping
            using btree
            (node_id_t);
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            INSERT INTO rdb_node_with_all_attri_view(gid, node_id, node_id_t,
                            extend_flag, height, link_num, branches, upgrade_node_id,
                            upgrade_node_id_t, geom_blob, the_geom)
            (
            SELECT a.gid, a.node_id, a.node_id_t, a.extend_flag, 
                   d.height, a.link_num, a.branches,
                   c.region_node_id AS upgrade_node_id,
                   NULL::integer AS upgrade_node_id_t, b.geom_blob, a.the_geom
               FROM rdb_node a
               LEFT JOIN temp_rdb_node_bigint_2_int_mapping m
               ON a.node_id_t = m.node_id_t AND a.node_id = m.node_id
               LEFT JOIN rdb_node_client b
               ON m.node_id_t = b.node_id_t AND m.node_id_32 = b.node_id
               LEFT JOIN rdb_region_layer4_node_mapping c
               ON a.node_id = c.node_id_14
               LEFT JOIN rdb_node_height d
               ON a.node_id = d.node_id
            );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self.CreateIndex2('rdb_node_with_all_attri_view_node_id_idx')
        self.CreateIndex2('rdb_node_with_all_attri_view_node_id_t_idx')
        # CLUSTER
        sqlcmd = """
            CLUSTER rdb_node_with_all_attri_view_node_id_idx
              ON rdb_node_with_all_attri_view;
            """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
