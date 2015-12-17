#encoding=utf-8
import resources
from PyQt4.QtGui import QAction, QIcon
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from RdbNaviSimulatorMapTool import RdbNaviSimulatorMapTool
from RdbNaviSimulatorDlg import RdbNaviSimulatorDlg

class RdbNaviSimulator:
    def __init__(self, iface):
        self.iface = iface
        self.naviDlg = RdbNaviSimulatorDlg()
        self.actions = []
        self.menu = u'&Rdb Navi Simulator'
        self.toolbar = self.iface.addToolBar(u'RdbNaviSimulatorToolbar')
        self.toolbar.setObjectName(u'RdbNaviSimulatorToolbar')

    def initGui(self):
        icon_path = ':/icons/GuideIcon.png' 
        actionRdbNavi = QAction(QIcon(icon_path), u"Rdb Navi Simulator", None)
        actionRdbNavi.triggered.connect(self.showNaviDlg)
        actionRdbNavi.setStatusTip(None)
        actionRdbNavi.setWhatsThis(None)
        self.toolbar.addAction(actionRdbNavi)
        self.iface.addPluginToMenu(self.menu, actionRdbNavi)
        actionRdbNavi.setEnabled(True)
        actionRdbNavi.setCheckable(True)
        self.actions.append(actionRdbNavi)
        self.guideInfoMapTool = RdbNaviSimulatorMapTool(self.iface.mapCanvas(), self.naviDlg)
        self.guideInfoMapTool.setAction(actionRdbNavi)
        return

    def unload(self):
        for action in self.actions:
            self.iface.removePluginMenu(None, action)
            self.iface.removeToolBarIcon(action)
        # Unset the map tool in case it's set
        self.iface.mapCanvas().unsetMapTool(self.guideInfoMapTool)
        return

    def showNaviDlg(self):
        self.naviDlg.exec_()
        self.naviDlg.show()
        # activate our spotguide tool
        self.iface.mapCanvas().setMapTool(self.guideInfoMapTool)
        return
