# -*- coding: utf-8 -*-
import os
import psycopg2
from datParser import DatParser

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem

FORM_CLASS, _ = uic.loadUiType(os.path.join(
    os.path.dirname(__file__), 'show_image_dialog_base.ui'))


class ShowImageDialog(QtGui.QDialog, FORM_CLASS):
    def __init__(self, datBinaryData, strFeatureInfo, parent=None):
        super(ShowImageDialog, self).__init__(parent)
        self.setupUi(self)
        self.mDatBinaryData = datBinaryData
        self.mDatPaser = DatParser()
        errMsg = ['']
        self.mDatPaser.initFromMemory(errMsg, self.mDatBinaryData)
        pixmap = QPixmap()
        pixmap.loadFromData(self.mDatPaser.getPicBufferByIndex(errMsg, 0))
        scene = QGraphicsScene()
        scene.addItem(QGraphicsPixmapItem(pixmap))
        self.graphicsViewShowImage.setScene(scene)
        self.textEditFeatureInfo.setText(strFeatureInfo)

