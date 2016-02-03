# -*- coding: utf-8 -*-
import os
import psycopg2
from NodeDataItem import NodeDataItem
from LaneDataItem import LaneDataItem
from GuideDataManager import GuideDataManager
from GuideinfoCommon import GuideinfoCommon
from LaneShowImageWidget import LaneShowImageWidget

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QPainter, QPen, QColor, QBrush
from PyQt4.QtCore import QRectF, Qt
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature, QgsGeometry
from qgis.gui import QgsHighlight

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
        self.theCanvas = theCanvas
        self.theLayer = theLayer
        self.selFeatureIds = selFeatureIds
        self.conn = None
        self.pg = None
        self.nodeDataItemList = []
        self.highlightList = []

        self.fillNodeDataList()
        self.initPg()
        self.disableAllGuideBtns()
        self.enableGuideBtns()
        self.dataManager = GuideDataManager(self.pg, self.nodeDataItemList)
        self.dataManager.initData()

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
        self.connect(self.comboBoxSpotguideOutlinkid, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxSpotguideOutlinkidChanged)

        # lane ui controls
        self.laneCtrl = LaneShowImageWidget()
        self.comboBoxLaneOutlinkid = QtGui.QComboBox()
        self.textEditLane = QtGui.QTextEdit()
        self.gridLayoutLane = QtGui.QGridLayout()
        self.gridLayoutLane.addWidget(self.laneCtrl, 0, 0)
        self.gridLayoutLane.addWidget(self.comboBoxLaneOutlinkid, 1, 0)
        self.gridLayoutLane.addWidget(self.textEditLane, 2, 0)
        self.gridLayoutLane.setRowStretch(0, 1)
        self.gridLayoutLane.setRowStretch(1, 2)
        self.gridLayoutLane.setRowStretch(2, 5)
        self.tabLane.setLayout(self.gridLayoutLane)
        self.connect(self.comboBoxLaneOutlinkid, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxLaneOutlinkidChanged)

    def resizeEvent(self, event):
        if self.tabWidgetMain.currentWidget() == self.tabSpotguide:
            self.showSpotguidePixmaps()
        return

    def onBtnSpotguide(self):
        if self.tabWidgetMain.currentWidget() == self.tabSpotguide:
            return

        self.tabWidgetMain.setCurrentWidget(self.tabSpotguide)
        self.initComboBoxSpotguideOutlinkid()
        self.comboBoxSpotguideOutlinkid.setFocus()
        errMsg = ['']
        self.showSpotguideDetail(errMsg, self.comboBoxSpotguideOutlinkid.currentText())
        if errMsg[0] != '':
            QMessageBox.information(self, "Show Spotguide", errMsg[0])
            return
        return

    def initComboBoxSpotguideOutlinkid(self):
        while(self.comboBoxSpotguideOutlinkid.count() > 0):
            self.comboBoxSpotguideOutlinkid.removeItem(0)
        for oneKey in self.dataManager.spotguideDataItemDict:
            self.comboBoxSpotguideOutlinkid.addItem(oneKey)

    def comboBoxSpotguideOutlinkidChanged(self, txt):
        errMsg = ['']
        self.showSpotguideDetail(errMsg, txt)
        if errMsg[0] != '':
            QMessageBox.information(self, "Show Spotguide", errMsg[0])
        return

    def showSpotguideDetail(self, errMsg, spotguideItemKey):
        if self.dataManager.spotguideDataItemDict.has_key(spotguideItemKey) == False:
            errMsg[0] = """has no key: %s.""" % spotguideItemKey
            return

        spotguideItem = self.dataManager.spotguideDataItemDict[spotguideItemKey]
        self.textEditSpotguideInfo.setText(spotguideItem.toString())

        self.spotguidePixmapList = []
        patternPixmap = QPixmap()
        patternData = spotguideItem.getPatternPicture(errMsg)
        if errMsg[0] <> '':
            return
        patternPixmap.loadFromData(patternData)
        self.spotguidePixmapList.append(patternPixmap)

        arrowPixmap = QPixmap()
        arrowData = spotguideItem.getArrowPicture(errMsg)
        if errMsg[0] == '':
            arrowPixmap.loadFromData(arrowData)
            self.spotguidePixmapList.append(arrowPixmap)
        else:
            errMsg[0] = ''
        self.showSpotguidePixmaps()
        self.clearHighlight()
        self.highlightByLinkIdList(errMsg, [spotguideItem.in_link_id], QColor(0, 255, 0, 128))
        self.highlightByLinkIdList(errMsg, [spotguideItem.out_link_id], QColor(255, 0, 0, 128))
        return
    
    def showSpotguidePixmaps(self):
        # remove all items in spotguide graphics view.
        for oneItem in self.spotguideScene.items():
            self.spotguideScene.removeItem(oneItem)

        for onePixmap in self.spotguidePixmapList:
            self.spotguideScene.addPixmap(self.getPixMapSizedByWidget(onePixmap, self.graphicsViewSpotguide))

        self.spotguideScene.setSceneRect(0, 0, self.graphicsViewSpotguide.width()-5, self.graphicsViewSpotguide.height()-5)
        return

    def getPixMapSizedByWidget(self, pixmap, theWidgt):
        return pixmap.scaled(theWidgt.width(),
                             theWidgt.height(),
                             QtCore.Qt.IgnoreAspectRatio,
                             QtCore.Qt.SmoothTransformation)

    def onBtnSignpost(self):
        self.tabWidgetMain.setCurrentWidget(self.tabSignpost)
        return

    def onBtnLane(self):
        if self.tabWidgetMain.currentWidget() == self.tabLane:
            return
        self.tabWidgetMain.setCurrentWidget(self.tabLane)
        self.initComboboxLaneOutlinkid()
        if self.comboBoxLaneOutlinkid.count() > 0:
            self.comboBoxLaneOutlinkidChanged(self.comboBoxLaneOutlinkid.currentText())
        return

    def initComboboxLaneOutlinkid(self):
        while(self.comboBoxLaneOutlinkid.count() > 0):
            self.comboBoxLaneOutlinkid.removeItem(0)
        for inlinkid, dict1 in self.dataManager.laneDataItemDict.items():
            for outlinkid, laneItemList in dict1.items():
                for oneLaneItem in laneItemList:
                    self.comboBoxLaneOutlinkid.addItem(oneLaneItem.getKeyString())
        return

    def comboBoxLaneOutlinkidChanged(self, txt):
        errMsg = ['']
        self.showLaneDetail(errMsg, txt)
        if errMsg[0] != '':
            QMessageBox.information(self, "Show Spotguide", errMsg[0])
        return

    def showLaneDetail(self, errMsg, comboboxTxt):
        inlinkId, nodeId, outlinkId = LaneDataItem.parseKeyString(errMsg, comboboxTxt)
        if errMsg[0] <> '':
            return

        grayItemList = []
        laneDataItemDict = self.dataManager.laneDataItemDict
        for oneOutlinkId, laneItemList in (laneDataItemDict[inlinkId]).items():
            grayItemList.extend(laneItemList)
        highlightItemList = laneDataItemDict[inlinkId][outlinkId]
        self.laneCtrl.initLanePixmaps(grayItemList, highlightItemList)
        self.laneCtrl.update()
        self.clearHighlight()
        self.textEditLane.setText(highlightItemList[0].toString())
        self.highlightByLinkIdList(errMsg, [inlinkId], QColor(0, 255, 0, 128))
        self.highlightByLinkIdList(errMsg, [outlinkId], QColor(255, 0, 0, 128))
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

    # enable the buttons reflecting rdb_node.extendflag attribute.
    def enableGuideBtns(self):
        errMsg = ['']
        for oneNodeItem in self.nodeDataItemList:
            for key, value in self.mBitToFeatureDict.items():
                if oneNodeItem.extend_flag & 2**key:
                    value.setEnabled(True)
        return

    # this plugin base on rdb_node table, so when some nodes are selected, 
    # they should be added to self.nodeDataItemList.
    def fillNodeDataList(self):
        errMsg = ['']
        for theFeatureId in self.selFeatureIds:
            theFeature = self.getFeatureByFeatureId(errMsg, theFeatureId)
            if errMsg[0] <> '':
                continue

            nodeDataItem = self.featureToNodeDataItem(errMsg, theFeature)
            if errMsg[0] <> '':
                continue
            self.nodeDataItemList.append(nodeDataItem)
        return

    def initPg(self):
        uri = QgsDataSourceURI(self.theLayer.source())
        self.conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
            (uri.host(), uri.database(), uri.username(), uri.password()))
        self.pg = self.conn.cursor()
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
        featureIter = self.theLayer.getFeatures(QgsFeatureRequest(theFeatureId).setFlags(QgsFeatureRequest.NoGeometry))
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

    # convert feature to node data item.
    def featureToNodeDataItem(self, errMsg, theFeature):
        if self.mIsMyFeature(theFeature) == False:
            errMsg[0] = """input feature is not Rdb_Node feature."""
            return None

        gid = theFeature.attribute('gid')
        node_id = theFeature.attribute('node_id')
        node_id_t = theFeature.attribute('node_id_t')
        extend_flag = theFeature.attribute('extend_flag')
        link_num = theFeature.attribute('link_num')
        branches = theFeature.attribute('branches')
        nodeDataItem = NodeDataItem()
        nodeDataItem.initNormal(gid, node_id, node_id_t, extend_flag, link_num, branches)
        return nodeDataItem

    def highlightByLinkIdList(self, errMsg, linkIdList, color=QColor(255,0,0,128)):
        strLinkList = ','.join(str(x).strip('L') for x in linkIdList)
        sqlcmd = """
select st_asbinary(the_geom) from rdb_link where link_id in (%s)
""" % strLinkList
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            qgsGeometry = QgsGeometry()
            qgsGeometry.fromWkb(str(row[0]))
            self.highlightByGeometry(qgsGeometry, color)

    def highlightByGeometry(self, geometry, color=QColor(255,0,0,128)):
        highlight = QgsHighlight(self.theCanvas, geometry, self.theLayer)
        highlight.setColor(color)
        highlight.setFillColor(color)
        highlight.setBuffer(0.5)
        highlight.setMinWidth(6)
        highlight.setWidth(6)
        highlight.show()
        self.highlightList.append(highlight)
        return

    def clearHighlight(self):
        for oneHighlight in self.highlightList:
            del oneHighlight
        self.highlightList = []
        return





