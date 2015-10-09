# -*- coding: utf-8 -*-
from qgis.gui import QgsMapTool, QgsMapToolIdentify
from qgis.core import QgsMapLayer, QgsFeature
from PyQt4.QtCore import Qt
from PyQt4.QtGui import QCursor, QMessageBox
from SpotguideShowImageDlg import SpotguideShowImageDlg


class SpotguideMapTool(QgsMapTool):
    
    def __init__(self, canvas):
        
        super(QgsMapTool, self).__init__(canvas)
        self.mCanvas = canvas
        self.mCursor = QCursor(Qt.ArrowCursor)
        self.mDragging = False
        
    def activate(self):
        self.mCanvas.setCursor(self.mCursor)

    def canvasMoveEvent(self, mouseEvent):
        theLayer = self.mCanvas.currentLayer()
        if theLayer is None:
            return
        if theLayer.type() != QgsMapLayer.VectorLayer:
            return
        if theLayer.featureCount() == 0:
            return
        if mouseEvent.buttons() & Qt.LeftButton:
            self.mDragging = True
            self.mCanvas.panAction(mouseEvent);
        return

    def canvasReleaseEvent(self, mouseEvent): 
        theLayer = self.mCanvas.currentLayer()
        if theLayer is None:
            return
        if theLayer.type() != QgsMapLayer.VectorLayer:
            return
        if theLayer.featureCount() == 0:
            return
          
        qgsMapTollIndentify = QgsMapToolIdentify(self.mCanvas)
        resultList = qgsMapTollIndentify.identify(mouseEvent.x(), mouseEvent.y(), -1)
        if resultList == []: # no feature selected, pan the canvas
            if self.mDragging == True:
                self.mCanvas.panActionEnd(mouseEvent.pos())
                self.mDragging = False
            else: # add pan to mouse cursor
                # transform the mouse pos to map coordinates
                center = self.mCanvas.getCoordinateTransform().toMapPoint(mouseEvent.x(), mouseEvent.y())
                self.mCanvas.setCenter(center)
                self.mCanvas.refresh()
        else: # select the features
            theLayer.removeSelection()
            featureList = []
            featureIdList = []
            for oneResult in resultList:
                featureList.append(oneResult.mFeature)
                featureIdList.append(oneResult.mFeature.id())
            theLayer.select(featureIdList)
            dlg = SpotguideShowImageDlg(theLayer, featureList)
            result = dlg.exec_()
            if result:
                pass
            else:
                pass
            return















