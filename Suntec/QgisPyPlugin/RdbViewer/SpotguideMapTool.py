# -*- coding: utf-8 -*-
from qgis.gui import QgsMapTool, QgsMapToolIdentify
from qgis.core import QgsMapLayer, QgsMapToPixel, QgsFeature, QgsFeatureRequest, QgsGeometry
from PyQt4.QtGui import QCursor, QPixmap, QMessageBox
from PyQt4.QtCore import Qt, QCoreApplication
from SpotguideShowImageDlg import SpotguideShowImageDlg


class SpotguideMapTool(QgsMapTool):
    
    def __init__(self, canvas):
        
        super(QgsMapTool, self).__init__(canvas)
        self.canvas = canvas
        self.cursor = QCursor(Qt.ArrowCursor)
        
    def activate(self):
        self.canvas.setCursor(self.cursor)
    
    def screenToLayerCoords(self, screenPos, layer):
        transform = self.canvas.getCoordinateTransform()
        canvasPoint = QgsMapToPixel.toMapCoordinates(transform, screenPos.x(), screenPos.y())
        # Transform if required
        layerEPSG = layer.crs().authid()
        projectEPSG = self.canvas.mapRenderer().destinationCrs().authid()
        if layerEPSG != projectEPSG:
            renderer = self.canvas.mapRenderer()
            layerPoint = renderer.mapToLayerCoordinates(layer, canvasPoint)
        else:
            layerPoint = canvasPoint
        
        # Convert this point (QgsPoint) to a QgsGeometry
        return QgsGeometry.fromPoint(layerPoint)

    #Each time the mouse is clicked on the map canvas, perform 
    #the following tasks:
    #    Loop through all visible vector layers and for each:
    #        Ensure no features are selected
    #        Determine the distance of the closes feature in the oneLayer to the mouse click
    #        Keep track of the oneLayer id and id of the closest feature
    #    Select the id of the closes feature
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

        dlg = SpotguideShowImageDlg(theLayer, featureList)
        result = dlg.exec_()
        if result:
            pass
        else:
            pass
        return
















