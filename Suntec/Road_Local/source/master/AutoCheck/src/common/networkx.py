# -*- coding: UTF8 -*-
'''
Created on 2015-10-15

@author: liushengqiang
'''



import os
import copy
import heapq
import threading

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

class CGraph_PG(CGraph):
    def __init__(self):
        import common.database
        self.__pg = common.database.CDB()
        self.__pg.connect()
    
    def __del__(self):
        self.__pg.close()
    
    def prepareData(self):
        pass
    
    def clearData(self):
        pass
    
    def _getLinkInfo(self, link_id, link_tbl = 'rdb_link', link_laneinfo_tbl = 'rdb_linklane_info'):
        # this function returns (adjlinkid, adjcost, adjnode)
        sqlcmd = """
                SELECT link_id, road_type, toll, link_length, one_way, 
                    start_node_id, end_node_id, fazm_path, tazm_path, 
                    ops_width, neg_width
                FROM %s a
                LEFT JOIN %s b
                    ON a.lane_id = b.lane_id
                WHERE b.lane_id IS NOT NULL and link_id = %s
            """ % (str(link_tbl), str(link_laneinfo_tbl), str(link_id))

        self.__pg.execute(sqlcmd)
        link_rec = self.__pg.fetchone()
        return CLink(link_rec)
    
    def _getAdjLinkList(self, node_id, link_tbl = 'rdb_link', link_laneinfo_tbl = 'rdb_linklane_info', delete_road_type = '(7, 8, 9, 14)'):
        # this function returns (adjlinkid, adjcost, adjnode)
        sqlcmd = """
                SELECT link_id, road_type, toll, link_length, one_way, 
                    start_node_id, end_node_id, fazm_path, tazm_path, 
                    ops_width, neg_width
                FROM %s a
                LEFT JOIN %s b
                    ON a.lane_id = b.lane_id
                WHERE b.lane_id IS NOT NULL and %s in (start_node_id, end_node_id) and road_type not in %s
            """ % (str(link_tbl), str(link_laneinfo_tbl), str(node_id), str(delete_road_type))

        self.__pg.execute(sqlcmd)
        link_recs = self.__pg.fetchall()
        adjlinks = []
        for link_rec in link_recs:
            adjlinks.append(CLink(link_rec))
        return adjlinks
    
    def _get_inlink(self, node_id, link_tbl = 'rdb_link'):
        
        sqlcmd = """
                SELECT link_id
                FROM %s
                WHERE (start_node_id = %s and one_way in (1, 3)) or (end_node_id = %s and one_way in (1, 2))
            """ % (str(link_tbl), str(node_id), str(node_id))

        return self.__pg.get_batch_data(sqlcmd)
    
    def _get_outlink(self, node_id, link_tbl = 'rdb_link'):
        
        sqlcmd = """
                SELECT link_id
                FROM %s
                WHERE (start_node_id = %s and one_way in (1, 2)) or (end_node_id = %s and one_way in (1, 3))
            """ % (str(link_tbl), str(node_id), str(node_id))
        
        return self.__pg.get_batch_data(sqlcmd)

