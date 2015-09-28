# -*- coding: utf-8 -*-
import psycopg2
from PyQt4.QtCore import QVariant
from qgis.core import QgsVectorLayer, QgsField, QgsFields, QgsMapLayerRegistry
from qgis.core import QgsDataSourceURI
from qgis.core import QgsMapLayer, QgsMapToPixel, QgsFeature, QgsPoint, QgsGeometry
import types

FeatureTypeList = \
[
    """spotguide""",
    """lane"""
]

class myDbManager(object):
    def __init__(self):
        return


    # 
    def generateNewLayer(self, errMsg, host, port, dbname, 
                         user, password, newLayerName, featureType):
        if host is None or \
           port is None or \
           dbname is None or \
           user is None or \
           password is None or \
           newLayerName is None or \
           featureType is None:
            errMsg[0] = """Input arguments not correct.\n"""+\
                        """host: %s\n"""+\
                        """port: %s\n"""+\
                        """dbname: %s\n"""+\
                        """user: %s\n"""+\
                        """password: %s\n"""+\
                        """newLayerName: %s\n"""+\
                        """featureType: %s\n""" % \
                        (host, port, dbname, user, 
                         password, newLayerName, featureType)
            return
        sqlcmd = """drop table if exists %s;\n"""+\
                 """create table %s \n"""+\
                 """as \n"""+\
                 """(%s);"""
        userSqlcmd = self.getuserSqlcmd(featureType)
        sqlcmd = sqlcmd % (newLayerName, newLayerName, userSqlcmd)
        try:
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (host, dbname, user, password))
            pg = conn.cursor()
            pg.execute(sqlcmd)
            conn.commit()
        except Exception, ex:
            errMsg[0] = ex
            return

        self.removeAllLayersWithSpecName(newLayerName, errMsg)
        if errMsg[0] != '':
            return
        uri = QgsDataSourceURI()        
        uri.setConnection(host, port, dbname, user, password)        
        uri.setDataSource("public", newLayerName, "the_geom", "")
        layer = QgsVectorLayer(uri.uri(), newLayerName, "postgres")
        layer.name = newLayerName
        if not layer.isValid():
            errMsg[0] = """Create new layer failed. """+\
                        """host: %s, dbName: %s, user: %s, password: %s""" % \
                (host, port, dbname, user, password)
            return 
        # Add layer to the registry
        QgsMapLayerRegistry.instance().addMapLayer(layer)
        return
    
    # make sure errMsg is a 'string array' which has at least one element.
    def removeAllLayersWithSpecName(self, specLayerName, errMsg):
        layerList = QgsMapLayerRegistry.instance().mapLayersByName(specLayerName)
        for oneLayer in layerList:
            QgsMapLayerRegistry.instance().removeMapLayer(oneLayer.id())
        return

    def getPictureBinaryData(self, errMsg, layer, theFeature):
        try:
            uri = QgsDataSourceURI(layer.source())
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (uri.host(), uri.database(), uri.username(), uri.password()))
            pg = conn.cursor()
            strFilter = ''''''
            fieldList = theFeature.fields()
            attrList = theFeature.attributes()
            for oneField, oneAttr in zip(fieldList, attrList):
                if oneAttr is None:
                    continue
                if type(oneAttr) is types.StringType:
                    strFilter += ''' %s='%s' and ''' % (oneField.name(), oneAttr)
                if type(oneAttr) is types.IntType or\
                   type(oneAttr) is types.LongType or\
                   type(oneAttr) is types.FloatType:
                    strFilter += ''' %s=%s and ''' % (oneField.name(), oneAttr)
                else:
                    continue

            if self.isTableExistsColumn_data() == False:
                errMsg[0] = """The table %s does not has a 'data' column.""" % uri.table()
                return

            sqlcmd = \
"""SET bytea_output TO escape;
select data 
from %s
where %s true
""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            theData = pg.fetchone()
            return theData[0]
        except Exception, ex:
            errMsg[0] = ex.message
            return None

    def isTableExistsColumn_data(self):
        return True

    def getuserSqlcmd(self, featureTypeText):
        if featureTypeText == """spotguide""":
            return \
"""select a.*, b.data as pattern_data, c.data as arrow_data, d.the_geom as the_geom
from 
rdb_guideinfo_spotguidepoint as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_guideinfo_pic_blob_bytea as c
on a.arrow_id=c.gid
left join rdb_node as d
on a.node_id=d.node_id"""
        elif  featureTypeText == """lane""":
            return ''
        else:
            return ''