# -*- coding: cp936 -*-
'''
Created on 2014-8-16

@author: liuxinxing
'''
import os
import sys
import time
import copy
import heapq
import math
import multiprocessing
import common.rdb_common

class CGraph:
    def __init__(self):
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
                next_path = copy.copy(path)
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
                next_path = copy.copy(path)
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
    
    def searchShortestPaths(self, inlink, outlink, innode=None, outnode=None):
        # stack init
        paths = []                  # efftive path [(cur_distance, path),]
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_distance, cur_node, in_angle, last_node, last_link, path)
        
        # search init
        if innode is not None:
            touch_nodes.push((0, 0, innode, None, None, inlink, [inlink]))
        else:
            link = self._getLinkInfo(inlink)
            if link.one_way in (1,2):
                touch_nodes.push((0, 0, link.end_node_id, None, None, inlink, [inlink]))
            if link.one_way in (1,3):
                touch_nodes.push((0, 0, link.start_node_id, None, None, inlink, [inlink]))
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        max_distance=1000
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
                next_path = copy.copy(path)
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
    
    def searchDijkstraPaths(self,
                            from_node, to_node, forward=True,
                            is_legal_path='self._is_legal_path',
                            max_buffer=5000, max_path_num=2,
                            cost_scope=1.05, max_cost=None,
                            get_link_cost='CLinkCost.getCost',
                            log=None):
        # stack init
        paths = []                  # efftive path [path,]
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_node, in_angle, last_node, last_link, path)
        
        # search init
        touch_nodes.push((0, from_node, None, None, 0, []))
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        while not touch_nodes.isempty():
            #print 'from_node = %s, to_node = %s' % (str(from_node), str(to_node))
            #print 'touch_nodes.length() =', touch_nodes.length()
            if touch_nodes.length() > max_buffer:
                break
            
            (cur_cost, cur_node, in_angle, last_node, last_link, path) = touch_nodes.pop()
            #print cur_node, last_link, path
            
            if (to_node == cur_node):
                paths.append(path)
                if len(paths) >= max_path_num:
                    break
                else:
                    continue
            
            #print 'search node', (cost, node_id, path)
            if arrive_nodes.has_key(cur_node):
                if cur_cost > arrive_nodes[cur_node] * cost_scope:
                    continue
            else:
                arrive_nodes[cur_node] = cur_cost
            
            adjlink_list = self._getAdjLinkList(cur_node)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
                    continue
                
                # check if walk this link
                if not is_legal_path(adjlink):
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
                #print 'touch', adjnode, adjcost
                
                if max_cost is None or next_cost <= max_cost:
                    touch_nodes.push((next_cost, next_node, next_in_angle, cur_node, adjlink.link_id, next_path))
        
        return paths
    
    # set from_node as the root of min-spanning-tree,
    # and set to_node_list as the leaves of min-spanning-tree,
    # then return all paths from the root to every leaf.
    def searchMinSpanningTree(self, 
                              from_node, to_node_list=None, forward=True, 
                              is_legal_path='self._is_legal_path', is_leaf='self._is_leaf', stop_at_leaf=True,
                              max_cost=None, get_link_cost='CLinkCost.getCost',
                              max_buffer=5000, cost_scope=1.05,
                              max_single_path_num=2, max_all_path_num=None,
                              log=None):
        # stack init
        paths = {}                  # efftive path {to_node:[(cost,path),]}
        arrive_nodes = {}           # arrive nodes {(arrive_node:[cost,path_num])}
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_node, in_angle, last_node, last_link, path)
        
        # search init
        touch_nodes.push((0, from_node, None, None, 0, []))
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        all_path_num = 0
        all_access_buffer = 0
        while not touch_nodes.isempty():
            all_access_buffer += 1
            if all_access_buffer > max_buffer:
                break
            #if touch_nodes.length() > max_buffer:
            #    break
            
            (cur_cost, cur_node, in_angle, last_node, last_link, path) = touch_nodes.pop()
            #print cur_node, last_link, path
            
            # check if cur_node is a leaf
            if is_leaf(cur_node, leaves=to_node_list, forward=forward) and (cur_node != from_node):
                # add to paths of root-2-leaf
                this_paths = paths.get(cur_node)
                if not this_paths:
                    this_paths = [(cur_cost,path)]
                    paths[cur_node] = this_paths
                    all_path_num += 1
                else:
                    if len(this_paths) < max_single_path_num:
                        this_paths.append((cur_cost,path))
                        all_path_num += 1
                
                #if log:
                #    log.info('all_path_num = %s, cur_node = %s, cur_cost = %s' % (str(all_path_num), str(cur_node), str(cur_cost)))
                #time.sleep(0.1)
                # check if all leaves have been found
                if max_all_path_num is not None and all_path_num >= max_all_path_num:
                    break
                else:
                    if stop_at_leaf:
                        continue
            
            # check if cur_node had been accessed
            arrive_memory = arrive_nodes.get(cur_node)
            if not arrive_memory:
                arrive_memory = [cur_cost,1]
                arrive_nodes[cur_node] = arrive_memory
            else:
                if arrive_memory[1] >= max_single_path_num or cur_cost > arrive_memory[0] * cost_scope:
                    continue
                else:
                    arrive_memory[1] = arrive_memory[1] + 1
            
            # touch adj links
            adjlink_list = self._getAdjLinkList(cur_node)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
                    continue
                
                # check if walk this link
                if not is_legal_path(adjlink):
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
                #print 'touch', adjnode, adjcost
                
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
    
    def _is_legal_path(self, adjlink):
        return True
    
    def _is_legal_path_of_sa(self, adjlink):
        return (adjlink.link_type in (7,))
    
    def _is_legal_path_of_ramp(self, adjlink):
        return (adjlink.link_type in (5,))
    
    def _is_legal_path_of_no_sa(self, adjlink):
        return not (adjlink.link_type in (7,))
    
    def _is_legal_path_of_no_ramp(self, adjlink):
        return not (adjlink.link_type in (5,))
    
    def _is_leaf(self, nodeid, leaves=None, forward=True):
        return False
    
    def _is_leaf_of_region_mesh(self, nodeid, leaves=None, forward=True):
        return (leaves is not None and nodeid in leaves)
    
    def _is_leaf_of_sa(self, nodeid, leaves=None, forward=True):
        adjlink_list = self._getAdjLinkList(nodeid)
        adjlink_list = self._traffic_flow_filter(nodeid, adjlink_list, forward)
        for adjlink in adjlink_list:
            if not (adjlink.link_type in (7,)):
                return True
        
        return False
    
    def _is_leaf_of_ramp(self, nodeid, leaves=None, forward=True):
        adjlink_list = self._getAdjLinkList(nodeid)
        adjlink_list = self._traffic_flow_filter(nodeid, adjlink_list, forward)
        for adjlink in adjlink_list:
            if not (adjlink.link_type in (5,)):
                return True
        
        return False

