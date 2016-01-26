# -*- coding: utf-8 -*-
import os
import psycopg2
from GuideDataManager import GuideDataManager
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem, QPainter, QPen, QColor
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'GuideDlgDesign.ui'))

class GuideDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theCanvas, theLayer, selFeatureIds, parent=None):
        super(GuideDlg, self).__init__(parent)
        self.setupUi(self)
        self.mBitToFeatureDict = {  #0: # 高速施設データ有無フラグ
                                    #1: # 駐車場情報有無フラグ
                                    2: self.btnCrossName, # 交差点和普通路路名誘導情報の有無フラグ
                                    3: self.btnTowardName, # 方面名称誘導情報の有無フラグ
                                    4: self.btnSpotguide, # spotguide誘導情報の有無フラグ
                                    5: self.btnSignpost, # 方面看板誘導情報の有無フラグ
                                    6: self.btnLane, # Direction Indicator誘導情報の有無フラグ
                                    7: self.btnRoadStructure, # 道路構造物誘導情報の有無フラグ
                                    8: self.btnBuildingStructure, # 建造物・施設誘導情報の有無フラグ
                                    #9: # 信号灯有无Flag
                                    10: self.btnTollStation, # 收费站
                                    #11: # 北美方面看板诱导情报有无flag
                                    #12: # 边界点flag
                                    13: self.btnCaution, # 诱导cautionflag
                                    #14: # Smart IC flag
                                    15: self.btnForceguide, # force guide诱导情报有无flag
                                    #16: # stop sign诱导情报有无flag
                                    17: self.btnNaturalGuidence, # natural guidence诱导情报有无flag
                                    18: self.btnHookturn, # hook_turn诱导情报有无flag
                                    #19: # Bifurcation诱导情报有无flag
                                    }
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
        self.mSelFeatureIds = selFeatureIds
        self.disableAllGuideBtns()
        self.enableGuideBtns()
        #self.dataManager = GuideDataManager()
        # todo: new a thread to initialise self.dataManager.

        self.btnSpotguide.clicked.connect(self.onBtnSpotguide)
        self.btnSignpost.clicked.connect(self.onBtnSignpost)
        self.btnLane.clicked.connect(self.onBtnLane)
        self.btnBuildingStructure.clicked.connect(self.onBtnBuildingStructure)
        self.btnCaution.clicked.connect(self.onBtnCaution)
        self.btnCrossName.clicked.connect(self.onBtnCrossName)
        self.btnForceguide.clicked.connect(self.onBtnForceguide)
        self.btnHookturn.clicked.connect(self.onBtnHookturn)
        self.btnNaturalGuidence.clicked.connect(self.onBtnNaturalGuidence)
        self.btnRoadStructure.clicked.connect(self.onBtnRoadStructure)
        self.btnSignpostUc.clicked.connect(self.onBtnSignpostUC)
        self.btnTowardName.clicked.connect(self.onBtnTowardName)
        self.btnTollStation.clicked.connect(self.onBtnTollStation)

        self.tabWidgetMain.tabBar().hide()
        self.tabWidgetMain.setCurrentWidget(self.tabDefault)
        #spotguide ui controls
        self.spotguideScene = QGraphicsScene()
        self.spotguidePixmapList = []
        self.graphicsViewSpotguide.setScene(self.spotguideScene)

        # lane ui controls


    def onBtnSpotguide(self):
        self.tabWidgetMain.setCurrentWidget(self.tabSpotguide)
        self.graphicsViewSpotguide
        return

    def onBtnSignpost(self):
        self.tabWidgetMain.setCurrentWidget(self.tabSignpost)
        return
    def onBtnLane(self):
        self.tabWidgetMain.setCurrentWidget(self.tabLane)
        return
    def onBtnBuildingStructure(self):
        self.tabWidgetMain.setCurrentWidget(self.tabBuildingStructure)
        return
    def onBtnCaution(self):
        self.tabWidgetMain.setCurrentWidget(self.tabCaution)
        return
    def onBtnCrossName(self):
        self.tabWidgetMain.setCurrentWidget(self.tabCrossName)
        return
    def onBtnForceguide(self):
        self.tabWidgetMain.setCurrentWidget(self.tabForceguide)
        return
    def onBtnHookturn(self):
        self.tabWidgetMain.setCurrentWidget(self.tabHookturn)
        return
    def onBtnNaturalGuidence(self):
        self.tabWidgetMain.setCurrentWidget(self.tabNaturalGuidence)
        return
    def onBtnRoadStructure(self):
        self.tabWidgetMain.setCurrentWidget(self.tabRoadStructure)
        return
    def onBtnSignpostUC(self):
        self.tabWidgetMain.setCurrentWidget(self.tabSignpostUC)
        return
    def onBtnTowardName(self):
        self.tabWidgetMain.setCurrentWidget(self.tabTowardName)
        return
    def onBtnTollStation(self):
        self.tabWidgetMain.setCurrentWidget(self.tabTollStation)
        return

    def enableGuideBtns(self):
        errMsg = ['']
        for theFeatureId in self.mSelFeatureIds:
            theFeature = self.getFeatureByFeatureId(errMsg, theFeatureId)
            if errMsg[0] <> '':
                return
            extend_flag = theFeature.attribute('extend_flag')
            for key, value in self.mBitToFeatureDict.items():
                if extend_flag & 2**key:
                    value.setEnabled(True)
        return

    def disableAllGuideBtns(self):
        self.btnSpotguide.setEnabled(False)
        self.btnSignpost.setEnabled(False)
        self.btnLane.setEnabled(False)
        self.btnBuildingStructure.setEnabled(False)
        self.btnCaution.setEnabled(False)
        self.btnCrossName.setEnabled(False)
        self.btnForceguide.setEnabled(False)
        self.btnHookturn.setEnabled(False)
        self.btnNaturalGuidence.setEnabled(False)
        self.btnRoadStructure.setEnabled(False)
        self.btnSignpostUc.setEnabled(False)
        self.btnTowardName.setEnabled(False)
        self.btnTollStation.setEnabled(False)
        return

    def getFeatureByFeatureId(self, errMsg, theFeatureId):
        featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest(theFeatureId).setFlags(QgsFeatureRequest.NoGeometry))
        theFeature = QgsFeature()
        if featureIter.nextFeature(theFeature) == False:
            errMsg[0] = '''can't get feature by specified feature id: %s.''' % theFeatureId
            return None
        return theFeature

    def mIsMyFeature(self, theFeature):
        return GuideDlg.isMyFeature(theFeature)

    @staticmethod
    def isMyFeature(theFeature):
        try:
            gid = theFeature.attribute('gid')
            node_id = theFeature.attribute('node_id')
            node_id_t = theFeature.attribute('node_id_t')
            extend_flag = theFeature.attribute('extend_flag')
            link_num = theFeature.attribute('link_num')
            branches = theFeature.attribute('branches')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True



