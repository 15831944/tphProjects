# -*- coding: utf-8 -*-
import os
import psycopg2
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem, QPainter, QPen, QColor
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature, QgsGeometry
from qgis.gui import QgsHighlight

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'NostraIllustCheckDlgDesign.ui'))

class NostraIllustCheckDlg(QtGui.QDialog, FORM_CLASS):

    def __init__(self, theCanvas, theLayer, selFeatureIds, parent=None):
        super(NostraIllustCheckDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
        self.mSelFeatureIds = selFeatureIds
        self.mAllFeatureIds = []
        self.highlightList = []
        uri = QgsDataSourceURI(self.mTheLayer.source())
        self.conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
            (uri.host(), uri.database(), uri.username(), uri.password()))
        self.pg = self.conn.cursor()

        # members shown in graphic view.
        self.mScene = QGraphicsScene()
        self.graphicsViewShowImage.setScene(self.mScene)
        self.mPixmapList = []

        featureIter = self.mTheLayer.getFeatures(QgsFeatureRequest().setFlags(QgsFeatureRequest.NoGeometry))
        inti = 0
        theFeature = QgsFeature()
        while(featureIter.nextFeature(theFeature)):
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

    def resizeEvent(self, event):
        self.showImageInGraphicsView()
        return

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
                self.comboBoxOutlinkid.addItem(str(theFeature.attribute('arc1')) + str(theFeature.attribute('arc2')))

    def showFeatureDetail(self, errMsg, theFeature):
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        if self.mIsMyFeature(theFeature) == False:
            return

        errMsg = ['']
        day_pic = theFeature.attribute('day_pic')
        arrowimg = theFeature.attribute('arrowimg')
        basePath = r"\\tangpinghui\20151010_nostra_junctionview\output_jpg_and_png"
        day_pic_path = os.path.join(basePath, day_pic+".psd\Main_Day.jpg")
        arrowimg_path = ''
        tempPath = os.path.join(basePath, day_pic+".psd")
        for curDir, subDirs, fileNames in os.walk(tempPath):
            for oneFile in fileNames:
                if oneFile.find(arrowimg) != -1 and oneFile.find('_en') != -1:
                    arrowimg_path = os.path.join(curDir, oneFile)

        self.mPixmapList = []
        patternPixmap = QPixmap()
        patternPixmap.load(day_pic_path)
        self.mPixmapList.append(patternPixmap)
        arrowPixmap = QPixmap()
        arrowPixmap.load(arrowimg_path)
        self.mPixmapList.append(arrowPixmap)
        self.showImageInGraphicsView()
        return
    
    def showImageInGraphicsView(self):
        # remove all items in member QGraphicsScene.
        for oneItem in self.mScene.items():
            self.mScene.removeItem(oneItem)

        for onePixmap in self.mPixmapList:
            self.mScene.addPixmap(self.getPixMapSizedByWidgt(onePixmap, self.graphicsViewShowImage))

        self.mScene.setSceneRect(0, 0, self.graphicsViewShowImage.width()-5, self.graphicsViewShowImage.height()-5)
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

    def comboBoxOutlinkidChanged(self, txt):
        errMsg = ['']
        inti = self.comboBoxOutlinkid.currentIndex()
        self.selectFeatureById(errMsg, self.mSelFeatureIds[inti], bZoomToSelected=False)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Nostra Illust Check", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonPrev(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()-1)
        prevFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, prevFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Nostra Illust Check", """error:\n%s"""%errMsg[0])
            return
        return

    def onPushButtonNext(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()+1)
        nextFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        errMsg = ['']
        self.selectFeatureById(errMsg, nextFeatureId)
        if errMsg[0] <> '':
            QMessageBox.information(self, "Nostra Illust Check", """error:\n%s"""%errMsg[0])
            return
        return

    def selectFeatureById(self, errMsg, featureId, bZoomToSelected=True):
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(featureId)
        theFeature = self.mTheLayer.selectedFeatures()[0]
        self.showFeatureDetail(errMsg, theFeature)
        if errMsg[0] <> '':
            return
        if bZoomToSelected == True:
            center = self.mTheCanvas.panToSelected(self.mTheLayer)
            self.mTheCanvas.refresh()
        self.clearHighlight()
        arc1 = "%.0f" % theFeature.attribute('arc1')
        sqlcmd = """
SET bytea_output TO escape;
select st_asbinary(the_geom) from temp_topo_link where routeid=%s
""" % arc1
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            qgsGeometry = QgsGeometry()
            qgsGeometry.fromWkb(str(row[0]))
            self.highlightByGeometry(qgsGeometry, QColor(249,27,15,168))

        arc2 = "%.0f" % theFeature.attribute('arc2')
        sqlcmd = """
SET bytea_output TO escape;
select st_asbinary(the_geom) from temp_topo_link where routeid=%s
""" % arc2
        self.pg.execute(sqlcmd)
        rows = self.pg.fetchall()
        for row in rows:
            qgsGeometry = QgsGeometry()
            qgsGeometry.fromWkb(str(row[0]))
            self.highlightByGeometry(qgsGeometry, QColor(22,151,0,168))

        new_arc1 = "%.0f" % theFeature.attribute('new_arc1')
        if new_arc1 is not None and new_arc1 <> arc1:
            sqlcmd = """
SET bytea_output TO escape;
select st_asbinary(the_geom) from temp_topo_link where routeid=%s
""" % new_arc1
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            for row in rows:
                qgsGeometry = QgsGeometry()
                qgsGeometry.fromWkb(str(row[0]))
                self.highlightByGeometry(qgsGeometry, QColor(253,158,153,128))

        new_arc2 = "%.0f" % theFeature.attribute('new_arc2')
        if new_arc2 is not None and new_arc2 <> arc2:
            sqlcmd = """
SET bytea_output TO escape;
select st_asbinary(the_geom) from temp_topo_link where routeid=%s
""" % new_arc2
            self.pg.execute(sqlcmd)
            rows = self.pg.fetchall()
            for row in rows:
                qgsGeometry = QgsGeometry()
                qgsGeometry.fromWkb(str(row[0]))
                self.highlightByGeometry(qgsGeometry, QColor(147,255,155,128))

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
        return NostraIllustCheckDlg.isMyFeature(theFeature)

    @staticmethod
    def isMyFeature(theFeature):
        try:
            gid = theFeature.attribute('gid')
            arc1 = theFeature.attribute('arc1')
            arc2 = theFeature.attribute('arc2')
            day_pic = theFeature.attribute('day_pic')
            night_pic = theFeature.attribute('night_pic')
            arrowimg = theFeature.attribute('arrowimg')
            lon = theFeature.attribute('lon')
            lat = theFeature.attribute('lat')
            new_arc1 = theFeature.attribute('new_arc1')
            new_arc2 = theFeature.attribute('new_arc2')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True

    def highlightFeature(self, theFeature):
        highlight = QgsHighlight(self.mTheCanvas, theFeature.geometry(), self.mTheLayer)
        highlight.setColor(QColor(255,0,0,128))
        highlight.setFillColor(QColor(255,0,0,128))
        highlight.setBuffer(0.5)
        highlight.setMinWidth(6)
        highlight.setWidth(6)
        highlight.show()
        self.highlightList.append(highlight)
        return

    def highlightByGeometry(self, geometry, color=QColor(255,0,0,128)):
        highlight = QgsHighlight(self.mTheCanvas, geometry, self.mTheLayer)
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


