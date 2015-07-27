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
        self._GenerateLanenumLrTbl()
        self._GenerateLaneListOnDir()
        self._GenerateLaneTbl()
        self.log.info(self.ItemName + ': end of making lane ...')         
        return 0

    # 创建包含了left,right车线信息的临时表mid_lanenum_lr
    def _GenerateLanenumLrTbl(self):
        self.pg.execute2("select mid_make_lanenum_lr();")
        self.pg.commit2()
        return 0
    
    # rdf_lane连rdf_link
    # 求出每条link的F和T方向的车线列表和bus lane信息。
    # 生成一张临时表temp_lane_count_in_link_dir存储这些车线信息。
    def _GenerateLaneListOnDir(self):
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
                    where lane_travel_direction='T'
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
                    where lane_travel_direction='F'
                    group by link_id
                ) as t2
                on t1.link_id=t2.link_id;
                 '''

        if self.pg.execute2(sqlcmd) == -1:
            return -1
        
        self.pg.commit2()
        return 0
    
    # 主处理函数，从rdf_lane_nav_strand等表中获取相关信息，转化后存入到lane_tbl中。
    def _GenerateLaneTbl(self):
        lane_tbl_insert_str = '''
                                INSERT INTO lane_tbl(
                                id, nodeid, inlinkid, outlinkid, passlid, passlink_cnt, lanenum, 
                                laneinfo, arrowinfo, lanenuml, lanenumr, buslaneinfo, exclusive)
                                VALUES (%d, %d, %d, %d, '%s', %d, %d, 
                                        '%s', %d, %d, %d, '%s', %d);
                              '''
        sqlcmd = '''
                select e.link_id_list, e.node_id, e.inlink_lane_dir,  
                       e.inlink_lane_category, e.inlink_lane_num_list,
                       case when e.inlink_lane_dir='T' then f.T_lane_num_list 
                            else f.F_lane_num_list end as total_lane_list_on_dir,
                       case when e.inlink_lane_dir='T' then f.T_is_bus_lane_list
                            else f.F_is_bus_lane_list end as is_bus_lane_list,
                       g.lanenum_l, g.lanenum_r
                from
                (
                    select link_id_list, node_id, inlink_lane_dir, inlink_lane_category, 
                           array_agg(inlink_lane_num) as inlink_lane_num_list
                    from
                    (
                        
                        select array_agg(link_id) as link_id_list, 
                               (array_agg(node_id))[1] as node_id,  
                               (array_agg(lane_travel_direction))[1] as inlink_lane_dir, 
                               (array_agg(direction_category))[1] as inlink_lane_category,  
                               (array_agg(lane_number))[1] as inlink_lane_num
                        from
                        (
                            select a.lane_nav_strand_id, a.seq_num, a.lane_id, a.node_id, 
                                   b.lane_travel_direction, b.direction_category, 
                                   b.link_id, b.lane_number
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
                    group by link_id_list, node_id, inlink_lane_category, inlink_lane_dir
                ) as e
                left join temp_lane_list_in_link_dir as f
                on e.link_id_list[1]=f.link_id
                left join mid_lanenum_lr as g
                on e.link_id_list[1]=g.link_id and e.node_id=g.node_id;
                '''
        if self.pg.execute2(sqlcmd) == -1:
            return -1
        rows = self.pg.fetchall2()
        for row in rows:
            link_id_list = row[0]
            node_id = row[1]
            inlink_lane_dir = row[2]
            inlink_lane_category = row[3] if row[3] else 0
            inlink_lane_num_list = row[4]
            total_lane_list_on_dir = row[5]
            is_bus_lane_list = row[6]
            lanenum_l = row[7] if row[7] else 0 
            lanenum_r = row[8] if row[8] else 0
            
            # 此诱导链中只有一条lane信息，丢弃并报warning。
            passlidStr = ''
            if len(link_id_list) < 2:
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
            arrowinfo = inlink_lane_category
            
            laneinfo = ''
            if inlink_lane_dir == 'F':
                for oneLane in total_lane_list_on_dir:
                    if oneLane in inlink_lane_num_list:
                        laneinfo = laneinfo + '1'
                    else:
                        laneinfo = laneinfo + '0'
            elif inlink_lane_dir == 'T':
                for oneLane in total_lane_list_on_dir:
                    if oneLane in inlink_lane_num_list:
                        laneinfo = '1' + laneinfo
                    else:
                        laneinfo = '0' + laneinfo
            else:
                #
                continue
            
            businfoStr = ''
            if inlink_lane_dir == 'F':
                for i in range(0, len(total_lane_list_on_dir)):
                    if is_bus_lane_list[i] == 1:
                        businfoStr = businfoStr + '1'
                    else:
                        businfoStr = businfoStr + '0'
            elif inlink_lane_dir == 'T':
                for i in range(0, len(total_lane_list_on_dir)):
                    if is_bus_lane_list[i] == 1:
                        businfoStr = '1' + businfoStr 
                    else:
                        businfoStr = '0' + businfoStr
            else:
                #
                continue
            
                
            sqlcmd = lane_tbl_insert_str % (1, node_id, inlink_id, outlink_id, passlidStr, passlink_cnt, 
                                            len(total_lane_list_on_dir), laneinfo, arrowinfo, lanenum_l, 
                                            lanenum_r, businfoStr, 0)
            
            self.pg.execute2(sqlcmd)
        
        self.pg.commit2()
        return 0



























    