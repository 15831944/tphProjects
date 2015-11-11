# -*- coding: UTF8 -*-
'''
Created on 2015-5-4

@author:
'''



import component.default.guideinfo_forceguide

class comp_guideinfo_forceguide_ni(component.default.guideinfo_forceguide.com_guideinfo_forceguide):
    def __init__(self):
        '''
        Constructor
        '''
        component.default.guideinfo_forceguide.com_guideinfo_forceguide.__init__(self)
    
    def _make_forceguide_from_origin(self):
        
        self.log.info('Begin making force guide from origin data...')
        
        self._make_mid_temp_force_guide_tbl()
        self._update_force_guide_tbl_by_origin_data()
        
        self.log.info('End making force guide from origin data.')
        return 0
    
    def _make_mid_temp_force_guide_tbl(self):
        
        self.log.info('Begin making mid_temp_force_guide_tbl...')
        
        # 作成表单mid_temp_force_guide_tbl记录原始数据提供的force guide信息
        self.CreateIndex2('org_cond_mapid_condid_idx')
        self.CreateIndex2('org_cnl_mapid_condid_idx')
        self.CreateTable2('mid_temp_force_guide_tbl')
        
        self.CreateFunction2('ni_update_mid_temp_force_guide_tbl')
        self.pg.callproc('ni_update_mid_temp_force_guide_tbl')
        self.pg.commit2()
        
        self.log.info('End making mid_temp_force_guide_tbl.')
        return 0