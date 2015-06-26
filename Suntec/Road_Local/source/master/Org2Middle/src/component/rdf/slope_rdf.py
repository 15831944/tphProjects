# -*- coding: UTF8 -*-
'''
Created on 2014-10-9

@author: zhaojie
'''

import io
import os

import component.component_base

class comp_slope_rdf(component.component_base.comp_base):
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'slope') 
        
    def _DoCreateTable(self):
        
        self.CreateTable2('grad')
        self.CreateTable2('temp_adas_link_geometry')
        
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_adas_link_delete_addad_node')
        self.CreateFunction2('mid_get_adas_slope')
        self.CreateFunction2('mid_get_adas_len_3D')
        self.CreateFunction2('mid_get_adas_real_num')
        self.CreateFunction2('mid_transform_slope_value')
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        self.CreateIndex2('grad_link_id_idx')
        self.CreateIndex2('grad_objectid_idx')
        
        return 0
      
    def _Do(self):
        
        #jude country UC
        sqlcmd = '''
                SELECT count(*)
                FROM rdf_country
                where iso_country_code in ('USA','CAN','PRI','VIR','MEX');
                '''
              
        row = self.__GetRows(sqlcmd)
        if row[0][0] == 0:
            return 0
        
        self.__delete_added_node()
        self._insert_into_grad()
        
        return 0
    
    def __delete_added_node(self):
        self.log.info('start delete added node!')
        
        self.CreateIndex2('adas_link_geometry_link_id_idx')
        sqlcmd = '''
                insert into temp_adas_link_geometry(link_id, slope_lib)
                (
                    select link_id, unnest(mid_adas_link_delete_addad_node(seq_num_array, lat_array,
                        lon_array, z_coord_array, flage_array, slope_array)) as slope_lib
                    from
                    (
                        select link_id, array_agg(seq_num) as seq_num_array, array_agg(lat) as lat_array
                            , array_agg(lon) as lon_array, array_agg(z_coord) as z_coord_array
                            , array_agg(vertical_flag) as flage_array, array_agg(slope) as slope_array
                        from adas_link_geometry
                        where slope is not null
                        group by link_id
                    )temp
                );
                '''
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info('end delete added node!')
        return 0
    
    def _insert_into_grad(self):
        self.log.info('start insert into  grad!')

        sqlcmd = '''
                insert into grad(objectid, link_id, grad_value, shape)
                (
                    select gid, link_id, grad_value, ST_Multi(ST_MakeLine(shape_array)) as shape
                    from
                    (
                        select gid, link_id, slope_array[1]::integer as grad_value, 
                            slope_array[2:array_upper(slope_array,1)]::geometry[] as shape_array
                        from
                        (
                            select gid, link_id, regexp_split_to_array(slope_lib, E'\\\|+') as slope_array
                            from temp_adas_link_geometry
                        )temp
                    )temp1
                );
                '''
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        self.log.info('end insert into  grad!')
        return 0
    
    def __GetRows(self,sqlcmd):
        if sqlcmd:
            self.pg.execute2(sqlcmd)
            return self.pg.fetchall2()
        else:
            self.log.error('sqlcmd is null;')
    
    def _DoCheckValues(self):
        self.log.info('start CheckValues!')
        sqlcmd = '''    
                select count(*)
                from grad
                where grad_value not in(-40,-30,-20,-10,0,10,20,30,40);
             '''
        rows = self.__GetRows(sqlcmd)
        if rows[0][0] > 0:            
            self.log.error('grad table grad_value is error, %s',rows[0][0])
            return -1
            
        self.log.info('end CheckValues!')
        return 0
    
    def _DoCheckLogic(self):
        self.log.info('start CheckLogic!')
        
        sqlcmd = '''
                select count(*)
                from grad as a
                join link_tbl as b
                on a.link_id = b.link_id and not ST_Contains(b.the_geom, a.shape);              
             '''
#        rows = self.__GetRows(sqlcmd)
#        if rows[0][0] > 0:            
#            self.log.error('grad table geom is error, %s',rows[0][0])
#            return -1
            
        self.log.info('end CheckLogic!')

        return 0
    

