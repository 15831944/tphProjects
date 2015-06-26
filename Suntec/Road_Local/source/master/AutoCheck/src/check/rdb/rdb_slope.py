'''
Created on 2014-05-23

@author: Muwenjie
'''

import platform.TestCase

class CCheckSlopeValue(platform.TestCase.CTestCase):
    '''
    Check slope value 
    '''
    def _do(self):
        
        sqlcmd = """
            ALTER TABLE rdb_slope_tbl DROP CONSTRAINT if exists check_slope_value;
            ALTER TABLE rdb_slope_tbl
                ADD CONSTRAINT check_slope_value CHECK (slope_value = ANY (ARRAY[0, 1, 2, 3, 4, 5, 6, 7, 8]));
            """
        
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            self.pg.commit()
            return True

        
class CCheckSlopeLinkID(platform.TestCase.CTestCase):
    '''
    Check slope link_id 
    '''
    def _do(self):
        sqlcmd = """
                SELECT * 
                FROM (
                    SELECT 
                        b.link_id AS link_id,
                        c.park_link_id as park_link_id,
                        a.slope_value AS slope_value
                    FROM rdb_slope_tbl a
                    LEFT JOIN rdb_link b
                    ON a.link_id = b.link_id
                    left join rdb_park_link as c
                    ON a.link_id = c.park_link_id
                ) AS result
                WHERE link_id IS NULL and park_link_id is null;
                """
                       
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:
            if self.pg.getcnt() == False:
                return True
            else:
                return False
        
class CCheckSlopeShape(platform.TestCase.CTestCase):
    '''
    Check slope shape 
    '''
    def _do(self):
        
        sqlcmd = """
            SELECT COUNT(*)  
            FROM(
                SELECT 
                    abs(len_slope-len_link) AS len_d
                FROM(
                    SELECT 
                        len_link AS len_link,
                        abs(slope_pos_s-slope_pos_e)*ST_Length(slope_shape) AS len_slope
                    FROM(    
                        SELECT 
                            len_link AS len_link,
                            slope_shape AS slope_shape,
                            ST_Line_Locate_Point(slope_shape, link_pt_s) AS slope_pos_s,
                            ST_Line_Locate_Point(slope_shape, link_pt_e) AS slope_pos_e
                        FROM(            
                            SELECT  
                                b.slope_shape AS slope_shape,
                                abs(b.link_pos_s-b.link_pos_e)*ST_Length(a.the_geom) AS len_link,
                                ST_Line_Interpolate_Point(a.the_geom, b.link_pos_s) AS link_pt_s,
                                ST_Line_Interpolate_Point(a.the_geom, b.link_pos_e) AS link_pt_e
                            FROM rdb_link AS a
                            INNER JOIN temp_rdb_slope AS b
                            ON a.link_id = b.rdb_link_id
                        ) AS c
                    ) AS d
                ) AS e
            ) AS f
            WHERE len_d > CAST('0.001' AS float)
                 
            """
            
        if self.pg.execute(sqlcmd) == -1:
            return False
        else:    
            row = self.pg.fetchone()
            if row:
                if row[0] == 0:
                    return True
            
        return False
    
    