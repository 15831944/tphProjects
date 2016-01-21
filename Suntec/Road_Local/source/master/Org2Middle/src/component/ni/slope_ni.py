# -*- coding: UTF8 -*-
'''
Created on 2015-12-30

@author: zhaojie
'''
import component.component_base

class comp_slope_ni(component.component_base.comp_base):
    '''slope
    '''

    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'slope_tbl')
        
    def _DoCreateTable(self): 
        self.CreateTable2('grad')
        
    def _Do(self):
        self.log.info('start get grad....')
        sqlcmd = '''
                insert into grad(objectid, link_id, grad_value, shape)
                select row_number() over() as objectid,
                       outlinkid::bigint as link_id,
                       (case when slope = '1' then 40
                             when slope = '2' then 0
                             when slope = '3' then -40
                             else -1 end) as grad_value,
                        (case when a.id = b.snodeid then ST_Multi(b.the_geom)
                              when a.id = b.enodeid then ST_Multi(ST_Reverse(b.the_geom))
                              else null end) as shape
                from org_c as a
                left join org_r as b
                on a.outlinkid = b.id
                where a.condtype = '7';
                '''
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        self.CreateIndex2('grad_link_id_idx')
        self.CreateIndex2('grad_objectid_idx')
        self.CreateIndex2('grad_shape_idx')