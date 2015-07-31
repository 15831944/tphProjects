# -*- coding: cp936 -*-
'''
Created on 2012-3-27

@author:
'''
import os
import component
from common.common_func import GetPath
import base


class comp_guideinfo_forceguide_msm(base.component_base.comp_base):
    def __init__(self):
        '''
        Constructor
        '''
        base.component_base.comp_base.__init__(self, 'Guideinfo_ForceGuide')

    def _DoCreateTable(self):
        if self.CreateTable2('org_force_guide_patch') == -1:
            return -1
        if self.CreateTable2('force_guide_tbl') == -1:
            return -1

    def _DoCreateFunction(self):
        return 0

    def _Do(self):
        self._make_org_forceguide_patch()
        self._make_forceguide_tbl()
        self._make_forceguide_for_wrong_uturn()

        return 0

    def _make_org_forceguide_patch(self):
        sqlcmd = '''
            INSERT INTO org_force_guide_patch(
                inlinkid, outlinkid, passlid, passlink_cnt, guide_type,
                in_folder, out_folder)
            VALUES (121338, 121341, '', 0, 13, 'KL', 'KL')
                    ,(121338, 121339, '', 0, 3, 'KL', 'KL');
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()

    def _make_forceguide_tbl(self):
        insert_sql = '''
            INSERT INTO force_guide_tbl(
            force_guide_id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
            guide_type, position_type)
            VALUES (%s, %s, %s, %s, %s, %s,%s, %s);
        '''
        sqlcmd = '''
            SELECT inlinkid, outlinkid, passlid, passlink_cnt, guide_type,
               in_folder, out_folder
            FROM org_force_guide_patch;
        '''
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        i = 1
        for row in rows:
            inlinkid = row[0]
            outlinkid = row[1]
            passlid = row[2]
            passlink_cnt = row[3]
            guide_type = row[4]
            in_folder = row[5]
            out_folder = row[6]
            inlinkid_mid = self._get_midlink_by_orglink(inlinkid, in_folder)
            outlinkid_mid = self._get_midlink_by_orglink(outlinkid, out_folder)
            nodeid = self._get_nodeid_bylinks(inlinkid_mid, outlinkid_mid)
            self.pg.execute2(insert_sql, (i, nodeid, inlinkid_mid, outlinkid_mid,
                                          passlid, passlink_cnt, guide_type, 0))
            i = i + 1
        self.pg.commit2()

    def _get_midlink_by_orglink(self, orglinkid, folder):
        sqlcmd = '''
            SELECT new_link_id
            FROM temp_topo_link
            where folder = %s and link_id = %s;
        '''
        self.pg.execute2(sqlcmd, (folder, orglinkid))
        row = self.pg.fetchone2()
        return row[0]

    def _get_nodeid_bylinks(self, inlinkid, outlinkid):
        sqlcmd = '''
            SELECT start_node_id, end_node_id
            FROM temp_topo_link
            where new_link_id in (%s,%s);
        '''
        self.pg.execute(sqlcmd, (inlinkid, outlinkid))
        rows = self.pg.fetchall2()
        firs_node = (rows[0])[0]
        if firs_node in rows[1]:
            return firs_node
        else:
            return (rows[0])[1]

    def _make_forceguide_for_wrong_uturn(self):
        self.CreateFunction2('rdb_cal_zm')   
        self.CreateFunction2('rdb_cal_zm_lane') 
        
        self.CreateTable2('temp_link_wrong_uturn')  
        self.CreateTable2('temp_link_wrong_uturn_rectify')
               
        # Add forceguide to correct wrong U-turn.
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
                    
        sqlcmd = '''
            INSERT INTO force_guide_tbl(
                force_guide_id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt,
                guide_type, position_type)
            SELECT nextval('temp_link_forceguide_seq')
                ,b.e_node,a.in_link_id,a.out_link_id
                ,'',0,5,0
            FROM temp_link_wrong_uturn_rectify a
            left join link_tbl b
            on a.in_link_id = b.link_id;
        '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()   
