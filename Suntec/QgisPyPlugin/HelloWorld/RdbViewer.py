# -*- coding: utf-8 -*-
from PyQt4.QtCore import QSettings, QTranslator, qVersion, QCoreApplication
from PyQt4.QtGui import QAction, QIcon
import resources_rc
from GenerateNewLayerDlg import GenerateNewLayerDlg
from SpotguideMapTool import SpotguideMapTool
import os.path

class RdbViewer:
    def __init__(self, iface):
        self.iface = iface
        self.plugin_dir = os.path.dirname(__file__)
        locale = QSettings().value('locale/userLocale')[0:2]
        locale_path = os.path.join(
            self.plugin_dir,
            'i18n',
            'RdbViewer_{}.qm'.format(locale))

        if os.path.exists(locale_path):
            self.translator = QTranslator()
            self.translator.load(locale_path)
            if qVersion() > '4.3.3':
                QCoreApplication.installTranslator(self.translator)

        self.dlg = GenerateNewLayerDlg()
        self.actions = []
        self.menu = self.tr(u'&RdbViewer0')
        self.toolbar = self.iface.addToolBar(u'RdbViewer1')
        self.toolbar.setObjectName(u'RdbViewer2')

    def tr(self, message):
        return QCoreApplication.translate('RdbViewer3', message)


    def add_action(
        self,
        icon_path,
        text,
        callback,
        enabled_flag=True,
        add_to_menu=True,
        add_to_toolbar=True,
        status_tip=None,
        whats_this=None,
        parent=None):
        icon = QIcon(icon_path)
        action = QAction(icon, text, parent)
        action.triggered.connect(callback)
        action.setEnabled(enabled_flag)

        if status_tip is not None:
            action.setStatusTip(status_tip)

        if whats_this is not None:
            action.setWhatsThis(whats_this)

        if add_to_toolbar:
            self.toolbar.addAction(action)

        if add_to_menu:
            self.iface.addPluginToMenu(
                self.menu,
                action)

        self.actions.append(action)
        return action

    def initGui(self):
        icon_path = ':/plugins/NearestFeature/icon.png'       
        action = self.add_action(
                    icon_path,
                    text=self.tr(u'Select nearest feature.'),
                    callback=self.run,
                    parent=self.iface.mainWindow())
        
        action.setCheckable(True)
        # Create a new NearestFeatureMapTool and keep reference
        self.nearestFeatureMapTool = SpotguideMapTool(self.iface.mapCanvas())
        self.nearestFeatureMapTool.setAction(action)

        actionGenerateLayer = self.add_action(
                    icon_path, 
                    text=self.tr(u'Generate a special layer.'),
                    callback = self.generateLayer,
                    parent = self.iface.mainWindow())
        actionGenerateLayer.setCheckable(True)

    def unload(self):
        for action in self.actions:
            self.iface.removePluginMenu(
                self.tr(u'&Nearest Feature'),
                action)
            self.iface.removeToolBarIcon(action)
        
        # Unset the map tool in case it's set
        self.iface.mapCanvas().unsetMapTool(self.nearestFeatureMapTool)
        return

    def run(self):
        # activate our tool
        self.iface.mapCanvas().setMapTool(self.nearestFeatureMapTool)
        return

    def generateLayer(self):
        self.dlg.show()
        result = self.dlg.exec_()
        return

    


































