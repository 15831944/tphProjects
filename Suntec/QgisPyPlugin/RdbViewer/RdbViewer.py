# -*- coding: utf-8 -*-
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from PyQt4.QtGui import QAction, QIcon
import resources
from GenerateNewLayerDlg import GenerateNewLayerDlg
from SpotguideMapTool import SpotguideMapTool
import os.path

class RdbViewer:
    def __init__(self, iface):
        self.iface = iface
        self.dlg = GenerateNewLayerDlg()
        self.actions = []
        self.menu = u'&Rdb Viewer'
        self.toolbar = self.iface.addToolBar(u'RdbViewerToolbar')
        self.toolbar.setObjectName(u'RdbViewerToolbar')

    def initGui(self):
        icon_path = ':/icons/GenearteNewLayer.png' 
        actionGenerateLayer = QAction(QIcon(icon_path), u"Generate Layer", None)
        actionGenerateLayer.triggered.connect(self.generateLayer)
        actionGenerateLayer.setStatusTip(None)
        actionGenerateLayer.setWhatsThis(None)
        self.toolbar.addAction(actionGenerateLayer)
        self.iface.addPluginToMenu(self.menu, actionGenerateLayer)
        actionGenerateLayer.setEnabled(True)
        self.actions.append(actionGenerateLayer)


        icon_path = ':/icons/SpotguideShowImage.png' 
        actionShowSpotguide = QAction(QIcon(icon_path), u"Show Spotguide", None)
        actionShowSpotguide.triggered.connect(self.showSpotguide)
        actionShowSpotguide.setStatusTip(None)
        actionShowSpotguide.setWhatsThis(None)
        self.toolbar.addAction(actionShowSpotguide)
        self.iface.addPluginToMenu(self.menu, actionShowSpotguide)
        actionShowSpotguide.setEnabled(True)
        actionShowSpotguide.setCheckable(True)
        self.actions.append(actionShowSpotguide)

        self.spotguideMapTool = SpotguideMapTool(self.iface.mapCanvas())
        self.spotguideMapTool.setAction(actionShowSpotguide)
        return

    def unload(self):
        for action in self.actions:
            self.iface.removePluginMenu(None, action)
            self.iface.removeToolBarIcon(action)
        
        # Unset the map tool in case it's set
        self.iface.mapCanvas().unsetMapTool(self.spotguideMapTool)
        return

    def showSpotguide(self):
        # activate our spotguide tool
        self.iface.mapCanvas().setMapTool(self.spotguideMapTool)
        return

    def generateLayer(self):
        result = self.dlg.exec_()
        return

    


































