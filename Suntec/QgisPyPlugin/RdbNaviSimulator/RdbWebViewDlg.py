import os
import psycopg2
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from PyQt4.QtCore import QUrl

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'RdbWebViewDlgDesign.ui'))

class RdbWebViewDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, iface, parent=None):
        super(RdbWebViewDlg, self).__init__(parent)
        self.setupUi(self)
        self.iface = iface
        self.webViewMain.setUrl(QUrl(r"http://www.earthol.org/city-4.html"))
        return