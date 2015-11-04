# -*- coding: UTF8 -*-
'''
Created on 2015-4-27

@author: liushengqiang
'''



import component.default.guideinfo_forceguide

class comp_guideinfo_forceguide_rdf(component.default.guideinfo_forceguide.com_guideinfo_forceguide):
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
        
        'use rdf_condition and rdf_nav_strand to make mid_temp_force_guide_tbl when rdf_condition.condition_type = 14(through route)'
        self.log.info('Begin making mid_temp_force_guide_tbl...')
        
        self.CreateTable2('mid_temp_force_guide_tbl')
        self.CreateFunction2('rdf_update_mid_temp_force_guide_tbl')
        self.pg.callproc('rdf_update_mid_temp_force_guide_tbl')
        self.pg.commit2()
        
        self.log.info('End making mid_temp_force_guide_tbl.')
        return 0