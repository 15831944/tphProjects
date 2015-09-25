import os
import psycopg2

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'nearest_feature_dialog_base.ui'))


class NearestFeatureDialog(QtGui.QDialog, FORM_CLASS):
    def __init__(self, dbManager, parent=None):
        super(NearestFeatureDialog, self).__init__(parent)
        self.setupUi(self)
        self.mDbManager = dbManager
        self.lineEditHost.setText("""192.168.10.14""")
        self.lineEditPort.setText("""5432""")
        self.lineEditDbName.setText("""SGPMYS_RDF_2015Q3_0065_0003""")
        self.lineEditUser.setText("""postgres""")
        self.lineEditPassword.setText("""pset123456""")
        self.textEditSqlCommand.setText(
"""select b.gid as pic_blob_gid, a.node_id, b.data as data, c.the_geom 
from 
rdb_guideinfo_spotguidepoint as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_node as c
on a.node_id=c.node_id
limit 10""")
        self.lineEditNewLayerName.setText("""temp_spotguide_nodes""")
        self.lineEditImageColumn.setText("""data""")
        self.pushButtonGenerateLayer.clicked.connect(self.generateLayer)
        self.pushButtonTestConnect.clicked.connect(self.testConnect)

    # generate a new layer with specified name.   
    def generateLayer(self):
        self.mDbManager.host = self.lineEditHost.text()
        self.mDbManager.port = self.lineEditPort.text()
        self.mDbManager.dbname = self.lineEditDbName.text()
        self.mDbManager.user = self.lineEditUser.text()
        self.mDbManager.password = self.lineEditPassword.text()
        self.mDbManager.userSqlcmd = self.textEditSqlCommand.toPlainText()
        self.mDbManager.newLayerName = self.lineEditNewLayerName.text()
        self.mDbManager.imageColumn = self.lineEditImageColumn.text()
        errMsg = ['']
        self.mDbManager.generateNewLayer(errMsg)
        return
    

    def testConnect(self):
        inti = 0
        inti += 1
        return
