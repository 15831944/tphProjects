# -*- coding: cp936 -*-
'''
Created on 2012-3-26

@author: sunyifeng
'''

import base
import common
from component.jdb.import_forceguide_patch import comp_import_forceguide_patch_jdb


class com_guideinfo_forceguide_jpn(base.component_base.comp_base):
    '''强制诱导
    '''


    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Guideinfo_ForceGuide')
        self.patch = comp_import_forceguide_patch_jdb()

    def _Do(self):

        self.CreateTable2('force_guide_tbl')
        # make road_gid with rdb data
        self.CreateTable2('road_gid')
        self.__CreateRoadGid()
        self.patch.make_forceguide_patch()
        self.CreateFunction2('mid_getSplitLinkID')
        self.CreateFunction2('mid_get_new_passlid')
        self.CreateFunction2('mid_getConnectionNodeID')
        self.CreateFunction2('rdb_get_connected_nodeid_ipc')
        self.CreateFunction2('mid_convert_forceguide')

        self.pg.callproc('mid_convert_forceguide')
        self.pg.commit2()

        self.CreateIndex2('force_guide_tbl_node_id_idx')

        self._check_passlink()
        return 0

    def __CreateRoadGid(self):
        sqlcmd = """
            insert into road_gid (objectid,fromlinkid,tolinkid, midcount,midlinkid,guidetype)
            (
                -- objectid, fromlink, tolink, midlinkcnt, midlinkid, guidetype
                select objectid,linkids[1] as fromlink, linkids[all_link_cnt] as tolink, (all_link_cnt-2) as midlinkcnt,
                (case when all_link_cnt = 2 then null else array_to_string(linkids[2:all_link_cnt-1], ',') end ) as midlinkid,
                guidetype_c from
                (
                    select b.objectid, b.linkids, array_upper(linkids,1) as all_link_cnt, c.guidetype_c from
                    (
                        select min(objectid) as objectid, array_agg(link_id) as linkids, inf_id from
                        (
                            select objectid,link_id,linkdir_c,sequence,inf_id from lq_guide 
                            order by inf_id, sequence
                        ) as a
                        group by inf_id
                    ) as b
                    left join inf_guide as c
                    on b.inf_id = c.objectid
                ) as d
            )
        """

        if self.pg.execute2(sqlcmd) == -1:
            exit(1)
        else:
            self.pg.commit2()

        return 0

    def _check_passlink(self):
        self.CreateFunction2('mid_check_guide_item_new')
        check_sql = '''
            select mid_check_guide_item_new({0});
        '''
#         try:
        self.pg.execute2(check_sql.format("\'force_guide_tbl\'"))
        row = self.pg.fetchone2()
        if row[0]:
            self.log.warning('passlink is not continuous!!!,num=%s', row[0])
            pass
#         except Exception:
#             self.log.error('passlink is not continuous!!!')
#             self.pg.commit2()
#         return
