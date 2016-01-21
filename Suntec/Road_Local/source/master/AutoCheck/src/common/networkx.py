# -*- coding: UTF8 -*-
'''
Created on 2015-10-15

@author: liushengqiang
'''



import os
import copy
import heapq
import threading
from math import *
import common.cache_file

class CGraph:
    def __init__(self):
        pass
    
    def prepareData(self):
        pass
    
    def clearData(self):
        pass
    
    def _getLinkInfo(self, link_id):
        # this function returns object of CLink
        pass
    
    def _getAdjLinkList(self, node_id):
        # this function returns array of CLinks
        pass
    
    def searchShortestPaths(self, inlink, outlink, innode=None, outnode=None, distance = 10000.0, link_tbl = 'rdb_link', link_laneinfo_tbl = 'rdb_linklane_info'):
        # stack init
        paths = []                  # efftive path [(cur_distance, path),]
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_distance, cur_node, in_angle, last_node, last_link, path)
        
        # search init
        #max_distance = 1000
        max_distance = distance * 2.0
        if innode is not None:
            touch_nodes.push((0, 0, innode, None, None, inlink, [inlink]))
        else:
            link = self._getLinkInfo(inlink, link_tbl, link_laneinfo_tbl)
            if link.one_way in (1,2):
                touch_nodes.push((0, 0, link.end_node_id, None, None, inlink, [inlink]))
            if link.one_way in (1,3):
                touch_nodes.push((0, 0, link.start_node_id, None, None, inlink, [inlink]))
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        while not touch_nodes.isempty():
            (cur_cost, cur_distance, cur_node, in_angle, last_node, last_link, path) = touch_nodes.pop()
            #print cur_node, last_link, path
            
            if (last_link == outlink) and (outnode is None or outnode == last_node):
                paths.append((cur_distance,path))
                if max_distance > cur_distance * 1.1:
                    max_distance = cur_distance * 1.1
                continue
            
            #print 'search node', (cost, node_id, path)
            if arrive_nodes.has_key(cur_node):
                continue
            else:
                arrive_nodes[cur_node] = True
            
            #adjlink_list = self._getAdjLinkList(cur_node, link_tbl, link_laneinfo_tbl)
            adjlink_list = self._getAdjLinkList(cur_node)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
                    continue
                
                # judge direction of traffic flow
                if (adjlink.start_node_id == cur_node) and adjlink.one_way in (1,2):
                    pass
                elif (adjlink.end_node_id == cur_node) and adjlink.one_way in (1,3):
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
                if next_distance < max_distance:
                    touch_nodes.push((next_cost, next_distance, next_node, next_in_angle, cur_node, adjlink.link_id, next_path))
        else:
            return paths

