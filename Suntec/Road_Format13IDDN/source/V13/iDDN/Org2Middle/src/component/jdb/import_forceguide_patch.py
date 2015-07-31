# -*- coding: UTF-8 -*-
'''
Created on 2013-12-6

@author: zhangliang
'''

import base
import common
import io


class comp_import_forceguide_patch_jdb(base.component_base.comp_base):

    def __init__(self):

        base.component_base.comp_base.__init__(self, 'force_guide_patch')

    def _copy_forceguide_X_patch_data(self,forceguide_patch_full_path, road_name):
        self.CreateTable2(road_name)

        pg = self.pg
        pgcur = self.pg.pgcur2

        f_force_guide_patch = io.open(forceguide_patch_full_path,
                                      'r', 8192, 'euc-jp')

        pgcur.copy_from(f_force_guide_patch, road_name, '\t', "", 8192,
                        ('guide_code','guide_code_int','meshcode1','node1',
                       'meshcode2','node2',
                       'meshcode3','node3',
                       'meshcode4','node4',
                       'meshcode5','node5',
                       'meshcode6','node6',
                       'meshcode7','node7'))
        pg.commit2()

        f_force_guide_patch.close()

        return 0

    def _Do_Create_Temp_Table(self):
        if self.CreateTable2('temp_update_patch_force_guide_tbl') == -1:
            return -1

        if self.CreateTable2('temp_append_patch_force_guide_tbl') == -1:
            return -1

        if self.CreateTable2('temp_update_path_force_guide_tbl_matching_tbl') == -1:
            return -1

        if self.CreateTable2('temp_friendly_append_patch_force_guide_tbl') == -1:
            return -1
        if self.CreateTable2('temp_append_update_path_force_guide_tbl_matching_tbl') == -1:
            return -1
        if self.CreateTable2('temp_friendly_update_path_force_guide_tbl_matching_tbl') == -1:
            return -1
        return 0

    def _Do_Create_Temp_Function(self):

        if self.CreateFunction2('mid_make_link_list_by_nodes') == -1:
            return -1
        if self.CreateFunction2('mid_findpasslinkbybothnodes') == -1:
            return -1
        return 0

    def __deal_with_forceguide_update(self):
        # import data
        forceguide_patch_update_full_path = common.GetPath('forceguide_update')
        self._copy_forceguide_X_patch_data(forceguide_patch_update_full_path,
                                           'road_force_guide_update_patch_tbl')

        # make temporarily table and then to update current force guide status
        sqlcmd = """
            insert into temp_update_patch_force_guide_tbl
            (
                select gid, link_array[1] as fromlinkid,
                    link_array[link_cnt] as tolinkid,
                    (link_cnt -2) as midcount,
                    array_to_string(link_array[2:link_cnt-1],',') as midlink,
                    (guide_code_int + 1) as guidetype
                from
                (
                    select gid, link_array,
                        array_upper(link_array,1) as link_cnt, guide_code_int
                    from
                    (
                        select gid, mid_make_link_list_by_nodes(array[node1,node2,node3,node4,node5,node6,node7]) as link_array,guide_code_int
                        from road_force_guide_update_patch_tbl
                    ) as a
                ) as b
            )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self._split_link_handler('temp_update_patch_force_guide_tbl', 'road_force_guide_update_patch_tbl')
        self._update_old_patch('temp_update_path_force_guide_tbl_matching_tbl',
                               'temp_update_patch_force_guide_tbl')

    def __deal_with_forceguide_append(self):
        # import forceguide_patch data into database
        forceguide_patch_append_full_path = common.GetPath('forceguide_append')
        self._copy_forceguide_X_patch_data(forceguide_patch_append_full_path,
                                           'road_force_guide_append_patch_tbl')

        sqlcmd = """
            insert into temp_append_patch_force_guide_tbl
            (
            select gid, link_array[1] as fromlinkid,
                link_array[link_cnt] as tolinkid,(link_cnt -2) as midcount,
                array_to_string(link_array[2:link_cnt-1],',') as midlink,
                (guide_code_int +1) as guidetype
                from
                (
                    select gid, link_array,
                        array_upper(link_array,1) as link_cnt, guide_code_int
                    from
                    (
                        select gid,mid_make_link_list_by_nodes(array[node1,node2,node3,node4,node5,node6,node7]) as link_array,guide_code_int
                        from road_force_guide_append_patch_tbl
                    ) as a
                ) as b
            )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self._split_link_handler('temp_append_patch_force_guide_tbl', 'road_force_guide_append_patch_tbl')
        self._update_old_patch('temp_append_update_path_force_guide_tbl_matching_tbl',
                               'temp_append_patch_force_guide_tbl')


    def __deal_with_forceguide_friendly_append(self):
        # import forceguide_patch data into database
        forceguide_friendly_patch_append_full_path = \
                                common.GetPath('forceguide_friendly_append')
        self._copy_forceguide_X_patch_data(forceguide_friendly_patch_append_full_path, 'road_force_guide_friendly_append_patch_tbl')

        sqlcmd = """
            insert into temp_friendly_append_patch_force_guide_tbl
            (
                select gid, link_array[1] as fromlinkid, link_array[link_cnt] as tolinkid, (link_cnt -2) as midcount, array_to_string(link_array[2:link_cnt-1],',') as midlink,
                    guide_code_int as guidetype
                from
                (
                    select gid, link_array, array_upper(link_array,1) as link_cnt, guide_code_int
                    from
                    (
                        select gid,mid_make_link_list_by_nodes(array[node1,node2,node3,node4,node5,node6,node7]) as link_array,guide_code_int
                        from road_force_guide_friendly_append_patch_tbl
                    ) as a
                ) as b
            )
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

        self._split_link_handler('temp_friendly_append_patch_force_guide_tbl', 'road_force_guide_friendly_append_patch_tbl')

        self._update_old_patch('temp_friendly_update_path_force_guide_tbl_matching_tbl',
                               'temp_friendly_append_patch_force_guide_tbl')
        # append records into road_gid from append patch
#         sqlcmd = """
#                 select max(objectid) as max_id
#                 from road_gid
#                 """
#
#         self.pg.execute2(sqlcmd)
#         row = self.pg.fetchone2()
#         if row:
#             max_id = row[0]
#
#         sqlcmd = """
#             insert into road_gid(gid, objectid, fromlinkid, tolinkid,
#             midcount,midlinkid, guidetype)
#             (
#                 select %s+ 7000 + objectid, %s+ 7000+ objectid, fromlinkid, tolinkid,
#                 midcount, midlinkid, guidetype
#                 from temp_friendly_append_patch_force_guide_tbl
#                 where objectid not in (
#                     select objectid
#                     from temp_friendly_update_path_force_guide_tbl_matching_tbl
#                 )
#             )
#         """ %(max_id, max_id)
#
#         self.pg.execute2(sqlcmd)
#         self.pg.commit2()

    def _split_link_handler(self,temp_tbl_name,org_tbl_name):
        sqlcmd = '''
            SELECT objectid, guidetype,array[node1, node2, node3,  node4, node5, node6, node7]
            FROM {0} as a
            left join {1} as b
            on a.objectid = b.gid
            where a.fromlinkid is null or a.tolinkid is null;
        '''
        self.pg.execute2(sqlcmd.format(temp_tbl_name,org_tbl_name))
        c_rows = self.pg.fetchall2()
        for c_row in c_rows:
            all_linklib = ''
            objectid = c_row[0]
            nodes = c_row[2]
            for i in range(6):
                f_node = nodes[i]
                t_node = nodes[i + 1]
                if f_node and t_node:
                    self.pg.execute2('''
                        select mid_findpasslinkbybothnodes(%s,%s)
                    ''', (f_node, t_node))
                    sub_linklib = (self.pg.fetchone2())[0]
                    if sub_linklib:
                        all_linklib = all_linklib + sub_linklib + ','
            link_array = all_linklib.split(',')
            length = len(link_array)
            if length >= 3:
                midlinkid = ''
                j = 1
                while j < length - 2:
                    midlinkid = midlinkid + link_array[j] + ','
                    j = j + 1
                self.pg.execute2('''
                UPDATE {0}
                SET fromlinkid=%s, tolinkid=%s, midcount=%s,
                    midlinkid=%s
                 WHERE objectid = %s;
                '''.format(temp_tbl_name), (link_array[0], link_array[length - 2], length - 3,
                      midlinkid.rstrip(','), objectid))
            else:
                self.log.error('get route is fail!!!')
        self.pg.commit2()
        return
    
    def _update_old_patch(self, match_table_name, table_name):
        sqlcmd = """
                insert into {0}
                (
                select a.objectid as new_objectid, a.fromlinkid as new_fromlinkid,
                    a.tolinkid as new_tolinkid,a.midcount as new_midcount,
                    a.midlinkid as new_midlinkid,a.guidetype as new_guidetype,
                   b.objectid as old_objectid, b.fromlinkid as old_fromlinkid,
                   b.tolinkid as old_tolinkid, b.midcount as old_midcount,
                   b.midlinkid as old_midlinkid, b.guidetype as  old_guidetype
                from {1} as a
                left join road_gid as b
                on a.fromlinkid = b.fromlinkid and a.tolinkid = b.tolinkid
                    and (a.midlinkid = b.midlinkid
                    or (a.midcount = 0 and b.midcount =0) )
                );
            """
        self.pg.execute2(sqlcmd.format(match_table_name, table_name))
        self.pg.commit2()

        sqlcmd = """
            update road_gid
            set guidetype = a.new_guidetype
            from {0} as a
            where a.old_objectid is not null
                and road_gid.objectid = a.old_objectid
        """
        self.pg.execute2(sqlcmd.format(match_table_name))
        self.pg.commit2()

        # append records into road_gid from update patch
        sqlcmd = """
                select max(objectid) as max_id
                from road_gid
                """

        self.pg.execute2(sqlcmd)
        row = self.pg.fetchone2()
        if row:
            max_id = row[0]

        sqlcmd = """
            insert into road_gid(gid, objectid, fromlinkid, tolinkid,
            midcount,midlinkid, guidetype)
            (
                select %s+ 5000+ new_objectid, %s+ 5000+ new_objectid,
                    new_fromlinkid, new_tolinkid,
                new_midcount, new_midlinkid, new_guidetype
                from {0}
                where old_objectid is null
            )
        """ % (max_id, max_id)

        self.pg.execute2(sqlcmd.format(match_table_name))
        self.pg.commit2()

    def make_forceguide_patch(self):
        self._Do_Create_Temp_Table()
        self._Do_Create_Temp_Function()
        # judgment whether needs to do forceguide patch operation
        forceguide_patch_run_status = common.GetPath('forceguide_patch_open')
        if cmp(forceguide_patch_run_status.strip().upper(),'TRUE') <> 0 :
            return 0
        self.__deal_with_forceguide_update()
        self.__deal_with_forceguide_append()
        self.__deal_with_forceguide_friendly_append()