class CGraph_PG(CGraph):
    def __init__(self):
        pass
    
    def prepareData(self,
                    rdb_link='rdb_link'):
        self.rdb_link = rdb_link
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
                        link_type,
                        toll,
                        link_length,
                        one_way,
                        start_node_id,
                        end_node_id,
                        (fazm + 32768) * 360.0 / 65536.0,
                        (tazm + 32768) * 360.0 / 65536.0,
                        ops_width,
                        neg_width
                from [rdb_link] as a
                left join rdb_linklane_info as b
                on a.lane_id = b.lane_id
                where [node_id] in (a.start_node_id, a.end_node_id)
                """
        sqlcmd = sqlcmd.replace('[rdb_link]', self.rdb_link)
        sqlcmd = sqlcmd.replace('[node_id]', str(node_id))
        self.pg.execute(sqlcmd)
        link_recs = self.pg.fetchall2()
        adjlinks = []
        for link_rec in link_recs:
            adjlinks.append(CLink(link_rec))
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

class CGraph_Memory(CGraph):
    auto_overlap = {
                    16:0,
                    15:0,
                    14:2,
                    13:4,
                    12:8,
                    11:12,
                    10:16,
                    9:32,
                    8:64,
                    7:128,
                    6:256,
                    5:512,
                    4:1024,
                    3:2048,
                    2:4096,
                    1:8192,
                    }
    def __init__(self):
        self.links = {}
        self.nodes = {}
        self.boundary_nodes = set()
    
    def prepareData(self, 
                    meshid,
                    rdb_link='rdb_link',
                    rdb_node_boundary='rdb_node_boundary',
                    use_mesh_overwrap=0                     # the number of tile16 in overwrap
                    ):
        self.meshid = meshid
        self.rdb_link = rdb_link
        self.rdb_node_boundary = rdb_node_boundary
        self.use_mesh_overwrap = use_mesh_overwrap
        
        if not use_mesh_overwrap:
            self._prepareData()
        else:
            self._prepareDataWithOverWrap()
    
    def _prepareData(self):
        import common.rdb_database
        self.pg = common.rdb_database.rdb_pg()
        self.pg.connect2()
        
        # mesh scope
        tz = (self.meshid >> 32) & 127
        tx = (self.meshid >> 16) & 65535
        ty = self.meshid & 65535
        self.tz = tz
        self.tx_16_min = (tx << (16 - tz))
        self.tx_16_max = (((tx + 1) << (16 - tz)) - 1)
        self.ty_16_min = (ty << (16 - tz))
        self.ty_16_max = (((ty + 1) << (16 - tz)) - 1)
        
        # init links & nodes
        sqlcmd = """
                select  a.link_id,
                        a.road_type,
                        a.link_type,
                        a.toll,
                        a.link_length,
                        a.one_way,
                        a.start_node_id,
                        a.end_node_id,
                        (a.fazm + 32768) * 360.0 / 65536.0 as fazm,
                        (a.tazm + 32768) * 360.0 / 65536.0 as tazm,
                        a.ops_width,
                        a.neg_width
                from [rdb_link] as a
                where a.meshid = [meshid]
                """
        sqlcmd = sqlcmd.replace('[rdb_link]', self.rdb_link)
        sqlcmd = sqlcmd.replace('[meshid]', str(self.meshid))
        
        link_recs = self.pg.get_batch_data2(sqlcmd)
        for link_rec in link_recs:
            objLink = CLink(link_rec)
            self.links[objLink.link_id] = objLink
            
            if not self.nodes.has_key(objLink.start_node_id):
                self.nodes[objLink.start_node_id] = [objLink]
            else:
                self.nodes[objLink.start_node_id].append(objLink)
                
            if not self.nodes.has_key(objLink.end_node_id):
                self.nodes[objLink.end_node_id] = [objLink]
            else:
                self.nodes[objLink.end_node_id].append(objLink)
        
        # init boundary_nodes
        sqlcmd = """
                select node_id
                from [rdb_node_boundary]
                where meshid = [meshid]
                """
        sqlcmd = sqlcmd.replace('[meshid]', str(self.meshid))
        sqlcmd = sqlcmd.replace('[rdb_node_boundary]', self.rdb_node_boundary)
        node_recs = self.pg.get_batch_data2(sqlcmd)
        for node_rec in node_recs:
            self.boundary_nodes.add(node_rec[0])
        
        self.pg.close2()
    
    def _prepareDataWithOverWrap(self):
        import common.rdb_database
        self.pg = common.rdb_database.rdb_pg()
        self.pg.connect2()
        
        # overlap area is level-16-tile around the mesh
        tz = (self.meshid >> 32) & 127
        tx = (self.meshid >> 16) & 65535
        ty = self.meshid & 65535
        
        if self.use_mesh_overwrap < 0:
            #overlap_t16 = 18 - tz
            overlap_t16 = self.auto_overlap.get(tz)
        else:
            overlap_t16 = self.use_mesh_overwrap
        
        self.tz = tz
        self.tx_16_min = (tx << (16 - tz)) - overlap_t16
        self.tx_16_max = (((tx + 1) << (16 - tz)) - 1) + overlap_t16
        self.ty_16_min = (ty << (16 - tz)) - overlap_t16
        self.ty_16_max = (((ty + 1) << (16 - tz)) - 1) + overlap_t16
        
        # init links & nodes
        sqlcmd = """
                select  link_id,
                        road_type,
                        link_type,
                        toll,
                        link_length,
                        one_way,
                        start_node_id,
                        end_node_id,
                        (fazm + 32768) * 360.0 / 65536.0 as fazm,
                        (tazm + 32768) * 360.0 / 65536.0 as tazm,
                        ops_width,
                        neg_width,
                        meshid,
                        start_node_tx_16,
                        start_node_ty_16,
                        end_node_tx_16,
                        end_node_ty_16
                from [rdb_link]
                where   (
                            (start_node_tx_16 >= [tx_16_min] and start_node_tx_16 <= [tx_16_max])
                            and
                            (start_node_ty_16 >= [ty_16_min] and start_node_ty_16 <= [ty_16_max])
                        )
                        or
                        (
                            (end_node_tx_16 >= [tx_16_min] and end_node_tx_16 <= [tx_16_max])
                            and
                            (end_node_ty_16 >= [ty_16_min] and end_node_ty_16 <= [ty_16_max])
                        );
                """
        sqlcmd = sqlcmd.replace('[rdb_link]', self.rdb_link)
        sqlcmd = sqlcmd.replace('[meshid]', str(self.meshid))
        sqlcmd = sqlcmd.replace('[tx_16_min]', str(self.tx_16_min))
        sqlcmd = sqlcmd.replace('[tx_16_max]', str(self.tx_16_max))
        sqlcmd = sqlcmd.replace('[ty_16_min]', str(self.ty_16_min))
        sqlcmd = sqlcmd.replace('[ty_16_max]', str(self.ty_16_max))
        
        link_recs = self.pg.get_batch_data2(sqlcmd)
        for link_rec in link_recs:
            objLink = CLink(link_rec)
            
            # link
            self.links[objLink.link_id] = objLink
            
            # node
            if not self.nodes.has_key(objLink.start_node_id):
                self.nodes[objLink.start_node_id] = [objLink]
            else:
                self.nodes[objLink.start_node_id].append(objLink)
            
            if not self.nodes.has_key(objLink.end_node_id):
                self.nodes[objLink.end_node_id] = [objLink]
            else:
                self.nodes[objLink.end_node_id].append(objLink)
            
            # boundary nodes
            if (
                objLink.start_node_tx < self.tx_16_min or objLink.start_node_tx > self.tx_16_max
                or
                objLink.start_node_ty < self.ty_16_min or objLink.start_node_ty > self.ty_16_max
                ):
                self.boundary_nodes.add(objLink.start_node_id)
            
            if (
                objLink.end_node_tx < self.tx_16_min or objLink.end_node_tx > self.tx_16_max
                or
                objLink.end_node_ty < self.ty_16_min or objLink.end_node_ty > self.ty_16_max
                ):
                self.boundary_nodes.add(objLink.end_node_id)
        
        self.pg.close2()
    
    def clearData(self):
        pass
    
    def _getAdjLinkList(self, node_id):
        adjlinks = self.nodes.get(node_id)
        if not adjlinks:
            adjlinks = []
        return adjlinks
    
    def group_nodes_by_weakly_connection(self, target_node_list):
        groups = []                 # [(nodes, from_nodes, to_nodes)]
        
        target_node_set = set(target_node_list)
        while len(target_node_set) > 0:
            #
            group_nodes = set()
            group_from_nodes = set()
            group_to_nodes = set()
            walk_nodes = set()
            touch_nodes = []
            
            root_node = target_node_set.pop()
            walk_nodes.add(root_node)
            touch_nodes.append(root_node)
            while len(touch_nodes) > 0:
                nodeid = touch_nodes.pop()
                
                boundary_node_flag = False
                if nodeid == root_node:
                    group_nodes.add(nodeid)
                    boundary_node_flag = True
                if nodeid in target_node_set:
                    group_nodes.add(nodeid)
                    boundary_node_flag = True
                    target_node_set.remove(nodeid)
                
                adjlink_list = self._getAdjLinkList(nodeid)
                for adjlink in adjlink_list:
                    if adjlink.start_node_id == nodeid:
                        nextnode = adjlink.end_node_id
                        if boundary_node_flag:
                            if adjlink.one_way in (1,2):
                                group_from_nodes.add(nodeid)
                            if adjlink.one_way in (1,3):
                                group_to_nodes.add(nodeid)
                    else:
                        nextnode = adjlink.start_node_id
                        if boundary_node_flag:
                            if adjlink.one_way in (1,3):
                                group_from_nodes.add(nodeid)
                            if adjlink.one_way in (1,2):
                                group_to_nodes.add(nodeid)
                    
                    if nextnode in walk_nodes:
                        continue
                    else:
                        walk_nodes.add(nextnode)
                        touch_nodes.append(nextnode)
            #
            groups.append((group_nodes, group_from_nodes, group_to_nodes))
        
        return groups
    
    def searchTrunkLinks(self, log=None, get_link_cost='CLinkCost.getCost'):
        trunk_paths = self.searchTrunkPaths(log=log, get_link_cost=get_link_cost)
        trunk_links = set()
        for from_node, all_shortest_paths in trunk_paths.items():
            for to_node, paths in all_shortest_paths.items():
                for (cost,path) in paths:
                    trunk_links.update(path)
        return trunk_links
    
    def searchTrunkPaths(self, log=None, get_link_cost='CLinkCost.getCost'):
        trunk_paths = {}
        nCount = len(self.boundary_nodes)
        nIndex = 0
        boundary_nodes_group = self.group_nodes_by_weakly_connection(self.boundary_nodes)
        for (group_nodes, group_from_nodes, group_to_nodes) in boundary_nodes_group:
            for from_node in group_from_nodes:
                nIndex += 1
                #if log:
                #    log.info('processing node %s.%s (%s/%s)...' % (str(self.meshid), str(from_node), str(nIndex), str(nCount)))
                all_paths = self.searchMinSpanningTree(from_node, 
                                                       to_node_list=group_to_nodes,
                                                       is_legal_path=self._is_legal_path,
                                                       is_leaf=self._is_leaf_of_region_mesh,
                                                       max_buffer=len(self.nodes)*4,
                                                       get_link_cost=get_link_cost,
                                                       log=log)
                trunk_paths[from_node] = all_paths
        return trunk_paths
    
    def simplify(self):
        #
        simple_graph = CGraph_Memory()
        simple_graph.boundary_nodes = set(self.boundary_nodes)
        link_mapping = {}
        
        #
        prep_sublink_array = set(self.links.keys())
        while len(prep_sublink_array) > 0:
            # get one sublink
            sublinkid = prep_sublink_array.pop()
            sublink = self.links[sublinkid]
            linkrow = CLink(sublink.link_rec)
            linkrow.sublinks = set([sublink.link_id])
            #linkrow.sublinkarray = [sublink.link_id]
            
            # search linkrow
            search_nodes = []
            search_nodes.append((True, linkrow.end_node_id))    # search forward
            search_nodes.append((False, linkrow.start_node_id)) # search backward
            for (search_dir, reach_node) in search_nodes:
                while 1:
                    if reach_node in self.boundary_nodes:
                        break
                    
                    adjlinks = self.nodes.get(reach_node)
                    if len(adjlinks) > 2:
                        break
                    
                    is_same_adjlink = False
                    for adjlink in adjlinks:
                        if adjlink.link_id in linkrow.sublinks:
                            continue
                        
                        if adjlink.link_id not in prep_sublink_array:
                            continue
                        
                        if (adjlink.road_type != sublink.road_type):
                            continue
                        
                        #if (adjlink.link_type != sublink.link_type):
                        #    continue
                        
                        if (adjlink.toll != sublink.toll):
                            continue
                        
                        if (reach_node == adjlink.start_node_id):
                            next_node = adjlink.end_node_id
                            if search_dir:
                                one_way = adjlink.one_way
                                ops_width = adjlink.ops_width
                                neg_width = adjlink.neg_width
                                fazm = adjlink.fazm
                                tazm = adjlink.tazm
                            else:
                                if adjlink.one_way == 2:
                                    one_way = 3
                                elif adjlink.one_way == 3:
                                    one_way = 2
                                else:
                                    one_way = adjlink.one_way
                                
                                ops_width = adjlink.neg_width
                                neg_width = adjlink.ops_width
                                fazm = adjlink.tazm
                                tazm = adjlink.fazm
                        else:
                            next_node = adjlink.start_node_id
                            if search_dir:
                                if adjlink.one_way == 2:
                                    one_way = 3
                                elif adjlink.one_way == 3:
                                    one_way = 2
                                else:
                                    one_way = adjlink.one_way
                                
                                ops_width = adjlink.neg_width
                                neg_width = adjlink.ops_width
                                fazm = adjlink.tazm
                                tazm = adjlink.fazm
                            else:
                                one_way = adjlink.one_way
                                ops_width = adjlink.ops_width
                                neg_width = adjlink.neg_width
                                fazm = adjlink.fazm
                                tazm = adjlink.tazm
                        
                        if not (
                                (linkrow.one_way == one_way)
                                and
                                (linkrow.ops_width == ops_width)
                                and
                                (linkrow.neg_width == neg_width)
                            ):
                            continue
                        
                        #
                        is_same_adjlink = True
                        reach_node = next_node
                        prep_sublink_array.remove(adjlink.link_id)
                        if search_dir:
                            linkrow.end_node_id = reach_node
                            linkrow.link_length += adjlink.link_length
                            linkrow.sublinks.add(adjlink.link_id)
                            #linkrow.sublinkarray.append(adjlink.link_id)
                            linkrow.tazm = tazm
                        else:
                            linkrow.start_node_id = reach_node
                            linkrow.link_length += adjlink.link_length
                            linkrow.sublinks.add(adjlink.link_id)
                            #linkrow.sublinkarray.insert(0, adjlink.link_id)
                            linkrow.fazm = fazm
                        break
                    #end of for loop, check all adjlink of current reach_node
                    
                    if not is_same_adjlink:
                        break
                #end of while loop, search in one direction
            #end of for loop, search in two direction
            
            # add into mapping
            link_mapping[linkrow.link_id] = linkrow.sublinks
            #link_mapping[linkrow.link_id] = linkrow.sublinkarray
            simple_graph.links[linkrow.link_id] = linkrow
            if not simple_graph.nodes.has_key(linkrow.start_node_id):
                simple_graph.nodes[linkrow.start_node_id] = [linkrow]
            else:
                simple_graph.nodes[linkrow.start_node_id].append(linkrow)
            if not simple_graph.nodes.has_key(linkrow.end_node_id):
                simple_graph.nodes[linkrow.end_node_id] = [linkrow]
            else:
                simple_graph.nodes[linkrow.end_node_id].append(linkrow)
            #print (linkrow.link_id, linkrow.start_node_id, linkrow.end_node_id, len(linkrow.sublinks), linkrow.one_way)
        #end of while loop, search all sublinks
        
        return (link_mapping, simple_graph)
    
    def searchTrunkLinksUsingSimplifing(self, log=None):
        trunk_links = set()
        link_mapping, simple_graph = self.simplify()
        trunk_linkrows = simple_graph.searchTrunkLinks(log=log, get_link_cost=CLinkCost.getCost)
        for linkrow in trunk_linkrows:
            trunk_links.update(link_mapping[linkrow])
        return trunk_links
    
    def contract(self, log=None, max_link_num=1000):
        proxy_graph = CGraph_Memory()
        proxy_graph.boundary_nodes = set(self.boundary_nodes)
        proxy_link_id = 0
        proxy_link_mapping = {}
        
        all_sub_graph = self._divide_into_submesh(max_link_num=max_link_num)
        #print 'all_sub_graph num =', len(all_sub_graph)
        for sub_graph in all_sub_graph:
            sub_link_mapping, simple_sub_graph = sub_graph.simplify()
            sub_paths = simple_sub_graph.searchTrunkPaths(log=log, get_link_cost=CLinkCost.getCost)
            for from_node, all_shortest_paths in sub_paths.items():
                for to_node, paths in all_shortest_paths.items():
                    for (cost,path) in paths:
                        head_link = self.links[sub_link_mapping[path[0]][0]]
                        tail_link = self.links[sub_link_mapping[path[-1]][-1]]
                        
                        objLink = CLink(head_link.link_rec)
                        proxy_link_id += 1
                        objLink.link_id = proxy_link_id
                        objLink.start_node_id = from_node
                        objLink.end_node_id = to_node
                        objLink.one_way = 2
                        objLink.link_length = cost
                        if from_node == head_link.start_node_id:
                            objLink.fazm = head_link.fazm
                        else:
                            objLink.fazm = head_link.tazm
                        if to_node == tail_link.start_node_id:
                            objLink.tazm = tail_link.fazm
                        else:
                            objLink.tazm = tail_link.tazm
                        
                        proxy_graph.links[proxy_link_id] = objLink
                        if not proxy_graph.nodes.has_key(from_node):
                            proxy_graph.nodes[from_node] = [objLink]
                        else:
                            proxy_graph.nodes[from_node].append(objLink)
                        if not proxy_graph.nodes.has_key(to_node):
                            proxy_graph.nodes[to_node] = [objLink]
                        else:
                            proxy_graph.nodes[to_node].append(objLink)
                        
                        sublinks = set()
                        for link in path:
                            sublinks.update(sub_link_mapping[link])
                        proxy_link_mapping[proxy_link_id] = sublinks
        #print 'proxy_graph.links =', len(proxy_graph.links)
        return proxy_link_mapping, proxy_graph
    
    def _divide_into_submesh(self, max_link_num=1000):
        #
        all_sub_graph = {}
        node_submesh_mapping = {}
        dz = int(math.log(len(self.links) / max_link_num, 16))
        tz = self.tz + dz
        #print self.tz, len(self.links), tz
        
        # divide links into sub meshes
        for objLink in self.links.values():
            # find sub_mesh
            tx = objLink.start_node_tx >> (16 - tz)
            ty = objLink.start_node_ty >> (16 - tz)
            meshid = (tx << 16) | ty
            sub_graph = all_sub_graph.get(meshid)
            if sub_graph is None:
                sub_graph = CGraph_Memory()
                all_sub_graph[meshid] = sub_graph
            
            # add link and node
            sub_graph.links[objLink.link_id] = objLink
            
            if not sub_graph.nodes.has_key(objLink.start_node_id):
                sub_graph.nodes[objLink.start_node_id] = [objLink]
            else:
                sub_graph.nodes[objLink.start_node_id].append(objLink)
                
            if not sub_graph.nodes.has_key(objLink.end_node_id):
                sub_graph.nodes[objLink.end_node_id] = [objLink]
            else:
                sub_graph.nodes[objLink.end_node_id].append(objLink)
            
            #
            if not node_submesh_mapping.has_key(objLink.start_node_id):
                node_submesh_mapping[objLink.start_node_id] = set([meshid])
            else:
                node_submesh_mapping[objLink.start_node_id].add(meshid)
                
            if not node_submesh_mapping.has_key(objLink.end_node_id):
                node_submesh_mapping[objLink.end_node_id] = set([meshid])
            else:
                node_submesh_mapping[objLink.end_node_id].add(meshid)
        
        # set boundary nodes
        for meshid,sub_graph in all_sub_graph.items():
            #print meshid, len(sub_graph.links)
            for node in sub_graph.nodes.keys():
                if node in self.boundary_nodes:
                    sub_graph.boundary_nodes.add(node)
                if len(node_submesh_mapping[node]) > 1:
                    sub_graph.boundary_nodes.add(node)
        
        return all_sub_graph.values()
    
    def searchTrunkLinksUsingContract(self, max_link_num=1000, log=None):
        if len(self.links) < max_link_num:
            return self.searchTrunkLinksUsingSimplifing(log=None)
        else:
            trunk_links = set()
            link_mapping, proxy_graph = self.contract(log=None,max_link_num=max_link_num)
            trunk_linkrows = proxy_graph.searchTrunkLinks(log=log, get_link_cost=CLinkCost.getCost3)
            for linkrow in trunk_linkrows:
                trunk_links.update(link_mapping[linkrow])
            return trunk_links

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
    def __init__(self, link_rec=[]):
        self.link_rec = link_rec
        
        if self.link_rec:
            self.link_id = link_rec[0]
            self.road_type = link_rec[1]
            self.link_type = link_rec[2]
            self.toll = link_rec[3]
            self.link_length = link_rec[4]
            self.one_way = link_rec[5]
            self.start_node_id = link_rec[6]
            self.end_node_id = link_rec[7]
            self.fazm = link_rec[8]
            self.tazm = link_rec[9]
            self.ops_width = link_rec[10]
            self.neg_width = link_rec[11]
        
            if len(link_rec) == 17:
                self.meshid = link_rec[12]
                self.start_node_tx = link_rec[13]
                self.start_node_ty = link_rec[14]
                self.end_node_tx = link_rec[15]
                self.end_node_ty = link_rec[16]

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
    
    @staticmethod
    def getCost3(link_cost, road_type, toll, one_way, width, in_angle, out_angle):
        if in_angle is None:
            angle = 360
        else:
            angle = int(out_angle - in_angle)
            if angle <= 0:
                angle += 360
            if angle > 360:
                angle -= 360
        angle = int(angle / 5)
        cost = link_cost + CLinkCost.cost_turn[angle]
        return cost
    
class CHierarchy:
    def __init__(self):
        import common.log
        import common.rdb_database
        self.log = common.log.common_log.instance().logger('Region_Hierarchy')
        self.pg = common.rdb_database.rdb_pg()
        self.pg.connect2()
    
    def __del__(self):
        self.pg.close2()
    
    def make(self,
             org_link_table='rdb_link',
             org_link_filter='True',
             target_link_table='temp_hierarchy_trunk_links',
             base_level=10,
             max_level=14,
             mesh_road_limit=1000,
             tile_offset=0,
             overwrap=0
             ):
        self.org_link_table = org_link_table
        self.org_link_filter = org_link_filter
        self.target_link_table = target_link_table
        self.overwrap = overwrap
        
        self.log.info('making %s(base_level=%s,max_level=%s,overwrap=%s)...' 
                      % (target_link_table,str(base_level),str(max_level),str(overwrap)))
        self._splitDataIntoMeshes(base_level, max_level, mesh_road_limit, tile_offset)
        if overwrap:
            #self._searchImportantNodes()
            self._multiSearchImportantNodes()
        #self._searchTrunkInMeshes()
        self._multiSearchTrunkRoad()
        self._makeTargetTable()
        
    def _splitDataIntoMeshes(self, base_level, max_level, mesh_road_limit, tile_offset):
        self.log.info('split data into meshes...')
        
        # split into mesh, and auto resize the scope of mesh
        self.pg.CreateFunction2('rdb_locate_to_tile')
        self.pg.CreateFunction2('rdb_get_meshid_by_zlevel')
        
        #
        self.log.info('split data into meshes: dispatch nodes into base_level...')
        sqlcmd = """
                drop table if exists temp_hierarchy_mesh_nodes;
                create table temp_hierarchy_mesh_nodes
                as
                select  node_id,
                        tx as tx_16,
                        ty as ty_16,
                        link_num,
                        [base_level] as meshlevel,
                        rdb_get_meshid_by_zlevel(tx, ty, [base_level], [tile_offset]) as meshid
                from
                (
                    select  node_id,
                            (rdb_locate_to_tile(the_geom, 16)).*,
                            count(link_id) as link_num
                    from
                    (
                        select link_id, start_node_id as node_id, st_startpoint(the_geom) as the_geom
                        from [org_link_table]
                        where [org_link_filter]
                        union
                        select link_id, end_node_id, st_endpoint(the_geom) as the_geom
                        from [org_link_table]
                        where [org_link_filter]
                    )as t1
                    group by node_id, the_geom
                )as t2;
                
                create index temp_hierarchy_mesh_nodes_meshid_idx
                    on temp_hierarchy_mesh_nodes
                    using btree
                    (meshid);
                
                create index temp_hierarchy_mesh_nodes_node_id_idx
                    on temp_hierarchy_mesh_nodes
                    using btree
                    (node_id);
                
                analyze temp_hierarchy_mesh_nodes;
                """
        sqlcmd = sqlcmd.replace('[org_link_table]', self.org_link_table)
        sqlcmd = sqlcmd.replace('[org_link_filter]', self.org_link_filter)
        sqlcmd = sqlcmd.replace('[base_level]', str(base_level))
        sqlcmd = sqlcmd.replace('[tile_offset]', str(tile_offset))
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        #
        self.log.info('split data into meshes: dispatch nodes into max_level...')
        while 1:
            sqlcmd = """
                    drop table if exists temp_hierarchy_mesh_too_big;
                    create table temp_hierarchy_mesh_too_big
                    as
                    select meshlevel, meshid, count(*) as intersection_count
                    from
                    (
                        select meshlevel, meshid, node_id
                        from temp_hierarchy_mesh_nodes
                        where meshlevel < [max_level] and link_num > 2
                    )as t
                    group by meshlevel, meshid
                    having count(*) > [mesh_road_limit];
                    
                    create index temp_hierarchy_mesh_too_big_meshid_idx
                        on temp_hierarchy_mesh_too_big
                        using btree
                        (meshid);
                    
                    analyze temp_hierarchy_mesh_too_big;
                    """
            sqlcmd = sqlcmd.replace('[max_level]', str(max_level))
            sqlcmd = sqlcmd.replace('[mesh_road_limit]', str(mesh_road_limit))
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            sqlcmd = 'select count(*) from temp_hierarchy_mesh_too_big'
            big_mesh_count = self.pg.getOnlyQueryResult(sqlcmd)
            if big_mesh_count == 0:
                break
            else:
                sqlcmd = """
                        update temp_hierarchy_mesh_nodes as a
                        set meshlevel = a.meshlevel + 1,
                            meshid = rdb_get_meshid_by_zlevel(a.tx_16, a.ty_16, a.meshlevel + 1, [tile_offset])
                        from temp_hierarchy_mesh_too_big as b
                        where a.meshid = b.meshid;
                        
                        reindex index temp_hierarchy_mesh_nodes_meshid_idx;
                        """
                sqlcmd = sqlcmd.replace('[tile_offset]', str(tile_offset))
                self.pg.execute2(sqlcmd)
                self.pg.commit2()

        # mesh links
        self.log.info('split data into meshes: dispatch links into meshes...')
        sqlcmd = """
                drop table if exists temp_hierarchy_mesh_links;
                create table temp_hierarchy_mesh_links
                as
                select  link_id,
                        start_node_id,
                        end_node_id,
                        road_type,
                        link_type,
                        toll,
                        link_length,
                        one_way,
                        fazm,
                        tazm,
                        s.meshid,
                        s.tx_16 as start_node_tx_16,
                        s.ty_16 as start_node_ty_16,
                        e.tx_16 as end_node_tx_16,
                        e.ty_16 as end_node_ty_16,
                        a.ops_width,
                        a.neg_width
                from [org_link_table] as m
                left join temp_hierarchy_mesh_nodes as s
                on m.start_node_id = s.node_id
                left join temp_hierarchy_mesh_nodes as e
                on m.end_node_id = e.node_id
                left join rdb_linklane_info as a
                on m.lane_id = a.lane_id
                where [org_link_filter];
                
                create index temp_hierarchy_mesh_links_meshid_idx
                    on temp_hierarchy_mesh_links
                    using btree
                    (meshid);
                
                create index temp_hierarchy_mesh_links_start_node_tx_ty_idx
                    on temp_hierarchy_mesh_links
                    using btree
                    (start_node_tx_16, start_node_ty_16);
                
                create index temp_hierarchy_mesh_links_end_node_tx_ty_idx
                    on temp_hierarchy_mesh_links
                    using btree
                    (end_node_tx_16, end_node_ty_16);
                
                analyze temp_hierarchy_mesh_links;
                """
        sqlcmd = sqlcmd.replace('[org_link_table]', self.org_link_table)
        sqlcmd = sqlcmd.replace('[org_link_filter]', self.org_link_filter)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        self.pg.CreateIndex2('temp_hierarchy_mesh_links_link_id_idx')
        self.pg.CreateIndex2('temp_hierarchy_mesh_links_start_node_id_idx')
        self.pg.CreateIndex2('temp_hierarchy_mesh_links_end_node_id_idx')
        
        # find boundary nodes
        self.log.info('split data into meshes: find boundary nodes...')
        sqlcmd = """
                drop table if exists temp_hierarchy_mesh_boundary_nodes;
                create table temp_hierarchy_mesh_boundary_nodes
                as
                select node_id, unnest(array_agg(meshid)) as meshid
                from
                (
                    select distinct a.node_id, b.meshid
                    from temp_hierarchy_mesh_nodes as a
                    inner join temp_hierarchy_mesh_links as b
                    on a.node_id in (b.start_node_id, b.end_node_id)
                )as t
                group by node_id
                having count(*) > 1;
                
                create index temp_hierarchy_mesh_boundary_nodes_meshid_idx
                    on temp_hierarchy_mesh_boundary_nodes
                    using btree
                    (meshid);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # test view
        if False:
            sqlcmd = """
                    drop table if exists temp_hierarchy_mesh_links_view;
                    create table temp_hierarchy_mesh_links_view
                    as
                    select a.*, b.the_geom
                    from temp_hierarchy_mesh_links as a
                    left join rdb_link as b
                    on a.link_id = b.link_id;
                    
                    alter table temp_hierarchy_mesh_links_view add column gid serial primary key;
                    CREATE INDEX temp_hierarchy_mesh_links_view_meshid_idx
                        on temp_hierarchy_mesh_links_view
                        using btree
                        (meshid);
                    CREATE INDEX temp_hierarchy_mesh_links_view_the_geom_idx
                        on temp_hierarchy_mesh_links_view
                        using gist
                        (the_geom);
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            sqlcmd = """
                    drop table if exists temp_hierarchy_mesh_boundary_nodes_view;
                    create table temp_hierarchy_mesh_boundary_nodes_view
                    as
                    select a.*, b.the_geom
                    from temp_hierarchy_mesh_boundary_nodes as a
                    left join rdb_node as b
                    on a.node_id = b.node_id;;
  
                    alter table temp_hierarchy_mesh_boundary_nodes_view add column gid serial primary key;
                    CREATE INDEX temp_hierarchy_mesh_boundary_nodes_view_meshid_idx
                        on temp_hierarchy_mesh_boundary_nodes_view
                        using btree
                        (meshid);
                    CREATE INDEX temp_hierarchy_mesh_boundary_nodes_view_the_geom_idx
                        on temp_hierarchy_mesh_boundary_nodes_view
                        using gist
                        (the_geom);
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        
        if True:
            sqlcmd = """
                    select count(distinct(meshid))
                    from temp_hierarchy_mesh_links;
                    """
            mesh_count = self.pg.getOnlyQueryResult(sqlcmd)
            self.log.info('there are %s meshes...' % (str(mesh_count)))
    
    def _searchImportantNodes(self):
        self.log.info('search important links in every mesh...')
        self.pg.CreateTable2('temp_hierarchy_important_links')

        mesh_list = self._getMeshList()
        while not mesh_list.empty():
            (meshid, nIndex, nCount) = mesh_list.get(timeout=1.0)
            print (meshid, nIndex, nCount)
            self._searchTrunkRoadInMesh(meshid, nIndex, nCount, 
                                        rdb_link='temp_hierarchy_mesh_links',
                                        rdb_node_boundary='temp_hierarchy_mesh_boundary_nodes',
                                        use_mesh_overwrap=self.overwrap, 
                                        target_table='temp_hierarchy_important_links')
        
        self.pg.CreateIndex2('temp_hierarchy_important_links_link_id_idx')
        
        # test view
        if False:
            sqlcmd = """
                    drop table if exists temp_hierarchy_important_links_view;
                    create table temp_hierarchy_important_links_view
                    as
                    select a.*, b.the_geom
                    from temp_hierarchy_important_links as a
                    left join rdb_link as b
                    on a.link_id = b.link_id;
                    
                    alter table temp_hierarchy_important_links_view add column gid serial primary key;
                    CREATE INDEX temp_hierarchy_important_links_view_meshid_idx
                        on temp_hierarchy_important_links_view
                        using btree
                        (meshid);
                    CREATE INDEX temp_hierarchy_important_links_view_the_geom_idx
                        on temp_hierarchy_important_links_view
                        using gist
                        (the_geom);
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
        self._searchImportantMeshBoundaryNodes()
    
    def _multiSearchImportantNodes(self):
        self.log.info('search important links in every mesh...')
        self.pg.CreateTable2('temp_hierarchy_important_links')
        
        exception_event = multiprocessing.Value('b', False)
        mesh_queue = self._getMeshList()
        thread_count = int(common.rdb_common.GetPath('thread_count'))
        thread_list = []
        for i in range(thread_count):
            objThread = CProcess_SearchTrunkRoadInMesh(mesh_queue,
                                                       exception_event,
                                                       rdb_link='temp_hierarchy_mesh_links',
                                                       rdb_node_boundary='temp_hierarchy_mesh_boundary_nodes',
                                                       use_mesh_overwrap=self.overwrap,
                                                       target_table='temp_hierarchy_important_links',
                                                       log=None)
            objThread.daemon = True
            thread_list.append(objThread)
        
        for objThread in thread_list:
            objThread.start()
        
        for objThread in thread_list:
            objThread.join()
        
        if exception_event.value:
            raise Exception, 'error happened while processing meshes'
        
        self.pg.CreateIndex2('temp_hierarchy_important_links_link_id_idx')
        self._searchImportantMeshBoundaryNodes()
    
    def _searchImportantMeshBoundaryNodes(self):
        # find important boundary nodes
        self.log.info('search important boundary nodes...')
        sqlcmd = """
                drop table if exists temp_hierarchy_mesh_boundary_nodes;
                create table temp_hierarchy_mesh_boundary_nodes
                as
                select a.node_id, a.meshid
                from
                (
                    select node_id, unnest(array_agg(meshid)) as meshid
                    from
                    (
                        select distinct a.node_id, b.meshid
                        from temp_hierarchy_mesh_nodes as a
                        inner join temp_hierarchy_mesh_links as b
                        on a.node_id in (b.start_node_id, b.end_node_id)
                    )as t
                    group by node_id
                    having count(*) > 1
                )as a
                inner join
                (
                    select unnest(ARRAY[b.start_node_id,b.end_node_id]) as node_id
                    from temp_hierarchy_important_links as a
                    inner join temp_hierarchy_mesh_links as b
                    on a.link_id = b.link_id and a.meshid = b.meshid
                )as b
                on a.node_id = b.node_id;
                
                create index temp_hierarchy_mesh_boundary_nodes_meshid_idx
                    on temp_hierarchy_mesh_boundary_nodes
                    using btree
                    (meshid);
                """
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
        
        # test view
        if False:
            sqlcmd = """
                    drop table if exists temp_hierarchy_important_links_view;
                    create table temp_hierarchy_important_links_view
                    as
                    select a.*, b.the_geom
                    from temp_hierarchy_important_links as a
                    left join rdb_link as b
                    on a.link_id = b.link_id;
                    
                    alter table temp_hierarchy_important_links_view add column gid serial primary key;
                    CREATE INDEX temp_hierarchy_important_links_view_meshid_idx
                        on temp_hierarchy_important_links_view
                        using btree
                        (meshid);
                    CREATE INDEX temp_hierarchy_important_links_view_the_geom_idx
                        on temp_hierarchy_important_links_view
                        using gist
                        (the_geom);
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
            
            sqlcmd = """
                    drop table if exists temp_hierarchy_mesh_important_nodes_view;
                    create table temp_hierarchy_mesh_important_nodes_view
                    as
                    select a.*, b.the_geom
                    from temp_hierarchy_mesh_boundary_nodes as a
                    left join rdb_node as b
                    on a.node_id = b.node_id;
  
                    alter table temp_hierarchy_mesh_important_nodes_view add column gid serial primary key;
                    CREATE INDEX temp_hierarchy_mesh_important_nodes_view_meshid_idx
                        on temp_hierarchy_mesh_important_nodes_view
                        using btree
                        (meshid);
                    CREATE INDEX temp_hierarchy_mesh_important_nodes_view_the_geom_idx
                        on temp_hierarchy_mesh_important_nodes_view
                        using gist
                        (the_geom);
                    """
            self.pg.execute2(sqlcmd)
            self.pg.commit2()
    
    def _searchTrunkRoad(self):
        self.log.info('search trunk road in every mesh...')
        self.pg.CreateTable2('temp_hierarchy_trunk_links')
        
        mesh_list = self._getMeshList()
        while not mesh_list.empty():
            (meshid, nIndex, nCount) = mesh_list.get(timeout=1.0)
            self._searchTrunkRoadInMesh(meshid, nIndex, nCount, 
                                        rdb_link='temp_hierarchy_mesh_links',
                                        rdb_node_boundary='temp_hierarchy_mesh_boundary_nodes',
                                        use_mesh_overwrap=0, 
                                        target_table='temp_hierarchy_trunk_links')
        
        self.pg.CreateIndex2('temp_hierarchy_trunk_links_link_id_idx')
    
    def _multiSearchTrunkRoad(self):
        self.log.info('search trunk road in every mesh...')
        self.pg.CreateTable2('temp_hierarchy_trunk_links')
        
        exception_event = multiprocessing.Value('b', False)
        mesh_queue = self._getMeshList()
        thread_count = int(common.rdb_common.GetPath('thread_count'))
        thread_list = []
        for i in range(thread_count):
            objThread = CProcess_SearchTrunkRoadInMesh(mesh_queue,
                                                       exception_event,
                                                       rdb_link='temp_hierarchy_mesh_links',
                                                       rdb_node_boundary='temp_hierarchy_mesh_boundary_nodes',
                                                       use_mesh_overwrap=0,
                                                       target_table='temp_hierarchy_trunk_links',
                                                       log=None)
            objThread.daemon = True
            thread_list.append(objThread)
        
        for objThread in thread_list:
            objThread.start()
        
        for objThread in thread_list:
            objThread.join()
        
        if exception_event.value:
            raise Exception, 'error happened while processing meshes'
        
        self.pg.CreateIndex2('temp_hierarchy_trunk_links_link_id_idx')
    
    def _makeTargetTable(self):
        self.log.info('make target table of trunk links...')
        sqlcmd = """
                drop table if exists [rdb_region_links];
                create table [rdb_region_links]
                as
                select b.*
                from temp_hierarchy_trunk_links as a
                left join rdb_link as b
                on a.link_id = b.link_id;
                
                create index [rdb_region_links]_link_id_idx
                    on [rdb_region_links]
                    using btree
                    (link_id);
                
                create index [rdb_region_links]_start_node_id_idx
                    on [rdb_region_links]
                    using btree
                    (start_node_id);
                
                create index [rdb_region_links]_end_node_id_idx
                    on [rdb_region_links]
                    using btree
                    (end_node_id);
                """
        sqlcmd = sqlcmd.replace('[rdb_region_links]', self.target_link_table)
        self.pg.execute2(sqlcmd)
        self.pg.commit2()
    
    def _getMeshList(self):
        #import Queue
        #meshes = Queue.Queue()
        #meshes.put((51753322372, 1, 1))
        #meshes.put((43002626504, 1, 1))
        #return meshes
        meshes = multiprocessing.Queue()
        sqlcmd = """
                select distinct meshid
                from temp_hierarchy_mesh_links
                order by meshid
                """
        self.pg.execute2(sqlcmd)
        rows = self.pg.fetchall2()
        nCount = len(rows)
        for nIndex in xrange(nCount):
            meshid = rows[nIndex][0]
            meshes.put((meshid, nIndex+1, nCount))
        
        return meshes
    
    def _searchTrunkRoadInMesh(self, 
                               meshid, nIndex, nCount, 
                               rdb_link='temp_hierarchy_mesh_links', 
                               rdb_node_boundary='temp_hierarchy_mesh_boundary_nodes',
                               use_mesh_overwrap=0,
                               target_table='temp_hierarchy_trunk_links'):
        self.log.info('processing mesh %s (%s/%s)...' % (str(meshid), str(nIndex), str(nCount)))
        graph = CGraph_Memory()
        graph.prepareData(meshid, 
                          rdb_link, 
                          rdb_node_boundary,
                          use_mesh_overwrap)
        links = graph.searchTrunkLinksUsingSimplifing(log=self.log)
        
        import common.cache_file
        temp_file = common.cache_file.open('%s_%s' % (target_table,str(meshid)))
        for linkid in links:
            temp_file.write('%s\t%s\n' % (str(meshid), str(linkid)))
        graph.clearData()
        temp_file.seek(0)
        self.pg.copy_from2(temp_file, target_table)
        self.pg.commit2()
        common.cache_file.close(temp_file,True)

