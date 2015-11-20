import os
import psycopg2
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from qgis.core import QgsVectorLayer, QgsMapLayerRegistry, QgsDataSourceURI

FeatureTypeList = \
[
    """spotguide""",
    """signpost""",
    """lane"""
]

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'GenerateNewLayerDlgDesign.ui'))

class GenerateNewLayerDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, parent=None):
        super(GenerateNewLayerDlg, self).__init__(parent)
        self.setupUi(self)
        self.lineEditHost.setText("""192.168.10.14""")
        self.lineEditPort.setText("""5432""")
        self.lineEditDbName.setText("""post13_MEA8_RDF_CI""")
        self.lineEditUser.setText("""postgres""")
        self.lineEditPassword.setText("""pset123456""")
        for oneText in FeatureTypeList:
            self.comboBoxSelectFeature.addItem(oneText)
        self.connect(self.comboBoxSelectFeature, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxSelectFeatureChanged);
        self.lineEditNewLayerName.setText("""temp_spotguide_nodes""")
        self.pushButtonGenerateLayer.clicked.connect(self.generateLayer)
        self.pushButtonTestConnect.clicked.connect(self.testConnect)

    # generate a new layer with specified name.   
    def generateLayer(self):
        host = self.lineEditHost.text()
        port = self.lineEditPort.text()
        dbname = self.lineEditDbName.text()
        user = self.lineEditUser.text()
        password = self.lineEditPassword.text()
        featureType = self.comboBoxSelectFeature.currentText()
        newLayerName = self.lineEditNewLayerName.text()
        if host is None or \
           port is None or \
           dbname is None or \
           user is None or \
           password is None or \
           newLayerName is None or \
           featureType is None:
            errMsg = """error:\n"""+\
                    """Input arguments not correct.\n"""+\
                    """host: %s\n"""+\
                    """port: %s\n"""+\
                    """dbname: %s\n"""+\
                    """user: %s\n"""+\
                    """password: %s\n"""+\
                    """newLayerName: %s\n"""+\
                    """feature: %s\n""" % \
                    (host, port, dbname, user, 
                     password, newLayerName, featureType)
            QMessageBox.information(self, "Generate New Layer", errMsg[0])
            return
        sqlcmd = ''
        
        if featureType == FeatureTypeList[0]: # spotguide
            sqlcmd = \
"""
drop table if exists %s;
create table %s 
as 
(
    select a.*, b.image_id as pattern_name, b.data as pattern_dat, 
           c.image_id as arrow_name, c.data as arrow_dat, 
           st_union(array[d.the_geom, e.the_geom, f.the_geom]) as the_geom
    from 
    rdb_guideinfo_spotguidepoint as a
    left join rdb_guideinfo_pic_blob_bytea as b
    on a.pattern_id=b.gid
    left join rdb_guideinfo_pic_blob_bytea as c
    on a.arrow_id=c.gid
    left join rdb_node as d
    on a.node_id=d.node_id
    left join rdb_link as e
    on a.in_link_id=e.link_id
    left join rdb_link as f
    on a.out_link_id=f.link_id);""" % (newLayerName, newLayerName)
        elif featureType == FeatureTypeList[1]: # signpost
            sqlcmd = \
"""
drop table if exists %s;
create table %s 
as 
(
    select a.*, b.image_id as pattern_name, b.data as pattern_dat, 
           c.image_id as arrow_name, c.data as arrow_dat, 
           st_union(array[d.the_geom, e.the_geom, f.the_geom]) as the_geom
    from 
    rdb_guideinfo_signpost as a
    left join rdb_guideinfo_pic_blob_bytea as b
    on a.pattern_id=b.gid
    left join rdb_guideinfo_pic_blob_bytea as c
    on a.arrow_id=c.gid
    left join rdb_node as d
    on a.node_id=d.node_id
    left join rdb_link as e
    on a.in_link_id=e.link_id
    left join rdb_link as f
    on a.out_link_id=f.link_id);""" % (newLayerName, newLayerName)
        elif  featureType == FeatureTypeList[2]: # lane
            sqlcmd = \
"""
drop table if exists %s;
create table %s 
as 
(
    select a.*, st_union(array[b.the_geom, c.the_geom, d.the_geom]) as the_geom
    from
    rdb_guideinfo_lane as a
    left join rdb_node as b
    on a.node_id=b.node_id
    left join rdb_link as c
    on a.in_link_id=c.link_id
    left join rdb_link as d
    on a.out_link_id=d.link_id);""" % (newLayerName, newLayerName)
        else:
            QMessageBox.information(self, "Generate New Layer", "error:\nnot spotguide or lane.")
            return

        #-------------------------------------------------------------------------------------------------------
        try:
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (host, dbname, user, password))
            pg = conn.cursor()
            pg.execute(sqlcmd)
            conn.commit()
        except Exception, ex:
            QMessageBox.information(self, "Generate New Layer", """error:\n%s"""%ex.message)
            return

        errMsg = ['']
        self.removeAllLayersWithSpecName(errMsg, newLayerName)
        if errMsg[0] != '':
            QMessageBox.information(self, "Generate New Layer", """error:\n%s"""%errMsg[0])
            return
        uri = QgsDataSourceURI()        
        uri.setConnection(host, port, dbname, user, password)        
        uri.setDataSource("public", newLayerName, "the_geom", "")
        layer = QgsVectorLayer(uri.uri(), newLayerName, "postgres")
        layer.name = newLayerName
        if not layer.isValid():
            errMsg[0] = """error:\nCreate new layer failed. """+\
                        """host: %s, dbName: %s, user: %s, password: %s""" % \
                        (host, port, dbname, user, password)
            return 
        # Add layer to the registry
        QgsMapLayerRegistry.instance().addMapLayer(layer)
        return

    # make sure errMsg is a 'string array' which has at least one element.
    def removeAllLayersWithSpecName(self, errMsg, specLayerName):
        layerList = QgsMapLayerRegistry.instance().mapLayersByName(specLayerName)
        for oneLayer in layerList:
            QgsMapLayerRegistry.instance().removeMapLayer(oneLayer.id())
        return
    def comboBoxSelectFeatureChanged(self):
        featureType = self.comboBoxSelectFeature.currentText()
        if featureType == FeatureTypeList[0]:
            self.lineEditNewLayerName.setText("""temp_spotguide_nodes""")
        elif featureType == FeatureTypeList[1]:
            self.lineEditNewLayerName.setText("""temp_signpost_nodes""")
        elif featureType == FeatureTypeList[2]:
            self.lineEditNewLayerName.setText("""temp_lane_nodes""")
        return

    def testConnect(self):
        inti = 0
        inti += 1
        return
    




