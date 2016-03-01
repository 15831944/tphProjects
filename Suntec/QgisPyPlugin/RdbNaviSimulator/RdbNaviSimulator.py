#encoding=utf-8
import resources
from PyQt4.QtGui import QAction, QIcon
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from RdbNaviSimulatorDlg import RdbNaviSimulatorDlg
from RdbWebViewDlg import RdbWebViewDlg
from RdbNaviSimulatorMapTool import RdbNaviSimulatorMapTool

class RdbNaviSimulator:
    def __init__(self, iface):
        self.iface = iface
        self.naviDlg = RdbWebViewDlg(iface)
        self.naviSimulatorMapTool = RdbNaviSimulatorMapTool(self.iface.mapCanvas(), self.naviDlg)
        self.actions = []
        self.menu = u'&Rdb Navi Simulator'
        self.toolbar = self.iface.addToolBar(u'RdbNaviSimulatorToolbar')
        self.toolbar.setObjectName(u'RdbNaviSimulatorToolbar')

    def initGui(self):
        icon_path = ':/icons/GuideIcon.png' 
        actionRdbNavi = QAction(QIcon(icon_path), u"Rdb Navi Simulator", None)
        actionRdbNavi.triggered.connect(self.actionRdbNaviTriggered)
        actionRdbNavi.setStatusTip(None)
        actionRdbNavi.setWhatsThis(None)
        self.toolbar.addAction(actionRdbNavi)
        self.iface.addPluginToMenu(self.menu, actionRdbNavi)
        actionRdbNavi.setEnabled(True)
        actionRdbNavi.setCheckable(True)
        self.actions.append(actionRdbNavi)
        return

    def unload(self):
        for action in self.actions:
            self.iface.removePluginMenu(None, action)
            self.iface.removeToolBarIcon(action)
        return

    def actionRdbNaviTriggered(self):
        # activate our navi simulator map tool
        self.iface.mapCanvas().setMapTool(self.naviSimulatorMapTool)

        # show navi dlg
        self.naviDlg.show()
        result = self.naviDlg.exec_()



