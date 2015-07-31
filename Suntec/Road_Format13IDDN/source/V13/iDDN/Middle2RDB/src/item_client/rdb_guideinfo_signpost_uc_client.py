# -*- coding: cp936 -*-
'''
Created on 2012-7-24

@author: hongcz
'''
from base.rdb_ItemBase import ItemBase

class rdb_guideinfo_signpost_uc_client(ItemBase):
    '''方面看板(海外使用)
    '''

    def __init__(self):
        '''
        Constructor
        '''

        ItemBase.__init__(self, 'Signpost_uc_client'
                          , 'rdb_guideinfo_signpost_uc'
                          , 'gid'
                          , 'rdb_guideinfo_signpost_uc_client'
                          , 'gid'
                          , False)


    def Do_CreateTable(self):
        if self.CreateTable2('rdb_guideinfo_signpost_uc_client') == -1:
            return -1
        return 0

    def Do_CreatIndex(self):
        self.CreateIndex2('rdb_guideinfo_signpost_uc_client_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_signpost_uc_client_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_signpost_uc_client_node_id_t_idx')
        return 0

    def Do(self):

        sqlcmd = """
                INSERT INTO rdb_guideinfo_signpost_uc_client(
                            in_link_id, in_link_id_t, node_id, node_id_t,
                            out_link_id, out_link_id_t, passlink_count,
                            sp_name, route_no1, route_no2, route_no3,
                            route_no4, exit_no)
                (
                SELECT rdb_split_tileid(in_link_id), in_link_id_t,
                       rdb_split_tileid(node_id), node_id_t,
                       rdb_split_tileid(out_link_id), out_link_id_t,
                       passlink_count, sp_name, route_no1, route_no2,
                       route_no3, route_no4, exit_no
                  FROM rdb_guideinfo_signpost_uc
                  order by gid
                );
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        return 0
