import os
import psycopg2
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from qgis.core import QgsVectorLayer, QgsMapLayerRegistry, QgsDataSourceURI

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'RdbNaviSimulatorDlgDesign.ui'))

class RdbNaviSimulatorDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, parent=None):
        super(RdbNaviSimulatorDlg, self).__init__(parent)
        self.setupUi(self)