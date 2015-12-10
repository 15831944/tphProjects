# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox
from qgis.core import QgsDataSourceURI, QgsFeatureRequest, QgsFeature

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'RegulationShowInfoDlgDesign.ui'))

class RegulationShowInfoDlg(QtGui.QDialog, FORM_CLASS):

    def __init__(self, theCanvas, theLayer, parent=None):
        super(RegulationShowInfoDlg, self).__init__(parent)
        self.setupUi(self)
        self.mTheCanvas = theCanvas
        self.mTheLayer = theLayer
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
            if RegulationShowInfoDlg.isMyFeature(oneFeature):
                self.comboBoxRecordNo.addItem("%.0f" % oneFeature.attribute('record_no'))

    def showFeatureDetail(self, errMsg, theFeature):
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        if RegulationShowInfoDlg.isMyFeature(theFeature) == False:
            return
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
            QMessageBox.information(self, "Show Regulation", """error:\n%s"""%errMsg[0])
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
            QMessageBox.information(self, "Show Regulation", """error:\n%s"""%errMsg[0])
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
            QMessageBox.information(self, "Show Regulation", """error:\n%s"""%errMsg[0])
            return
        self.comboBoxRecordNo.setFocus()
        center = self.mTheCanvas.zoomToSelected(self.mTheLayer)
        self.mTheCanvas.refresh()
        return

    @staticmethod
    def isMyFeature(theFeature):
        try:
            record_no = theFeature.attribute('record_no')
            regulation_id = theFeature.attribute('regulation_id')
            regulation_type = theFeature.attribute('regulation_type')
            is_seasonal = theFeature.attribute('is_seasonal')
            first_link_id = theFeature.attribute('first_link_id')
            first_link_id_t = theFeature.attribute('first_link_id_t')
            last_link_id = theFeature.attribute('last_link_id')
            last_link_dir = theFeature.attribute('last_link_dir')
            last_link_id_t = theFeature.attribute('last_link_id_t')
            link_num = theFeature.attribute('link_num')
            key_string = theFeature.attribute('key_string')
            order_id = theFeature.attribute('order_id')
        except KeyError, kErr:
            return False
        except Exception, ex:
            return False
        return True







