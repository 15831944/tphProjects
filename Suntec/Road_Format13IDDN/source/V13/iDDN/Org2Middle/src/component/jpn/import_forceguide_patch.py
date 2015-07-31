# -*- coding: UTF-8 -*-
'''
Created on 2013-12-6

@author: zhangliang
'''

import base
import common
import io
import os


class comp_import_forceguide_patch_jpn(base.component_base.comp_base):

    def __init__(self):

        base.component_base.comp_base.__init__(self, 'force_guide_patch')
    
    def _copy_forceguide_X_patch_data(self,forceguide_patch_full_path, road_name):
        self.CreateTable2(road_name)
       
        pg = self.pg
        pgcur = self.pg.pgcur2
        
        f_force_guide_patch = io.open(forceguide_patch_full_path, 'r', 8192, 'euc-jp')
         
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
    
    def _DoCreateTable(self):
        if self.CreateTable2('temp_update_patch_force_guide_tbl') == -1:
            return -1
        
        if self.CreateTable2('temp_append_patch_force_guide_tbl') == -1:
            return -1
        
        if self.CreateTable2('temp_update_path_force_guide_tbl_matching_tbl') == -1:
            return -1
        
        if self.CreateTable2('temp_friendly_append_patch_force_guide_tbl') == -1:
            return -1
        
        return 0
    
    def _DoCreateFunction(self):
        
        if self.CreateFunction2('mid_make_link_list_by_nodes') == -1:
            return -1
            
        return 0
    
    def __deal_with_forceguide_update(self):
        # import data
        forceguide_patch_update_full_path = common.GetPath('forceguide_update')
        self._copy_forceguide_X_patch_data(forceguide_patch_update_full_path, 'road_force_guide_update_patch_tbl')
    
        # make temporarily table and then to update current force guide status
        sqlcmd = """
            insert into temp_update_patch_force_guide_tbl
            (
                select gid, link_array[1] as fromlinkid, link_array[link_cnt] as tolinkid, (link_cnt -2) as midcount, array_to_string(link_array[2:link_cnt-1],',') as midlink,
                    (guide_code_int + 1) as guidetype
                from
                (
                    select gid, link_array, array_upper(link_array,1) as link_cnt, guide_code_int
                    from
                    (
                        select gid,mid_make_link_list_by_nodes(array[node1,node2,node3,node4,node5,node6,node7]) as link_array,guide_code_int
                        from road_force_guide_update_patch_tbl
                    ) as a
                ) as b
            ) 
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            insert into temp_update_path_force_guide_tbl_matching_tbl
            (
                select a.objectid as new_objectid, a.fromlinkid as new_fromlinkid, a.tolinkid as new_tolinkid,a.midcount as new_midcount,a.midlinkid as new_midlinkid,a.guidetype as new_guidetype,
               b.objectid as old_objectid, b.fromlinkid as old_fromlinkid, b.tolinkid as old_tolinkid, b.midcount as old_midcount, b.midlinkid as old_midlinkid, b.guidetype as  old_guidetype
              from temp_update_patch_force_guide_tbl as a
              left join road_gid as b
              on a.fromlinkid = b.fromlinkid and a.tolinkid = b.tolinkid and (a.midlinkid = b.midlinkid or (a.midcount = 0 and b.midcount =0) )
            );
        """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        sqlcmd = """
            update road_gid
            set guidetype = a.new_guidetype
            from temp_update_path_force_guide_tbl_matching_tbl as a
            where a.old_objectid is not null and road_gid.objectid = a.old_objectid
        """
        
        self.pg.execute2(sqlcmd)
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
                select %s+ 5000+ new_objectid, %s+ 5000+ new_objectid, new_fromlinkid, new_tolinkid,
                new_midcount, new_midlinkid, new_guidetype 
                from temp_update_path_force_guide_tbl_matching_tbl
                where old_objectid is null
            )
        """ %(max_id, max_id)

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __deal_with_forceguide_append(self):
        # import forceguide_patch data into database
        forceguide_patch_append_full_path = common.GetPath('forceguide_append')
        self._copy_forceguide_X_patch_data(forceguide_patch_append_full_path, 'road_force_guide_append_patch_tbl')
        
        sqlcmd = """
            insert into temp_append_patch_force_guide_tbl
            (
                select gid, link_array[1] as fromlinkid, link_array[link_cnt] as tolinkid, (link_cnt -2) as midcount, array_to_string(link_array[2:link_cnt-1],',') as midlink,
                    (guide_code_int +1) as guidetype
                from
                (
                    select gid, link_array, array_upper(link_array,1) as link_cnt, guide_code_int
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
        
        # append records into road_gid from append patch
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
                select %s+ 6000 + objectid, %s+ 6000+ objectid, fromlinkid, tolinkid,
                midcount, midlinkid, guidetype 
                from temp_append_patch_force_guide_tbl
            )
        """ %(max_id, max_id)

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def __deal_with_forceguide_friendly_append(self):
        # import forceguide_patch data into database
        forceguide_friendly_patch_append_full_path = common.GetPath('forceguide_friendly_append')
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
        
        # append records into road_gid from append patch
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
                select %s+ 7000 + objectid, %s+ 7000+ objectid, fromlinkid, tolinkid,
                midcount, midlinkid, guidetype 
                from temp_friendly_append_patch_force_guide_tbl
            )
        """ %(max_id, max_id)

        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
    def _Do(self):
        # judgment whether needs to do forceguide patch operation
        forceguide_patch_run_status = common.GetPath('forceguide_patch_open')
        if cmp(forceguide_patch_run_status.strip().upper(),'TRUE') <> 0 :
            return 0
        
        self.__deal_with_forceguide_update()
        self.__deal_with_forceguide_append()
        self.__deal_with_forceguide_friendly_append()
        
        
        
        
        
        

        
        
        
        
        
 