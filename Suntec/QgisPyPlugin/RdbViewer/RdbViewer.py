# -*- coding: utf-8 -*-
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from PyQt4.QtGui import QAction, QIcon
import resources
from GenerateNewLayerDlg import GenerateNewLayerDlg
from SpotguideMapTool import SpotguideMapTool
from LaneMapTool import LaneMapTool

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

        icon_path = ':/icons/LaneShowImage.png' 
        actionShowLane = QAction(QIcon(icon_path), u"Show Lane", None)
        actionShowLane.triggered.connect(self.showLane)
        actionShowLane.setStatusTip(None)
        actionShowLane.setWhatsThis(None)
        self.toolbar.addAction(actionShowLane)
        self.iface.addPluginToMenu(self.menu, actionShowLane)
        actionShowLane.setEnabled(True)
        actionShowLane.setCheckable(True)
        self.actions.append(actionShowLane)
        self.laneMapTool = LaneMapTool(self.iface.mapCanvas())
        self.laneMapTool.setAction(actionShowLane)
        return

    def unload(self):
        for action in self.actions:
            self.iface.removePluginMenu(None, action)
            self.iface.removeToolBarIcon(action)
        
        # Unset the map tool in case it's set
        self.iface.mapCanvas().unsetMapTool(self.spotguideMapTool)
        self.iface.mapCanvas().unsetMapTool(self.laneMapTool)
        return
    
    def generateLayer(self):
        result = self.dlg.exec_()
        return

    def showSpotguide(self):
        # activate our spotguide tool
        self.iface.mapCanvas().setMapTool(self.spotguideMapTool)
        return

    def showLane(self):
        # activate our lane tool
        self.iface.mapCanvas().setMapTool(self.laneMapTool)
        return

    


































