# -*- coding: utf-8 -*-
import os
import psycopg2
from DatParser import DatParser
from LaneDataItem import LaneDataItem

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'LaneShowImageDlgDesign.ui'))

class LaneShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theCanvas, theLayer, selFeatureIds, parent=None):
        super(LaneShowImageDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
        self.mSelFeatureIds = selFeatureIds
        self.mAllFeatureIds = []
        self.graphicViewsMap = {
                0 : self.graphicView_00,
                1 : self.graphicView_01,
                2 : self.graphicView_02,
                3 : self.graphicView_03,
                4 : self.graphicView_04,
                5 : self.graphicView_05,
                6 : self.graphicView_06,
                7 : self.graphicView_07,
                8 : self.graphicView_08,
                9 : self.graphicView_09,
                10 : self.graphicView_10,
                11 : self.graphicView_11,
                12 : self.graphicView_12,
                13 : self.graphicView_13,
                14 : self.graphicView_14,
                15 : self.graphicView_15
                }

        self.initAllGraphicViews()
        featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest().setFlags(QgsFeatureRequest.NoGeometry))
        inti = 0
        theFeature = QgsFeature()
        while(featureIter.nextFeature(theFeature) and inti<512):
            inti += 1
            self.mAllFeatureIds.append(theFeature.id())
        self.spinBoxFeatureIndex.setValue(1)
        self.spinBoxFeatureIndex.setMinimum(1)
        self.spinBoxFeatureIndex.setMaximum(inti)

        errMsg = ['']
        self.initComboBoxOutlinkid()
        self.selectFeatureById(errMsg, self.mSelFeatureIds[0], bZoomToSelected=False)
        self.comboBoxOutlinkid.setFocus()

        self.pushButtonPrev.clicked.connect(self.onPushButtonPrev)
        self.pushButtonNext.clicked.connect(self.onPushButtonNext)
        self.connect(self.comboBoxOutlinkid, QtCore.SIGNAL('activated(QString)'), self.comboBoxOutlinkidChanged)
        

    def disableAllControls(self):
        self.pushButtonPrev.setEnabled(False)
        self.pushButtonPrev.setEnabled(False)
        self.comboBoxOutlinkid.setEnabled(False)
        self.textEditFeatureInfo.setEnabled(False)
        return

    def initComboBoxOutlinkid(self):
        while(self.comboBoxOutlinkid.count() > 0):
            self.comboBoxOutlinkid.removeItem(0)
        for oneFeatureId in self.mSelFeatureIds:
            featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest(oneFeatureId).setFlags(QgsFeatureRequest.NoGeometry))
            theFeature = QgsFeature()
            if featureIter.nextFeature(theFeature) == False:
                return
            if self.mIsMyFeature(theFeature):
                self.comboBoxOutlinkid.addItem(str(theFeature.attribute('out_link_id')))

    def getFeatureInfoString(self, theFeature):
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFeatureInfo = "field count: %d\n" % len(fieldList)
        for oneField, oneAttr in zip(fieldList, attrList):
            if isinstance(oneAttr, float):
                strFeatureInfo += "%s: %.0f\n" % (oneField.name(), oneAttr)
            else:
                strFeatureInfo += "%s: %s\n" % (oneField.name(), oneAttr)
        return strFeatureInfo

    def comboBoxOutlinkidChanged(self, txt):
        errMsg = ['']
        inti = self.comboBoxOutlinkid.currentIndex()
        self.selectFeatureById(errMsg, self.mSelFeatureIds[inti], bZoomToSelected=False)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Lane Info", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonPrev(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()-1)
        prevFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, prevFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Lane Info", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonNext(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()+1)
        nextFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, nextFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Lane Info", """error:\n%s"""%errMsg[0])
            return
        return

    def selectFeatureById(self, errMsg, featureId, bZoomToSelected=True):
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(featureId)
        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[0])
        if errMsg[0] <> '':
            return
        if bZoomToSelected == True:
            center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
            self.mTheCanvas.refresh()
        return

    def mIsMyFeature(self, theFeature):
        return LaneShowImageDlg.isMyFeature(theFeature)

    @staticmethod
    def isMyFeature(theFeature):
        # check if this feature is a rdb_guideinfo_lane's instance.
        try:
            in_link_id = theFeature.attribute('in_link_id')
            in_link_id_t = theFeature.attribute('in_link_id_t')
            node_id = theFeature.attribute('node_id')
            node_id_t = theFeature.attribute('node_id_t')
            out_link_id = theFeature.attribute('out_link_id')
            out_link_id_t = theFeature.attribute('out_link_id_t')
            lane_num = theFeature.attribute('lane_num')
            lane_info = theFeature.attribute('lane_info')
            arrow_info = theFeature.attribute('arrow_info')
            lane_num_l = theFeature.attribute('lane_num_l')
            lane_num_r = theFeature.attribute('lane_num_r')
            passlink_count = theFeature.attribute('passlink_count')
            #exclusive = theFeature.attribute('exclusive') # jdb has no this column
            #order_id = theFeature.attribute('order_id') # jdb has no this column
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True

    def showFeatureDetail(self, errMsg, theFeature):
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        if LaneShowImageDlg.isMyFeature(theFeature) == False:
            return

        try:
            self.hideAllGraphicViews()
            self.clearAllGraphicViewsContent()
            uri = QgsDataSourceURI(self.mTheLayer.source())
            conn = psycopg2.connect("""host='%s' dbname='%s' user='%s' password='%s'""" %\
                (uri.host(), uri.database(), uri.username(), uri.password()))
            pg = conn.cursor()

            # draw all lanes in this link
            in_link_id = theFeature.attribute('in_link_id')
            node_id = theFeature.attribute('node_id')
            out_link_id = theFeature.attribute('out_link_id')
            strFilter = '''in_link_id=%s and node_id=%s''' % (in_link_id, node_id)
            sqlcmd = """select lane_num, lane_info, arrow_info
                        from %s
                        where %s""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            rows = pg.fetchall()
            if len(rows) <= 0:
                errMsg[0] = '''get no lane record.'''
                return
            totalLaneCount = rows[0][0] # lane count of inlink.
            self.showNeededGraphicViews(totalLaneCount)

            for row in rows:
                lane_num = row[0] # total lane count in this link
                lane_info = row[1] # which lane is specified
                arrow_info = row[2] # arrow catagory

                if lane_num != totalLaneCount:
                    errMsg[0] = """some lane_count not the same."""
                    return
                self.drawLaneGray(errMsg, lane_num, lane_info, arrow_info)
                if errMsg[0] <> '':
                    return

            # draw highlighted guide lanes 
            strFilter = '''in_link_id=%s and node_id=%s and out_link_id=%s''' %\
                        (in_link_id, node_id, out_link_id)
            sqlcmd = """select lane_num, lane_info, arrow_info
                        from %s
                        where %s""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            rows = pg.fetchall()
            if len(rows) <= 0:
                errMsg[0] = '''get no lane record.'''
                return
            totalLaneCount = rows[0][0] # lane count of inlink.
            for row in rows:
                lane_num = row[0] # total lane count in this link
                lane_info = row[1] # which lane is specified
                arrow_info = row[2] # arrow catagory

                if lane_num != totalLaneCount:
                    errMsg[0] = """some lane_count not the same."""
                    return
                self.drawLaneHighlight(errMsg, lane_num, lane_info, arrow_info)
                if errMsg[0] <> '':
                    return
            return 
        except KeyError, kErr:
            errMsg[0] = """Selected feature is not a rdb lane feature."""
            return
        except Exception, ex:
            errMsg[0] = ex.message
            return
        return

    def drawLaneHighlight(self, errMsg, totalLaneCount, whichLane, arrowInfo):
        self.drawLane(errMsg, totalLaneCount, whichLane, 
                      arrowInfo, LaneDataItem.arrowImagesMap_highlight)
        return

    def drawLaneGray(self, errMsg, totalLaneCount, whichLane, arrowInfo):
        self.drawLane(errMsg, totalLaneCount, whichLane, 
                      arrowInfo, LaneDataItem.arrowImagesMap_gray)
        return

    def drawLane(self, errMsg, totalLaneCount, whichLane, arrowInfo, arrowImagesMap):
        if whichLane > 2**totalLaneCount-1: # specified lane not exist.
            errMsg[0] = """lane_info is incorrect, total lane count is %s""" % totalLaneCount
            return

        pixmapList = []
        if arrowInfo == 0:
            pixmapList.append(QPixmap(arrowImagesMap[0]))
        else:
            for oneKey, oneImagePath in arrowImagesMap.items():
                if arrowInfo & oneKey:
                    pixmapList.append(QPixmap(oneImagePath))

        if len(pixmapList) <= 0:
            errMsg[0] = "arrow_info is incorrect, cannot find arrow's picture."
            return

        for oneKey, oneGraphicView in self.graphicViewsMap.items():
            if oneKey >= totalLaneCount:
                continue
            if 2**(totalLaneCount-oneKey-1) & whichLane:
                scene = oneGraphicView.scene()
                for onePixmap in pixmapList:
                    scene.addPixmap(QPixmap(onePixmap))
        return

    # hide all graphic views.
    def hideAllGraphicViews(self):
        for oneKey, oneGraphicView in self.graphicViewsMap.items():
            oneGraphicView.setVisible(False)
        return

    # show the graphic views that are needed.
    def showNeededGraphicViews(self, totalLaneCount):
        for oneKey, oneGraphicView in self.graphicViewsMap.items():
            if oneKey < totalLaneCount:
                oneGraphicView.setVisible(True)
        return

    # generate scene for every arrow image graphicview.
    def initAllGraphicViews(self):
        for oneKey, oneGraphicView in self.graphicViewsMap.items():
            scene = QGraphicsScene()
            oneGraphicView.setScene(scene)
        return

    # remove all content drawn in the graphic views.
    def clearAllGraphicViewsContent(self):
        for oneKey, oneGraphicView in self.graphicViewsMap.items():
            scene = oneGraphicView.scene()
            for oneItem in scene.items():
                scene.removeItem(oneItem)
        return
