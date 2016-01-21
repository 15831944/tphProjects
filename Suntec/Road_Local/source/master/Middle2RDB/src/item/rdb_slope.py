# -*- coding: cp936 -*-
'''
Created on 2014-5-6

@author: mwj
'''
from base.rdb_ItemBase import ItemBase
from common import rdb_log
from common import rdb_common

class rdb_slope(ItemBase):
    '''
    rdb_slope
    '''
    def __init__(self):
        '''
        Constructor
        '''        
        ItemBase.__init__(self,'slope','','','','')
       
    def Do_CreateTable(self):
        self.CreateTable2('rdb_slope_tbl')
        self.CreateTable2('temp_org_slope')
        self.CreateTable2('temp_rdb_slope')
         
        return 0
    
    def Do_CreateFunction(self):
        '''
        Create Function
        '''
        if self.CreateFunction2('rdb_cnv_slope_value') == -1:
            return -1
        if self.CreateFunction2('rdb_check_slopes_ipc') == -1:
            return -1
                
        return 0
    
    def Do(self):
        '''
        Make slope info 
        '''
        if not self.pg.IsExistTable('grad'):
            return 0

#        slope_data_status = rdb_common.GetPath('slope_data_support')
#        if slope_data_status.lower() != 'true':
#            return 0
        
        # step 1: insert info to temp_org_slope
        sqlcmd = """
            INSERT INTO temp_org_slope(
                rdb_link_id,
                org_link_id,
                slope_value,
                slope_shape,
                link_geom)
            (
                SELECT 
                    b.target_link_id,
                    a.link_id,
                    a.grad_value,    
                    st_transform( a.shape, 4326 ),
                    b.target_geom
                FROM (
                    SELECT 
                        link_id,
                        grad_value,
                        (ST_Dump(shape)).geom AS shape
                    FROM grad
                ) AS a
                INNER JOIN temp_link_org_rdb AS b
                ON a.link_id = b.org_link_id
            );
            """
        rdb_log.log(self.ItemName, 'Now it is inserting to temp_org_slope...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        # step 2: create rdb_slope info into temp_rdb_slope
        sqlcmd = """
            INSERT INTO temp_rdb_slope(
                rdb_link_id,
                link_pos_s, 
                link_pos_e,
                slope_value,
                slope_shape)           
            (
                SELECT 
                    rdb_link_id,
                    link_pos_s,
                    link_pos_e,
                    slope_value,
                    slope_shape                  
                FROM(            
                    SELECT 
                        rdb_link_id,
                        link_pos_s,
                        link_pos_e,
                        slope_value,
                        slope_shape,
                        abs(ST_X(slope_pt_s)-ST_X(link_pt_s)) AS dx_s,
                        abs(ST_Y(slope_pt_s)-ST_Y(link_pt_s)) AS dy_s,
                        abs(ST_X(slope_pt_e)-ST_X(link_pt_e)) AS dx_e,
                        abs(ST_Y(slope_pt_e)-ST_Y(link_pt_e)) AS dy_e                    
                    FROM(
                        SELECT 
                            rdb_link_id,
                            link_pos_s,
                            link_pos_e,
                            slope_value,
                            slope_shape,
                            link_pt_s,
                            link_pt_e,
                            ST_Line_Interpolate_Point(slope_shape, slope_pos_s) AS slope_pt_s,
                            ST_Line_Interpolate_Point(slope_shape, slope_pos_e) AS slope_pt_e
                        FROM(
                            SELECT 
                                rdb_link_id,
                                slope_value,
                                link_pos_s,
                                link_pos_e,
                                link_pt_s,
                                link_pt_e,
                                slope_shape,
                                ST_Line_Locate_Point(slope_shape, link_pt_s) AS slope_pos_s,
                                ST_Line_Locate_Point(slope_shape, link_pt_e) AS slope_pos_e
                            FROM(
                                SELECT  
                                    rdb_link_id,
                                    slope_value,
                                    link_pos_s,
                                    link_pos_e,
                                    slope_shape,
                                    ST_Line_Interpolate_Point(link_geom, link_pos_s) AS link_pt_s,
                                    ST_Line_Interpolate_Point(link_geom, link_pos_e) AS link_pt_e
                                FROM(
                                     SELECT  
                                        rdb_link_id,
                                        slope_value,
                                        slope_shape,
                                        link_geom,
                                        ST_Line_Locate_Point(link_geom, ST_startpoint(slope_shape)) AS link_pos_s,
                                        ST_Line_Locate_Point(link_geom, ST_endpoint(slope_shape))   AS link_pos_e
                                     FROM temp_org_slope AS a
                                ) AS b
                                WHERE link_pos_s <> link_pos_e
                            ) AS c
                        ) AS d
                    ) AS e
                ) AS f
                WHERE dx_s < CAST('0.0001' AS float) AND
                      dy_s < CAST('0.0001' AS float) AND
                      dx_e < CAST('0.0001' AS float) AND
                      dY_e < CAST('0.0001' AS float)
            );           
            """
        rdb_log.log(self.ItemName, 'Now it is insert slope info into temp_rdb_slope...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        # step 3: insert rdb_slope info into rdb_slope_tbl
        # 如果倾斜数据片段的形点顺序与link的形点顺序相反，既start_pt > end_pt，就交换一下首尾点的顺序。
        # 如果首尾点顺序发生交换，那么倾斜数值value也要同时取反，这一操作在函数rdb_cnv_slope_value（）内完成。            
        sqlcmd = """    
            INSERT INTO rdb_slope_tbl(
                link_id,
                link_id_t,
                slope_pos_s, 
                slope_pos_e,
                slope_value)
            (
                SELECT 
                    rdb_link_id,
                    rdb_link_id >> 32 as link_id_t,
                    (CASE WHEN link_pos_s > link_pos_e THEN link_pos_e ELSE link_pos_s END),
                    (CASE WHEN link_pos_s > link_pos_e THEN link_pos_s ELSE link_pos_e END),
                    rdb_cnv_slope_value(slope_value, link_pos_s > link_pos_e)
                FROM temp_rdb_slope
            );
            """
        rdb_log.log(self.ItemName, 'Now it is insert slope info into rdb_slope_tbl...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
        # Create Index for rdb_slope_tbl
        sqlcmd = """
             CREATE INDEX rdb_slope_tbl_link_id_idx 
                ON rdb_slope_tbl 
                USING btree
                (link_id);
             """
        rdb_log.log(self.ItemName, 'Now it is create Index for rdb_slope_tbl...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
                               
        # step 4: update rdb_link 
        sqlcmd = """
             UPDATE rdb_link 
             SET tilt_flag = TRUE 
             FROM (
                 SELECT DISTINCT link_id
                 FROM rdb_slope_tbl        
             ) AS A
             WHERE rdb_link.link_id = A.link_id;
             """
        rdb_log.log(self.ItemName, 'Now it is update slope flag into rdb_link...', 'info')
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        return 0
    
    def _DoCheckLogic(self):
        '''
        Check slope shape 
        '''
        if self.pg.callproc('rdb_check_slopes_ipc') == -1:
            raise Exception, 'Check slope data error.'
        else:
            self.pg.commit2()
          
        return 0
    
    def _DoCheckValues(self):
        '''
        Check slope value 
        '''
        sqlcmd = """
            ALTER TABLE rdb_slope_tbl DROP CONSTRAINT if exists check_slope_value;
            ALTER TABLE rdb_slope_tbl
                ADD CONSTRAINT check_slope_value CHECK (slope_value = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8]));
            """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
          
        return 0 
    