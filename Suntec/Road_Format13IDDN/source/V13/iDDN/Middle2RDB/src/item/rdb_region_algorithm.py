# -*- coding: cp936 -*-
'''
Created on 2014-8-16

@author: liuxinxing
'''
import os
import copy
import heapq
from common import rdb_log


class CGraph:
    def __init__(self):
        #rdb_log.log('Graph', 'search from dock to dock...', 'info')
        pass
    
    def prepareData(self):
        pass
    
    def clearData(self):
        pass
    
    def _getAdjLinkList(self, node_id):
        # this function returns (adjlinkid, adjcost, adjnode)
        pass
    
    def _isDockNode(self, node_id):
        # this function returns true/false
        pass
    
    def _isRegionNode(self, node_id):
        # this function returns true/false
        pass
    
    def searchFromDockToDock(self, node_id, forward=True):
        # init
        paths = []                  # efftive path (target_node_id, path)
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_distance, cur_node, in_angle, last_link, path)
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        touch_nodes.push((0, 0, node_id, None, None, []))
        while not touch_nodes.isempty():
            (cur_cost, cur_distance, cur_node, in_angle, last_link, path) = touch_nodes.pop()
            #print cur_node, last_link, path
            #print 'arrive_nodes =', arrive_nodes.keys()
            
            #print 'search node', (cost, node_id, path)
            if arrive_nodes.has_key(cur_node):
                continue
            else:
                arrive_nodes[cur_node] = True
            
            if (cur_node != node_id) and self._isDockNode(cur_node):
                paths.append((cur_node,path))
                continue
            
            adjlink_list = self._getAdjLinkList(cur_node)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
                    continue
                
                # judge roadtype
                if adjlink.road_type in (7,8,9,10,14):
                    continue
                
                # judge direction of traffic flow
                if (adjlink.start_node_id == cur_node) and forward is True and adjlink.one_way in (1,2):
                    pass
                elif (adjlink.start_node_id == cur_node) and forward is False and adjlink.one_way in (1,3):
                    pass
                elif (adjlink.end_node_id == cur_node) and forward is True and adjlink.one_way in (1,3):
                    pass
                elif (adjlink.end_node_id == cur_node) and forward is False and adjlink.one_way in (1,2):
                    pass
                else:
                    continue
                
                # judge next node walked or not
                if adjlink.start_node_id == cur_node:
                    next_node = adjlink.end_node_id
                    cur_out_angle = adjlink.fazm
                    next_in_angle = adjlink.tazm
                    next_witdh = adjlink.ops_width
                else:
                    next_node = adjlink.start_node_id
                    cur_out_angle = adjlink.tazm
                    next_in_angle = adjlink.fazm
                    next_witdh = adjlink.neg_width
                
                # touch
                next_path = copy.deepcopy(path)
                next_path.append(adjlink.link_id)
                next_distance = cur_distance + adjlink.link_length
                adj_cost = CLinkCost.getCost(adjlink.link_length, adjlink.road_type, 
                                             adjlink.toll, adjlink.one_way, 
                                             next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost
                #print 'touch', adjnode, adjcost
                if next_distance < 1000:
                    touch_nodes.push((next_cost, next_distance, next_node, next_in_angle, adjlink.link_id, next_path))
        else:
            return paths
    
    def searchFromDockToRegion(self, node_id, forward=True):
        # if dock node is region node, then cancle the search
        if self._isRegionNode(node_id):
            return []
        
        # init
        paths = []                  # efftive path (target_node_id, path)
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_distance, cur_node, in_angle, last_link, path)
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        touch_nodes.push((0, 0, node_id, None, None, []))
        while not touch_nodes.isempty():
            (cur_cost, cur_distance, cur_node, in_angle, last_link, path) = touch_nodes.pop()
            
            #print 'search node', (cost, node_id, path)
            if arrive_nodes.has_key(cur_node):
                continue
            else:
                arrive_nodes[cur_node] = True
            
            #
            if (cur_node != node_id) and self._isRegionNode(cur_node):
                paths.append((cur_node,path))
            
            adjlink_list = self._getAdjLinkList(cur_node)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
                    continue
                
                # judge roadtype
                if adjlink.road_type in (7,8,9,10,14):
                    continue
                
                # judge direction of traffic flow
                if (adjlink.start_node_id == cur_node) and forward is True and adjlink.one_way in (1,2):
                    pass
                elif (adjlink.start_node_id == cur_node) and forward is False and adjlink.one_way in (1,3):
                    pass
                elif (adjlink.end_node_id == cur_node) and forward is True and adjlink.one_way in (1,3):
                    pass
                elif (adjlink.end_node_id == cur_node) and forward is False and adjlink.one_way in (1,2):
                    pass
                else:
                    continue
                
                # judge next node
                if adjlink.start_node_id == cur_node:
                    next_node = adjlink.end_node_id
                    cur_out_angle = adjlink.fazm
                    next_in_angle = adjlink.tazm
                    next_witdh = adjlink.ops_width
                else:
                    next_node = adjlink.start_node_id
                    cur_out_angle = adjlink.tazm
                    next_in_angle = adjlink.fazm
                    next_witdh = adjlink.neg_width
                if arrive_nodes.has_key(next_node):
                    continue
                
                # touch
                next_path = copy.deepcopy(path)
                next_path.append(adjlink.link_id)
                next_distance = cur_distance + adjlink.link_length
                adj_cost = CLinkCost.getCost(adjlink.link_length, adjlink.road_type, 
                                             adjlink.toll, adjlink.one_way, 
                                             next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost
                #print 'touch', adjnode, adjcost
                if next_distance < 10000:
                    touch_nodes.push((next_cost, next_distance, next_node, next_in_angle, adjlink.link_id, next_path))
        else:
            return paths

class CGraph_PG(CGraph):
    def __init__(self):
        pass
    
    def prepareData(self):
        import common.rdb_database
        self.pg = common.rdb_database.rdb_pg()
        self.pg.connect2()
    
    def clearData(self):
        self.pg.close2()
    
    def _getAdjLinkList(self, node_id):
        # this function returns (adjlinkid, adjcost, adjnode)
        sqlcmd = """
                select  link_id,
                        road_type,
                        toll,
                        link_length,
                        one_way,
                        start_node_id,
                        end_node_id,
                        fazm,
                        tazm,
                        ops_width,
                        neg_width
                from rdb_link as a
                left join rdb_linklane_info as b
                on a.lane_id = b.lane_id
                where %s in (a.start_node_id, a.end_node_id)
                """ % (str(node_id))
        self.pg.execute(sqlcmd)
        link_recs = self.pg.fetchall2()
        adjlinks = []
        for link_rec in link_recs:
            adjlinks.append(CAdjLink(link_rec))
        return adjlinks
    
    def _isDockNode(self, node_id):
        sqlcmd = """
                select count(*)
                from temp_region_rdb_node_dock
                where node_id = %s
                """ % str(node_id)
        self.pg.execute(sqlcmd)
        result = self.pg.fetchone2()
        return result[0] > 0
    
    def _isRegionNode(self, node_id):
        sqlcmd = """
                select count(*)
                from temp_region_orgnode_level4
                where node_id = %s
                """ % str(node_id)
        self.pg.execute(sqlcmd)
        result = self.pg.fetchone2()
        return result[0] > 0

class CHeap:
    def __init__(self):
        self.data = []
    
    def push(self, item):
        heapq.heappush(self.data, item)
    
    def pop(self):
        return heapq.heappop(self.data)
    
    def isempty(self):
        if self.data:
            return False
        else:
            return True

class CAdjLink:
    def __init__(self, link_rec):
        self.link_id = link_rec[0]
        self.road_type = link_rec[1]
        self.toll = link_rec[2]
        self.link_length = link_rec[3]
        self.one_way = link_rec[4]
        self.start_node_id = link_rec[5]
        self.end_node_id = link_rec[6]
        self.fazm = link_rec[7]
        self.tazm = link_rec[8]
        self.ops_width = link_rec[9]
        self.neg_width = link_rec[10]

class CLinkCost:
    
    speed_roadtype_toll = [
        [ 80. * 0.277778, 80. * 0.277778 ], #///< road kind = 0
        [ 60. * 0.277778, 65. * 0.277778 ], #///< 1
        [ 40. * 0.277778, 60. * 0.277778 ], #///< 2
        [ 35. * 0.277778, 45. * 0.277778 ], #///< 3
        [ 28. * 0.277778, 35. * 0.277778 ], #///< 4
        [ 25. * 0.277778, 30. * 0.277778 ], #///< 5
        [ 20. * 0.277778, 25. * 0.277778 ], #///< 6
        [ 20. * 0.277778, 20. * 0.277778 ], #///< 7
        [ 15. * 0.277778, 15. * 0.277778 ], #///< 8
        [ 15. * 0.277778, 15. * 0.277778 ], #///< 9
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 10
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 11
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 12
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 13
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 14
        [ 30. * 0.277778, 30. * 0.277778 ], #///< 15
    ]
    
    # width, one_way
    speed_width = [
        [0.4, 0.54],
        [0.66, 0.83],
        [1.05, 1.14],
        [1.13, 1.18],
        [1.22, 1.31]
    ]
    
    cost_turn = [ 
        200, 200, 200, 200, 200, 200, 200, 100, 100, #///< 0 ~ 45
        80, 80, 80, 15, 15, 15, 15, 15, 15, #///< 45 ~ 90
        13, 13, 13, 10, 10, 10, 8, 8, 8, #///< 90 ~ 135
        6, 6, 6, 0, 0, 0, 0, 0, 0, #///< 135 ~ 180
        0, 0, 0, 0, 0, 0, 5, 5, 5, #///< 180 ~ 225
        8, 8, 8, 8, 8, 8, 10, 10, 10, #///< 225 ~ 270
        10, 10, 10, 10, 10, 10, 80, 80, 80, #///< 270 ~ 315
        100, 100, 200, 200, 200, 200, 200, 200, 200, #///< 315 ~ 360
        0, #///< delta angle equals to 360 degree
    ]
    
    @staticmethod
    def getCost(link_length, road_type, toll, width, one_way, in_angle, out_angle):
        if toll != 1:
            toll = 0
        
        if width == 4:
            width = 2
        
        if one_way in (2,3):
            one_way = 1
        else:
            one_way = 0
        
        if in_angle is None:
            angle = 360
        else:
            in_angle = (in_angle + 32768) * 360.0 / 65536.0
            out_angle = (out_angle + 32768) * 360.0 / 65536.0
            angle = int(out_angle - in_angle)
            if angle <= 0:
                angle += 360
            if angle > 360:
                angle -= 360
        angle = int(angle / 5)
        
        speed = CLinkCost.speed_roadtype_toll[road_type][toll] * CLinkCost.speed_width[width][one_way]
        cost = link_length / speed + CLinkCost.cost_turn[angle]
        
        return cost
    
    
    