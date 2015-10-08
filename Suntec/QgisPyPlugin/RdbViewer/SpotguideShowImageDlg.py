# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem
from PyQt4.QtCore import QRectF

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'SpotguideShowImageDlgDesign.ui'))

class SpotguideShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theLayer, selectedFeatureList, parent=None):
        if(len(selectedFeatureList) == 0):
            QMessageBox.information(self, "warning", """No feature selected.""")
            return
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

        # no feature has a 'out_link_id'
        # then show the first feature's information only.
        if self.comboBoxSelectLink.count() <= 0:
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

    def selectFeatureByComboIdx(self, comboIdx):
        theFeature = self.mFeatureList[comboIdx]
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        
        errMsg = ['']
        binData = self.getSpotguidePictures(errMsg, self.mTheLayer, theFeature)
        if errMsg[0] != '':
            QMessageBox.information(self, "error", errMsg[0])
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
    
    # make sure errMsg is a 'string array' which has at least one element.
    def removeAllLayersWithSpecName(self, errMsg, specLayerName):
        layerList = QgsMapLayerRegistry.instance().mapLayersByName(specLayerName)
        for oneLayer in layerList:
            QgsMapLayerRegistry.instance().removeMapLayer(oneLayer.id())
        return

    def getSpotguidePictures(self, errMsg, layer, theFeature):
        try:
            uri = QgsDataSourceURI(layer.source())
            conn = psycopg2.connect('''host='%s' dbname='%s' user='%s' password='%s' ''' %\
                (uri.host(), uri.database(), uri.username(), uri.password()))
            pg = conn.cursor()
            strFilter = ''''''
            fieldList = theFeature.fields()
            attrList = theFeature.attributes()
            bInlinkFound = False
            bNodeFound = False
            bOutlinkFound = False
            bPasslinkCountFound = False
            bPatternFound = False
            bArrowFound = False
            for oneField, oneAttr in zip(fieldList, attrList):
                if oneAttr is None:
                    continue
                if oneField.name() == 'in_link_id':
                    strFilter += ''' in_link_id=%s and ''' % oneAttr
                    bInlinkFound = True

                if oneField.name() == 'node_id':
                    strFilter += ''' node_id=%s and ''' % oneAttr
                    bNodeFound = True
                    
                if oneField.name() == 'out_link_id':
                    strFilter += ''' out_link_id=%s and ''' % oneAttr
                    bOutlinkFound = True

                if oneField.name() == 'passlink_count':
                    strFilter += ''' passlink_count=%s and ''' % oneAttr
                    bPasslinkCountFound = True

                if oneField.name() == 'pattern_data':
                    strFilter += ''' pattern_data=%s and ''' % oneAttr
                    bPatternFound = True

                if oneField.name() == 'arrow_data':
                    strFilter += ''' arrow_data=%s''' % oneAttr
                    bArrowFound = True


            if bInlinkFound == False or\
               bNodeFound == False or\
               bOutlinkFound == False or\
               bPasslinkCountFound == False or\
               bPatternFound == False or\
               bArrowFound == False:
                errMsg[0] = """Selected feature is not a rdb lane feature."""
                return


            sqlcmd = \
"""SET bytea_output TO escape;
select pattern_data, arrow_data
from %s
where %s true
""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            return pg.fetchone()
        except Exception, ex:
            errMsg[0] = ex.message
            return None


