# -*- coding: UTF8 -*-
'''
Created on 2015-4-27

@author: liushengqiang
'''
import os
import component
import psycopg2

import component.default.guideinfo_forceguide
from common.common_func import GetPath


class comp_guideinfo_forceguide_rdf(component.default.guideinfo_forceguide.com_guideinfo_forceguide):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_forceguide.com_guideinfo_forceguide.__init__(self)

    def _DoCreateTable(self):
        
        self.CreateTable2('force_guide_tbl')
        
        return 0

    def _DoCreateFunction(self):
        
        self.CreateFunction2('mid_findpasslinkbybothnodes')
        
        return 0

    def _Do(self):
        
        self._deal_temp_patch_force_guide_tbl()  
        self._make_mid_temp_force_guide_tbl()
        self._update_force_guide_tbl()
        
        return 0
    
    def _make_mid_temp_force_guide_tbl(self):
        
        'use rdf_condition and rdf_nav_strand to make mid_temp_force_guide_tbl when rdf_condition.condition_type = 14(through route)'
        self.log.info('Now it is making mid_temp_force_guide_tbl...')
        
        self.CreateTable2('mid_temp_force_guide_tbl')
        self.CreateFunction2('rdf_update_mid_temp_force_guide_tbl')
        self.pg.callproc('rdf_update_mid_temp_force_guide_tbl')
        self.pg.commit2()
        
        self.log.info('making mid_temp_force_guide_tbl succeeded')
        return 0
    
    def _update_force_guide_tbl(self):
        
        self.log.info('Now it is updating force_guide_tbl...')
        sqlcmd = '''
            drop sequence if exists temp_link_forceguide_seq;
            create sequence temp_link_forceguide_seq;
            select setval('temp_link_forceguide_seq', cast(max_id as bigint))
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
                    force_guide_id, nodeid, inlinkid, outlinkid, passlid,
                    passlink_cnt, guide_type, position_type
                )
                select nextval('temp_link_forceguide_seq') as force_guide_id,
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                    0 as guide_type, 0 as position_type
                from (
                    select distinct a.nodeid, a.inlinkid, a.outlinkid, a.passlid, a.passlink_cnt
                    from mid_temp_force_guide_tbl a 
                    left join force_guide_tbl b
                        on 
                            b.nodeid = a.nodeid and
                            b.inlinkid = a.inlinkid and
                            b.outlinkid = a.outlinkid and
                            not(b.passlid is distinct from a.passlid) and
                            b.passlink_cnt = a.passlink_cnt
                    where b.gid IS NULL
                ) as c;
            """
        
        self.pg.do_big_insert2(sqlcmd)
        
        self.log.info('updating force_guide_tbl succeeded') 
        return 0
