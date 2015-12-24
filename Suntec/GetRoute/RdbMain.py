#encoding=utf-8
import psycopg2
from LatLonPoint import LatLonPoint
from RdbDiGraph import RdbDiGraph

class TestComputeRoute(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
        self.G_L6 = RdbDiGraph(self.pg)

    # 返回：level14的node序列
    def computeRoute_L14(self, errMsg, startNode_L14, endNode_L14):
        startUpNode_L14 = self.findUpNode_L14_to_L4(startNode_L14)
        endUpNode_L14 = self.findUpNode_L14_to_L4(endNode_L14)
        path1 = self.computeRoute14Directly(errMsg, startNode_L14, startUpNode_L14)
        path2 = self.computeRoute14Directly(errMsg, endUpNode_L14, endNode_L14)
        midPath_L4 = self.computeRoute_L4(errMsg,
                                           TestComputeRoute.node_L14_to_node_L4(startUpNode_L14),
                                           TestComputeRoute.node_L14_to_node_L4(endUpNode_L14))
        midPath_L14 = TestComputeRoute.path_L4_to_path_L14(midPath_L4)
        path_L14 = path1
        path_L14.extend(midPath_L14[1:])
        path_L14.extend(path2[1:])
        return path_L14

    # 返回：level14的node序列
    def computeRoute_L14Directly(self, errMsg, startNode, endNode):
        G_L14 = RdbDiGraph(self.pg)
        tileIDList_L14 = LatLonPoint.getTileIdListByNodeId(errMsg, startNode, endNode, offset=3)
        G_L14.addEdgeByTileIDList(errMsg, tileIDList_L14)
        weightList, pathList = RdbDiGraph.single_source_dijkstra(G_L14, startNode, endNode)
        return pathList[endNode]

    # 返回：level4的node序列
    def computeRoute_L4(self, errMsg, startNode_L4, endNode_L4):
        startUpNode_L4 = self.findUpNode_L4_to_L6(startNode)
        endUpNode_L4 = self.findUpNode_L4_to_L6(endNode)
        path1 = self.computeRoute_L4Directly(errMsg, startNode_L4, startUpNode_L4)
        path2 = self.computeRoute_L4Directly(errMsg, endUpNode_L4, endNode_L4)

        path1_L14 = []
        path2_L14 = []
        midPath_L6 = self.computeRoute_L6(errMsg, 
                                          TestComputeRoute.node_L4_to_node_L6(startUpNode_L4),
                                          TestComputeRoute.node_L4_to_node_L6(endUpNode_L4))
        midPath_L4 = TestComputeRoute.path_L6_to_path_L4(midPath_L6)
        path_L4 = path1
        path_L4.extend(midPath_L4[1:])
        path_L4.extend(path2[1:])
        return path_L4

    # 返回：level4的node序列
    def computeRoute_L4Directly(self, errMsg, startNode_L4, endNode_L4):
        G_L4 = RdbDiGraph(self.pg)
        tileIDList_L4 = LatLonPoint.getTileIdListByNodeId(errMsg, startNode_L4, endNode_L4, offset=3)
        G_L4.addEdgeByTileIDList(errMsg, tileIDList_L4, 'rdb_region_link_layer4_tbl')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(G_L4, startNode_L4, endNode_L4)
        return pathList[endNode_L4]

    # 返回：level6的node序列
    def computeRoute_L6Directly(self, errMsg, startNode_L6, endNode_L6):
        self.G_L6.addEdgeFromTable(errMsg, 'rdb_region_link_layer6_tbl')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L6, startNode_L6, endNode_L6)
        return pathList[endNode_L6]

    # 在node_L14的附近寻找上迁至level4的上迁点
    # 返回level14的node值。
    @staticmethod
    def findUpNode_L14_to_L4(self, errMsg, node_L14):
        return 1

    # 在node_L4的附近寻找上迁至level6的上迁点
    # 返回level4的node值。
    @staticmethod
    def findUpNode_L4_to_L6(self, errMsg, node_L4):
        return 1

    @staticmethod
    def node_L14_to_node_L4(node_L14):
        sqlcmd = """
select region_node_id 
from rdb_region_layer4_node_mapping
where node_id_14=%s
"""
        self.pg.execute(sqlcmd%node_L14)
        rows = self.pg.fetchall()
        if len(rows) == 0:
            errMsg[0] = "this level 14 node: %s has no up grade node to level 4." % node_L14
            return None
        else:
            return rows[0][0]

    @staticmethod
    def node_L4_to_node_L14(node_L4):
        node_L14 = node_L4
        return node_L14

    @staticmethod
    def path_L4_to_path_L14(path_L4):
        path_L14=[]
        path_L14.append(TestComputeRoute.node_L4_to_node_L14(x) for x in _Lpath4)
        return path_L14

    @staticmethod
    def node_L4_to_node_L6(node_L4):
        sqlcmd = """
select * 
from
rdb_region_layer4_node_mapping as a
left join rdb_node as b
on a.node_id_14 = b.node_id
left join rdb_region_layer6_node_mapping
"""
        node_L6 = node_L4
        return node_L6

    @staticmethod
    def node_L6_to_node_L4(node_L6):
        node_L4 = node_L6
        return node_L4

    @staticmethod
    def path_L6_to_path_L4(path_L6):
        path_L4=[]
        path_L4.append(TestComputeRoute.node_L6_to_node_L4(x) for x in path_L6)
        return path_L4

def main():
    errMsg = ['']
    dbIP = '''192.168.10.20'''
    dbNameList = ['C_NIMIF_15SUM_0082_0003']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            computeRoute = TestComputeRoute(dbIP, dbName, userName, password)
            path_L6 = computeRoute.computeRoute_L6Directly(errMsg, 2305843022098617489, 2305843013508663116)
            inti = 0
            inti += 1
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

if __name__ == "__main__":
    main()