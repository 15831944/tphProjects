#encoding=utf-8
import sys
import heapq
import networkx
from LatLonPoint import LatLonPoint
from RdbLinkObject import RdbLinkObject
from RdbLinkRegulationObject import RdbLinkRegulationObject

class RdbDiGraph(networkx.DiGraph):
    def __init__(self, _pg):
        networkx.DiGraph.__init__(self)
        self.pg = _pg
        self.loadedTile = []
        # {(fromLinkID,toLinkID):{'attr1':value1, 'attr2':value2, ...}}
        self.regulationDict = {}
        self.loadedTileRegulation = []
        return

    # 根据起点和终点的经纬度将需要的link加载到内存中。
    def addEdgeByLatLonPoints(self, errMsg, latLonPoint1, latLonPoint2):
        tileIDList = LatLonPoint.getTileIdList(errMsg, latLonPoint1, latLonPoint2, offset=3)
        self.addEdgeByTileIDList(errMsg, tileIDList)
        return
    
    # 从rdb_link表中搜索此tile所包含的link，并添加到图中。
    def addEdgeByTileID(self, errMsg, tileID, link_tbl_name='rdb_link'):
        # 此tile已加入图中，无需再次加载
        if tileID in self.loadedTile:
            return
        else:
            self.loadedTile.append(tileID)

        sqlcmd = """
select  link_id, road_type, toll, link_length, one_way, 
        start_node_id, end_node_id, fazm, tazm
from %s 
where link_id_t in (%s)
""" % (link_tbl_name, tileID)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            rdbLinkObj = RdbLinkObject()
            rdbLinkObj.initFromRecord_11(errMsg, row)
            if errMsg[0] <> '':
                return
            self.addEdgeByRdbLinkObj(rdbLinkObj)
        return
    
    # 从rdb_link表中搜索此组tile所包含的link，并添加到图中。
    def addEdgeByTileIDList(self, errMsg, tileIDList, link_tbl_name='rdb_link'):
        # 过滤已加载的tile
        tileIDListToAdd = list(set(tileIDList).difference(set(self.loadedTile)))
        if len(tileIDListToAdd) == 0:
            return
        else:
            self.loadedTile.extend(tileIDListToAdd)

        strTileIDList = ','.join(("%d"%x) for x in tileIDListToAdd)
        sqlcmd = """
select  link_id, road_type, toll, link_length, one_way, 
        start_node_id, end_node_id, end_node_id, end_node_id
from %s 
where link_id_t in (%s)
""" % (link_tbl_name, strTileIDList)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            rdbLinkObj = RdbLinkObject()
            rdbLinkObj.initFromRecord_11(errMsg, row)
            if errMsg[0] <> '':
                return
            
            self.addEdgeByRdbLinkObj(rdbLinkObj)
        return

    # 将某张表的数据全部load至内存中
    # 此表需要与rdb_link表具有相同字段。
    def addEdgeFromTable(self, errMsg, link_tbl_name='rdb_region_link_layer6_tbl'):
        sqlcmd = """select array_agg(distinct link_id_t) from %s;"""
        self.pg.execute(sqlcmd % link_tbl_name)
        allTileList = self.pg.fetchone()[0]
        tileIDListToAdd = list(set(allTileList).difference(self.loadedTile))
        if len(tileIDListToAdd) == 0:
            return
        else:
            self.loadedTile.extend(tileIDListToAdd)

        strTileIDList = ','.join(("%d"%x) for x in tileIDListToAdd)
        sqlcmd = """
select  link_id, road_type, toll, link_length, one_way, 
        start_node_id, end_node_id, end_node_id, end_node_id
from %s 
where link_id_t in (%s)
""" % (link_tbl_name, strTileIDList)
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            rdbLinkObj = RdbLinkObject()
            rdbLinkObj.initFromRecord_11(errMsg, row)
            if errMsg[0] <> '':
                return
            
            self.addEdgeByRdbLinkObj(rdbLinkObj)
        return

    # 把此Rdb link对象添加到图中。
    def addEdgeByRdbLinkObj(self, rdbLinkObj):
        if rdbLinkObj.one_way == 0:
            return
        elif rdbLinkObj.one_way == 1:
            self.add_edge(rdbLinkObj.start_node_id, rdbLinkObj.end_node_id, rdbLinkObj.getArgDist_11())
            self.add_edge(rdbLinkObj.end_node_id, rdbLinkObj.start_node_id, rdbLinkObj.getArgDist_11())
        elif rdbLinkObj.one_way == 2:
            self.add_edge(rdbLinkObj.start_node_id, rdbLinkObj.end_node_id, rdbLinkObj.getArgDist_11())
        elif rdbLinkObj.one_way == 3:
            self.add_edge(rdbLinkObj.end_node_id, rdbLinkObj.start_node_id, rdbLinkObj.getArgDist_11())
        else:
            return

    # 根据起点和终点的经纬度将需要的规制信息加载到内存中。
    def addRegulationByLatLonPoints(self, errMsg, latLonPoint1, latLonPoint2):
        tileIDList = LatLonPoint.getTileIdList(errMsg, latLonPoint1, latLonPoint2, offset=3)
        self.loadLinkFromTileIDList(tileIDList)
        return

    # 从rdb_link_regulation表中load此tile所包含的规制。
    def addRegulationByTileID(self, errMsg, tileID):
        # 此tile已加入图中，无需再次加载
        if tileID in self.loadedTileRegulation:
            return
        self.loadedTileRegulation.append(tileID)
        rows = RdbLinkRegulationObject.getRegulationRecordByTileID(errMsg, self.pg, tileID)
        if errMsg[0] <> '':
            return
        
        for row in rows:
            regulationObj = RdbLinkRegulationObject()
            regulationObj.initFromRecord_11(errMsg, row)
            self.addRegulation(regulationObj)
        return
      
    # 从rdb_link表中load此组tile所包含的规制。
    def addRegulationByTileIDList(self, errMsg, tileIDList):
        # 过滤已加载的tile
        tileIDListToAdd = list(set(tileIDList).difference(set(self.loadedTile)))
        self.loadedTileRegulation.extend(tileIDListToAdd)
        
        rows = RdbLinkRegulationObject.getRegulationRecordByTileIDList(errMsg, self.pg, tileIDList)
        for row in rows:
            regulationObj = RdbLinkRegulationObject()
            regulationObj.initFromRecord_11(errMsg, row)
            self.addRegulation(regulationObj)
        return  
    
    def addRegulationByObject(self, regulationObj):
        linkList = regulationObj.getLinkList()
        linkCount = len(linkList)
        for i in range(0, linkCount):
            self.addRegulation((linkList[i], linkList[i+1], regulationObj.getArgDist_14()))
        return
    
    def addRegulation(self, linkPair, attr_dict):
        if linkPair not in self.regulationDict:
            self.regulationDict[linkPair]={}
            
        datadict=self.regulationDict.get(linkPair,{})
        RdbLinkRegulationObject.mergeRegulationDictionary(datadict, attr_dict)
        self.regulationDict[linkPair]=datadict
    
    # 根据规制表信息判断是否可通行
    def isRightOfWay(self, linkPair, nowTime):
        if self.regulationDict.has_key(linkPair) == True:
            return False
        return True
    
    # 算路
    @staticmethod
    def bidirectional_dijkstra(G, source, target, weight = 'weight'):
        if source == target: 
            return (0, [source])
        
        if G.is_multigraph():
            raise Exception("The input Graph must be a DiGraph which is not a multigraph.")
            return None
        
        if G.is_directed() == False:
            raise Exception("The input Graph must be a DiGraph.")
            return None
            
        
        #Init:   Forward             Backward
        dists =  [{},                {}]# dictionary of final distances
        paths =  [{source:[source]}, {target:[target]}] # dictionary of paths
        fringe = [[],                []] #heap of (distance, node) tuples for extracting next node to expand
        seen =   [{source:0},        {target:0} ]#dictionary of distances to nodes seen
        #initialize fringe heap
        heapq.heappush(fringe[0], (0, source))
        heapq.heappush(fringe[1], (0, target))
        #neighs for extracting correct neighbor information
        neighs = [G.successors_iter, G.predecessors_iter]
        #variables to hold shortest discovered path
        finaldist = 1e30000
        finalpath = []
        dir = 1
        while fringe[0] and fringe[1]:
            # choose direction
            # dir == 0 is forward direction and dir == 1 is back
            dir = 1-dir
            # extract closest to expand
            (dist, v)= heapq.heappop(fringe[dir])
            if v in dists[dir]:
                # Shortest path to v has already been found
                continue
            # update distance
            dists[dir][v] = dist #equal to seen[dir][v]
            if v in dists[1-dir]:
                # if we have scanned v in both directions we are done
                # we have now discovered the shortest path
                return (finaldist, finalpath)
    
            for w in neighs[dir](v):
                # this way can't be walked.
                #if G.isRightOfWay(v, w) == False:
                #    continue
                
                if(dir==0): #forward
                    minweight=G[v][w].get(weight,1)
                    vwLength = dists[dir][v] + minweight #G[v][w].get(weight,1)
                else: #back, must remember to change v,w->w,v
                    minweight=G[w][v].get(weight,1)
                    vwLength = dists[dir][v] + minweight #G[w][v].get(weight,1)
    
                if w in dists[dir]:
                    if vwLength < dists[dir][w]:
                        raise ValueError("Contradictory paths found: negative weights?")
                elif w not in seen[dir] or vwLength < seen[dir][w]:
                    # relaxing
                    seen[dir][w] = vwLength
                    heapq.heappush(fringe[dir], (vwLength,w))
                    paths[dir][w] = paths[dir][v]+[w]
                    if w in seen[0] and w in seen[1]:
                        #see if this path is better than than the already
                        #discovered shortest path
                        totaldist = seen[0][w] + seen[1][w]
                        if finalpath == [] or finaldist > totaldist:
                            finaldist = totaldist
                            revpath = paths[1][w][:]
                            revpath.reverse()
                            finalpath = paths[0][w] + revpath[1:]
        raise Exception("No path between %s and %s." % (source, target))
    

    @staticmethod
    def single_source_dijkstra(G, startNode, endNode, maxWeight=sys.maxint, weight='weight'):
        if G.is_directed() == False:
            # this function referenced 'DiGraph.successors_iter'
            # you must input a graph which inherits from DiGraph.
            raise ValueError('the input Graph must be directed graph.')
            return None
            
        if G.is_multigraph():
            raise ValueError('the input Graph must be non-multi graph.')
            return None
        
        if startNode == endNode:
            return ({startNode:0}, {startNode:[startNode]})
        
        weightList = {}  # {destNode: cost}
        pathList = {startNode:[startNode]}  # {destNode: [node1, node2, ...]}
        seenNodeDict = {startNode:0} # (seenNode, totalCost)
        fringeNodeList = [] # [(totalCost, fringeNode)]
        heapq.heappush(fringeNodeList, (0, startNode))
        while fringeNodeList:
            (weight, curNode) = heapq.heappop(fringeNodeList)
            if curNode in weightList:
                continue # already searched this node.
            weightList[curNode] = weight
            if curNode == endNode:
                break
            
            succNodeiter = iter(G[curNode].items())
            for succNode, succNodeAttr in succNodeiter:
                tempTotalWeight = weightList[curNode] + succNodeAttr.get(weight, 1)
                if tempTotalWeight > maxWeight:
                    continue

                if succNode in weightList:
                    if tempTotalWeight < weightList[succNode]:
                        raise ValueError('Contradictory pathList found:', 'negative weights?')
                
                if succNode in seenNodeDict and tempTotalWeight < seenNodeDict[succNode]:
                    continue
                
                heapq.heappush(fringeNodeList, (tempTotalWeight, succNode))
                seenNodeDict[succNode] = tempTotalWeight
                pathList[succNode] = pathList[curNode]+[succNode]
                
        return (weightList, pathList)

    # return: list of nodes that are not connected
    @staticmethod
    def getDisconnectedNodeList(G):
        if G.is_directed() == False:
            # this function referenced 'DiGraph.successors_iter'
            # you must input a graph which inherits from DiGraph.
            raise ValueError('the input Graph must be directed graph.')
            return None
            
        if G.is_multigraph():
            raise ValueError('the input Graph must be non-multi graph.')
            return None
        
        startNode = None
        try:
            startNode = G.node.items()[0][0]
        except:
            raise Exception("""There is not any nodes in this graph.""")
            return None

        weightList = {}  # {destNode: cost}
        seenNodeDict = {startNode:0} # (seenNode, totalCost)
        fringeNodeList = [] # [(totalCost, fringeNode)]
        heapq.heappush(fringeNodeList, (0, startNode))
        while fringeNodeList:
            (weight, curNode) = heapq.heappop(fringeNodeList)
            if curNode in weightList:
                continue # already searched this node.
            weightList[curNode] = weight
            succNodeiter = iter(G[curNode].items())
            for succNode, succNodeAttr in succNodeiter:
                tempTotalWeight = weightList[curNode] + succNodeAttr.get(weight, 1)
                if succNode in weightList:
                    if tempTotalWeight < weightList[succNode]:
                        raise ValueError('Contradictory pathList found:', 'negative weights?')
                
                if succNode in seenNodeDict and tempTotalWeight < seenNodeDict[succNode]:
                    continue
                
                heapq.heappush(fringeNodeList, (tempTotalWeight, succNode))
                seenNodeDict[succNode] = tempTotalWeight

        resultList = []
        for x in G.node:
           if x not in weightList:
               resultList.append(x)
        return resultList

















