# -*- coding: utf-8 -*-
from qgis.gui import QgsMapTool, QgsMapToolIdentify
from qgis.core import QgsMapLayer, QgsFeature
from PyQt4.QtCore import Qt
from PyQt4.QtGui import QCursor, QMessageBox
from ComputePath.RdbLinkObject import RdbLinkObject

class RdbNaviSimulatorMapTool(QgsMapTool):
    def __init__(self, canvas, naviDlg):
        super(QgsMapTool, self).__init__(canvas)
        self.mCanvas = canvas
        self.mCursor = QCursor(Qt.ArrowCursor)
        self.mDragging = False
        self.mNaviDlg = naviDlg

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

        qgsMapToolIndentify = QgsMapToolIdentify(self.mCanvas)
        resultList = qgsMapToolIndentify.identify(mouseEvent.x(), mouseEvent.y(), -1)
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
            theFeature = (resultList[0]).mFeature
            if RdbLinkObject.isMyFeature(theFeature) == True:
                nodeId = theFeature.attribute('start_node_id')
                self.mNaviDlg.addNode(nodeId)
            return














