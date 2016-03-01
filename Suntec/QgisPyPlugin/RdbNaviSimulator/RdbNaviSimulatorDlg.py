import os
import psycopg2
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from qgis.core import QgsVectorLayer, QgsMapLayerRegistry, QgsDataSourceURI
from RdbNaviSimulatorMapTool import RdbNaviSimulatorMapTool
from ComputePath.RdbComputePath import RdbComputePath

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'RdbNaviSimulatorDlgDesign.ui'))

class RdbNaviSimulatorDlg(QtGui.QDialog, FORM_CLASS):
    EDIT_NODE_START = 1
    EDIT_NODE_END = 2
    def __init__(self, iface, parent=None):
        super(RdbNaviSimulatorDlg, self).__init__(parent)
        self.setupUi(self)
        self.iface = iface
        self.rdbNaviMapTool = RdbNaviSimulatorMapTool(self.iface.mapCanvas(), self)
        self.pushButtonCalculatePath.clicked.connect(self.onPushButtonCalculatePath)
        # for keep status of editing start node? or end node?
        self.editNodeFlag = RdbNaviSimulatorDlg.EDIT_NODE_START

    def onPushButtonCalculatePath(self):
        startNodeId = int(self.lineEditStart.text())
        endNodeId = int(self.lineEditEnd.text())

        errMsg = ['']
        dbIP = '''192.168.10.20'''
        dbNameList = ['C_NIMIF_15SUM_0082_0003']
        userName = '''postgres'''
        password = '''pset123456'''
        linkList = []
        for dbName in dbNameList:
            try:
                computePath = RdbComputePath(dbIP, dbName, userName, password)
                linkList = computePath.computePath_L14(errMsg, startNodeId, endNodeId)
            except Exception, ex:
                errMsg = '''%s/%s.\nmsg:\t%s\n''' % (dbIP, dbName, ex)
                QMessageBox.information(self, "Rdb Simulator", errMsg)
                return
        strTemp = ",\n".join(str(x) for x in linkList)
        self.plainTextEditResult.clear()
        self.plainTextEditResult.appendPlainText(strTemp)
        return

    def addNode(self, nodeId):
        if self.editNodeFlag == RdbNaviSimulatorDlg.EDIT_NODE_START:
            self.lineEditStart.setText(str(nodeId))
            self.lineEditStart.selectAll()
            self.editNodeFlag = RdbNaviSimulatorDlg.EDIT_NODE_END
        else:
            self.lineEditEnd.setText(str(nodeId))
            self.lineEditEnd.selectAll()
            self.editNodeFlag = RdbNaviSimulatorDlg.EDIT_NODE_START
        return

