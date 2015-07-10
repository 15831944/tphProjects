# -*- coding: UTF8 -*-
'''
Created on 2012-11-9

@author: hongchenzai
'''

import component.component_base
from common.common_func import GetPath
import os
import common.cache_file

class com_guideinfo_forceguide(component.component_base.comp_base):
    '''强制诱导的Base Class
    '''


    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_ForceGuide')
        
    def _DoCreateTable(self):
        
        self.CreateTable2('force_guide_tbl')
        
        return 0
    
    def _DoCreateFunction(self):
        
        return 0
        
        
    def _Do(self):
        
        return 0
    
    def _deal_temp_patch_force_guide_tbl(self):
        
        self.log.info('Now it is dealing temp_patch_force_guide_tbl.csv...')
        
        if self.import_patch() == -1:
            return -1
        
        self._update_temp_force_guide_patch_node_tbl()
        self._update_temp_force_guide_patch_link_tbl()
        self._make_force_guide_tbl()
        
        self.log.info('dealing temp_patch_force_guide_tbl.csv succeeded')
        return 0
    
    def import_patch(self): 
        
        forceguide_patch_full_path = GetPath('forceguide_patch_full_path')
        if forceguide_patch_full_path:
            self.log.info('Now it is importing force_guide_patch...')
            if os.path.exists(forceguide_patch_full_path):
                self.CreateTable2('temp_force_guide_patch_tbl')
                temp_file_obj = common.cache_file.open('temp_force_guide_patch_tbl')
                f_force_guide_patch = open(forceguide_patch_full_path, 'r') 
                for line in f_force_guide_patch.readlines():
                    linelist = []
                    linelist = line.split('\t')
                    linelength = len(linelist)
                    guidetype = int(linelist[0])
                    temp_geom = ''
                    temp_z = ''
                    loop = 1
                    while loop <= (linelength-2):
                        if linelist[loop] != '':
                            temp_geom += linelist[loop]
                            temp_z += linelist[loop+1]
                            temp_geom += '|'
                            temp_z += '|'
                        loop += 2
                        
                    if temp_geom[-1] == '|':
                        temp_geom = temp_geom[0:-1]
                        
                    if temp_z[-1] == '|':
                        temp_z = temp_z[0:-1]
                    
                    rec_string = '%d\t%s\t%s\n' % (guidetype, temp_geom, temp_z)
                    temp_file_obj.write(rec_string)
                temp_file_obj.seek(0)
                self.pg.copy_from2(temp_file_obj, 'temp_force_guide_patch_tbl')
                self.pg.commit2()
                common.cache_file.close(temp_file_obj,True)
                f_force_guide_patch.close()
            
            self.CreateTable2('temp_node_z_tbl')
            self.CreateFunction2('mid_update_temp_node_z_tbl')
            self.pg.callproc('mid_update_temp_node_z_tbl')
            self.pg.commit2()
            
            self.log.info('importing force_guide_patch succeeded')
            return 0
        
        return -1
    
    def _update_temp_force_guide_patch_node_tbl(self):
        
        self.log.info('Now it is updating temp_force_guide_patch_node_tbl...')
        
        self.CreateTable2('temp_force_guide_patch_node_tbl')
        
        self.CreateFunction2('mid_update_temp_force_guide_patch_node_tbl')
        self.pg.callproc('mid_update_temp_force_guide_patch_node_tbl')
        self.pg.commit2()
        
        self.log.info('updating temp_force_guide_patch_node_tbl succeeded')
        return 0
    
    def _update_temp_force_guide_patch_link_tbl(self):
        
        self.log.info('Now it is updating temp_force_guide_patch_link_tbl...')
        
        self.CreateTable2('temp_force_guide_patch_link_tbl')
        
        self.CreateFunction2('mid_findpasslinkbybothnodes')
        self.CreateFunction2('mid_update_temp_force_guide_patch_link_tbl')
        self.pg.callproc('mid_update_temp_force_guide_patch_link_tbl')
        self.pg.commit2()
        
        self.CreateIndex2('temp_force_guide_patch_link_tbl_objectid')
        
        self.log.info('updating temp_force_guide_patch_link_tbl succeeded')
        return 0
    
    def _make_force_guide_tbl(self):
        
        self.log.info('Now it is making force_guide_tbl...')
        
        self.CreateFunction2('mid_get_connectnode_by_links')
        self.CreateFunction2('mid_get_position_type_by_links')
        
        sqlcmd = '''
            drop sequence if exists temp_force_guide_tbl_seq;
            create sequence temp_force_guide_tbl_seq;
            select setval('temp_force_guide_tbl_seq', cast(max_id as bigint))
            from
            (
                select max(force_guide_id) as max_id
                from force_guide_tbl
            )as a;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2() 
        
        sqlcmd = """
                insert into force_guide_tbl (
                    force_guide_id,
                    nodeid, 
                    inlinkid,
                    outlinkid,
                    passlid,
                    passlink_cnt,
                    guide_type,
                    position_type
                )
                select
                    nextval('temp_force_guide_tbl_seq') as force_guide_id,
                    mid_get_connectnode_by_links(inlinkid, secondlink) as nodeid,
                    inlinkid, outlinkid, passlid, passlink_cnt, guide_type,
                    mid_get_position_type_by_links(inlinkid, outlinkid) as position_type
                from (
                    select 
                        link_id_list[1] as inlinkid, 
                        link_id_list[2] as secondlink,
                        link_id_list[array_upper(link_id_list, 1)] as outlinkid, 
                        case when array_upper(link_id_list, 1) - 2 = 0 then null else array_to_string(link_id_list[2:array_upper(link_id_list, 1)-1], '|') end as passlid,
                        array_upper(link_id_list, 1) - 2 as passlink_cnt,
                        a.guide_type
                    from temp_force_guide_patch_node_tbl a
                    left join temp_force_guide_patch_link_tbl b
                        on a.id = b.objectid
                    where b.objectid is not null
                ) c
            """
            
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('making force_guide_tbl succeeded')
        return 0