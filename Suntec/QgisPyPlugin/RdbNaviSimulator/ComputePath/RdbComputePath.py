#encoding=utf-8
import psycopg2
from LatLonPoint import LatLonPoint
from RdbDiGraph import RdbDiGraph

class RdbComputePath(object):
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
        possibleUpgradeNodeList_start.sort(lambda x,y:cmp(x[2],y[2]))
        upgradeNode_start = possibleUpgradeNodeList_start[0][0]
        path1 = possibleUpgradeNodeList_start[0][3]

        possibleUpgradeNodeList_end = self.findUpGradeNode_to_L4(errMsg, endNode_L14)
        possibleUpgradeNodeList_end.sort(lambda x,y:cmp(x[2],y[2]))
        upgradeNode_end = possibleUpgradeNodeList_end[0][0]
        path5 = possibleUpgradeNodeList_end[0][3]

        path2, path3, path4 = self.computePath_L4(errMsg, upgradeNode_start, upgradeNode_end)
        linkList = []
        tempLinkList = []

        path1.extend(path2[1:])
        for i in range(0, len(path1)-1):
            tempLinkList.append(self.G_L14[path1[i]][path1[i+1]]['link_id'])
        linkList.extend(tempLinkList)
        if path3 is not None:
            linkList.extend(path3)

        tempLinkList = []
        #path4.extend(path5[1:])
        for i in range(0, len(path4)-1):
            tempLinkList.append(self.G_L14[path4[i]][path4[i+1]]['link_id'])
        linkList.extend(tempLinkList)
        return linkList

    #
    def computePath_L4(self, errMsg, startNode_L14, endNode_L14):
        # 找到附近所有可能的上迁点
        # 返回值为一个列表，结构如下：
        # [（L14 node id， L6 node id， cost， L14 path）]
        possibleUpgradeNodeList_start = self.findUpGradeNode_to_L6(errMsg, startNode_L14)
        # 对这些记录按照cost进行排序
        possibleUpgradeNodeList_start.sort(lambda x,y:cmp(x[2],y[2]))
        # 取cost最小的一条路径
        path1 = possibleUpgradeNodeList_start[0][3]

        possibleUpgradeNodeList_end = self.findUpGradeNode_to_L6(errMsg, endNode_L14)
        possibleUpgradeNodeList_end.sort(lambda x,y:cmp(x[2],y[2]))
        path2 = possibleUpgradeNodeList_end[0][3]

        midPath = self.computePath_L6(errMsg,
                                      possibleUpgradeNodeList_start[0][1],
                                      possibleUpgradeNodeList_end[0][1])
        return path1, midPath, path2

    #
    def computePath_L6(self, errMsg, startNode_L6, endNode_L6):
        path_L6 = self.computePath_L6Directly(errMsg, startNode_L6, endNode_L6)
        return self.findLinkList_L14From_path_L6(path_L6)

    #
    def computePath_L14Directly(self, errMsg, startNode, endNode, tileOffset=3):
        tileIDList_L14 = LatLonPoint.getTileIdListByNodeId(errMsg, startNode, endNode, tileOffset)
        self.G_L14.addEdgeByTileIDList(errMsg, tileIDList_L14, 'rdb_link')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L14, startNode, endNode)
        return pathList[endNode]

    #
    def computePath_L4Directly(self, errMsg, startNode_L4, endNode_L4, tileOffset=3):
        tileIDList_L4 = LatLonPoint.getTileIdListByNodeId(errMsg, startNode_L4, endNode_L4, tileOffset)
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
        tileOffset = 0
        upgradeNodeList = None
        sqlcmdFormat = """
select node_id, b.region_node_id
from rdb_node as a
left join rdb_region_layer4_node_mapping as b
on a.node_id=b.node_id_14
where a.node_id_t in (%s) and b.node_id_14 is not null;
"""
        while (True):
            tileOffset += 1
            tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, tileOffset)
            strTileIDList = ','.join(("%d"%x) for x in tileIdList_L14)
            sqlcmd = sqlcmdFormat % strTileIDList
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            if len(rows) < 4:
                continue
            else:
                upgradeNodeList = list(rows)
                break

        tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, tileOffset+3)
        self.G_L14.addEdgeByTileIDList(errMsg, tileIdList_L14, 'rdb_link')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L14, node_L14, None)

        resultList = [] # [(node_L14, node_L4, weight, pathlist)]
        for oneUpgradeNode in upgradeNodeList:
            if pathList.has_key(oneUpgradeNode[0]):
                resultList.append((oneUpgradeNode[0],
                                   oneUpgradeNode[1],
                                   weightList[oneUpgradeNode[0]],
                                   pathList[oneUpgradeNode[0]]))
        return resultList

    # 在此node的附近寻找上迁至level6的上迁点
    # 返回一个列表，此列表每个元素信息如下：
    # （L14 node id， L6 node id， cost， L14 path）
    def findUpGradeNode_to_L6(self, errMsg, node_L14):
        tileId_L14 = node_L14 >> 32
        tileOffset = 0
        upgradeNodeList = None
        sqlcmdFormat = """
select a.node_id, b.region_node_id
from rdb_node as a
left join rdb_region_layer6_node_mapping as b
on a.node_id=b.node_id_14
where a.node_id_t in (%s) and b.node_id_14 is not null;
"""
        while (True):
            tileOffset += 1
            tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, tileOffset)
            strTileIDList = ','.join(("%d"%x) for x in tileIdList_L14)
            sqlcmd = sqlcmdFormat % strTileIDList
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            if len(rows) < 4:
                continue
            else:
                upgradeNodeList = list(rows)
                break

        tileIdList_L14 = LatLonPoint.getTileIdListByTileId(errMsg, tileId_L14, tileId_L14, tileOffset+3)
        self.G_L14.addEdgeByTileIDList(errMsg, tileIdList_L14, 'rdb_link')
        weightList, pathList = RdbDiGraph.single_source_dijkstra(self.G_L14, node_L14, None)

        resultList = []
        for oneUpgradeNode in upgradeNodeList:
            if pathList.has_key(oneUpgradeNode[0]):
                resultList.append((oneUpgradeNode[0],
                                   oneUpgradeNode[1],
                                   weightList[oneUpgradeNode[0]],
                                   pathList[oneUpgradeNode[0]]))
        return resultList

    def findLinkList_L14From_path_L6(self, path_L6):
        sqlcmd = """
create or replace function path_L6_to_path_L14(path_L6 bigint[])
returns bigint[]
language plpgsql
as
$$
declare
    nIndex integer;
    nCount integer;
    linkList_L14 bigint[];
    tempLinkIdList_L14 bigint[];
begin
    nCount := array_upper(path_L6, 1);
    for nIndex in 1..nCount-1 loop
        select link_id_14 
        into tempLinkIdList_L14
        from 
        rdb_region_link_layer6_tbl as a
        left join rdb_region_layer6_link_mapping as b
        on a.link_id=b.region_link_id
        where (start_node_id=path_L6[nIndex] and end_node_id=path_L6[nIndex+1]) or
              (start_node_id=path_L6[nIndex+1] and end_node_id=path_L6[nIndex]);
        linkList_L14 = array_cat(linkList_L14, tempLinkIdList_L14);
    end loop;
    return linkList_L14;
end;
$$;
select path_L6_to_path_L14(array[%s]);
"""
        strPath_L6 = ','.join(str(x) for x in path_L6)
        self.pg.execute(sqlcmd % strPath_L6)
        return self.pg.fetchone()[0]

