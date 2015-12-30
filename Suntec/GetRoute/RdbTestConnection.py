#encoding=utf-8
import psycopg2
from LatLonPoint import LatLonPoint
from RdbDiGraph import RdbDiGraph

class TestConnection(object):
    def __init__(self, dbIP, dbName, userName, password):
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                     (dbIP, dbName, userName, password))
        self.pg = self.conn.cursor()
        self.G = RdbDiGraph(self.pg)

    def getDisconnectedList(self, errMsg):
        self.G.addEdgeFromTable(errMsg, 'rdb_region_link_layer4_tbl')
        return RdbDiGraph.getDisconnectedNodeList(self.G)

def main():
    errMsg = ['']
    dbIP = '''192.168.10.20'''
    dbNameList = ['C_NIMIF_15SUM_0082_0003']
    userName = '''postgres'''
    password = '''pset123456'''
    for dbName in dbNameList:
        try:
            testConnection = TestConnection(dbIP, dbName, userName, password)
            nodeList = testConnection.getDisconnectedList(errMsg)
            with open(r"c:\my\1.txt", "w") as oFStream:
                for oneNode in nodeList:
                    oFStream.write("""%s,\n""" % oneNode)
        except Exception, ex:
            print '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)

if __name__ == "__main__":
    main()