# -*- coding: utf-8 -*-
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from PyQt4.QtGui import QAction, QIcon
import resources
from GenerateNewLayerDlg import GenerateNewLayerDlg
from GuideMapTool import GuideMapTool

class RdbViewer:
    def __init__(self, iface):
        self.iface = iface
        self.generateLayerDlg = GenerateNewLayerDlg()
        self.actions = []
        self.menu = u'&Rdb Viewer'
        self.toolbar = self.iface.addToolBar(u'RdbViewerToolbar')
        self.toolbar.setObjectName(u'RdbViewerToolbar')

    def initGui(self):
        icon_path = ':/icons/NewLayerIcon.png' 
        actionGenerateLayer = QAction(QIcon(icon_path), u"Generate Layer", None)
        actionGenerateLayer.triggered.connect(self.generateLayer)
        actionGenerateLayer.setStatusTip(None)
        actionGenerateLayer.setWhatsThis(None)
        self.toolbar.addAction(actionGenerateLayer)
        self.iface.addPluginToMenu(self.menu, actionGenerateLayer)
        actionGenerateLayer.setEnabled(True)
        self.actions.append(actionGenerateLayer)

        icon_path = ':/icons/GuideIcon.png' 
        actionShowGuideInfo = QAction(QIcon(icon_path), u"Show GuideInfo", None)
        actionShowGuideInfo.triggered.connect(self.showGuideInfo)
        actionShowGuideInfo.setStatusTip(None)
        actionShowGuideInfo.setWhatsThis(None)
        self.toolbar.addAction(actionShowGuideInfo)
        self.iface.addPluginToMenu(self.menu, actionShowGuideInfo)
        actionShowGuideInfo.setEnabled(True)
        actionShowGuideInfo.setCheckable(True)
        self.actions.append(actionShowGuideInfo)
        self.guideInfoMapTool = GuideMapTool(self.iface.mapCanvas())
        self.guideInfoMapTool.setAction(actionShowGuideInfo)
        return

    def unload(self):
        for action in self.actions:
            self.iface.removePluginMenu(None, action)
            self.iface.removeToolBarIcon(action)
        
        # Unset the map tool in case it's set
        self.iface.mapCanvas().unsetMapTool(self.guideInfoMapTool)
        return
    
    def generateLayer(self):
        result = self.generateLayerDlg.exec_()
        return

    def showGuideInfo(self):
        # activate our spotguide tool
        self.iface.mapCanvas().setMapTool(self.guideInfoMapTool)
        return
