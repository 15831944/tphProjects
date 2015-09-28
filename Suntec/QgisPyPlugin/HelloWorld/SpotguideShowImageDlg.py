# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser
from MyDbManager import MyDbManager

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'SpotguideShowImageDlg_design.ui'))

class SpotguideShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theLayer, selectedFeatureList, parent=None):
        super(SpotguideShowImageDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheLayer = theLayer
        self.mFeatureList = selectedFeatureList
        for oneFeature in self.mFeatureList:
            self.comboBoxSelectLink.addItem(oneFeature.name())

    def xxxx(self):
        errMsg = ['']
        dbManager = MyDbManager()
        for oneFeature in self.featureList:
            self.mDatBinaryDataList.append(dbManager.getPictureBinaryData(errMsg, theLayer, oneFeature))
            if errMsg[0] != '':
                QMessageBox.information(self.canvas, "warnning", errMsg[0])
            return

        strFeatureInfo = self.getFeatureInfoString(self.featureList[0])
        self.textEditFeatureInfo.setText(strFeatureInfo)
        mDatPaser = MyDatParser()
        mDatPaser.initFromMemory(errMsg, self.mDatBinaryData[0])
        pixmap = QPixmap()
        pixmap.loadFromData(self.mDatPaser.getPicBufferByIndex(errMsg, 0))
        scene = QGraphicsScene()
        scene.addItem(QGraphicsPixmapItem(pixmap))
        self.graphicsViewShowImage.setScene(scene)
        return

    
    def getFeatureInfoString(self, theFeature):
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFeatureInfo = "field count: %d\n" % len(fieldList)
        for oneField, oneAttr in zip(fieldList, attrList):
            strFeatureInfo += "%s: %s\n" % (oneField.name(), oneAttr)
        return strFeatureInfo
       
    


