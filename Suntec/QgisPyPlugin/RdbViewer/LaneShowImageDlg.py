# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser

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
        # 
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

        # 
        self.arrowImagesMap_highlight = {
            0: ":/lane/0.png",
            2**0 : ":/lane/1.png",
            2**1 : ":/lane/2.png",
            2**2 : ":/lane/4.png",
            2**3 : ":/lane/8.png",
            2**4 : ":/lane/16.png",
            2**5 : ":/lane/32.png",
            2**6 : ":/lane/64.png",
            2**7 : ":/lane/128.png",
            2**8 : ":/lane/256.png",
            2**9 : ":/lane/512.png",
            2**10 : ":/lane/1024.png",
            2**11 : ":/lane/2048.png",
            2**12 : ":/lane/4096.png",
            2**13 : ":/lane/8192.png"
            }

        #
        self.arrowImagesMap_gray = {
            0: ":/lane/0_gray.png",
            2**0 : ":/lane/1_gray.png",
            2**1 : ":/lane/2_gray.png",
            2**2 : ":/lane/4_gray.png",
            2**3 : ":/lane/8_gray.png",
            2**4 : ":/lane/16_gray.png",
            2**5 : ":/lane/32_gray.png",
            2**6 : ":/lane/64_gray.png",
            2**7 : ":/lane/128_gray.png",
            2**8 : ":/lane/256_gray.png",
            2**9 : ":/lane/512_gray.png",
            2**10 : ":/lane/1024_gray.png",
            2**11 : ":/lane/2048_gray.png",
            2**12 : ":/lane/4096_gray.png",
            2**13 : ":/lane/8192_gray.png"
            }

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
        self.initComboBoxOutlink(self.mSelFeatureIds)
        self.selectFeatureById(errMsg, self.mSelFeatureIds[0])
        self.comboBoxSelectLink.setFocus()

        self.pushButtonPrev.clicked.connect(self.onPushButtonPrev)
        self.pushButtonNext.clicked.connect(self.onPushButtonNext)
        self.connect(self.comboBoxSelectLink, QtCore.SIGNAL('activated(QString)'), self.comboBoxSelectLinkChanged)
        

    def disableAllControls(self):
        self.pushButtonPrev.setEnabled(False)
        self.pushButtonPrev.setEnabled(False)
        self.comboBoxSelectLink.setEnabled(False)
        self.textEditFeatureInfo.setEnabled(False)
        return

    def initComboBoxOutlink(self, featureIdList):
        while(self.comboBoxOutlink.count() > 0):
            self.comboBoxOutlink.removeItem(0)
        for oneFeatureId in self.featureIdList:
            featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest(oneFeatureId).setFlags(QgsFeatureRequest.NoGeometry))
            theFeature = QgsFeature()
            if featureIter.nextFeature(theFeature) == False:
                return
            if self.mIsMyFeature(theFeature):
                self.comboBoxOutlink.addItem(str(oneFeature.attribute('out_link_id')))

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

    def comboBoxOutlinkChanged(self, txt):
        errMsg = ['']
        inti = self.comboBoxOutlink.currentIndex()
        self.selectFeatureById(errMsg, self.mSelFeatureIds[inti])
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

    def selectFeatureById(self, errMsg, featureId):
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(featureId)
        center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
        self.mTheCanvas.refresh()
        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[0])
        if errMsg[0] <> '':
            return
        return

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
            type = theFeature.attribute('lane_num')
            passlink_count = theFeature.attribute('lane_info')
            pattern_id = theFeature.attribute('arrow_info')
            arrow_id = theFeature.attribute('lane_num_l')
            is_exist_sar = theFeature.attribute('lane_num_r')
            order_id = theFeature.attribute('passlink_count')
            pattern_name = theFeature.attribute('exclusive')
            arrow_name = theFeature.attribute('order_id')
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
        self.drawLane(errMsg, totalLaneCount, whichLane, arrowInfo, self.arrowImagesMap_highlight)
        return

    def drawLaneGray(self, errMsg, totalLaneCount, whichLane, arrowInfo):
        self.drawLane(errMsg, totalLaneCount, whichLane, arrowInfo, self.arrowImagesMap_gray)
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
