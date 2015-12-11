# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'GuideInfoCommonDlgDesign.ui'))

class GuideInfoCommonDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theCanvas, theLayer, category="Guideinfo Common",  parent=None):
        super(GuideInfoCommonDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
        self.mAllFeatureIds = []
        self.mCategory = category
        self.setWindowTitle(self.mCategory)

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
        self.initcomboBoxRecordNo()

        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[0])
        self.comboBoxRecordNo.setFocus()

        self.pushButtonPrev.clicked.connect(self.onPushButtonPrev)
        self.pushButtonNext.clicked.connect(self.onPushButtonNext)
        self.connect(self.comboBoxRecordNo, QtCore.SIGNAL('activated(QString)'), self.comboBoxRecordNoChanged)

    def disableAllControls(self):
        self.pushButtonPrev.setEnabled(False)
        self.pushButtonPrev.setEnabled(False)
        self.comboBoxRecordNo.setEnabled(False)
        self.textEditFeatureInfo.setEnabled(False)
        return

    def initcomboBoxRecordNo(self):
        while(self.comboBoxRecordNo.count() > 0):
            self.comboBoxRecordNo.removeItem(0)
        for oneFeature in self.mTheLayer.selectedFeatures():
            if self.mIsMyFeature(oneFeature):
                self.comboBoxRecordNo.addItem("%.0f" % oneFeature.attribute('out_link_id'))

    def showFeatureDetail(self, errMsg, theFeature):
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
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

    def comboBoxRecordNoChanged(self, txt):
        errMsg = ['']
        inti = self.comboBoxRecordNo.currentIndex()
        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[inti])
        if errMsg[0] <> '':
            QMessageBox.information(self, self.mCategory, """error:\n%s"""% errMsg[0])
            return
        return

    def onPushButtonPrev(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()-1)
        prevFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(prevFeatureId)
        self.initcomboBoxRecordNo()

        errMsg = ['']
        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[0])
        if errMsg[0] <> '':
            QMessageBox.information(self, self.mCategory, """error:\n%s"""% errMsg[0])
            return
        self.comboBoxRecordNo.setFocus()
        center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
        self.mTheCanvas.refresh()
        return

    def onPushButtonNext(self):
        self.spinBoxFeatureIndex.setValue(self.spinBoxFeatureIndex.value()+1)
        nextFeatureId = self.mAllFeatureIds[self.spinBoxFeatureIndex.value()-1]
        self.mTheLayer.removeSelection()
        self.mTheLayer.select(nextFeatureId)
        self.initcomboBoxRecordNo()

        errMsg = ['']
        self.showFeatureDetail(errMsg, self.mTheLayer.selectedFeatures()[0])
        if errMsg[0] <> '':
            QMessageBox.information(self, self.mCategory, """error:\n%s"""% errMsg[0])
            return
        self.comboBoxRecordNo.setFocus()
        center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
        self.mTheCanvas.refresh()
        return

    def mIsMyFeature(self, theFeature):
        return True







