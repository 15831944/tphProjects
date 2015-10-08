# -*- coding: utf-8 -*-
from qgis.gui import QgsMapTool, QgsMapToolIdentify
from qgis.core import QgsMapLayer, QgsFeature
from PyQt4.QtCore import Qt
from PyQt4.QtGui import QCursor, QMessageBox
from LaneShowImageDlg import LaneShowImageDlg

class LaneMapTool(QgsMapTool):
    def __init__(self, canvas):
        super(QgsMapTool, self).__init__(canvas)
        self.canvas = canvas
        self.cursor = QCursor(Qt.ArrowCursor)
        
    def activate(self):
        self.canvas.setCursor(self.cursor)
    
    # Select the nearest node, then show its information.
    def canvasReleaseEvent(self, mouseEvent): 

        theLayer = self.canvas.currentLayer()
        if theLayer is None:
            return
        if theLayer.type() != QgsMapLayer.VectorLayer:
            return
        if theLayer.featureCount() == 0:
            return
          
        theLayer.removeSelection()
        qgsMapTollIndentify = QgsMapToolIdentify(self.canvas)
        resultList = qgsMapTollIndentify.identify(mouseEvent.x(), mouseEvent.y(), -1)
        if resultList == []:
            return       
        # Select the feature
        featureList = []
        featureIdList = []
        for oneResult in resultList:
            featureList.append(oneResult.mFeature)
            featureIdList.append(oneResult.mFeature.id())
        theLayer.select(featureIdList)

        dlg = LaneShowImageDlg(theLayer, featureList)
        result = dlg.exec_()
        if result:
            pass
        else:
            pass
        return
















