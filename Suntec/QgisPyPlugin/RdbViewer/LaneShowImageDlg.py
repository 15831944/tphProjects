# -*- coding: utf-8 -*-
import os
import psycopg2
from MyDatParser import MyDatParser

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import QMessageBox, QGraphicsScene, QPixmap, QGraphicsPixmapItem
from PyQt4.QtCore import QRectF
from qgis.core import QgsDataSourceURI

FORM_CLASS, _ = uic.loadUiType(os.path.join(os.path.dirname(__file__),
                               'LaneShowImageDlgDesign.ui'))

class LaneShowImageDlg(QtGui.QDialog, FORM_CLASS):
    def __init__(self, theLayer, selectedFeatureList, parent=None):
        if(len(selectedFeatureList) == 0):
            QMessageBox.information(self, "Show Lane", """error:\nNo feature selected.""")
            return
        super(LaneShowImageDlg, self).__init__(parent)
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
            errMsg = """Selected feature is not a rdb lane feature."""
            QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
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
    
    def comboBoxSelectLinkChanged(self, txt):
        inti = self.comboBoxSelectLink.currentIndex()
        self.selectFeatureByComboIdx(inti)
        return

    def selectFeatureByComboIdx(self, comboIdx):
        theFeature = self.mFeatureList[comboIdx]
        strFeatureInfo = self.getFeatureInfoString(theFeature)
        self.textEditFeatureInfo.setText(strFeatureInfo)
        
        try:
            uri = QgsDataSourceURI(self.mTheLayer.source())
            conn = psycopg2.connect("""host='%s' dbname='%s' user='%s' password='%s'""" %\
                (uri.host(), uri.database(), uri.username(), uri.password()))
            pg = conn.cursor()

            # all these lane's keys must be found
            # if anyone is not found, a 'KeyError' exception will be thrown.
            in_link_id = theFeature.attribute('in_link_id')
            node_id = theFeature.attribute('node_id')
            out_link_id = theFeature.attribute('out_link_id')
            passlink_count = theFeature.attribute('passlink_count')
            lane_num = theFeature.attribute('lane_num')
            lane_info = theFeature.attribute('lane_info')
            arrow_info = theFeature.attribute('arrow_info')

            # get guide info lanes 
            strFilter = '''in_link_id=%s and node_id=%s and out_link_id=%s
                           and passlink_count=%s''' %\
                        (in_link_id, node_id, out_link_id, passlink_count)
            sqlcmd = """select lane_num, lane_info, arrow_info
                        from %s
                        where %s""" % (uri.table(), strFilter)
            pg.execute(sqlcmd)
            rows = pg.fetchall()
            if len(rows) <= 0:
                errMsg = '''get no lane record.'''
                QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
                return
            totalLaneCount = rows[0][0] # lane count of inlink.
            for row in rows:
                if row[0] != totalLaneCount:
                    errMsg = """some lane_count not the same."""
                    QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
                    return

            return 
        except KeyError, kErr:
            errMsg = """Selected feature is not a rdb lane feature."""
            QMessageBox.information(self, "Show Lane", """error:\n%s"""%errMsg)
            return
        except Exception, ex:
            QMessageBox.information(self, "Show Lane", """error:\n%s"""%ex.message)
            return
        return
    
    def getFeatureInfoString(self, theFeature):
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFeatureInfo = "field count: %d\n" % len(fieldList)
        for oneField, oneAttr in zip(fieldList, attrList):
            strFeatureInfo += "%s: %s\n" % (oneField.name(), oneAttr)
        return strFeatureInfo

    def drawLane(self, whichLane, arrowInfo, bHighlight):
        if arrowInfo & 1:
            pass

        if arrowInfo & 2:
            pass

        if arrowInfo & 4:
            pass

        if arrowInfo & 8:
            pass

        if arrowInfo & 16:
            pass

        if arrowInfo & 32:
            pass

        if arrowInfo & 64:
            pass

        if arrowInfo & 128:
            pass

        if arrowInfo & 256:
            pass

        if arrowInfo & 512:
            pass

        if arrowInfo & 1024:
            pass

        if arrowInfo & 2048:
            pass

        if arrowInfo & 4096:
            pass

        if arrowInfo & 8192:
            pass

        if whichLane & 1:
            pass
        return
