#encoding=utf-8
import psycopg2
from LatLonPoint import LatLonPoint
from RdbDiGraph import RdbDiGraph

class TestComputePath(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
        self.G_L6 = RdbDiGraph(self.pg)
        self.G_L4 = RdbDiGraph(self.pg)
        self.G_L14 = RdbDiGraph(self.pg)

    #
    def computePath_L14(self, errMsg, startNode_L14, endNode_L14):
        possibleUpgradeNodeList_start = self.findUpGradeNode_to_L4(errMsg, startNode_L14)
        possibleUpgradeNodeList_start.sort(lambda x,y:cmp(x[1],y[1]))
        upgradeNode_start = possibleUpgradeNodeList_start[0][0]
        path1 = possibleUpgradeNodeList_start[0][3]

        possibleUpgradeNodeList_end = self.findUpGradeNode_to_L4(errMsg, endNode_L14)
        possibleUpgradeNodeList_end.sort(lambda x,y:cmp(x[1],y[1]))
        upgradeNode_end = possibleUpgradeNodeList_end[0][0]
        path5 = possibleUpgradeNodeList_end[0][3]

        path2, path3, path4 = self.computePath_L4(errMsg, upgradeNode_start, upgradeNode_end)
        return path1, path2, path3, path4, path5

    #
    def computePath_L4(self, errMsg, startNode_L14, endNode_L14):
        possibleUpgradeNodeList_start = self.findUpGradeNode_to_L6(errMsg, startNode_L14)
        possibleUpgradeNodeList_start.sort(lambda x,y:cmp(x[1],y[1]))
        upgradeNode_start = possibleUpgradeNodeList_start[0][0]
        path1 = possibleUpgradeNodeList_start[0][3]

        possibleUpgradeNodeList_end = self.findUpGradeNode_to_L6(errMsg, endNode_L14)
        possibleUpgradeNodeList_end.sort(lambda x,y:cmp(x[1],y[1]))
        upgradeNode_end = possibleUpgradeNodeList_end[0][0]
        path2 = possibleUpgradeNodeList_end[0][3]

        midPath = self.computePath_L6(errMsg,
                                      possibleUpgradeNodeList_start[0][1],
                                      possibleUpgradeNodeList_end[0][1])
        return path1, midPath, path2

    #
    def computePath_L6(self, errMsg, startNode_L6, endNode_L6):
        path_L6 = self.computePath_L6Directly(errMsg, startNode_L6, endNode_L6)
        return self.path_L6_to_path_L14(path_L6)

    #
    def computePath_L14Directly(self, errMsg, startNode, endNode, offset=3):
        tileIDList_L14 = LatLonPoint.getTileIdListByNodeId(errMsg, startNode, endNode, offset)
        self.G_L14.addEdgeByTileIDList(errMsg, tileIDList_L14, 'rdb_link')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L14, startNode, endNode)
        return pathList[endNode]

    #
    def computePath_L4Directly(self, errMsg, startNode_L4, endNode_L4):
        tileIDList_L4 = LatLonPoint.getTileIdListByNodeId(errMsg, startNode_L4, endNode_L4, offset=3)
        self.G_L4.addEdgeByTileIDList(errMsg, tileIDList_L4, 'rdb_region_link_layer4_tbl')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L4, startNode_L4, endNode_L4)
        return pathList[endNode_L4]

    #
    def computePath_L6Directly(self, errMsg, startNode_L6, endNode_L6):
        self.G_L6.addEdgeFromTable(errMsg, 'rdb_region_link_layer6_tbl')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L6, startNode_L6, endNode_L6)
        return pathList[endNode_L6]

    # 在此node_L14的附近寻找可上迁至layer4的上迁点
    def findUpGradeNode_to_L4(self, errMsg, node_L14):
        tileId_L14 = node_L14 >> 32
        offset = 0
        upgradeNodeList = None
        sqlcmdFormat = """
select node_id, b.region_node_id
from rdb_node as a
left join rdb_region_layer4_node_mapping as b
on a.node_id=b.node_id_14
where a.node_id_t in (%s) and b.node_id_14 is not null;
"""
        while (True):
            offset += 1
            tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, offset)
            strTileIDList = ','.join(("%d"%x) for x in tileIdList_L14)
            sqlcmd = sqlcmdFormat % strTileIDList
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            if len(rows) < 4:
                continue
            else:
                upgradeNodeList = list(rows)
                break

        tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, offset+3)
        self.G_L14.addEdgeByTileIDList(errMsg, tileIdList_L14, 'rdb_link')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L14, node_L14, None)

        resultList = [] # node_L14, node_L4, weight, pathlist
        for oneUpgradeNode in upgradeNodeList:
            if pathList.has_key(oneUpgradeNode[0]):
                resultList.append((oneUpgradeNode[0],
                                   oneUpgradeNode[1],
                                   weightList[oneUpgradeNode[0]],
                                   pathList[oneUpgradeNode[0]]))
        return resultList

    # 在此node的附近寻找上迁至level6的上迁点
    def findUpGradeNode_to_L6(self, errMsg, node_L14):
        tileId_L14 = node_L14 >> 32
        offset = 0
        upgradeNodeList = None
        sqlcmdFormat = """
select a.node_id, b.region_node_id
from rdb_node as a
left join rdb_region_layer6_node_mapping as b
on a.node_id=b.node_id_14
where a.node_id_t in (%s) and b.node_id_14 is not null;
"""
        while (True):
            offset += 1
            tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, offset)
            strTileIDList = ','.join(("%d"%x) for x in tileIdList_L14)
            sqlcmd = sqlcmdFormat % strTileIDList
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            if len(rows) < 4:
                continue
            else:
                upgradeNodeList = list(rows)
                break

        tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, offset+3)
        self.G_L14.addEdgeByTileIDList(errMsg, tileIdList_L14, 'rdb_link')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L14, node_L14, None)

        resultList = [] # node_L14, node_L4, weight, pathlist
        for oneUpgradeNode in upgradeNodeList:
            if pathList.has_key(oneUpgradeNode[0]):
                resultList.append((oneUpgradeNode[0],
                                   oneUpgradeNode[1],
                                   weightList[oneUpgradeNode[0]],
                                   pathList[oneUpgradeNode[0]]))
        return resultList

    def path_L6_to_path_L14(self, path_L6):
        return path_L6

