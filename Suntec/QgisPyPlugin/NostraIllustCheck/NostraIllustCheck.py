# -*- coding: utf-8 -*-
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from PyQt4.QtGui import QAction, QIcon
import resources
from NostraIllustCheckDlg import NostraIllustCheckDlg
from NostraIllustCheckMapTool import NostraIllustCheckMapTool

class NostraIllustCheck:
    def __init__(self, iface):
        self.iface = iface
        self.actions = []
        self.menu = u'&Nostra Illust Check'
        self.toolbar = self.iface.addToolBar(u'NostraIllustCheckToolbar')
        self.toolbar.setObjectName(u'NostraIllustCheckToolbar')

    def initGui(self):
        self.nostraIllustCheckMapTool = NostraIllustCheckMapTool(self.iface.mapCanvas())
        icon_path = ':/icons/NostraIllustCheckIcon.png' 
        actionNostraIllustCheck = QAction(QIcon(icon_path), u"Show GuideInfo", None)
        actionNostraIllustCheck.triggered.connect(self.onNostraIllustCheck)
        actionNostraIllustCheck.setStatusTip(None)
        actionNostraIllustCheck.setWhatsThis(None)
        self.toolbar.addAction(actionNostraIllustCheck)
        self.iface.addPluginToMenu(self.menu, actionNostraIllustCheck)
        actionNostraIllustCheck.setEnabled(True)
        actionNostraIllustCheck.setCheckable(True)
        self.actions.append(actionNostraIllustCheck)
        self.nostraIllustCheckMapTool.setAction(actionNostraIllustCheck)
        return

    def unload(self):
        for action in self.actions:
            self.iface.removePluginMenu(None, action)
            self.iface.removeToolBarIcon(action)
        
        # Unset the map tool in case it's set
        self.iface.mapCanvas().unsetMapTool(self.nostraIllustCheckMapTool)
        return
    
    def onNostraIllustCheck(self):
        # activate our spotguide tool
        self.iface.mapCanvas().setMapTool(self.nostraIllustCheckMapTool)
        return
