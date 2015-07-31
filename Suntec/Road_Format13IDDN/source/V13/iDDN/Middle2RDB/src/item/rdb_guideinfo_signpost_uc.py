# -*- coding: cp936 -*-
'''
Created on 2012-7-24

@author: hongcz
'''
from base.rdb_ItemBase import ItemBase, ITEM_EXTEND_FLAG_IDX


class rdb_guideinfo_signpost_uc(ItemBase):
    '''方面看板(海外使用)
    '''

    def __init__(self):
        '''
        Constructor
        '''

        ItemBase.__init__(self, 'Signpost_UC'
                          , 'signpost_uc_tbl'
                          , 'gid'
                          , 'rdb_guideinfo_signpost_uc'
                          , 'gid'
                          , True
                          , ITEM_EXTEND_FLAG_IDX.get('SIGNPOST_UC'))

    def Do_CreateTable(self):
        if self.CreateTable2('rdb_guideinfo_signpost_uc') == -1:
            return -1
        return 0

    def Do_CreatIndex(self):
        self.CreateIndex2('rdb_guideinfo_signpost_uc_in_link_id_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_signpost_uc_node_id_idx')
        self.CreateIndex2('rdb_guideinfo_signpost_uc_node_id_t_idx')
        return 0

    def Do(self):

        sqlcmd = """
                INSERT INTO rdb_guideinfo_signpost_uc(
                            in_link_id, in_link_id_t, node_id, node_id_t,
                            out_link_id, out_link_id_t, passlink_count,
                            sp_name, route_no1, route_no2, route_no3,
                            route_no4, exit_no)
                (
                    SELECT a.tile_link_id, a.tile_id,
                           b.tile_node_id, b.tile_id,
                           c.tile_link_id, c.tile_id,
                           passlink_cnt, sp_name,
                           route_no1, route_no2,
                           route_no3, route_no4,
                           exit_no
                      FROM signpost_uc_tbl
                      LEFT OUTER JOIN rdb_tile_link as a
                      ON inlinkid = a.old_link_id
                      LEFT OUTER JOIN rdb_tile_node as b
                      ON nodeid = b.old_node_id
                      LEFT OUTER JOIN rdb_tile_link as c
                      ON outlinkid = c.old_link_id
                  );
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        return 0