class CGraph_Memory(CGraph):
    
    def __init__(self, link_tbl = 'rdb_link', link_laneinfo_tbl = 'rdb_linklane_info', delete_road_type = '(7, 8, 9, 14)'):
        
        import common.database
        self.pg = common.database.CDB()
        self.pg.connect()
        
        self.links = {}
        self.nodes = {}
        self._prepareData(link_tbl, link_laneinfo_tbl, delete_road_type)
    
    def __del__(self):
        
        import gc
        del self.links
        del self.nodes
        self.pg.close()
        gc.collect()
        
    def _prepareData(self, link_tbl = 'rdb_link', link_laneinfo_tbl = 'rdb_linklane_info', delete_road_type = '(7, 8, 9, 14)'):
        
        # init links & nodes
        sqlcmd = """
                SELECT  a.link_id,
                        a.road_type,
                        a.toll,
                        a.link_length,
                        a.one_way,
                        a.start_node_id,
                        a.end_node_id,
                        a.fazm_path as fazm,
                        a.tazm_path as tazm,
                        b.ops_width,
                        b.neg_width
                FROM %s a
                LEFT JOIN %s b
                    ON a.lane_id = b.lane_id
                WHERE b.lane_id IS NOT NULL and road_type not in %s
                """ % (str(link_tbl), str(link_laneinfo_tbl), str(delete_road_type))
        
        link_recs = self.pg.get_batch_data(sqlcmd)
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
    
    def _getAdjLinkList(self, node_id):
        adjlinks = self.nodes.get(node_id)
        if not adjlinks:
            adjlinks = []
        return adjlinks
    
    def searchShortestPaths(self, from_node, to_node, distance = 10000.0, cost_scope=1.05):
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
                if cur_cost > arrive_nodes[cur_node] * cost_scope:
                    continue
            else:
                arrive_nodes[cur_node] = cur_cost
            
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
            
    def searchDijkstraPaths(self, from_node, to_node, max_buffer=5000, max_path_num=6, cost_scope=1.05):
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
                adj_cost = CLinkCost.getCost(adjlink.link_length, adjlink.road_type, 
                                             adjlink.toll, adjlink.one_way, 
                                             next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost
                #print 'touch', adjnode, adjcost
                touch_nodes.push((next_cost, next_node, next_in_angle, cur_node, adjlink.link_id, next_path))
        
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
                SELECT b.link_id, b.road_type, b.toll, b.link_length, b.one_way, 
                    b.start_node_id, b.end_node_id, b.fazm_path as fazm, b.tazm_path as tazm, 
                    c.ops_width, c.neg_width, 
                    a.node_id, b.link_id_t, d.node_id_t as start_node_id_t, e.node_id_t as end_node_id_t 
                FROM (
                    SELECT node_id, unnest(branches) as link_id
                    FROM [node_tbl]
                    WHERE node_id_t = [node_id_t]
                ) a
                LEFT JOIN [link_tbl] b
                    ON a.link_id = b.link_id
                LEFT JOIN [link_laneinfo_tbl] c
                    ON b.lane_id = c.lane_id
                LEFT JOIN [node_tbl] d
                    ON b.start_node_id = d.node_id
                LEFT JOIN [node_tbl] e
                    ON b.end_node_id = e.node_id
                WHERE b.road_type not in [road_type]
            """
        
        sqlcmd = sqlcmd.replace('[node_tbl]', str(node_tbl))
        sqlcmd = sqlcmd.replace('[node_id_t]', str(tileid))
        sqlcmd = sqlcmd.replace('[link_tbl]', str(link_tbl))
        sqlcmd = sqlcmd.replace('[link_laneinfo_tbl]', str(link_laneinfo_tbl))
        sqlcmd = sqlcmd.replace('[road_type]', str(delete_road_type))
        
        link_recs = self.pg.get_batch_data(sqlcmd)
        for link_rec in link_recs:
            objLink = CLink(link_rec)
            
            if not nodes.has_key(objLink.node_id):
                nodes[objLink.node_id] = [objLink]
            else:
                nodes[objLink.node_id].append(objLink)
                
        return nodes
    
    def _getAdjLinkList(self, 
                        node_id, 
                        node_id_t, 
                        link_tbl = 'rdb_link', 
                        node_tbl = 'rdb_node', 
                        link_laneinfo_tbl = 'rdb_linklane_info', 
                        delete_road_type = '(7, 8, 9, 14)'):
        if node_id_t == self.cur_tile:
            pass
        else:
            self.mutex.acquire()
            self.cur_tile = node_id_t
            self.cur_tile_nodes = self.datacache.get(node_id_t)
            if not self.cur_tile_nodes:
                self.cur_tile_nodes = self._get_tile_data(node_id_t, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
                self.datacache.set(node_id_t, self.cur_tile_nodes)
            self.mutex.release()
            
        adjlinks = self.cur_tile_nodes.get(node_id)
        if not adjlinks:
            adjlinks = []
        return adjlinks

    def searchShortestPaths(self, 
                            from_node, 
                            from_node_t, 
                            to_node, 
                            to_node_t, 
                            distance = 10000.0,
                            link_tbl = 'rdb_link', 
                            node_tbl = 'rdb_node',
                            link_laneinfo_tbl = 'rdb_linklane_info', 
                            delete_road_type = '(7, 8, 9, 14)',  
                            cost_scope=1.05):
        # stack init
        paths = []                  # efftive path [(cur_distance, path),]
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_distance, cur_node, cur_node_t, in_angle, last_node, last_link, path)
        
        # search init
        max_distance = distance * 2.0
        touch_nodes.push((0, 0, from_node, from_node_t, None, None, 0, []))
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        while not touch_nodes.isempty():
            (cur_cost, cur_distance, cur_node, cur_node_t, in_angle, last_node, last_link, path) = touch_nodes.pop()
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
            
            adjlink_list = self._getAdjLinkList(cur_node, cur_node_t, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
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
                    next_node_t = adjlink.end_node_id_t
                    cur_out_angle = adjlink.fazm
                    next_in_angle = adjlink.tazm
                    next_witdh = adjlink.ops_width
                else:
                    next_node = adjlink.start_node_id
                    next_node_t = adjlink.start_node_id_t
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
                    touch_nodes.push((next_cost, next_distance, next_node, next_node_t, next_in_angle, cur_node, adjlink.link_id, next_path))
        else:
            return paths
    
    def searchShortestPaths2(self,  
                             from_node, from_node_t, to_node, to_node_t, forward=True,
                             distance = 10000.0, get_link_cost='CLinkCost.getCost2', 
                             link_tbl = 'rdb_link', node_tbl = 'rdb_node', link_laneinfo_tbl = 'rdb_linklane_info',  
                             delete_road_type = '(7, 8, 9, 14)'):
        
        paths = []                 
        arrive_nodes = {}          
        touch_nodes = CHeap()
        
        max_distance = distance * 2.0
        touch_nodes.push((0, from_node, from_node_t, None, None, 0, []))
        
        while not touch_nodes.isempty():
            (cur_cost, cur_node, cur_node_t, in_angle, last_node, last_link, path) = touch_nodes.pop()
            
            if (to_node == cur_node):
                paths.append((cur_cost, path))
                break

            if arrive_nodes.has_key(cur_node):
                continue
            else:
                arrive_nodes[cur_node] = True
            
            adjlink_list = self._getAdjLinkList(cur_node, cur_node_t, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                if adjlink.link_id == last_link:
                    continue
                
                # judge next node walked or not
                if adjlink.start_node_id == cur_node:
                    next_node = adjlink.end_node_id
                    next_node_t = adjlink.end_node_id_t
                    cur_out_angle = adjlink.fazm
                    next_in_angle = adjlink.tazm
                    next_witdh = adjlink.ops_width
                else:
                    next_node = adjlink.start_node_id
                    next_node_t = adjlink.start_node_id_t
                    cur_out_angle = adjlink.tazm
                    next_in_angle = adjlink.fazm
                    next_witdh = adjlink.neg_width

                next_path = copy.copy(path)
                next_path.append(adjlink.link_id)
                adj_cost = get_link_cost(adjlink.link_length, adjlink.road_type, adjlink.toll, adjlink.one_way, next_witdh, in_angle, cur_out_angle)
                next_cost = cur_cost + adj_cost

                if next_cost < max_distance:
                    touch_nodes.push((next_cost, next_node, next_node_t, next_in_angle, cur_node, adjlink.link_id, next_path))
        return paths
    
    def searchDijkstraPaths(self,
                            from_node, from_node_t, 
                            to_node, to_node_t, forward=True,
                            #is_legal_path='self._is_legal_path',
                            max_buffer=5000, max_path_num=2,
                            cost_scope=1.05, max_cost=None,
                            get_link_cost='CLinkCost.getCost',
                            log=None,
                            link_tbl = 'rdb_link', 
                            node_tbl = 'rdb_node', 
                            link_laneinfo_tbl = 'rdb_linklane_info', 
                            delete_road_type = '(7, 8, 9, 14)'):
        # stack init
        paths = []                  # efftive path [path,]
        arrive_nodes = {}           # arrive nodes
        touch_nodes = CHeap()       # touch_nodes, (cur_cost, cur_node, cur_node_t, in_angle, last_node, last_link, path)
        
        # search init
        touch_nodes.push((0, from_node, from_node_t, None, None, 0, []))
        
        # search
        #print 'search dock to region at node_id = %s...' % str(node_id)
        while not touch_nodes.isempty():
            #print 'from_node = %s, to_node = %s' % (str(from_node), str(to_node))
            #print 'touch_nodes.length() =', touch_nodes.length()
            if touch_nodes.length() > max_buffer:
                break
            
            (cur_cost, cur_node, cur_node_t, in_angle, last_node, last_link, path) = touch_nodes.pop()
            #print cur_node, last_link, path
            
            if (to_node == cur_node):
                paths.append((cur_cost, path))
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
            
            adjlink_list = self._getAdjLinkList(cur_node, cur_node_t, link_tbl, node_tbl, link_laneinfo_tbl, delete_road_type)
            adjlink_list = self._traffic_flow_filter(cur_node, adjlink_list, forward)
            for adjlink in adjlink_list:
                # not one-link-uturn
                if adjlink.link_id == last_link:
                    continue
                
                # check if walk this link
                #if not is_legal_path(adjlink):
                #    continue
                
                # judge next node walked or not
                if adjlink.start_node_id == cur_node:
                    next_node = adjlink.end_node_id
                    next_node_t = adjlink.end_node_id_t
                    cur_out_angle = adjlink.fazm
                    next_in_angle = adjlink.tazm
                    next_witdh = adjlink.ops_width
                else:
                    next_node = adjlink.start_node_id
                    next_node_t = adjlink.start_node_id_t
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
                    touch_nodes.push((next_cost, next_node, next_node_t, next_in_angle, cur_node, adjlink.link_id, next_path))
        
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
        self.link_id_t = link_rec[12]
        self.start_node_id_t = link_rec[13]
        self.end_node_id_t = link_rec[14]

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