#encoding=utf-8
import heapq
import networkx
from LatLonPoint import LatLonPoint

class RdbDiGraph(networkx.DiGraph):
    def __init__(self):
        self.loadedTile = []
        self.regulationDict = {}
        return
    
    # 获取最合适的tile集合
    def getAppropriateTileIDList(self, startLatLonPoint, endLatLonPoint):
        return
    
    # 从rdb_link表中load此tile所包含的link。
    def loadLinkFromTileID(self, tileID):
        return
    
    # 从rdb_link表中load此组tile所包含的link。
    def loadLinkFromTileIDList(self, tileIDList):
        return
    
    # 从rdb_link_regulation表中load此tile所包含的规制。
    def loadLinkRegulationFromTileID(self, tileID):
        return
      
    # 从rdb_link表中load此组tile所包含的规制。
    def loadLinkRegulationFromTileIDList(self, tileIDList):
        return  
    
    # 根据规制表信息判断是否可通行
    def canWalk(self, u, v):
        return True
    
    # 算路
    @staticmethod
    def bidirectional_dijkstra(G, source, target, weight = 'weight'):
        G.getAppropriateTileIDList()
        if source == target: 
            return (0, [source])
        
        if G.is_multigraph():
            raise Exception("The input Graph must be a DiGraph which is not a multigraph.")
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
        if G.is_directed():
            neighs = [G.successors_iter, G.predecessors_iter]
        else:
            neighs = [G.neighbors_iter, G.neighbors_iter]
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
                return (finaldist,finalpath)
    
            for w in neighs[dir](v):
                # this way can't be walked.
                if G.canWalk(v, w) == False:
                    continue
                
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
    



















