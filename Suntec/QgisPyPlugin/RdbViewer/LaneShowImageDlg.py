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
        self.labelsMap = {
            2**0 : self.label_0,
            2**1 : self.label_1,
            2**2 : self.label_2,
            2**3 : self.label_3,
            2**4 : self.label_4,
            2**5 : self.label_5,
            2**6 : self.label_6,
            2**7 : self.label_7,
            2**8 : self.label_8,
            2**9 : self.label_9,
            2**10 : self.label_10,
            2**11 : self.label_11,
            2**12 : self.label_12,
            2**13 : self.label_13,
            2**14 : self.label_14,
            2**15 : self.label_15
            }

        # 
        self.arrowImagesMap = {
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

            # get guide info lanes 
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
                if row[0] != totalLaneCount:
                    errMsg = """some lane_count not the same."""
                    QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
                    return
                self.drawLaneHighlight(totalLaneCount, 65535, 1)
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
        pixmapList = []
        for oneKey, oneValue in self.arrowImagesMap.items():
            if arrowInfo & oneKey:
                pixmapList.append(QPixmap(oneValue))

        if len(pixmapList) <= 0:
            errMsg = "pixmapList=0, can find arrow picture."
            QMessageBox.information(self, "Show Lane", """error:\n%s""" % errMsg)
            return
        for oneKey, oneValue in self.labelsMap.items():
            if whichLane & oneKey:
                oneValue.setPixmap(pixmapList[0])
        return
