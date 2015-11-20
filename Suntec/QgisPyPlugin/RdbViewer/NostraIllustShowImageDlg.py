# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem, QPainter, QPen, QColor
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'NostraIllustShowImageDlgDesign.ui'))

class NostraIllustShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theCanvas, theLayer, selectedFeatureList, parent=None):
        if(len(selectedFeatureList) == 0):
            QMessageBox.information(self, "Show Nostra Illust", """No feature selected.""")
            return
        super(NostraIllustShowImageDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
        self.mSelectedFeatureList = selectedFeatureList
        self.mAllFeatureIds = self.mTheLayer.allFeatureIds()

        for oneFeature in self.mSelectedFeatureList:
            fieldList = oneFeature.fields()
            attrList = oneFeature.attributes()
            for oneField, oneAttr in zip(fieldList, attrList):
                if(oneField.name() == 'arc2'):
                    self.comboBoxSelectArc2.addItem("%.0f" % oneAttr)

        self.connect(self.comboBoxSelectArc2, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxSelectLinkChanged)

        self.connect(self.pushButtonNext, 
                     QtCore.SIGNAL('clicked()'), 
                     self.onPushButtonNext)

        self.connect(self.pushButtonShow, 
                     QtCore.SIGNAL('clicked()'), 
                     self.onPushButtonShow)

        self.comboBoxSelectArc2.setFocus()
        self.showFeatureDetail(self.mSelectedFeatureList[0])
        self.spinBoxCurId.setMaximum(len(self.mAllFeatureIds)-1)
        self.spinBoxCurId.setMinimum(0)
        self.spinBoxCurId.setValue(0)
        return

    def comboBoxSelectLinkChanged(self, txt):
        inti = self.comboBoxSelectArc2.currentIndex()
        theFeature = self.mSelectedFeatureList[inti]
        self.showFeatureDetail(theFeature)
        return

    def showFeatureDetail(self, theFeature):
        arc1 = theFeature.attribute('arc1')
        arc2 = theFeature.attribute('arc2')
        day_pic = theFeature.attribute('day_pic')
        arrowimg = theFeature.attribute('arrowimg')
        
        picBasePath = r"""\\hexin\20151010_nostra_junctionview\output_jpg_and_png"""
        day_pic_path = os.path.join(picBasePath, day_pic+'.psd', 'Main_Day.jpg')
        arrow_pic_path = os.path.join(picBasePath, day_pic+'.psd', arrowimg+'_r_en.png')
        if os.path.isfile(arrow_pic_path) == False:
            arrow_pic_path = os.path.join(picBasePath, day_pic+'.psd', arrowimg+'_l_en.png')

        scene = QGraphicsScene()
        pixmap1 = QPixmap()
        pixmap1.load(day_pic_path)
        scene.addItem(QGraphicsPixmapItem(pixmap1))

        pixmap2 = QPixmap()
        pixmap2.load(arrow_pic_path)
        scene.addItem(QGraphicsPixmapItem(pixmap2))
        self.graphicsViewShowImage.setScene(scene)

        # update the feature info textbox
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText('feature count: %s\n%s' % (len(self.mSelectedFeatureList), strFeatureInfo))
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

    def onPushButtonNext(self):
        self.spinBoxCurId.setValue(self.spinBoxCurId.value()+1)
        nextFeatureId = self.mAllFeatureIds[self.spinBoxCurId.value()]
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(nextFeatureId)
        theFeature = self.mTheLayer.selectedFeatures()[0]
        self.showFeatureDetail(theFeature)
        center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
        self.mTheCanvas.refresh()
        return

    def onPushButtonShow(self):
        theFeatureId = self.mAllFeatureIds[self.spinBoxCurId.value()]
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(theFeatureId)
        theFeature = self.mTheLayer.selectedFeatures()[0]
        self.showFeatureDetail(theFeature)
        center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
        self.mTheCanvas.refresh()
        return
    