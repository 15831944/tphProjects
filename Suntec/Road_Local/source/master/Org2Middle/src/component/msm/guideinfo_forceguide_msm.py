# -*- coding: UTF8 -*-
'''
Created on 2012-3-27

@author:
'''



import component.default.guideinfo_forceguide

class comp_guideinfo_forceguide_msm(component.default.guideinfo_forceguide.com_guideinfo_forceguide):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_forceguide.com_guideinfo_forceguide.__init__(self)
    
    def _make_forceguide_from_origin(self):
        
        self.log.info('Begin making force guide from origin data...')
        
        self._make_forceguide_for_wrong_uturn()
        self._update_force_guide_tbl_by_origin_data()
        
        self.log.info('End making force guide from origin data.')
        return 0

    def _make_forceguide_for_wrong_uturn(self):
        
        self.log.info('Begin making force guide from wrong uturn...')
        
        self.CreateFunction2('rdb_cal_zm')   
        self.CreateFunction2('rdb_cal_zm_lane') 
        
        self.CreateTable2('temp_link_wrong_uturn')  
        self.CreateTable2('temp_link_wrong_uturn_rectify')
        
        self.CreateTable2('mid_temp_force_guide_tbl')
        
        # Add forceguide to correct wrong U-turn.
        
        sqlcmd = """
                INSERT INTO mid_temp_force_guide_tbl(
                    nodeid, inlinkid, outlinkid, passlid, passlink_cnt, guide_type
                )
                -- Add right turn. angle [70-105].
                SELECT b.e_node as nodeid, a.in_link_id as inlinkid, 
                    a.out_link_id as outlinkid, NULL::varchar as passlid, 
                    0::smallint as passlink_cnt, 5::smallint as guide_type
                FROM (
                    SELECT distinct in_link_id, out_link_id
                    FROM temp_link_wrong_uturn_rectify
                ) a
                LEFT JOIN link_tbl b
                    ON a.in_link_id = b.link_id
                
                UNION
                
                -- Add left turn. angle [70-110]. 
                SELECT b.e_node as nodeid, a.in_link_id as inlinkid,
                    a.cnt_link_id as outlinkid, NULL::varchar as passlid, 
                    0::smallint as passlink_cnt, 11::smallint as guide_type
                FROM (
                    SELECT distinct in_link_id, cnt_link_id, left_angle
                    FROM temp_link_wrong_uturn_rectify
                ) a
                LEFT JOIN link_tbl b
                    ON a.in_link_id = b.link_id
                WHERE a.left_angle >= 70 and a.left_angle <= 110
            """ 
            
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.log.info('End making force guide from wrong uturn.')
        return 0