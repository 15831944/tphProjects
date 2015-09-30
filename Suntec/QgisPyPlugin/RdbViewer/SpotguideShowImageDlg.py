# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser
from MyDbManager import MyDbManager

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem
from PyQt4.QtCore import QRectF

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'SpotguideShowImageDlg_design.ui'))

class SpotguideShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theLayer, selectedFeatureList, parent=None):
        super(SpotguideShowImageDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheLayer = theLayer
        self.mFeatureList = selectedFeatureList
        for oneFeature in self.mFeatureList:
            fieldList = oneFeature.fields()
            attrList = oneFeature.attributes()
            for oneField, oneAttr in zip(fieldList, attrList):
                if(oneField.name() == 'out_link_id'):
                    self.comboBoxSelectLink.addItem(str(oneAttr))

        if self.comboBoxSelectLink.count() <= 0:
            return
        self.selectFeatureByComboIdx(0)
        self.comboBoxSelectLink.setFocus()

        self.connect(self.comboBoxSelectLink, 
                     QtCore.SIGNAL('activated(QString)'), 
                     self.comboBoxSelectLinkChanged)

    def selectFeatureByComboIdx(self, comboIdx):
        errMsg = ['']
        dbManager = MyDbManager()
        theFeature = self.mFeatureList[comboIdx]
        binData = dbManager.getPictureBinaryData(errMsg, self.mTheLayer, theFeature)
        if errMsg[0] != '':
            QMessageBox.information(self, "warnning", errMsg[0])
            return
        
        scene = QGraphicsScene()
        pattern_data = binData[0]
        if pattern_data is not None:
            datPaser = MyDatParser()
            datPaser.initFromMemory(errMsg, pattern_data) # pattern picture
            pixmap1 = QPixmap()
            pixmap1.loadFromData(datPaser.getPatternPicByComboIdx(errMsg, 0))
            scene.addItem(QGraphicsPixmapItem(pixmap1))

        arrow_data = binData[1]
        if arrow_data is not None:
            datPaser = MyDatParser()
            datPaser.initFromMemory(errMsg, arrow_data) # arrow picture
            pixmap2 = QPixmap()
            pixmap2.loadFromData(datPaser.getPatternPicByComboIdx(errMsg, 0))
            scene.addItem(QGraphicsPixmapItem(pixmap2))

        self.graphicsViewShowImage.setScene(scene)

        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)

        return
    
    def getFeatureInfoString(self, theFeature):
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFeatureInfo = "field count: %d\n" % len(fieldList)
        for oneField, oneAttr in zip(fieldList, attrList):
            strFeatureInfo += "%s: %s\n" % (oneField.name(), oneAttr)
        return strFeatureInfo
    
    def comboBoxSelectLinkChanged(self, txt):
        inti = self.comboBoxSelectLink.currentIndex()
        self.selectFeatureByComboIdx(inti)
        return
    