class CProcess_SearchTrunkRoadInMesh(multiprocessing.Process):
    def __init__(self, 
                 mesh_queue, 
                 exp_event, 
                 rdb_link='temp_hierarchy_mesh_links', 
                 rdb_node_boundary='temp_hierarchy_mesh_boundary_nodes',
                 use_mesh_overwrap=0,
                 target_table='temp_hierarchy_important_links', 
                 log=None):
        multiprocessing.Process.__init__(self)
        self.mesh_queue = mesh_queue
        self.exp_event = exp_event
        self.rdb_link = rdb_link
        self.rdb_node_boundary = rdb_node_boundary
        self.use_mesh_overwrap = use_mesh_overwrap
        self.target_table = target_table
        self.log = log
        self.daemon = True
    
    def run(self):
        try:
            try:
                import common.rdb_database
                import common.log
                self.pg = common.rdb_database.rdb_pg()
                self.pg.connect2()
                #log = common.log.common_log.instance().logger('Region_Hierarchy')
                #log.info('processing mesh %s (%s/%s)...' % (str(meshid), str(nIndex), str(nCount)))
                while 1:
                    self._handleOneMesh()
            finally:
                self.pg.close2()
        except Exception, multiprocessing.Queue.Empty:
            pass
        except:
            self.exp_event.value = True
            raise
    
    def _handleOneMesh(self):
        meshid, nIndex, nCount = self.mesh_queue.get(timeout=1.0)
        print 'Thread-%d is processing mesh %d (%d/%d)...' % (self.pid, meshid, nIndex, nCount)
        if self.log:
            self.log.info('Thread-%d is processing mesh %d (%d/%d)...' % (self.pid, meshid, nIndex, nCount))
        sys.stdout.flush()
        graph = CGraph_Memory()
        graph.prepareData(meshid,
                          rdb_link=self.rdb_link, 
                          rdb_node_boundary=self.rdb_node_boundary,
                          use_mesh_overwrap=self.use_mesh_overwrap)
        links = graph.searchTrunkLinksUsingSimplifing(log=None)
        
        import common.cache_file
        temp_file = common.cache_file.open('%s_%s' % (self.target_table,str(meshid)))
        for linkid in links:
            temp_file.write('%s\t%s\n' % (str(meshid), str(linkid)))
        graph.clearData()
        temp_file.seek(0)
        self.pg.copy_from2(temp_file, self.target_table)
        self.pg.commit2()
        common.cache_file.close(temp_file,True)
    
    