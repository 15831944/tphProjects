# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem, QPainter, QPen, QColor
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'SpotguideShowImageDlgDesign.ui'))

class SpotguideShowImageDlg(QtGui.QDialog, FORM_CLASS):

    def __init__(self, theCanvas, theLayer, selFeatureIds, parent=None):
        super(SpotguideShowImageDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
        self.mSelFeatureIds = selFeatureIds
        self.mAllFeatureIds = []

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

    def showFeatureDetail(self, errMsg, theFeature):
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        if self.mIsMyFeature(theFeature) == False:
            return

        errMsg = ['']
        pattern_dat, arrow_dat = self.getSpotguidePictures(errMsg, self.mTheLayer, theFeature)
        if errMsg[0] != '':
            #QMessageBox.information(self, "Show Spotguide", errMsg[0])
            return
        
        scene = QGraphicsScene()
        if pattern_dat is not None:
            datParser = MyDatParser()
            datParser.initFromMemory(errMsg, pattern_dat) # pattern picture
            pixmap1 = QPixmap()
            pixmap1.loadFromData(datParser.getDatContentByIndex(errMsg, 0))
            scene.addPixmap(self.getPixMapSizedByWidgt(pixmap1, self.graphicsViewShowImage))

        if arrow_dat is not None:
            datParser = MyDatParser()
            datParser.initFromMemory(errMsg, arrow_dat) # arrow picture
            pixmap2 = QPixmap()
            pixmap2.loadFromData(datParser.getDatContentByIndex(errMsg, 0))

            if datParser.hasPointlist():
                vecCoors = datParser.getPointListCoordinatesByIndex(errMsg, datParser.getPointlistIndex())
                if errMsg[0] != '':
                    QMessageBox.information(self, "Show Spotguide", errMsg[0])
                    return

                with QPainter(pixmap2) as thePainter:
                    for oneXYPair in vecCoors:
                        thePainter.setPen(QPen(QColor(255, 0, 0)))
                        thePainter.drawPoint(oneXYPair[0], oneXYPair[1])
                        thePainter.drawPoint(oneXYPair[0]-1, oneXYPair[1])
                        thePainter.drawPoint(oneXYPair[0]+1, oneXYPair[1])
                        thePainter.drawPoint(oneXYPair[0], oneXYPair[1]-1)
                        thePainter.drawPoint(oneXYPair[0], oneXYPair[1]+1)

                strPointList = datParser.getPointListStringByIndex(errMsg, datParser.getPointlistIndex())
                if errMsg[0] != '':
                    QMessageBox.information(self, "Show Spotguide", errMsg[0])
                    return

                strTemp = self.textEditFeatureInfo.toPlainText()
                strTemp += """\n\npointlist:\n"""
                strTemp += strPointList
                self.textEditFeatureInfo.setText(strTemp)
            scene.addPixmap(self.getPixMapSizedByWidgt(pixmap2, self.graphicsViewShowImage))
        self.graphicsViewShowImage.setScene(scene)
        return
    
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

    # result[0] is pattern dat
    # result[1] is arrow dat
    def getSpotguidePictures(self, errMsg, layer, theFeature):
        try:
            uri = QgsDataSourceURI(layer.source())
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (uri.host(), uri.database(), uri.username(), uri.password()))
            pg = conn.cursor()

            # all these lane's keys must be found
            # if anyone is not found, a 'KeyError' exception will be thrown.
            in_link_id = theFeature.attribute('in_link_id')
            node_id = theFeature.attribute('node_id')
            out_link_id = theFeature.attribute('out_link_id')
            passlink_count = theFeature.attribute('passlink_count')
            pattern_id = theFeature.attribute('pattern_id')
            arrow_id = theFeature.attribute('arrow_id')

            # spotguide record filter.
            strFilter = '''in_link_id=%s and node_id=%s and out_link_id=%s and 
                           passlink_count=%s and pattern_id=%s and arrow_id=%s''' % \
                        (in_link_id, node_id, out_link_id, passlink_count, pattern_id, arrow_id)

            sqlcmd = \
"""SET bytea_output TO escape;
select pattern_dat, arrow_dat
from %s
where %s
""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            row = pg.fetchone()
            return row[0], row[1]
        except KeyError, kErr:
            errMsg[0] = """Selected feature is not a rdb spotguide feature."""
            return None, None
        except Exception, ex:
            errMsg[0] = ex.message
            return None, None

    def comboBoxOutlinkidChanged(self, txt):
        errMsg = ['']
        inti = self.comboBoxOutlinkid.currentIndex()
        self.selectFeatureById(errMsg, self.mSelFeatureIds[inti], bZoomToSelected=False)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Show Spotguide", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonPrev(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()-1)
        prevFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, prevFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Show Spotguide", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonNext(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()+1)
        nextFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, nextFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Show Spotguide", """error:\n%s"""%errMsg[0])
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

    def getPixMapSizedByWidgt(self, pixmap, theWidgt):
        pixmapWidth = pixmap.width()
        pixmapHeight = pixmap.height()
        gpViewWidth = theWidgt.width()
        gpViewHeight = theWidgt.height()

        destWidth = 0
        destHeight = 0
        if pixmapWidth > gpViewWidth-5:
            destWidth = gpViewWidth-5
        else:
            destWidth = pixmapWidth

        if pixmapHeight > gpViewHeight-5:
            destHeight = gpViewHeight-5
        else:
            destHeight = pixmapHeight

        return pixmap.scaled(destWidth, destHeight, QtCore.Qt.IgnoreAspectRatio, QtCore.Qt.SmoothTransformation)

    def mIsMyFeature(self, theFeature):
        return SpotguideShowImageDlg.isMyFeature(theFeature)

    @staticmethod
    def isMyFeature(theFeature):
        try:
            gid = theFeature.attribute('gid')
            in_link_id = theFeature.attribute('in_link_id')
            in_link_id_t = theFeature.attribute('in_link_id_t')
            node_id = theFeature.attribute('node_id')
            node_id_t = theFeature.attribute('node_id_t')
            out_link_id = theFeature.attribute('out_link_id')
            out_link_id_t = theFeature.attribute('out_link_id_t')
            type = theFeature.attribute('type')
            passlink_count = theFeature.attribute('passlink_count')
            pattern_id = theFeature.attribute('pattern_id')
            arrow_id = theFeature.attribute('arrow_id')
            is_exist_sar = theFeature.attribute('is_exist_sar')
            order_id = theFeature.attribute('order_id')
            pattern_name = theFeature.attribute('pattern_name')
            arrow_name = theFeature.attribute('arrow_name')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True