def main():
    errMsg = ['']
    dbIP = '''192.168.10.20'''
    dbNameList = ['C_NIMIF_15SUM_0082_0003']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            computePath = TestComputePath(dbIP, dbName, userName, password)
            #path_L6 = computePath.computePath_L6Directly(errMsg, 2305843022098603970, 2305843013508662291)
            #with open(r"c:\my\path_L6.txt", "w") as oFStream:
            #    for i in range(0, len(path_L6)-1):
            #        oFStream.write("""%s,\n""" % (computePath.G_L6[path_L6[i]][path_L6[i+1]]['link_id']))

            #path_L4 = computePath.computePath_L4Directly(errMsg, -6859402525099950070, -6859895114899128142)
            #with open(r"c:\my\path_L4.txt", "w") as oFStream:
            #    for i in range(0, len(path_L4)-1):
            #        oFStream.write("""%s,\n""" % (computePath.G_L4[path_L4[i]][path_L4[i+1]]['link_id']))

            #path_L14 = computePath.computePath_L14Directly(errMsg, -1394607968711868412, -1393341511705296893)
            #with open(r"c:\my\path_L14.txt", "w") as oFStream:
            #    for i in range(0, len(path_L14)-1):
            #        oFStream.write("""%s,\n""" % (computePath.G_L14[path_L14[i]][path_L14[i+1]]['link_id']))
            path1, path2, path3, path4, path5 = computePath.computePath_L14(errMsg, -1398548326328041464, -1360695083036311550)
            with open(r"c:\my\path_L14_2.txt", "w") as oFStream:
                for i in range(0, len(path1)-1):
                    oFStream.write("""%s,\n""" % (computePath.G_L14[path1[i]][path1[i+1]]['link_id']))
                for i in range(0, len(path2)-1):
                    oFStream.write("""%s,\n""" % (computePath.G_L14[path2[i]][path2[i+1]]['link_id']))
                for i in range(0, len(path3)-1):
                    oFStream.write("""%s,\n""" % (computePath.G_L6[path3[i]][path3[i+1]]['link_id']))
                for i in range(0, len(path4)-1):
                    oFStream.write("""%s,\n""" % (computePath.G_L14[path4[i]][path4[i+1]]['link_id']))
                for i in range(0, len(path5)-1):
                    oFStream.write("""%s,\n""" % (computePath.G_L14[path5[i]][path5[i+1]]['link_id']))

        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

if __name__ == "__main__":
    main()