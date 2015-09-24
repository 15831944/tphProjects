import os
import psycopg2

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from PyQt4.QtCore import QVariant
from qgis.core import QgsVectorLayer, QgsField, QgsFields, QgsMapLayerRegistry
from qgis.core import QgsMapLayer, QgsMapToPixel, QgsFeature, QgsPoint, QgsGeometry

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'nearest_feature_dialog_base.ui'))


class NearestFeatureDialog(QtGui.QDialog, FORM_CLASS):
    def __init__(self, parent=None):
        super(NearestFeatureDialog, self).__init__(parent)
        self.setupUi(self)
        self.textEditSqlCmd.setText(
"""drop table if exists temp_spotguide_nodes;
select b.gid as pic_blob_gid, a.node_id, c.the_geom 
int temp_spotguide_nodes
from 
rdb_guideinfo_spotguidepoint as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_node as c
on a.node_id=c.node_id""")
        self.lineEditLayerName.setText("""temp_spotguide_nodes""")
        self.lineEditIp.setText("""192.168.10.14""")
        self.lineEditDbName.setText("""SGPMYS_RDF_2015Q3_0065_0003""")
        self.lineEditUser.setText("""postgres""")
        self.lineEditPassword.setText("""pset123456""")
        self.pushButtonGenerateLayer.clicked.connect(self.generateLayer)
        self.pushButtonTestConnect.clicked.connect(self.testConnect)
        self.conn = None
        self.pg = None

    # generate a new layer with specified name.   
    def generateLayer(self):
        strIp = self.lineEditIp.text()
        strDbName = self.lineEditDbName.text()
        strUser = self.lineEditUser.text()
        strPassword = self.lineEditPassword.text()
        self.conn = psycopg2.connect(''' host='%s' dbname='%s' user='%s' password='%s' ''' % \
                                        (strIp, strDbName, strUser, strPassword))
        self.pg = self.conn.cursor()
        strLayerName = self.lineEditLayerName.text()
        strSqlCmd = self.textEditSqlCmd.toPlainText()
        self.lineLayer = QgsVectorLayer("LineString", strLayerName, "memory")
        self.lineLayer.startEditing()  
        self.pg.execute(strSqlCmd)
        rows = self.pg.fetchall()
        theQgsFields = QgsFields()
        theQgsFields.append(QgsField("pic_blob_gid", QVariant.Int))
        theQgsFields.append(QgsField("node_id", QVariant.Int))

        featureList = []
        for row in rows:
            pic_blob_gid = row[0]
            node_id = row[1]
            the_geom = row[2]
            oneFeature = QgsFeature()
            oneFeature.setFields(theQgsFields)
            oneFeature.setAttribute("pic_blob_gid", pic_blob_gid)
            oneFeature.setAttribute("node_id", node_id)
            oneFeature.setGeometry(QgsGeometry.fromPoint(QgsPoint(123, 456)))
            featureList.append(oneFeature)

        #####
        oneFeature = featureList[0]
        fieldList = oneFeature.fields()
        attrList = oneFeature.attributes()
        #####
        self.lineLayer.dataProvider().addFeatures(featureList)
        self.lineLayer.commitChanges()
        QgsMapLayerRegistry.instance().addMapLayer(self.lineLayer) 
        return
    

    def testConnect(self):
        inti = 0
        inti += 1
        return
