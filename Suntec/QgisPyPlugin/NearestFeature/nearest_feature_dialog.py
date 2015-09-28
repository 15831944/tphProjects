import os
import psycopg2
from myDbManager import myDbManager
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'nearest_feature_dialog_base.ui'))


class NearestFeatureDialog(QtGui.QDialog, FORM_CLASS):
    def __init__(self, parent=None):
        super(NearestFeatureDialog, self).__init__(parent)
        self.setupUi(self)
        self.lineEditHost.setText("""192.168.10.14""")
        self.lineEditPort.setText("""5432""")
        self.lineEditDbName.setText("""post13_NI_Dummy_CI""")
        self.lineEditUser.setText("""postgres""")
        self.lineEditPassword.setText("""pset123456""")
        self.textEditSqlCommand.setText(
"""select b.gid as pic_blob_gid, a.node_id, a.type, b.data as data, c.the_geom 
from 
rdb_guideinfo_spotguidepoint as a
left join rdb_guideinfo_pic_blob_bytea as b
on a.pattern_id=b.gid
left join rdb_node as c
on a.node_id=c.node_id
limit 10""")
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
        userSqlcmd = self.textEditSqlCommand.toPlainText()
        newLayerName = self.lineEditNewLayerName.text()
        errMsg = ['']
        dbManager = myDbManager()
        dbManager.generateNewLayer(errMsg, host, port, dbname, user, password, 
                                   userSqlcmd, newLayerName)
        return
    

    def testConnect(self):
        inti = 0
        inti += 1
        return
