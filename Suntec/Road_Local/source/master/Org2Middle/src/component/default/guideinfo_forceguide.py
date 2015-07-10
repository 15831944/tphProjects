# -*- coding: UTF8 -*-
'''
Created on 2012-11-9

@author: hongchenzai
'''

import component.component_base
from common.common_func import GetPath
import os

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
        
        self.import_patch()
        self._update_temp_force_guide_patch_node_tbl()
        self._update_temp_force_guide_patch_link_tbl()
        self._make_force_guide_tbl()
        
        self.log.info('dealing temp_patch_force_guide_tbl.csv succeeded')
        return 0
    
    def import_patch(self):   
        self.log.info('Now it is importing force_guide_patch...')
        
        forceguide_patch_full_path = GetPath('forceguide_patch_full_path')
        if forceguide_patch_full_path:
            self.pg.execute2('''
                drop table if exists old_force_guide_patch;
                CREATE TABLE old_force_guide_patch
                (
                  gid serial NOT NULL,
                  guide_type smallint,
                  node1_geom geometry,
                  z1 smallint,
                  node2_geom geometry,
                  z2 smallint,
                  node3_geom geometry,
                  z3 smallint,
                  node4_geom geometry,
                  z4 smallint,
                  node5_geom geometry,
                  z5 smallint,
                  node6_geom geometry,
                  z6 smallint,
                  node7_geom geometry,
                  z7 smallint,
                  node8_geom geometry,
                  z8 smallint,
                  node9_geom geometry,
                  z9 smallint,
                  node10_geom geometry,
                  z10 smallint,
                  node11_geom geometry,
                  z11 smallint,
                  node12_geom geometry,
                  z12 smallint,
                  node13_geom geometry,
                  z13 smallint,
                  node14_geom geometry,
                  z14 smallint,
                  node15_geom geometry,
                  z15 smallint,
                  node16_geom geometry,
                  z16 smallint,
                  node17_geom geometry,
                  z17 smallint,
                  node18_geom geometry,
                  z18 smallint,
                  node19_geom geometry,
                  z19 smallint,
                  node20_geom geometry,
                  z20 smallint
                )
            ''')
            
            if os.path.exists(forceguide_patch_full_path):
                f_force_guide_patch = open(forceguide_patch_full_path, 'r')
                self.pg.copy_from2(f_force_guide_patch, 'old_force_guide_patch', ',', "", 8192,
                        ('guide_type',
                         'node1_geom', 'z1',
                         'node2_geom', 'z2',
                         'node3_geom', 'z3',
                         'node4_geom', 'z4',
                         'node5_geom', 'z5',
                         'node6_geom', 'z6',
                         'node7_geom', 'z7',
                         'node8_geom', 'z8',
                         'node9_geom', 'z9',
                         'node10_geom', 'z10',
                         'node11_geom', 'z11',
                         'node12_geom', 'z12',
                         'node13_geom', 'z13',
                         'node14_geom', 'z14',
                         'node15_geom', 'z15',
                         'node16_geom', 'z16',
                         'node17_geom', 'z17',
                         'node18_geom', 'z18',
                         'node19_geom', 'z19',
                         'node20_geom', 'z20'
                       ))
                f_force_guide_patch.close()
            
            self.pg.commit2()
            
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
                order by nodeid, inlinkid, outlinkid, guide_type
            """
            
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('making force_guide_tbl succeeded')
        return 0