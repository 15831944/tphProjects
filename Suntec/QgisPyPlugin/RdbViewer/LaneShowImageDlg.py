# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'LaneShowImageDlgDesign.ui'))

arrowBasePath = r"""C:\Users\hexin\.qgis2\python\plugins\RdbViewer\arrows"""

class LaneShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theLayer, selectedFeatureList, parent=None):
        if(len(selectedFeatureList) == 0):
            QMessageBox.information(self, "Show Lane", """error:\nNo feature selected.""")
            return
        super(LaneShowImageDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheLayer = theLayer
        self.mFeatureList = selectedFeatureList
        # 
        self.graphicViewsMap = {
            0 : self.graphicView_0,
            1 : self.graphicView_1,
            2 : self.graphicView_2,
            3 : self.graphicView_3,
            4 : self.graphicView_4,
            5 : self.graphicView_5,
            6 : self.graphicView_6,
            7 : self.graphicView_7,
            8 : self.graphicView_8,
            9 : self.graphicView_9,
            10 : self.graphicView_10,
            11 : self.graphicView_11,
            12 : self.graphicView_12,
            13 : self.graphicView_13,
            14 : self.graphicView_14,
            15 : self.graphicView_15
            }

        # 
        self.arrowImagesMap_highlight = {
            2**0 : os.path.join(arrowBasePath, "1.png"),
            2**1 : os.path.join(arrowBasePath, "2.png"),
            2**2 : os.path.join(arrowBasePath, "4.png"),
            2**3 : os.path.join(arrowBasePath, "8.png"),
            2**4 : os.path.join(arrowBasePath, "16.png"),
            2**5 : os.path.join(arrowBasePath, "32.png"),
            2**6 : os.path.join(arrowBasePath, "64.png"),
            2**7 : os.path.join(arrowBasePath, "128.png"),
            2**8 : os.path.join(arrowBasePath, "256.png"),
            2**9 : os.path.join(arrowBasePath, "512.png"),
            2**10 : os.path.join(arrowBasePath, "1024.png"),
            2**11 : os.path.join(arrowBasePath, "2048.png"),
            2**12 : os.path.join(arrowBasePath, "4096.png"),
            2**13 : os.path.join(arrowBasePath, "8192.png")            
            }

        self.arrowImagesMap_gray = {
            2**0 : os.path.join(arrowBasePath, "1_gray.png"),
            2**1 : os.path.join(arrowBasePath, "2_gray.png"),
            2**2 : os.path.join(arrowBasePath, "4_gray.png"),
            2**3 : os.path.join(arrowBasePath, "8_gray.png"),
            2**4 : os.path.join(arrowBasePath, "16_gray.png"),
            2**5 : os.path.join(arrowBasePath, "32_gray.png"),
            2**6 : os.path.join(arrowBasePath, "64_gray.png"),
            2**7 : os.path.join(arrowBasePath, "128_gray.png"),
            2**8 : os.path.join(arrowBasePath, "256_gray.png"),
            2**9 : os.path.join(arrowBasePath, "512_gray.png"),
            2**10 : os.path.join(arrowBasePath, "1024_gray.png"),
            2**11 : os.path.join(arrowBasePath, "2048_gray.png"),
            2**12 : os.path.join(arrowBasePath, "4096_gray.png"),
            2**13 : os.path.join(arrowBasePath, "8192_gray.png")            
            }

        for oneFeature in self.mFeatureList:
            fieldList = oneFeature.fields()
            attrList = oneFeature.attributes()
            for oneField, oneAttr in zip(fieldList, attrList):
                if(oneField.name() == 'out_link_id'):
                    self.comboBoxSelectLink.addItem(str(oneAttr))

        # no feature has a 'out_link_id'
        # then show the first feature's information only.
        if self.comboBoxSelectLink.count() <= 0:
            errMsg = """Selected feature is not a rdb lane feature."""
            QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
            self.comboBoxSelectLink.setEnabled(False)
            theFeature = self.mFeatureList[0]
            strFeatureInfo = self.getFeatureInfoString(theFeature)
            self.textEditFeatureInfo.setText(strFeatureInfo)
            return

        self.selectFeatureByComboIdx(0)
        self.comboBoxSelectLink.setFocus()

        self.connect(self.comboBoxSelectLink, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxSelectLinkChanged)
    
    def comboBoxSelectLinkChanged(self, txt):
        inti = self.comboBoxSelectLink.currentIndex()
        self.selectFeatureByComboIdx(inti)
        return

    def selectFeatureByComboIdx(self, comboIdx):
        theFeature = self.mFeatureList[comboIdx]
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        
        try:
            uri = QgsDataSourceURI(self.mTheLayer.source())
            conn = psycopg2.connect("""host='%s' dbname='%s' user='%s' password='%s'""" %\
                (uri.host(), uri.database(), uri.username(), uri.password()))
            pg = conn.cursor()

            # all these lane's keys must be found
            # if anyone is not found, a 'KeyError' exception will be thrown.
            in_link_id = theFeature.attribute('in_link_id')
            node_id = theFeature.attribute('node_id')
            out_link_id = theFeature.attribute('out_link_id')
            passlink_count = theFeature.attribute('passlink_count')
            lane_num = theFeature.attribute('lane_num')
            lane_info = theFeature.attribute('lane_info')
            arrow_info = theFeature.attribute('arrow_info')

            # draw all lanes in this link
            strFilter = '''in_link_id=%s and node_id=%s and passlink_count=%s''' %\
                        (in_link_id, node_id, passlink_count)
            sqlcmd = """select lane_num, lane_info, arrow_info
                        from %s
                        where %s""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            rows = pg.fetchall()
            if len(rows) <= 0:
                errMsg = '''get no lane record.'''
                QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
                return
            totalLaneCount = rows[0][0] # lane count of inlink.

            self.clearAllGraphicViews() # clear all obsolete content in GraphicViews
            self.hideAllGraphicViews()
            self.showNeededGraphicViews(totalLaneCount)

            for row in rows:
                lane_num = row[0] # total lane count in this link
                lane_info = row[1] # which lane is specified
                arrow_info = row[2] # arrow catagory

                if lane_num != totalLaneCount:
                    errMsg = """some lane_count not the same."""
                    QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
                    return
                self.drawLaneGray(lane_num, lane_info, arrow_info)          

            # draw highlighted guide lanes 
            strFilter = '''in_link_id=%s and node_id=%s and out_link_id=%s
                           and passlink_count=%s''' %\
                        (in_link_id, node_id, out_link_id, passlink_count)
            sqlcmd = """select lane_num, lane_info, arrow_info
                        from %s
                        where %s""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            rows = pg.fetchall()
            if len(rows) <= 0:
                errMsg = '''get no lane record.'''
                QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
                return
            totalLaneCount = rows[0][0] # lane count of inlink.
            for row in rows:
                lane_num = row[0] # total lane count in this link
                lane_info = row[1] # which lane is specified
                arrow_info = row[2] # arrow catagory

                if lane_num != totalLaneCount:
                    errMsg = """some lane_count not the same."""
                    QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
                    return
                self.drawLaneHighlight(lane_num, lane_info, arrow_info)
            return 
        except KeyError, kErr:
            errMsg = """Selected feature is not a rdb lane feature."""
            QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
            return
        except Exception, ex:
            QMessageBox.information(self, "Show Lane", """error:\n%s"""%ex.message)
            return
        return
    
    def getFeatureInfoString(self, theFeature):
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFeatureInfo = "field count: %d\n" % len(fieldList)
        for oneField, oneAttr in zip(fieldList, attrList):
            strFeatureInfo += "%s: %s\n" % (oneField.name(), oneAttr)
        return strFeatureInfo

    def drawLaneHighlight(self, totalLaneCount, whichLane, arrowInfo):
        self.drawLane(totalLaneCount, whichLane, arrowInfo, self.arrowImagesMap_highlight)
        return

    def drawLaneGray(self, totalLaneCount, whichLane, arrowInfo):
        self.drawLane(totalLaneCount, whichLane, arrowInfo, self.arrowImagesMap_gray)
        return

    def drawLane(self, totalLaneCount, whichLane, arrowInfo, arrowImagesMap):
        if whichLane >= 2**(totalLaneCount+1): # specified lane not exist.
            errMsg = """lane_info is incorrect, total lane count is %s""" % totalLaneCount
            QMessageBox.information(self, "Show Lane", """error:\n%s""" % errMsg)
            return

        pixmapList = []
        for oneKey, oneImagePath in arrowImagesMap.items():
            if arrowInfo & oneKey:
                pixmapList.append(QPixmap(oneImagePath))

        if len(pixmapList) <= 0:
            errMsg = "arrow_info is incorrect, cannot find arrow's picture."
            QMessageBox.information(self, "Show Lane", """error:\n%s""" % errMsg)
            return

        for oneKey, oneGraphicView in self.graphicViewsMap.items():
            if oneKey >= totalLaneCount:
                continue
            if whichLane & 2**(totalLaneCount-oneKey-1):
                scene = QGraphicsScene()
                for onePixmap in pixmapList:
                    scene.addItem(QGraphicsPixmapItem(onePixmap))
                oneGraphicView.setScene(scene)
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

    # remove all content drawn in the graphic views.
    def clearAllGraphicViews(self):
        for oneKey, oneGraphicView in self.graphicViewsMap.items():
            #oneGraphicView.clear()
            continue
        return
