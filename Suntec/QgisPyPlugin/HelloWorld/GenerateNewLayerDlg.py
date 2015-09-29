import os
import psycopg2
from MyDbManager import MyDbManager, FeatureTypeList
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'GenerateNewLayerDlg_design.ui'))

class GenerateNewLayerDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, parent=None):
        super(GenerateNewLayerDlg, self).__init__(parent)
        self.setupUi(self)
        self.lineEditHost.setText("""192.168.10.14""")
        self.lineEditPort.setText("""5432""")
        self.lineEditDbName.setText("""post13_NI_Dummy_CI""")
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
        errMsg = ['']
        dbManager = MyDbManager()
        dbManager.generateNewLayer(errMsg, host, port, dbname, user, password, 
                                   newLayerName, featureType)
        if errMsg[0] != '':
            QMessageBox.information(self, "error", errMsg[0])
        return
    

    def testConnect(self):
        inti = 0
        inti += 1
        return

    def comboBoxSelectFeatureChanged(self, txt):
        return