class CGraph_Cache(CGraph): 
    
    def __init__(self, capacity = 10): 
        
        import common.database
        self.pg = common.database.CDB()
        self.pg.connect() 
        
        import common.datacache
        self.datacache = common.datacache.LRUCache(capacity)
        
        self.cur_tile = -1
        self.cur_tile_nodes = {}
        self.mutex = threading.Lock()
    
    def __del__(self):
        
        import gc
        
        del self.cur_tile_nodes
        self.datacache.clear()
        self.pg.close()
        
        gc.collect() 
    
    def _get_tile_data(self, 
                       tileid, 
                       link_tbl = 'rdb_link', 
                       node_tbl = 'rdb_node', 
                       link_laneinfo_tbl = 'rdb_linklane_info', 
                       delete_road_type = '(7, 8, 9, 14)'):
        
        nodes = {}
        
        sqlcmd = """
                SELECT b.link_id, b.road_type, b.toll, b.link_length, 
                    b.one_way, b.start_node_id, b.end_node_id, b.fazm_path AS fazm, 
                    b.tazm_path AS tazm, c.ops_width, c.neg_width, a.node_id, 
                    ST_X(a.the_geom) AS x_coord, ST_Y(a.the_geom) AS y_coord 
                FROM (
                    SELECT node_id, the_geom, unnest(branches) as link_id
                    FROM [node_tbl]
                    WHERE node_id_t = [node_id_t]
                ) a
                LEFT JOIN [link_tbl] b
                    ON a.link_id = b.link_id
                LEFT JOIN [link_laneinfo_tbl] c
                    ON b.lane_id = c.lane_id
                WHERE b.road_type NOT IN [road_type]
            """
        
        sqlcmd = sqlcmd.replace('[node_tbl]', str(node_tbl))
        sqlcmd = sqlcmd.replace('[node_id_t]', str(tileid))
        sqlcmd = sqlcmd.replace('[link_tbl]', str(link_tbl))
        sqlcmd = sqlcmd.replace('[link_laneinfo_tbl]', str(link_laneinfo_tbl))
        sqlcmd = sqlcmd.replace('[road_type]', str(delete_road_type))
        
        link_recs = self.pg.get_batch_data(sqlcmd)
        for link_rec in link_recs:
            x_coord = link_rec[12]
            y_coord = link_rec[13]
            objLink = CLink(link_rec)
            
            if not nodes.has_key(objLink.node_id):
                nodes[objLink.node_id] = (x_coord, y_coord, [objLink])
            else:
                nodes[objLink.node_id][2].append(objLink)
                
        return nodes
    
    def _getAdjLinkList(self, 
                        node_id, 
                        link_tbl = 'rdb_link', 
                        node_tbl = 'rdb_node', 
                        link_laneinfo_tbl = 'rdb_linklane_info', 
                        delete_road_type = '(7, 8, 9, 14)'):
        node_id_t = self._getTileId(node_id)
        self.mutex.acquire()
        cur_tile_nodes = self.datacache.get(node_id_t)
        if not cur_tile_nodes:
            cur_tile_nodes = self._get_tile_data(node_id_t, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            self.datacache.set(node_id_t, self.cur_tile_nodes)
        self.mutex.release()
        adjlinks = cur_tile_nodes.get(node_id)[2]
        if not adjlinks:
            adjlinks = []
        
        return adjlinks
    
    def _getNodeCoord(self,
                      node_id, 
                      link_tbl = 'rdb_link', 
                      node_tbl = 'rdb_node', 
                      link_laneinfo_tbl = 'rdb_linklane_info', 
                      delete_road_type = '(7, 8, 9, 14)'):
        node_id_t = self._getTileId(node_id)
        self.mutex.acquire()
        cur_tile_nodes = self.datacache.get(node_id_t)
        if not cur_tile_nodes:
            cur_tile_nodes = self._get_tile_data(node_id_t, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            self.datacache.set(node_id_t, cur_tile_nodes)
        self.mutex.release()
        
        x_coord = cur_tile_nodes.get(node_id)[0]
        y_coord = cur_tile_nodes.get(node_id)[1]
        
        return (x_coord, y_coord)
    
    def _getTileId(self, id):
        return id >> 32

    def searchShortestPaths(self, 
                            from_node,
                            to_node,
                            distance = 10000.0,
                            link_tbl = 'rdb_link', 
                            node_tbl = 'rdb_node',
                            link_laneinfo_tbl = 'rdb_linklane_info', 
                            delete_road_type = '(7, 8, 9, 14)',  
                            cost_scope=1.05):
        # stack init
        paths = []                  # efftive path [(cur_distance, path),]
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_distance, cur_node, in_angle, last_node, last_link, path)
        
        # search init
        max_distance = distance * 2.0
        touch_nodes.push((0, 0, from_node, None, None, 0, []))
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        while not touch_nodes.isempty():
            (cur_cost, cur_distance, cur_node, in_angle, last_node, last_link, path) = touch_nodes.pop()
            #print cur_node, last_link, path
            
            if (to_node == cur_node):
                paths.append((cur_distance,path))
                if max_distance > cur_distance * 1.1:
                    max_distance = cur_distance * 1.1
                continue
            
            #print 'search node', (cost, node_id, path)
            if arrive_nodes.has_key(cur_node):
                continue
            else:
                arrive_nodes[cur_node] = True
            
            adjlink_list = self._getAdjLinkList(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
                    continue
                
                # judge direction of traffic flow
                if (adjlink.start_node_id == cur_node) and adjlink.one_way in (1,2):
                    pass
                elif (adjlink.end_node_id == cur_node) and adjlink.one_way in (1,3):
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
                if next_distance < max_distance:
                    touch_nodes.push((next_cost, next_distance, next_node, next_in_angle, cur_node, adjlink.link_id, next_path))
        else:
            return paths
    
    def _searchShortestPath(self, 
                            from_node,                               # Start Node
                            to_node,                                 # End Node
                            forward=True,                            # 算路方向设定 True:S->E / False:E->S
                            distance = 10000.0,                      # 算路距离限值
                            get_link_cost='CLinkCost.getCost2',      # 算路cost函数
                            link_tbl = 'rdb_link',                   # 使用Link情报表
                            node_tbl = 'rdb_node',                   # 使用Node情报表
                            link_laneinfo_tbl = 'rdb_linklane_info', # 使用车线情报表
                            delete_road_type = '(7, 8, 9, 14)'       # 算路时不考虑的road_type
                            ):
        # 改善点：
        # 在最大距离限定内，若找到一条路径（最短路径）即终止探索，并返回路径；
        # 若找不到路径，直至touch_nodes为空，并返回空路径
        paths = []                 
        arrive_nodes = {}          
        touch_nodes = CHeap()
        
        max_distance = distance * 2.0
        touch_nodes.push((0, from_node, None, None, 0, []))
        
        while not touch_nodes.isempty():
            (cur_cost, cur_node, in_angle, last_node, last_link, path) = touch_nodes.pop()
            
            if (to_node == cur_node):
                paths.append((cur_cost, path))
                break

            if arrive_nodes.has_key(cur_node):
                continue
            else:
                arrive_nodes[cur_node] = True
            
            adjlink_list = self._getAdjLinkList(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                if adjlink.link_id == last_link:
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

                next_path = copy.copy(path)
                next_path.append(adjlink.link_id)
                adj_cost = get_link_cost(adjlink.link_length, adjlink.road_type, adjlink.toll, adjlink.one_way, next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost

                if next_cost < max_distance:
                    touch_nodes.push((next_cost, next_node, next_in_angle, cur_node, adjlink.link_id, next_path))
        return paths
    
    def _searchDijkstraPaths(self, 
                             from_node,                               # Start Node
                             to_node,                                 # End Node
                             forward=True,                            # 方向 True:S->E / False:E->S
                             max_buffer=5000,                         # touch_nodes限值
                             max_path_num=2,                          # 最大路径数限值
                             cost_scope=1.05,                         # 探索用cost估值参数
                             max_cost=None,                           # 探索用cost限值
                             get_link_cost='CLinkCost.getCost',       # 探索用cost计算方法
                             log=None,                                # log输出（暂时无用）    
                             link_tbl = 'rdb_link',                   # 探索用Link情报表
                             node_tbl = 'rdb_node',                   # 探索用Node情报表
                             link_laneinfo_tbl = 'rdb_linklane_info', # 探索用车线情报表
                             delete_road_type = '(7, 8, 9, 14)'       # 探索屏蔽road_type
                            ):
        # 使用Dijkstra算法进行算路，搜索范围近似半径为r的圆
        # stack init
        paths = []                  # efftive path [path,]
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_node, in_angle, last_node, last_link, path)
        
        # search init
        touch_nodes.push((0, from_node, None, None, 0, []))
        
        # search
        while not touch_nodes.isempty():
            if touch_nodes.length() > max_buffer:
                break
            
            (cur_cost, cur_node, in_angle, last_node, last_link, path) = touch_nodes.pop()
                    
            if (to_node == cur_node):
                paths.append((cur_cost, path))
                if len(paths) >= max_path_num:
                    break
                else:
                    continue

            if arrive_nodes.has_key(cur_node):
                if cur_cost > arrive_nodes[cur_node] * cost_scope:
                    continue
            else:
                arrive_nodes[cur_node] = cur_cost
                
            adjlink_list = self._getAdjLinkList(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
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
                next_path = copy.copy(path)
                next_path.append(adjlink.link_id)
                adj_cost = get_link_cost(adjlink.link_length, adjlink.road_type, 
                                         adjlink.toll, adjlink.one_way, 
                                         next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost
                
                if max_cost is None or next_cost <= max_cost:
                    touch_nodes.push((next_cost, next_node, next_in_angle, cur_node, adjlink.link_id, next_path))
        
        return paths
    
    def _traffic_flow_filter(self, cur_node, adjlink_list, forward=True):
        legal_adjlink_list = []
        
        for adjlink in adjlink_list:
            # judge direction of traffic flow
            if forward and (adjlink.start_node_id == cur_node) and adjlink.one_way in (1,2):
                pass
            elif forward and (adjlink.end_node_id == cur_node) and adjlink.one_way in (1,3):
                pass
            elif not forward and (adjlink.start_node_id == cur_node) and adjlink.one_way in (1,3):
                pass
            elif not forward and (adjlink.end_node_id == cur_node) and adjlink.one_way in (1,2):
                pass
            else:
                continue
            
            legal_adjlink_list.append(adjlink)
        
        return legal_adjlink_list
    
    def _searchBiDijkstraPaths(self, 
                               from_node,                               # Start Node
                               to_node,                                 # End Node
                               max_buffer=5000,                         # touch_nodes限值
                               max_path_num=2,                          # 最大路径数限值
                               cost_scope=1.05,                         # 探索用cost估值参数
                               max_cost=None,                           # 探索用cost限值
                               get_link_cost='CLinkCost.getCost',       # 探索用cost计算方法
                               log=None,                                # log输出（暂时无用）
                               link_tbl = 'rdb_link',                   # 探索用Link情报表
                               node_tbl = 'rdb_node',                   # 探索用Node情报表
                               link_laneinfo_tbl = 'rdb_linklane_info', # 探索用车线情报表 
                               delete_road_type = '(7, 8, 9, 14)'       # 探索屏蔽road_type
                               ):
        # 使用双向Dijkstra算法算路，搜索范围近似两个半径r/2的圆
        # stack init
        paths = []                  # efftive path [path,]
        touch_nodes = CHeap()       # 已经reach到的候选节点集合
        forward_arrive_nodes = {}   # 从touch_nodes弹出的forward节点（即从起点往终点探索过程中的节点）集合
        backward_arrive_nodes = {}  # 从touch_nodes弹出的backward节点（即从终点往起点探索过程中的节点）集合
        
        # search init
        touch_nodes.push((0, from_node, True, None, None, 0, []))
        touch_nodes.push((0, to_node, False, None, None, 0, []))
        
        # search
        while not touch_nodes.isempty():
            if touch_nodes.length() > max_buffer:
                break
            
            if len(paths) >= max_path_num:
                break
            
            (cur_cost, cur_node, forward, in_angle, last_node, last_link, path) = touch_nodes.pop()
                        
            if forward == True: # 方向：起点--->终点
                if forward_arrive_nodes.has_key(cur_node):
                    [forward_cost, forward_path] = forward_arrive_nodes[cur_node]
                    if cur_cost > forward_cost * cost_scope:
                        continue
                else:
                    forward_arrive_nodes[cur_node] = [cur_cost, path]
                    if backward_arrive_nodes.has_key(cur_node):
                        [backward_cost, backward_path] = backward_arrive_nodes[cur_node]
                        temp_path = copy.copy(path)
                        temp_path.extend(backward_path)
                        if False == self._judge_path_is_exists(paths, temp_path):
                            paths.append((cur_cost+backward_cost, temp_path))
                        continue
            else: # 方向：终点--->起点
                if backward_arrive_nodes.has_key(cur_node):
                    [backward_cost, backward_path] = backward_arrive_nodes[cur_node]
                    if cur_cost > backward_cost * cost_scope:
                        continue
                else:
                    backward_arrive_nodes[cur_node] = [cur_cost, path]
                    if forward_arrive_nodes.has_key(cur_node):
                        [forward_cost, forward_path] = forward_arrive_nodes[cur_node]
                        temp_path = copy.copy(forward_path)
                        temp_path.extend(path)
                        if False == self._judge_path_is_exists(paths, temp_path):
                            paths.append((forward_cost+cur_cost, temp_path))
                        continue

            adjlink_list = self._getAdjLinkList(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
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
                next_path = copy.copy(path)
                if forward == True:
                    next_path.append(adjlink.link_id)
                else:
                    next_path.insert(0, adjlink.link_id)
                adj_cost = get_link_cost(adjlink.link_length, adjlink.road_type, 
                                         adjlink.toll, adjlink.one_way, 
                                         next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost
                
                if max_cost is None or next_cost <= max_cost:
                    touch_nodes.push((next_cost, next_node, forward, next_in_angle, cur_node, adjlink.link_id, next_path))
        
        return paths
    
    def _judge_path_is_exists(self, paths, cur_path):
        
        # first judge the cur_path is not in paths
        cur_path_len = len(cur_path)
        for (cost, path) in paths:
            path_len = len(path) 
            if cur_path_len == path_len:
                if self._judge_path_have_diff_link(cur_path, path):
                    continue
                else:
                    return True
        return False
        
    def _searchAStarPaths(self,  
                          from_node,                               # Start Node 
                          to_node,                                 # End Node 
                          forward=True,                            # 方向 True:S->E / False:E->S 
                          max_buffer=5000,                         # touch_nodes限值 
                          max_path_num=2,                          # 最大路径数限值 
                          cost_scope=1.05,                         # 探索用cost估值参数 
                          max_cost=None,                           # 探索用cost限值 
                          get_link_cost='CLinkCost.getCost',       # 探索用cost计算方法 
                          log=None,                                # log输出（暂时无用） 
                          link_tbl = 'rdb_link',                   # 探索用Link情报表 
                          node_tbl = 'rdb_node',                   # 探索用Node情报表 
                          link_laneinfo_tbl = 'rdb_linklane_info', # 探索用车线情报表 
                          delete_road_type = '(7, 8, 9, 14)'       # 探索屏蔽road_type 
                          ):
        # 使用A star算法进行算路，搜索范围近似长轴为r/2的椭圆
        # 允许多次经过同一Node，但决不允许同一link被多次走过
        # stack init
        paths = []                  # efftive path [path,]
        arrive_nodes = {}           # arrive nodes（保存Node被walked次数）
        touch_nodes = CHeap()       # touch_nodes (cur_explore_cost, cur_cost, cur_node, 
                                    #              in_angle, last_node, path)
                                    
        #         ●---------●-------------●
        #         ●---------●----------------●
        # ★------●---------●--------------------☆----------★
        #         ●---------●----------------●
        #         ●---------●-------------●
        
        # search init
        #file_obj = common.cache_file.open('temp1')
        (from_node_x, from_node_y) = self._getNodeCoord(from_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
        (to_node_x, to_node_y) = self._getNodeCoord(to_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
        max_cost = self._evaluation_func_distance(from_node_y, from_node_x, to_node_y, to_node_x)
        touch_nodes.push((max_cost+0, 0, from_node, None, None, []))
        
        # search
        while not touch_nodes.isempty():
            if touch_nodes.length() > max_buffer:
                #while not touch_nodes.isempty(): 
                #    (cur_search_cost, cur_cost, cur_node, in_angle, last_node, path) = touch_nodes.pop() 
                #    rec_string = '%d,\n' % (cur_node) 
                #    file_obj.write(rec_string)
                #    file_obj.flush()
                print "buffer is overflow!!!\n"
                break
            
            (cur_search_cost, cur_cost, cur_node, in_angle, last_node, path) = touch_nodes.pop()
            #rec_string = '%d,\n' % (cur_node)
            #file_obj.write(rec_string)
            #file_obj.flush()
            if path:
                last_link = path[-1]
            else:
                last_link = 0
            #print cur_node
                
            if (to_node == cur_node):
                paths.append((cur_cost, path))
                #path_len = len(path)
                #temp_string = ''
                #for loop in range(0, path_len):
                #    str = '%d,' % path[loop]
                #    temp_string += str
                #str = '%f   ' % cur_cost
                #rec_string = str + temp_string
                #file_obj.write(rec_string)
                #file_obj.flush()
                if len(paths) >= max_path_num:
                    #while not touch_nodes.isempty(): 
                    #    (cur_search_cost, cur_cost, cur_node, in_angle, last_node, path) = touch_nodes.pop() 
                    #    rec_string = '%d,\n' % (cur_node) 
                    #    file_obj.write(rec_string)
                    #    file_obj.flush()
                    print "search success\n"
                    break
                else:
                    continue

            if arrive_nodes.has_key(cur_node):
                # 若一个Node被走过max_path_num次，就不再走；反之，继续走
                if arrive_nodes[cur_node][1] >= max_path_num or cur_cost > arrive_nodes[cur_node][0] * cost_scope:
                    continue
                else:
                    arrive_nodes[cur_node][1] += 1
            else:
                arrive_nodes[cur_node] = [cur_cost, 1] # 第一次走过Node
            
            (cur_node_x, cur_node_y) = self._getNodeCoord(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            evaluation_cost = self._evaluation_func_distance(cur_node_y, cur_node_x, to_node_y, to_node_x)
            adjlink_list = self._getAdjLinkList(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                # 不允许U Turn link；不允许link被重复walked（防止绕环）
                if (adjlink.link_id == last_link) or (adjlink.link_id in path):
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
                next_path = copy.copy(path)
                next_path.append(adjlink.link_id)
                adj_cost = get_link_cost(adjlink.link_length, adjlink.road_type, 
                                         adjlink.toll, adjlink.one_way, 
                                         next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost
                
                if max_cost is None or next_cost <= max_cost:
                    touch_nodes.push((evaluation_cost+next_cost, next_cost, next_node, next_in_angle, cur_node, next_path))
        else:
            print "No Road to go!!!\n"
        #common.cache_file.close(file_obj,False)
        return paths
    
    def _judge_path_have_diff_link(self, pathA = [], pathB = []):
        # 求Path A / B的差集
        return list(set(pathA).difference(set(pathB)))
    
    def _searchBi_AStarPaths(self,
                             from_node,                               # Start Node 
                             to_node,                                 # End Node
                             max_buffer=5000,                         # touch_nodes限值
                             max_path_num=2,                          # 最大路径数限值
                             cost_scope=1.05,                         # 探索用cost估值参数
                             max_cost=None,                           # 探索用cost限值
                             get_link_cost='CLinkCost.getCost',       # 探索用cost计算方法
                             log=None,                                # log输出（暂时无用）    
                             link_tbl = 'rdb_link',                   # 探索用Link情报表
                             node_tbl = 'rdb_node',                   # 探索用Node情报表
                             link_laneinfo_tbl = 'rdb_linklane_info', # 探索用车线情报表
                             delete_road_type = '(7, 8, 9, 14)'       # 探索屏蔽road_type
                            ):
        # 使用双向A星算路算路，搜索范围近似两个长轴为r/4的椭圆
        # stack init
        paths = []                  # efftive path [path,]
        touch_nodes = CHeap()       # 已经reach到的候选节点集合
        forward_arrive_nodes = {}   # 从touch_nodes弹出的forward节点（即从起点往终点探索过程中的节点）集合
        backward_arrive_nodes = {}  # 从touch_nodes弹出的backward节点（即从终点往起点探索过程中的节点）集合
        
        # search init
        #file_obj = common.cache_file.open('temp2')
        (from_node_x, from_node_y) = self._getNodeCoord(from_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
        (to_node_x, to_node_y) = self._getNodeCoord(to_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
        max_cost = self._evaluation_func_distance(from_node, from_node_y, from_node_x, to_node, to_node_y, to_node_x)
        touch_nodes.push((max_cost+0, 0, from_node, True, None, None, []))
        touch_nodes.push((max_cost+0, 0, to_node, False, None, None, []))

        # search
        while not touch_nodes.isempty():
            if touch_nodes.length() > max_buffer:
                #while not touch_nodes.isempty(): 
                #    (cur_search_cost, cur_cost, cur_node, forward, in_angle, last_node, path) = touch_nodes.pop() 
                #    rec_string = '%d,\n' % (cur_node) 
                #    file_obj.write(rec_string)
                #    file_obj.flush()
                if log:
                    log.warning("buffer is overflow!!!")
                break
            
            if len(paths) >= max_path_num:
                #while not touch_nodes.isempty(): 
                #    (cur_search_cost, cur_cost, cur_node, forward, in_angle, last_node, path) = touch_nodes.pop() 
                #    rec_string = '%d,\n' % (cur_node) 
                #    file_obj.write(rec_string)
                #    file_obj.flush()
                break
            
            (cur_search_cost, cur_cost, cur_node, forward, in_angle, last_node, path) = touch_nodes.pop()
            #rec_string = '%d,\n' % (cur_node)
            #file_obj.write(rec_string)
            #file_obj.flush()
            if path:
                last_link = path[-1]
            else:
                last_link = 0
                        
            if True == forward: # 方向：起点--->终点
                if forward_arrive_nodes.has_key(cur_node):
                    if forward_arrive_nodes[cur_node][1] >= max_path_num or cur_cost > forward_arrive_nodes[cur_node][0] * cost_scope:
                        continue
                    else:
                        forward_arrive_nodes[cur_node][1] += 1
                else:
                    forward_arrive_nodes[cur_node] = [cur_cost, 1, path]
                    if backward_arrive_nodes.has_key(cur_node):
                        temp_path = copy.copy(path)
                        temp_back_path = copy.copy(backward_arrive_nodes[cur_node][2])
                        temp_back_path.reverse()
                        temp_path.extend(temp_back_path)
                        if False == self._judge_path_is_exists(paths, temp_path):
                            paths.append((cur_cost+backward_arrive_nodes[cur_node][0], temp_path))
                            #path_len = len(temp_path)
                            #temp_string = ''
                            #for loop in range(0, path_len):
                            #    str = '%d,' % temp_path[loop]
                            #    temp_string += str
                            #str = '%f   ' % (cur_cost+backward_arrive_nodes[cur_node][0])
                            #rec_string = str + temp_string
                            #file_obj.write(rec_string)
                            #file_obj.flush()
                        continue
            else: # 方向：终点--->起点
                if backward_arrive_nodes.has_key(cur_node):
                    if backward_arrive_nodes[cur_node][1] >= max_path_num or cur_cost > backward_arrive_nodes[cur_node][0] * cost_scope:
                        continue
                    else:
                        backward_arrive_nodes[cur_node][1] += 1
                else:
                    backward_arrive_nodes[cur_node] = [cur_cost, 1, path]
                    if forward_arrive_nodes.has_key(cur_node):
                        temp_path = copy.copy(forward_arrive_nodes[cur_node][2])
                        temp_back_path = copy.copy(path)
                        temp_back_path.reverse()
                        temp_path.extend(temp_back_path)
                        if False == self._judge_path_is_exists(paths, temp_path):
                            paths.append((forward_arrive_nodes[cur_node][0]+cur_cost, temp_path))
                            #path_len = len(temp_path)
                            #temp_string = ''
                            #for loop in range(0, path_len):
                            #    str = '%d,' % temp_path[loop]
                            #    temp_string += str
                            #str = '%f   ' % (forward_arrive_nodes[cur_node][0]+cur_cost)
                            #rec_string = str + temp_string
                            #file_obj.write(rec_string)
                            #file_obj.flush()
                        continue
            
            (cur_node_x, cur_node_y) = self._getNodeCoord(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            if True == forward:
                # 正向搜索 估价函数是当前Node至目的地Node的cost
                evaluation_cost = self._evaluation_func_distance(cur_node, cur_node_y, cur_node_x, to_node, to_node_y, to_node_x)
            else:
                # 反向搜索 估价函数是当前Node至出发地Node的cost
                evaluation_cost = self._evaluation_func_distance(cur_node, cur_node_y, cur_node_x, from_node, from_node_y, from_node_x)
                
            adjlink_list = self._getAdjLinkList(cur_node, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                # 不允许U Turn link；不允许link被重复walked（防止绕环）
                if (adjlink.link_id == last_link) or (adjlink.link_id in path):
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
                next_path = copy.copy(path)
                next_path.append(adjlink.link_id)
                adj_cost = get_link_cost(adjlink.link_length, adjlink.road_type, adjlink.toll, adjlink.one_way, next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost
                
                if max_cost is None or next_cost <= max_cost:
                    touch_nodes.push((evaluation_cost+next_cost, next_cost, next_node, forward, next_in_angle, cur_node, next_path))
        else:
            if log:
                log.warning("No Road to go!!!")
        #common.cache_file.close(file_obj,False)
        return paths
    
    def _evaluation_func_distance(self, nodeA, Lat_A, Lng_A, nodeB, Lat_B, Lng_B): 
        
        # 估价函数：通过经纬度计算Node间的距离
        if nodeA == nodeB:
            return 0.0
        
        ra = 6378.140 # 赤道半径 (km) 
        rb = 6356.755 # 极半径 (km)
        flatten = (ra - rb) / ra # 地球扁率
        rad_lat_A = radians(Lat_A)
        rad_lng_A = radians(Lng_A)
        rad_lat_B = radians(Lat_B)
        rad_lng_B = radians(Lng_B)
        pA = atan(rb / ra * tan(rad_lat_A))
        pB = atan(rb / ra * tan(rad_lat_B))
        xx = acos(sin(pA) * sin(pB) + cos(pA) * cos(pB) * cos(rad_lng_A - rad_lng_B))
        c1 = (sin(xx) - xx) * (sin(pA) + sin(pB)) ** 2 / cos(xx / 2) ** 2
        c2 = (sin(xx) + xx) * (sin(pA) - sin(pB)) ** 2 / sin(xx / 2) ** 2
        dr = flatten / 8 * (c1 - c2)
        distance = ra * (xx + dr)
        cost = distance / 60 * 3600 # 车速设定60km/h，cost设定为秒
        return cost
    
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
    
    def length(self):
        return len(self.data)

class CLink:
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
        self.node_id = link_rec[11]

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
    def getCost(link_length, road_type, toll, one_way, width, in_angle, out_angle):
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
    
    @staticmethod
    def getCost2(link_length, road_type, toll, one_way, width, in_angle, out_angle):
        
        return link_length