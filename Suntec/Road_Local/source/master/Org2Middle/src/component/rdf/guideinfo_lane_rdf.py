'''
Created on 2013-7

@author: zym
'''

import component.component_base

class comp_guideinfo_lane_rdf(component.component_base.comp_base):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        component.component_base.comp_base.__init__(self, 'Guideinfo_Lane')
        
    def _DoCreateTable(self):
        if self.CreateTable2('temp_lane_guide_nav_strand') == -1:
            return -1
        
        if self.CreateTable2('temp_lane_guide_nav') == -1:
            return -1
        
        if self.CreateTable2('temp_lane_guide') == -1:
            return -1
        if self.CreateTable2('temp_lane_tbl') == -1:
            return -1
        if self.CreateTable2('temp_lane_guide_distinct') == -1:
            return -1
        
        if self.CreateTable2('lane_tbl') == -1:
            return -1
        
        return 0
    
    def _DoCreateIndex(self):
        'create index.'
        
        if self.CreateIndex2('lane_tbl_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('lane_tbl_inlinkid_idx') == -1:
            return -1
        
        if self.CreateIndex2('lane_tbl_outlinkid_idx') == -1:
            return -1
        

        return 0
    
    def _DoCreateFunction(self):
        
#        if self.CreateFunction2('mid_get_arrowinfo') == -1:
#            return -1
        if self.CreateFunction2('mid_get_lanenum') == -1:
            return -1
        if self.CreateFunction2('mid_make_temp_lane_guide_nav') == -1:
            return -1
        
        if self.CreateFunction2('mid_make_additional_lanenum') == -1:
            return -1
        if self.CreateFunction2('mid_make_guidelane_info') == -1:
            return -1
        
        if self.CreateFunction2('mid_make_lane_tbl') == -1:
            return -1

        return 0
    
    def _Do(self):
        
        self.log.info(self.ItemName + ': begin of making lane ...')
        if self.CreateIndex2('rdf_link_ref_node_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('rdf_link_nonref_node_id_idx') == -1:
            return -1

        if self.CreateIndex2('rdf_lane_nav_strand_lane_id_idx') == -1:
            return -1
        if self.CreateIndex2('rdf_access_allcolum_index') == -1:
            return -1
        
        if self.CreateIndex2('rdf_lane_nav_strand_condition_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('rdf_lane_lane_id_idx') == -1:
            return -1
        
        if self.CreateIndex2('rdf_lane_link_id_and_lane_travel_direction_index') == -1:
            return -1
        
        self._makeLaneTravsNavStrand()
        
        if self.CreateIndex2('temp_lane_guide_nav_strand_on_lane_nav_strand_id_index') == -1:
            return -1
        if self.pg.callproc('mid_make_temp_lane_guide_nav') == -1:
            return -1
        else:
            self.pg.commit2()
            
        self._makeLaneTravsData()
        self._deleteRepLaneTravs()
        
        if self.CreateIndex2('temp_lane_guide_distinct_inlink_id_and_nodeid_index') == -1:
            return -1
        
        self._make_temp_lane_tbl()
        
        sqlcmd = """
            select mid_make_lane_tbl();
                """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        return 0
     
#        if self.pg.callproc('mid_make_guideinfo_lane') == -1:
#            return -1
#        else:
#            self.pg.commit2()
#            
        self.log.info(self.ItemName + ': end of making lane ...')
                  
        return 0
    
    def _makeLaneTravsNavStrand(self):
        
        sqlcmd = """
            insert into temp_lane_guide_nav_strand(
                    lane_nav_strand_id, seq_num, condition_id, lane_id, node_id, link_id
                    )
                    select lane_nav_strand_id, seq_num, a.condition_id, a.lane_id, node_id, c.link_id
                        from rdf_lane_nav_strand as a 
                            left join rdf_condition as b 
                            on a.condition_id = b.condition_id 
                            left join rdf_lane as c 
                            on a.lane_id = c.lane_id
                        where b.condition_type = 13;
                """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        return 0
    
    
    def _makeLaneTravsData(self):
        
        sqlcmd = """
            insert into temp_lane_guide(
                lane_nav_strand_id, condition_id, lane_id, node_id, inlink_id, 
                outlink_id, passcount, passlid, lane_number, lane_type, lane_travel_direction, 
                direction_category, lane_forming_ending, physical_num_lanes, 
                bus_access_id
            )
            SELECT lane_nav_strand_id, condition_id, a.lane_id, node_id, inlink_id, outlink_id, passcount, passlid, 
                   b.lane_number, b.lane_type, b.lane_travel_direction,
                   case when b.direction_category is not null then b.direction_category else 0 end ,
                   b.lane_forming_ending,
                   mid_get_lanenum(inlink_id, b.lane_number), d.bus_access_id
              FROM temp_lane_guide_nav as a 
              left join rdf_lane as b 
              on a.lane_id = b.lane_id
              left join (
                    select access_id as bus_access_id 
                    from rdf_access 
                    where automobiles= 'N' and buses= 'Y' 
                    and taxis= 'N' and carpools= 'N' 
                    and pedestrians= 'N' and trucks= 'N' 
                    and deliveries= 'N' and emergency_vehicles= 'N' 
                    and through_traffic= 'N' and motorcycles= 'N'
                ) as d 
                on b.access_id = d.bus_access_id;
        """
        
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        return 0
    
    def _deleteRepLaneTravs(self):
        
        sqlcmd = """
                INSERT INTO temp_lane_guide_distinct(
                            inlink_id, node_id, outlink_id, passcount, passlid, lane_number, 
                            lane_travel_direction, direction_category, physical_num_lanes, 
                            bus_access_id
                    )
                     SELECT distinct inlink_id, node_id,outlink_id, passcount, passlid, lane_number,  
                             lane_travel_direction, direction_category, physical_num_lanes, bus_access_id
                    FROM temp_lane_guide;
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        return 0
    
    def _make_temp_lane_tbl(self):
        
        sqlcmd = """
               SELECT mid_make_guidelane_info(inlink_id, node_id, physical_num_lanes,lane_travel_direction ) 
                  FROM temp_lane_guide_distinct
                  GROUP BY inlink_id, node_id, physical_num_lanes,lane_travel_direction;
        """
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        else:
            self.pg.commit2()
        
        return 0

    
