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
        ItemBase.__init__(self,'slope')
       
    def Do_CreateTable(self):
        self.CreateTable2('rdb_slope_tbl')
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
        
        self.GetMiddleLinkSlope()
        self.GetRdbLinkSlope()
        
    def GetMiddleLinkSlope(self):
        self.log.info('get middle slope start...')

        self.CreateTable2('temp_org2middle_slope')
        sqlcmd = """
            INSERT INTO temp_org2middle_slope(middle_link_id, org_link_id, slope_value,
                                              slope_shape, middle_geom)
            (
                SELECT b.mid_link_id, a.link_id, a.grad_value,    
                       st_transform( a.shape, 4326 ), b.mid_geom
                FROM 
                (
                    SELECT link_id, grad_value, (ST_Dump(shape)).geom AS shape
                    FROM grad
                ) AS a
                INNER JOIN temp_link_org_rdb AS b
                ON a.link_id = b.org_link_id
            );
            """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()

        self.CreateFunction2('rdb_get_fraction_for_slope')
        self.CreateTable2('temp_middle_slope')           
        sqlcmd = """
                insert into temp_middle_slope(middle_link_id, link_pos_s, link_pos_e, 
                                              slope_value, middle_shape)
                (
                    select middle_link_id, 
                          (case when slope_array[1] > slope_array[2] then slope_array[2] else slope_array[1] end),
                          (case when slope_array[1] > slope_array[2] then slope_array[1] else slope_array[2] end),
                          (case when slope_array[1] > slope_array[2] then -slope_value else slope_value end),
                          middle_geom
                    from
                    (
                        select middle_link_id, slope_value, middle_geom, slope_shape, 
                            rdb_get_fraction_for_slope(slope_shape, middle_geom) as slope_array
                        from temp_org2middle_slope
                    )temp
                );
            """

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
            
    def GetRdbLinkSlope(self):
        self.log.info('get rdb slope start...')
#        self.CreateFunction2('rdb_get_point_num_slope')
#        self.CreateFunction2('rdb_add_point_in_line_slope')
#        sqlcmd = """
#                insert into temp_rdb_slope(rdb_link_id, link_pos_s, link_pos_e, slope_value, link_geom)
#                (
#                    select target_link_id as rdb_link_id,
#                           slope_pos_s as link_pos_s,
#                           (case when (link_length *(abs(link_pos_e - rdb_pos_e))) < 20.0 then rdb_pos_e
#                                 else link_pos_e end) as link_pos_e,
#                            slope_value,  
#                           (case when (link_length *(abs(link_pos_e - rdb_pos_e))) < 20.0 then target_geom
#                                 else rdb_add_point_in_line_slope(target_geom, link_pos_e) end) as link_geom
#                    from
#                    (
#                        select target_link_id, slope_value, link_pos_e, rdb_pos_s, link_length,
#                               (case when (link_length *(abs(link_pos_s - rdb_pos_s))) < 20.0 then rdb_pos_s
#                                     else link_pos_s end) as slope_pos_s,  
#                               (case when (link_length *(abs(link_pos_s - rdb_pos_s))) < 20.0 then target_geom
#                                     else rdb_add_point_in_line_slope(target_geom, link_pos_s) end) as target_geom
#                        from
#                        (
#                            select b.target_link_id, link_pos_s, link_pos_e, slope_value, a.target_geom, c.link_length,
#                                (case when link_pos_s in (0.0, 1.0) then link_pos_s 
#                                     else ST_Line_Locate_Point(ST_Line_Interpolate_Point(target_geom, link_pos_s))
#                                     end) as rdb_pos_s, 
#                                (case when link_pos_e = (0.0, 1.0) then link_pos_e 
#                                     else ST_Line_Locate_Point(ST_Line_Interpolate_Point(target_geom, link_pos_e))
#                                     end) as rdb_pos_e
#                            from temp_middle_slope as a
#                            left join temp_link_org_rdb as b
#                            on a.middle_link_id = b.mid_link_id
#                            left join rdb_link as c
#                            on b.target_link_id = c.link_id  
#                            where  link_pos_s <> link_pos_e
#                        )a
#                    )a           
#                );
#                """
        sqlcmd = """
                insert into temp_rdb_slope(rdb_link_id, link_pos_s, link_pos_e, slope_value, slope_shape)
                (
                    select b.target_link_id,
                           (case when flag = TRUE then s_fraction - ((s_fraction - e_fraction) * link_pos_s)
                                 else s_fraction + ((e_fraction - s_fraction) * link_pos_s)  end) as link_pos_s,
                           (case when flag = TRUE then s_fraction - ((s_fraction - e_fraction) * link_pos_e)
                                 else s_fraction + ((e_fraction - s_fraction) * link_pos_e)  end) as link_pos_e,                               
                           slope_value, b.target_geom
                    from temp_middle_slope as a
                    left join temp_link_org_rdb as b
                    on a.middle_link_id = b.mid_link_id
                    where  link_pos_s <> link_pos_e
                );
                """

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
                where link_pos_s <> link_pos_e
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
    