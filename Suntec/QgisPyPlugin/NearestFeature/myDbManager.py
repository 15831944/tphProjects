# -*- coding: utf-8 -*-
import psycopg2
from PyQt4.QtCore import QVariant
from qgis.core import QgsVectorLayer, QgsField, QgsFields, QgsMapLayerRegistry
from qgis.core import QgsDataSourceURI
from qgis.core import QgsMapLayer, QgsMapToPixel, QgsFeature, QgsPoint, QgsGeometry
import types

class myDbManager(object):
    def __init__(self):
        self.host = None
        self.port = None
        self.dbname = None
        self.user = None
        self.password = None
        self.userSqlcmd = None
        self.newLayerName = None
        self.imageColumn = None
        return

    # make sure errMsg is a 'string array' which has at least one element.
    def generateNewLayer(self, errMsg):
        if self.newLayerName is None or \
           self.host is None or \
           self.port is None or \
           self.dbname is None or \
           self.user is None or \
           self.password is None or \
           self.userSqlcmd is None:
            return
        sqlcmd = \
"""
drop table if exists %s;
create table %s
as 
(%s);
"""
        sqlcmd = sqlcmd % (self.newLayerName, self.newLayerName, self.userSqlcmd)
        try:
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (self.host, self.dbname, self.user, self.password))
            pg = conn.cursor()
            pg.execute(sqlcmd)
            conn.commit()
        except Exception, ex:
            errMsg[0] = ex
            return

        self.removeAllLayersWithSpecName(self.newLayerName, errMsg)
        if errMsg[0] != '':
            return
        uri = QgsDataSourceURI()        
        uri.setConnection(self.host, self.port, self.dbname, self.user, self.password)        
        uri.setDataSource("public", self.newLayerName, "the_geom", "")
        layer = QgsVectorLayer(uri.uri(), self.newLayerName, "postgres")
        layer.name = self.newLayerName
        if not layer.isValid():
            errMsg[0] = """Create new layer failed. host: %s, dbName: %s, user: %s, password: %s""" % \
                (self.host, self.port, self.dbname, self.user, self.password)
            return 
        # Add layer to the registry
        QgsMapLayerRegistry.instance().addMapLayer(layer)
        return
    
    # make sure errMsg is a 'string array' which has at least one element.
    def removeAllLayersWithSpecName(self, specLayerName, errMsg):
        layerList = QgsMapLayerRegistry.instance().mapLayersByName(specLayerName)
        for oneLayer in layerList:
            layerId = oneLayer.id()
            layerList2 = QgsMapLayerRegistry.instance().removeMapLayer(layerId)
        return

    def getPictureBinaryData(self, fieldList, attrList, errMsg):
        if len(fieldList) != len(attrList):
            errMsg[0] = '''fieldList's length is not equal to attrList's. '''
            return None
        strFilter = ''''''
        for oneField, oneAttr in zip(fieldList, attrList):
            if type(oneAttr) is types.StringType:
                strFilter += ''' %s='%s' and ''' % (oneField.name(), oneAttr)
            elif type(oneAttr) is types.IntType or\
                 type(oneAttr) is types.LongType or\
                 type(oneAttr) is types.FloatType:
                strFilter += ''' %s=%s and ''' % (oneField.name(), oneAttr)
            else:
                continue
        sqlcmd = \
"""SET bytea_output TO escape;
select %s 
from %s
where %s true
""" % (self.imageColumn, self.newLayerName, strFilter)
        try:
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (self.host, self.dbname, self.user, self.password))
            pg = conn.cursor()
            pg.execute(sqlcmd)
            theData = pg.fetchone()
            return theData[0]
        except Exception, ex:
            errMsg[0] = ex
            return None