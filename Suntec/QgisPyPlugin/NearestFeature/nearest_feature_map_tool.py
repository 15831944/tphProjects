# -*- coding: utf-8 -*-
from qgis.gui import QgsMapTool
from qgis.core import QgsMapLayer, QgsMapToPixel, QgsFeature, QgsFeatureRequest, QgsGeometry
from PyQt4.QtGui import QCursor, QPixmap, QMessageBox
from PyQt4.QtCore import Qt, QCoreApplication


class NearestFeatureMapTool(QgsMapTool):
    
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


    def canvasReleaseEvent(self, mouseEvent):
        """ 
        Each time the mouse is clicked on the map canvas, perform 
        the following tasks:
            Loop through all visible vector layers and for each:
                Ensure no features are selected
                Determine the distance of the closes feature in the oneLayer to the mouse click
                Keep track of the oneLayer id and id of the closest feature
            Select the id of the closes feature 
        """
        theLayer = self.canvas.currentLayer()
        if theLayer is None:
            return
        if theLayer.type() != QgsMapLayer.VectorLayer:
            return
        if theLayer.featureCount() == 0:
            return
                
        theLayer.removeSelection()
        
        # Determine the location of the click in real-world coords
        layerPoint = self.toLayerCoordinates(theLayer, mouseEvent.pos())
        shortestDistance = float("inf")
        
        # Loop through all features in the oneLayer
        mouseClickGeom = QgsGeometry.fromPoint(layerPoint)
        theFeature = None
        for oneFeature in theLayer.getFeatures():
            dist = oneFeature.geometry().distance(mouseClickGeom)
            if dist < shortestDistance:
                shortestDistance = dist
                theFeature = oneFeature
            if shortestDistance < 0.001:
                break
        
        # Select the closest feature
        theLayer.select(theFeature.id())
        
        fieldList = theFeature.fields()
        attrList = theFeature.attributes()
        strFields = "field count: %d\n" % len(fieldList)
        for oneField, oneQVariant in zip(fieldList, attrList):
            strFields += "%s: %s\n" % (oneField.name(), str(oneQVariant))
        QMessageBox.information(self.canvas, "Feature Fields", strFields)
        















