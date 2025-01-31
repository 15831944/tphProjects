# -*- coding: cp936 -*-
'''
Created on 2013-7
@author: zym
'''

from component.default.guideinfo_lane import comp_guideinfo_lane

class comp_guideinfo_lane_rdf(comp_guideinfo_lane):
    
    def __init__(self):
        comp_guideinfo_lane.__init__(self)
        
    def _DoCreateTable(self):
        self.CreateTable2('lane_tbl')
        return 0
    
    def _DoCreateIndex(self):
        return 0
    
    def _DoCreateFunction(self):
        self.CreateFunction2('mid_make_lanenum_lr')
        return 0
    
    def _Do(self):
        self.log.info(self.ItemName + ': begin of making lane ...')
        self._makeMidLanenumLrTbl()
        self._findLaneListOnLinkDir()
        self._generateLaneTbl()
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


    # 创建包含了left,right车线信息的临时表mid_lanenum_lr
    def _makeMidLanenumLrTbl(self):
        self.pg.execute2("select mid_make_lanenum_lr();")
        self.pg.commit2()
        return 0
    
    # rdf_lane连rdf_link
    # 求出每条link的F和T方向的车线列表和bus lane信息。
    # 生成一张临时表temp_lane_count_in_link_dir存储这些车线信息。
    def _findLaneListOnLinkDir(self):
        sqlcmd = '''
                drop table if exists temp_lane_list_in_link_dir;
                select case when t1.link_id is not null then t1.link_id else t2.link_id end as link_id,
                       t1.T_lane_num_list, 
                       t1.T_is_bus_lane_list,
                       t2.F_lane_num_list,
                       t2.F_is_bus_lane_list
                into temp_lane_list_in_link_dir
                from
                (
                    select link_id, array_agg(lane_number) as T_lane_num_list,
                           array_agg(
                                       case when automobiles= 'N' and buses= 'Y' 
                                       and taxis= 'N' and carpools= 'N' 
                                       and pedestrians= 'N' and trucks= 'N' 
                                       and deliveries= 'N' and emergency_vehicles= 'N' 
                                       and through_traffic= 'N' and motorcycles= 'N'
                                       then 1
                                       else 0
                                       end
                                    ) as T_is_bus_lane_list
                    from
                    (
                        select * 
                        from rdf_lane as a
                        left join rdf_access as b
                        on a.access_id=b.access_id
                        order by link_id, lane_number
                    ) as t
                    where lane_travel_direction='T' or lane_travel_direction='B'
                    group by link_id
                ) as t1
                full join
                (
                    select link_id, array_agg(lane_number) as F_lane_num_list, 
                           array_agg(
                                       case when automobiles= 'N' and buses= 'Y' 
                                       and taxis= 'N' and carpools= 'N' 
                                       and pedestrians= 'N' and trucks= 'N' 
                                       and deliveries= 'N' and emergency_vehicles= 'N' 
                                       and through_traffic= 'N' and motorcycles= 'N'
                                       then 1
                                       else 0
                                       end
                                    ) as F_is_bus_lane_list
                    from
                    (
                        select * 
                        from rdf_lane as a
                        left join rdf_access as b
                        on a.access_id=b.access_id
                        order by link_id, lane_number
                    ) as t
                    where lane_travel_direction='F' or lane_travel_direction='B'
                    group by link_id
                ) as t2
                on t1.link_id=t2.link_id;
                 '''

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        
        self.pg.commit2()
        return 0
    
    # 主处理函数，
    # 由rdf_lane_nav_strand连接前面生成的mid_make_lanenum_lr和temp_lane_count_in_link_dir等表，
    # 获取相关信息并转化后存入到lane_tbl中。
    def _generateLaneTbl(self):
        lane_tbl_insert_str = '''
                                INSERT INTO lane_tbl(
                                id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, lanenum, 
                                laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
                                VALUES (%s, %s, %s, %s, '%s', %s, %s, 
                                        '%s', %s, %s, %s, '%s', %s);
                              '''
        sqlcmd = '''
                select e.link_id_list, e.node_id, 
                       case when e.node_id=f.ref_node_id then 'T'
                            else 'F' end as inlink_lane_dir,  
                       e.inlink_lane_category, e.inlink_lane_num_list,
                       case when e.node_id=f.ref_node_id then g.T_lane_num_list 
                            else g.F_lane_num_list end as total_lane_list_on_dir,
                       case when e.node_id=f.ref_node_id then g.T_is_bus_lane_list 
                            else g.F_is_bus_lane_list end as is_bus_lane_list,
                       h.lanenum_l, h.lanenum_r
                from
                (
                    select link_id_list, node_id, inlink_lane_category, 
                           array_agg(inlink_lane_num) as inlink_lane_num_list
                    from
                    (
                        
                        select array_agg(link_id) as link_id_list, 
                               (array_agg(node_id))[1] as node_id,
                               (array_agg(direction_category))[1] as inlink_lane_category,  
                               (array_agg(lane_number))[1] as inlink_lane_num
                        from
                        (
                            select a.lane_nav_strand_id, a.seq_num, a.lane_id, a.node_id, 
                                   b.direction_category, b.link_id, b.lane_number
                            from 
                            rdf_lane_nav_strand as a
                            left join rdf_lane as b 
                            on a.lane_id=b.lane_id
                            left join rdf_condition as c
                            on a.condition_id=c.condition_id
                            where c.condition_type=13
                            order by a.lane_nav_strand_id, a.seq_num
                        ) as t1
                        group by lane_nav_strand_id
                    ) as t2
                    group by link_id_list, node_id, inlink_lane_category
                ) as e
                left join rdf_link as f
                on e.link_id_list[1]=f.link_id
                left join temp_lane_list_in_link_dir as g
                on e.link_id_list[1]=g.link_id
                left join mid_lanenum_lr as h
                on e.link_id_list[1]=h.link_id and e.node_id=h.node_id
                order by e.link_id_list, e.node_id, e.inlink_lane_category;
                '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        rows = self.pg.fetchall2()
        theID = 1
        for row in rows:
            link_id_list = row[0]
            node_id = row[1]
            inlink_lane_dir = row[2]
            inlink_lane_category = row[3]
            inlink_lane_num_list = row[4]
            total_lane_list_on_dir = row[5]
            is_bus_lane_list = row[6]
            lanenum_l = row[7] if row[7] else 0 
            lanenum_r = row[8] if row[8] else 0
            
            passlidStr = ''
            if len(link_id_list) < 2:
                # 此诱导链中只有一条lane信息，丢弃并报warning。
                self.log.warn("""there is a lane strand has only one lane.""")
                continue
            elif len(link_id_list) == 2:
                passlidStr = ''
                passlink_cnt = 0
            else:
                passlid = link_id_list[1:-1]
                passlidStr = '|'.join(str(i) for i in passlid)
                passlink_cnt = len(passlid)
            
            inlink_id = link_id_list[0]
            outlink_id = link_id_list[-1]
            
            # rdb协议中关于箭头属性的定义与here协议中一致，所以可直接使用inlink_lane_category字段。
            arrowinfo = inlink_lane_category if inlink_lane_category else 'NULL'
            
            laneinfo = ''
            businfoStr = ''
            if inlink_lane_dir == 'F':
                for oneLane, is_bus_lane  in zip(total_lane_list_on_dir, is_bus_lane_list):
                    if oneLane in inlink_lane_num_list:
                        laneinfo = laneinfo + '1'
                    else:
                        laneinfo = laneinfo + '0'
                        
                    if is_bus_lane == 1:
                        businfoStr = businfoStr + '1'
                    else:
                        businfoStr = businfoStr + '0'
            else:
                for oneLane, is_bus_lane  in zip(total_lane_list_on_dir, is_bus_lane_list):
                    if oneLane in inlink_lane_num_list:
                        laneinfo = '1' + laneinfo
                    else:
                        laneinfo = '0' + laneinfo
                        
                    
                    if is_bus_lane == 1:
                        businfoStr = '1' + businfoStr 
                    else:
                        businfoStr = '0' + businfoStr
                        
            theID += 1
            self.pg.execute2(lane_tbl_insert_str %  
                             (theID, node_id, inlink_id, outlink_id, passlidStr, 
                              passlink_cnt, len(total_lane_list_on_dir), laneinfo, 
                              arrowinfo, lanenum_l, lanenum_r, businfoStr, 0))
        
        self.pg.commit2()
        return 0



























    
