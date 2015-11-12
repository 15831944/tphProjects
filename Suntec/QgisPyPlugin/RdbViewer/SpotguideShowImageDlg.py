# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem, QPainter, QPen, QColor
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'SpotguideShowImageDlgDesign.ui'))

class SpotguideShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theLayer, selectedFeatureList, parent=None):
        if(len(selectedFeatureList) == 0):
            QMessageBox.information(self, "Show Spotguide", """No feature selected.""")
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
            errMsg = '''Selected feature is not a rdb spotguide feature.'''
            #QMessageBox.information(self, "Show Spotguide", """error:\n%s"""%errMsg)
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
            scene.addItem(QGraphicsPixmapItem(pixmap1))

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

            
            scene.addItem(QGraphicsPixmapItem(pixmap2))
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


